//=============================================================================
//
//	Copyright (C) 2004 by Eric Sunshine <sunshine@sunshineco.com>
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//=============================================================================
//-----------------------------------------------------------------------------
// OSXInstallPath.cpp
//
//	Platform-specific function to determine configuration and plugin paths.
//
//-----------------------------------------------------------------------------
#include "cssysdef.h"
#include "csutil/syspath.h"
#include "OSXInstallPath.h"
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// csGetConfigPath
//-----------------------------------------------------------------------------
csString csGetConfigPath()
{
  csString path;
  char buff[FILENAME_MAX];
  char* env;

  if (OSXGetInstallPath(buff, FILENAME_MAX, PATH_SEPARATOR))
    path = buff;
  else if ((env = getenv("CRYSTAL")) != 0 && *env != '\0')
    path = env;

  if (path.IsEmpty())
    path << "." << PATH_SEPARATOR;
  else if (path[path.Length() - 1] != PATH_SEPARATOR)
    path << PATH_SEPARATOR;

  return path;
}
