/*
    Copyright (C) 1998 by Jorrit Tyberghein

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

#ifndef __GLX2D_H__
#define __GLX2D_H__

#include "csutil/scf.h"
#include "cssys/unix/iunix.h"
#include "video/canvas/openglcommon/glcommon2d.h"

#include <GL/glx.h>

#include "iogldisp.h"

#define XK_MISCELLANY 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>

/// XLIB version.
class csGraphics2DGLX : public csGraphics2DGLCommon
{
  // The display context
  Display* dpy;
  int screen_num;
  int display_width, display_height;
  Window window;
  GC gc;
  XVisualInfo *active_GLVisual;
  GLXContext active_GLContext;

  // "WM_DELETE_WINDOW" atom
  Atom wm_delete_window;

  // we are using a specific displaydriver
  iOpenGLDisp *dispdriver;
  
  // Window colormap
  Colormap cmap;

  // Hardware mouse cursor or software emulation?
  bool do_hwmouse;
  /// Mouse cursors (if hardware mouse cursors are used)  
  Cursor MouseCursor [int(csmcWait) + 1];
  /// Empty mouse cursor (consist of EmptyPixmap)
  Cursor EmptyMouseCursor;
  /// A empty pixmap
  Pixmap EmptyPixmap;

  /// Pointer to DOS-specific interface
  iUnixSystemDriver* UnixSystem;

public:
  csGraphics2DGLX (iBase *iParent);
  virtual ~csGraphics2DGLX ();

  virtual bool Initialize (iSystem *pSystem);
  virtual bool Open (const char *Title);
  virtual void Close ();

  virtual void Print (csRect *area = NULL);

  /// Set mouse position.
  virtual bool SetMousePosition (int x, int y);

  /// Set mouse cursor shape
  virtual bool SetMouseCursor (csMouseCursorID iShape);

  /// Called on every frame by system driver
  virtual bool HandleEvent (csEvent &Event);
};

#endif // __XLIB2D_H__
