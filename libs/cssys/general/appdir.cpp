/*
    Copyright (C) 2003 by Frank Richter

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
#include "csutil/cstring.h"
#include "csutil/util.h"
#include <string.h>

char* csGetAppDir (const char* argv0)
{
  char* appdir = 0;
  char* apppath = csGetAppPath(argv0);
  if (apppath != 0)
  {
    char* slash = strrchr (apppath, PATH_SEPARATOR);
    if (slash != 0)
      *slash = '\0';
    appdir = csStrNew (apppath);
    delete[] apppath;
  }
  return appdir;
}
