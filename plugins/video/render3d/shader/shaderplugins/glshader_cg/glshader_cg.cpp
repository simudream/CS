/*
Copyright (C) 2002 by Anders Stenberg
                      Marten Svanfeldt

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"

#include "csutil/hashmap.h"
#include "csutil/objreg.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/csmd5.h"
#include "csutil/scfstr.h"
#include "csgeom/vector3.h"

#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/shader/shader.h"

#include "video/canvas/openglcommon/glextmanager.h"

#include "glshader_cgvp.h"
#include "glshader_cgfp.h"

#include "glshader_cg.h"

csRef<iObjectRegistry> csGLShader_CG::object_reg;
CGcontext csGLShader_CG::context;

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLShader_CG)

SCF_IMPLEMENT_IBASE(csGLShader_CG)
SCF_IMPLEMENTS_INTERFACE(iShaderProgramPlugin)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLShader_CG::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END


csGLShader_CG::csGLShader_CG(iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);

  context = cgCreateContext ();
  cgSetErrorCallback (ErrorCallback);

  enable = false;
  isOpen = false;
}

csGLShader_CG::~csGLShader_CG()
{
  cgDestroyContext (context);
  SCF_DESTRUCT_EMBEDDED_IBASE(scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

void csGLShader_CG::ErrorCallback ()
{
  CGerror error = cgGetError();
  csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
    "crystalspace.graphics3d.shader.glcg",
    cgGetErrorString (error), 0);
  if (error == CG_COMPILER_ERROR)
    csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
      "crystalspace.graphics3d.shader.glcg",
      cgGetLastListing (context), 0);
}

////////////////////////////////////////////////////////////////////
//                      iShaderProgramPlugin
////////////////////////////////////////////////////////////////////
bool csGLShader_CG::SupportType(const char* type)
{
  if (!enable)
    return false;
  if( strcasecmp(type, "vp") == 0)
    return true;
  if( strcasecmp(type, "fp") == 0)
    return true;
  return false;
}

csPtr<iShaderProgram> csGLShader_CG::CreateProgram(const char* type)
{
  if( strcasecmp(type, "vp") == 0)
    return csPtr<iShaderProgram>(new csShaderGLCGVP(this));
  else if( strcasecmp(type, "fp") == 0)
    return csPtr<iShaderProgram>(new csShaderGLCGFP(this));
  else return 0;
}

void csGLShader_CG::Open()
{
  if (isOpen) return;
  if(!object_reg)
    return;

  csRef<iGraphics3D> r = CS_QUERY_REGISTRY(object_reg,iGraphics3D);
  csRef<iShaderRenderInterface> sri = SCF_QUERY_INTERFACE(r,
	iShaderRenderInterface);

  csRef<iFactory> f = SCF_QUERY_INTERFACE (r, iFactory);
  if (f != 0 && strcmp ("crystalspace.graphics3d.opengl", 
	f->QueryClassID ()) == 0)
    enable = true;
  else
    return;

  csGLExtensionManager *ext;
  r->GetDriver2D()->PerformExtension ("getextmanager", &ext);

  bool route = false;
  csRef<iConfigManager> config (CS_QUERY_REGISTRY (object_reg, iConfigManager));
  if (config->KeyExists ("Video.OpenGL.Shader.Cg.PSRouting"))
  {
    route = config->GetBool ("Video.OpenGL.Shader.Cg.PSRouting");
    if (route)
      csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
        "crystalspace.graphics3d.shader.glcg",
        "Routing Cg fragment programs to Pixel Shader plugin ON (forced).", 0);
    else
      csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.graphics3d.shader.glcg",
      "Routing Cg fragment programs to Pixel Shader plugin OFF (forced).", 0);
  } else {
    ext->InitGL_ARB_fragment_program ();
    ext->InitGL_ATI_fragment_shader ();

    // If the hardware's got ATI_f_p, but isn't new enough to have ARB_f_p
    // we default to routing Cg fragment programs to the Pixel Shader plugin
    route = !ext->CS_GL_ARB_fragment_program && 
            ext->CS_GL_ATI_fragment_shader;
    if (route)
      csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.graphics3d.shader.glcg",
      "Routing Cg fragment programs to Pixel Shader plugin ON (default).", 0);
    else
      csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.graphics3d.shader.glcg",
      "Routing Cg fragment programs to Pixel Shader plugin OFF (default).", 0);
  }

  if (route)
  {
    csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY (object_reg,
      iPluginManager);

    psplg = CS_QUERY_PLUGIN_CLASS(plugin_mgr, 
      "crystalspace.graphics3d.shader.glps1", iShaderProgramPlugin);
    if(!psplg)
    {
      psplg = CS_LOAD_PLUGIN(plugin_mgr, 
        "crystalspace.graphics3d.shader.glps1", iShaderProgramPlugin);
      if (!psplg)
      {
        csReport (object_reg, CS_REPORTER_SEVERITY_WARNING,
          "crystalspace.graphics3d.shader.glcg",
          "Could not find crystalspace.graphics3d.shader.glps1. Cg to PS "
          "routing unavailable.", 0);
      }
    }
  }

  isOpen = true;
}

////////////////////////////////////////////////////////////////////
//                          iComponent
////////////////////////////////////////////////////////////////////
bool csGLShader_CG::Initialize(iObjectRegistry* reg)
{
  object_reg = reg;
  return true;
}

