/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "cssys/syspath.h"

#include <windows.h>

typedef DWORD (STDAPICALLTYPE* PFNGETLONGPATHNAMEA) (LPCSTR lpszShortPath, 
						     LPSTR lpszLongPath,
						     DWORD cchBuffer);

static DWORD STDAPICALLTYPE MyGetLPN (LPCSTR lpszShortPath, LPSTR lpszLongPath, 
				      DWORD cchBuffer)
{
  // @@@ Deal with UNC paths?

  // Because the we know the parameters with which this function will be 
  // called, some safety checks can be omitted.
  if (lpszShortPath == lpszLongPath)
  {
    CS_ALLOC_STACK_ARRAY (char, nshort, strlen (lpszShortPath) + 1);
    strcpy (nshort, lpszShortPath);
    lpszShortPath = nshort;
  }
  const char* nextpos = lpszShortPath;
  DWORD bufRemain = cchBuffer;
  *lpszLongPath = 0;

#define BUFCAT(s)				  \
  {						  \
    int len = strlen (s);			  \
    if (bufRemain > 0)				  \
    {						  \
      strncat (lpszLongPath, s, bufRemain);	  \
      bufRemain -= len;				  \
    }						  \
  }


  while (nextpos != 0)
  {
    char buf[MAX_PATH];
    const char* pos = nextpos;
    char* bs = strchr (pos, '\\');
    if (bs)
    {
      strncpy (buf, pos, (bs - pos));
      buf[bs - pos] = 0;
    }
    else
    {
      strcpy (buf, pos);
    }
    if (buf[1] == ':')
    {
      BUFCAT (buf);
    }
    else
    {
      BUFCAT ("\\");
      char* bufEnd = strchr (lpszLongPath, 0);
      strncpy (bufEnd, buf, bufRemain - 1);
      bufEnd[bufRemain - 1] = 0;

      WIN32_FIND_DATA fd;
      HANDLE hFind = FindFirstFile (lpszLongPath, &fd);
      if (hFind != INVALID_HANDLE_VALUE)
      {
	*bufEnd = 0;
	BUFCAT (fd.cFileName);
	FindClose (hFind);
      }
      else
      {
	return 0;
      }
    }
    nextpos = bs ? bs + 1 : 0;
  }
  return (cchBuffer - bufRemain);
#undef BUFCAT
}

char* csExpandPath (const char* path)
{
  if ((path == 0) || (*path == 0)) return 0;

  char fullName[MAX_PATH];
  GetFullPathName (path, sizeof(fullName), fullName, 0);

  DWORD result;
  PFNGETLONGPATHNAMEA GetLongPathName = 0;
  // unfortunately, GetLongPathName() is only supported on Win98+/W2k+
  HMODULE hKernel32 = LoadLibrary ("kernel32.dll");
  if (hKernel32 != 0)
  {
    GetLongPathName = 
      (PFNGETLONGPATHNAMEA)GetProcAddress (hKernel32, "GetLongPathNameA");
    if (GetLongPathName == 0)
    {
      GetLongPathName = MyGetLPN;
    }
    result = GetLongPathName (fullName, fullName, sizeof (fullName));
    FreeLibrary (hKernel32);
  }
  if (result == 0) 
  {
    return 0;
  }

  return (csStrNew (fullName));
}

bool csPathsIdentical (const char* path1, const char* path2)
{
  return (strcasecmp (path1, path2) == 0);
}

char* csGetAppPath (const char*)
{
  char appPath[MAX_PATH];
  GetModuleFileName (0, appPath, sizeof (appPath) - 1);
  return (csStrNew (appPath));
}

char* csGetAppDir (const char* argv0)
{
  char* apppath = csGetAppPath(argv0);
  char* slash = strrchr (apppath, PATH_SEPARATOR);
  if (slash)
    *slash = 0;
  char* appdir = csStrNew(apppath);
  delete[] apppath;
  return appdir;
}
