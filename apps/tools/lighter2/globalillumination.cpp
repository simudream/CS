/*
  Copyright (C) 2008 by Greg Hoffman

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
#include "common.h"
#include "Photonmap.h"
#include "lighter.h"
#include "globalillumination.h"
#include "primitive.h"
#include "lighter.h"
#include "lightmap.h"
#include "lightmapuv.h"
#include "lightmapuv_simple.h"
#include "primitive.h"
#include "raygenerator.h"
#include "raytracer.h"
#include "scene.h"

namespace lighter
{

  GlobalIllumination::GlobalIllumination()
  {
    finalGather = false;
  }

  GlobalIllumination::GlobalIllumination(Configuration::INDIProperties config)
  {
    finalGather = config.finalGather;
    numFinalGatherRays = config.numFinalGatherRays;
    searchRadius = config.sampleDistance;
    //searchRadius = 0.1f;
    numPhotonsPerLight = config.numPhotons;
  }
	
  void GlobalIllumination::ShadeIndirectLighting(Sector *sect, 
    Statistics::Progress& progress)
  {
    progress.SetProgress(0);
    size_t totalElements = 0;
    ObjectHash::GlobalIterator gitr = sect->allObjects.GetIterator();
    // Sum up the objects for progress calculations
    while (gitr.HasNext ())
    {
      csRef<Object> obj = gitr.Next ();

      if (!obj->GetFlags ().Check (OBJECT_FLAG_NOLIGHT))
      {
        if (obj->lightPerVertex)
          totalElements += obj->GetVertexData().positions.GetSize();
        else
        {
          csArray<PrimitiveArray>& submeshArray = obj->GetPrimitives ();
          for (size_t submesh = 0; submesh < submeshArray.GetSize (); ++submesh)
          {
            PrimitiveArray& primArray = submeshArray[submesh];

            for (size_t pidx = 0; pidx < primArray.GetSize (); ++pidx)
            {
              Primitive& prim = primArray[pidx];
              totalElements += prim.GetElementCount();
            }
          }
        }
      }
    }

    // Now do the calculations
    ProgressState progressState(progress, totalElements);
    gitr.Reset();
    while (gitr.HasNext())
    {
      csRef<Object> obj = gitr.Next();
      csArray<PrimitiveArray>& submeshArray = obj->GetPrimitives ();

      for (size_t submesh = 0; submesh < submeshArray.GetSize (); ++submesh)
      {
        PrimitiveArray& primArray = submeshArray[submesh];

        float area2pixel = 1.0f;

        for (size_t pidx = 0; pidx < primArray.GetSize (); ++pidx)
        {
          Primitive& prim = primArray[pidx];

          area2pixel = 
            1.0f / (prim.GetuFormVector () % prim.GetvFormVector ()).Norm();

          //const ElementAreas& areas = prim.GetElementAreas ();
          size_t numElements = prim.GetElementCount ();        
          Lightmap* normalLM = sect->scene->GetLightmap (
            prim.GetGlobalLightmapID (), 0, (Light*)0);

          csVector2 minUV = prim.GetMinUV ();

          // Iterate all elements
          for (size_t eidx = 0; eidx < numElements; ++eidx)
          {
            Primitive::ElementType elemType = prim.GetElementType (eidx);

            ElementProxy ep = prim.GetElement (eidx);
            size_t u, v;
            prim.GetElementUV (eidx, u, v);
            u += size_t (floorf (minUV.x));
            v += size_t (floorf (minUV.y));

            if (elemType == Primitive::ELEMENT_EMPTY)
            {     
              progressState.Advance();
              continue;
            }

            const float pixelAreaPart = 
              elemType == Primitive::ELEMENT_BORDER ? prim.ComputeElementFraction (eidx) : 
                                                      1.0f;

            // Get the position, radius,and normal to pull from
            csVector3 pos = ep.primitive.ComputeElementCenter(ep.element);
            // TODO: Pull this attribute from an outside source
            csVector3 normal = ep.primitive.ComputeNormal(pos);

            // Pull the color from the photon map
            csColor c;
            // check to make sure the photon map exists
            if (sect->photonMap)
            {
              // check to see if we are supposed to do a final gather
              if (finalGather)
              {
                // average over the number of FG rays
                csColor final(0,0,0);
                for (size_t num = 0; num < numFinalGatherRays; ++num)
                {
                  lighter::HitPoint hit;
                  hit.distance = FLT_MAX*0.9f;
                  lighter::Ray ray;

                  // create a new directional vector that faces towards the
                  // direction of the normal
                  csVector3 newDir = randVect.Get();
                  if (newDir*normal < 0.0)
                  {
                    newDir = newDir*-1.0;
                  }
                  ray.direction = newDir;
                  ray.origin = pos;
                  ray.minLength = 0.01f;

                  // raytrace another ray
                  if (lighter::Raytracer::TraceClosestHit(sect->kdTree, ray, hit))
                  {
                    if (hit.primitive)
                    {
                      csVector3 dirToSrc = csVector3(-ray.direction.x, 
                        -ray.direction.y, -ray.direction.z);
                      csVector3 hNorm = 
                        hit.primitive->ComputeNormal(hit.hitPoint);
                      final += 
                        sect->photonMap->SampleColor(hit.hitPoint, 
                        searchRadius, hNorm, dirToSrc);
                    }
                  }
                  else
                  {
                    final += sect->photonMap->SampleColor(pos, searchRadius, normal);
                  }
                }
                // average the color
                c = final * (1.0 / numFinalGatherRays);
              }
              else
              {
                c = sect->photonMap->SampleColor(pos, searchRadius, normal);
              }
            }
            progressState.Advance();
            normalLM->SetAddPixel (u, v, c * pixelAreaPart);
          }
        }
      }
    }
    progress.SetProgress(1);
  }

  void GlobalIllumination::EmitPhotons(Sector *sect)
  {
    // emit from the lights
    const LightRefArray& allNonPDLights = sect->allNonPDLights;
    for (size_t pdli = 0; pdli < allNonPDLights.GetSize(); ++pdli)
    {
      Light* pdl = allNonPDLights[pdli];
      const csVector3& pos = pdl->GetPosition();
      const csColor& color = pdl->GetColor();
      const csColor& power = pdl->GetPower();
      csVector3 dir;

      // send out photons in random directions since we only have point
      // lights at this point
      for (size_t num = 0; num < numPhotonsPerLight; ++num)
      {
        // generate new random direction vector
        dir = randVect.Get();
        sect->EmitPhoton(pos, dir, color, power);
      }
    }

    int temp = 0;
  }
}
