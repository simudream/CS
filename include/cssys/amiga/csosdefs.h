/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CSOSDEFS_H__
#define __CSOSDEFS_H__

// The 2D graphics driver used by software renderer on this platform
#define SOFTWARE_2D_DRIVER "crystalspace.graphics2d.amiga"

#if defined (SYSDEF_DIR)
#  define __NEED_GENERIC_ISDIR
#endif

#if defined (PROC_M68K)
#  define CS_BIG_ENDIAN
#else
#  error "Please define a suitable CS_XXX_ENDIAN macro in amiga/csosdefs.h!"
#endif

#endif // __CSOSDEFS_H__
