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

#ifndef __CS_RLLOADER_H__
#define __CS_RLLOADER_H__

#include "csutil/scf.h"
#include "csutil/strhash.h"
#include "csutil/csstring.h"
#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "imap/reader.h"
#include "itexture/itexfact.h"

#include "../common/parserenderstep.h"

class csRenderLoopLoader : public iComponent, iLoaderPlugin
{
protected:
  csRef<iObjectRegistry> object_reg;
  csRef<iSyntaxService> synldr;

  csRenderStepParser rsp;

  csStringHash tokens;
#define CS_TOKEN_ITEM_FILE "video/render3d/renderloop/loader/rlloader.tok"
#include "cstool/tokenlist.h"

  bool ParseRenderSteps (iRenderLoop* loop, iDocumentNode* node);
public:
  SCF_DECLARE_IBASE;

  csRenderLoopLoader (iBase *p);
  virtual ~csRenderLoopLoader ();

  virtual bool Initialize(iObjectRegistry *object_reg);

  virtual csPtr<iBase> Parse (iDocumentNode* node, 
    iLoaderContext* ldr_context, iBase* context);
};  


#endif
