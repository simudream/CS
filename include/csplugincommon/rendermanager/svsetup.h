/*
    Copyright (C) 2007-2008 by Marten Svanfeldt

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

#ifndef __CS_CSPLUGINCOMMON_RENDERMANAGER_SVSETUP_H__
#define __CS_CSPLUGINCOMMON_RENDERMANAGER_SVSETUP_H__

#include "csplugincommon/rendermanager/rendertree.h"
#include "csplugincommon/rendermanager/operations.h"

class csShaderVariable;

namespace CS
{
namespace RenderManager
{

  /**
   * Standard shader variable stack setup functor.
   * Assumes that the contextLocalId in each mesh is set.
   */
  template<typename RenderTree, typename LayerConfigType>
  class StandardSVSetup
  {
  public:

    StandardSVSetup (SVArrayHolder& svArrays, 
      const LayerConfigType& layerConfig) 
      : svArrays (svArrays), layerConfig (layerConfig)
    {
    }  

    void operator() (typename RenderTree::MeshNode* node)
    {
      csShaderVariableStack localStack;

      // @@TODO: keep the sv-name around in a better way
      static size_t svO2wName = node->owner.owner.GetPersistentData().svObjectToWorldName;

      for (size_t i = 0; i < node->meshes.GetSize (); ++i)
      {
        typename RenderTree::MeshNode::SingleMesh& mesh = node->meshes[i];

        csRenderMesh* rm = mesh.renderMesh;
        for (size_t layer = 0; layer < layerConfig.GetLayerCount (); ++layer)
        {
          svArrays.SetupSVStack (localStack, layer, mesh.contextLocalId);

          // Push all contexts here 
          // @@TODO: get more of them        
          localStack[svO2wName] = mesh.svObjectToWorld;
          if (rm->material)
            rm->material->GetMaterial ()->PushVariables (localStack);
          if (rm->variablecontext)
            rm->variablecontext->PushVariables (localStack);
          if (mesh.meshObjSVs)
            mesh.meshObjSVs->PushVariables (localStack);
        }
      }
      
    }

  private:
    SVArrayHolder& svArrays;
    const LayerConfigType& layerConfig;
  };  

  template<typename RenderTree, typename LayerConfigType>
  struct OperationTraits<StandardSVSetup<RenderTree, LayerConfigType> >
  {
    typedef OperationUnordered Ordering;
  };

  
  /**
   * Standard shader variable stack setup functor for setting up shader variables
   * from given shader and ticket arrays.
   * Assumes that the contextLocalId in each mesh is set.
   */
  template<typename RenderTree, typename LayerConfigType>
  class ShaderSVSetup
  {
  public:    
    typedef csArray<iShader*> ShaderArrayType;

    ShaderSVSetup (SVArrayHolder& svArrays, const ShaderArrayType& shaderArray,
      const LayerConfigType& layerConfig)
      : svArrays (svArrays), shaderArray (shaderArray),
      layerConfig (layerConfig)
    {
      tempStack.Setup (svArrays.GetNumSVNames ());
    }

    void operator() (typename RenderTree::MeshNode* node)
    {
      const size_t totalMeshes = node->owner.totalRenderMeshes;

      for (size_t i = 0; i < node->meshes.GetSize (); ++i)
      {
        typename RenderTree::MeshNode::SingleMesh& mesh = node->meshes[i];

        csRenderMesh* rm = mesh.renderMesh;

        for (size_t layer = 0; layer < layerConfig.GetLayerCount (); ++layer)
        {
          size_t layerOffset = layer*totalMeshes;
    
          tempStack.Clear ();

          iShader* shader = shaderArray[mesh.contextLocalId+layerOffset];
          if (shader) 
          {
            shader->PushVariables (tempStack);
          
            // Back-merge it onto the real one
            csShaderVariableStack localStack;
            svArrays.SetupSVStack (localStack, layer, mesh.contextLocalId);
            localStack.MergeFront (tempStack);
          }
        }
      }
    }

  private:
    SVArrayHolder& svArrays; 
    const ShaderArrayType& shaderArray;
    csShaderVariableStack tempStack;
    const LayerConfigType& layerConfig;
  };

  template<typename RenderTree, typename LayerConfigType>
  struct OperationTraits<ShaderSVSetup<RenderTree, LayerConfigType> >
  {
    typedef OperationUnordered Ordering;
  };


  /**
   * 
   */
  template<typename ContextNode, typename LayerConfigType>
  void SetupStandardSVs (ContextNode& context, LayerConfigType& layerConfig,
    iShaderManager* shaderManager, iSector* sector)
  {
    // Setup SV arrays
    context.svArrays.Setup (
      layerConfig.GetLayerCount(), 
      shaderManager->GetSVNameStringset ()->GetSize (),
      context.totalRenderMeshes);
      
    if (context.totalRenderMeshes == 0) return;

    // Push the default stuff
    csShaderVariableStack& svStack = shaderManager->GetShaderVariableStack ();

    {
      context.svArrays.SetupSVStack (svStack, 0, 0);

      shaderManager->PushVariables (svStack);
      sector->GetSVContext ()->PushVariables (svStack);

      // Replicate
      context.svArrays.ReplicateSet (0, 0, 1);
      context.svArrays.ReplicateLayerZero ();
    }
  }
  
}
}

#endif
