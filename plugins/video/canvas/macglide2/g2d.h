/*
    Copyright (C) 1998 by Jorrit Tyberghein and Dan Ogles
  
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

// g2d_glide.h
// Graphics2DGlide Class Declaration
// Written by xtrochu and Nathaniel

#ifndef G2D_GLIDE2_H
#define G2D_GLIDE2_H

#include <glide.h>
 
#include "csutil/scf.h"
#include "video/canvas/common/graph2d.h"
#include "video/canvas/glide2common/iglide2d.h"
#include "video/canvas/glide2common/glide2common2d.h"
// whats that ?
//#include "video/canvas/macglide2/xg2d.h"

#if defined(DISP_X11)
  #define XK_MISCELLANY 1
  #include <X11/Xlib.h>
  #include <X11/keysymdef.h>
#endif           

class csGraphicsGlide2x : public csGraphics2DGlideCommon
{
#if defined(DISP_X11)
private:
  Display* dpy;
  int screen_num;
  int display_width, display_height;
  Window window;
  GC gc;                 
#endif

public:
  csGraphics2DGlide2x(iBase *iParent);
  virtual ~csGraphics2DGlide2x(void);

  virtual bool Initialize (iSystem *pSystem);
  
  virtual bool Open (const char *Title);
  virtual void Close ();
  
  ///------------------ iGraphics2DGlide2x implementation ------------------
#if defined(OS_WIN32)
  ///
  virtual HWND GethWnd () = 0;
#endif
#if defined (OS_LINUX)
  virtual Display *GetDisplay () = 0;
#endif

protected:
#if defined(OS_WIN32)
  HWND m_hWnd;
#endif
};

#endif // G3D_GLIDE_H

