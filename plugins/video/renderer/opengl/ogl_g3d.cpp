/*
 * Copyright (C) 1998 by Jorrit Tyberghein
 * 
 * This library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 */

#include <stdarg.h>

#include <stdio.h>

#include "cssysdef.h"
#include "cssys/sysdriv.h"
#include "qint.h"
#include "csutil/scf.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/polyclip.h"
#include "csgeom/plane3.h"
#include "ogl_g3d.h"
#include "ogl_txtcache.h"
#include "csutil/inifile.h"
#include "isystem.h"
#include "igraph3d.h"
#include "itxtmgr.h"
#include "itexture.h"
#include "ipolygon.h"
#include "icamera.h"
#include "ilghtmap.h"
#include "igraph2d.h"
#include "csutil/garray.h"
#include "csutil/cscolor.h"

#include <GL/gl.h>
#include <GL/glu.h>

#define SysPrintf System->Printf


/*=========================================================================
 SCF macro section
=========================================================================*/

IMPLEMENT_FACTORY (csGraphics3DOpenGL)
EXPORT_CLASS_TABLE (gl3d)
EXPORT_CLASS (csGraphics3DOpenGL, "crystalspace.graphics3d.opengl",
	      "OpenGL 3D graphics driver for Crystal Space")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csGraphics3DOpenGL)
IMPLEMENTS_INTERFACE (iPlugIn)
IMPLEMENTS_INTERFACE (iGraphics3D)
IMPLEMENT_IBASE_END

/*=========================================================================
 Method implementations
=========================================================================*/

csGraphics3DOpenGL::csGraphics3DOpenGL (iBase *iParent) :
  csGraphics3DOGLCommon ()
{
  CONSTRUCT_IBASE (iParent);
}

csGraphics3DOpenGL::~csGraphics3DOpenGL ()
{

}

bool csGraphics3DOpenGL::Initialize (iSystem * iSys)
{
  return NewInitialize (iSys);
}

bool csGraphics3DOpenGL::Open (const char *Title)
{

  return NewOpen (Title);
}

void csGraphics3DOpenGL::Close ()
{
  csGraphics3DOGLCommon::Close ();
}


iGraphics3D *csGraphics3DOpenGL::CreateOffScreenRenderer 
  (iGraphics3D * /*parent_g2d*/, int width, int height,  csPixelFormat *pfmt, 
    void *buffer, RGBPixel * /*palette*/, int /*pal_size*/)
{
 (void) width;
 (void) height;
 (void) pfmt;
 (void) buffer;
 return NULL;
}
