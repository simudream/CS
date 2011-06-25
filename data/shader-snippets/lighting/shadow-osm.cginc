<!--
  Copyright (C) 2008 by Frank Richter

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
-->
<include>
<?Include lightfuncs.cginc ?>
<![CDATA[

#ifndef __SHADOW_OSM_CG_INC__
#define __SHADOW_OSM_CG_INC__

struct ShadowShadowMapDepth : ShadowShadowMap
{
  float4x4 shadowMapTF;
  float4 shadowMapCoords;
  float4 shadowMapCoordsProj;
  sampler2D shadowMap;
  float bias;
  float gradient;
  
  void InitVP (int lightNum, float4 surfPositionWorld,
               float3 normWorld,
               out float4 vp_shadowMapCoords,
               out float vp_gradientApprox)
  {
    float4x4 lightTransformInv = lightProps.transformInv[lightNum];
    // Transform world position into light space
    float4 view_pos = mul(lightTransformInv, surfPositionWorld);
    // Transform position in light space into "shadow map space"
    float4 shadowMapCoords;
    shadowMapTF = lightPropsSM.shadowMapTF[lightNum];
    /* CS' render-to-texture Y-flips render targets (so the upper left
       gets rendered to 0,0), we need to unflip here again. */
    float4x4 flipY;
    flipY[0] = float4 (1, 0, 0, 0);
    flipY[1] = float4 (0, -1, 0, 0);
    flipY[2] = float4 (0, 0, 1, 0);
    flipY[3] = float4 (0, 0, 0, 1);
    shadowMapTF = mul (flipY, shadowMapTF);
    shadowMapCoords = mul (shadowMapTF, view_pos);
    
    vp_shadowMapCoords = shadowMapCoords;
    vp_shadowMapCoords = view_pos;
    
    float3 normL = mul(lightTransformInv, float4 (normWorld, 0)).xyz;
    float3 normShadow = normalize (mul (shadowMapTF, float4 (normL, 0)).xyz);
    //float3 viewDirShadow = normalize (mul (shadowMapTF, float4 (0, 0, -1, 0)).xyz);
    vp_gradientApprox = 1-saturate (dot (normShadow, float3 (0, 0, -1)));
    
    vp_gradientApprox = view_pos.z;
    
    /* @@@ FIXME: This should prolly be:
    float3 viewDirShadow = -normalize (shadowMapCoords.xyz);
    vp_gradientApprox = 1-saturate (dot (normShadow, viewDirShadow));
    */
  }
  
  void Init (int lightNum, float4 vp_shadowMapCoords, float vp_gradient)
  {
    shadowMapCoords = vp_shadowMapCoords;
    shadowMap = lightPropsSM.shadowMap[lightNum];
    gradient = vp_gradient;
    
    // Project SM coordinates
    shadowMapCoordsProj = shadowMapCoords;
    shadowMapCoordsProj.xyz /= shadowMapCoordsProj.w;
    
    // FWIW, this should prolly be made some kind of setting.
    bias = 0.0001;
    //bias *= 1 + (gradient*gradient*256);
  }
  
  float getMapValue(int i, float2 position)
  {
    if (position.x < 0 || position.y < 0 || position.x > 1 || position.y > 1)
      return 0;
  
    if (i == 0)
      return tex2D(lightPropsSM.shadowMap[0], position).r;
    if (i == 1)
      return tex2D(lightPropsSM.shadowMap[1], position).r;
    if (i == 2)
      return tex2D(lightPropsSM.shadowMap[2], position).r;
    if (i == 3)
      return tex2D(lightPropsSM.shadowMap[3], position).r;	
    if (i == 4)
      return tex2D(lightPropsSM.shadowMap[4], position).r;	
    if (i == 5)
      return tex2D(lightPropsSM.shadowMap[5], position).r;	
    if (i == 6)
      return tex2D(lightPropsSM.shadowMap[6], position).r;	
    if (i == 7)
      return tex2D(lightPropsSM.shadowMap[7], position).r;
      
    return 0;
  }
  
  half GetVisibility()
  {
    float3 shadowMapCoordsBiased = (float3(0.5)*shadowMapCoordsProj.xyz) + float3(0.5);    
    half inLight = 1;
	
    int numSplits = lightPropsSM.shadowMapNumSplits;

    float previousSplit = 0, nextSplit;
  
    for (int i = 0 ; i <= numSplits ; i ++)
    {
      previousSplit = lightPropsSM.splitDists[i];
      nextSplit = lightPropsSM.splitDists[i + 1];
      
      if (gradient < nextSplit || i == numSplits)
      {
        float4x4 flipY;
        flipY[0] = float4 (1, 0, 0, 0);
        flipY[1] = float4 (0, -1, 0, 0);
        flipY[2] = float4 (0, 0, 1, 0);
        flipY[3] = float4 (0, 0, 0, 1);        

        float4x4 shadowMapTFPrev = mul (flipY, lightPropsSM.shadowMapTF[i]);
        float4 shadowMapCoordsPrev = mul (shadowMapTFPrev, shadowMapCoords);      
        float4 shadowMapCoordsProjPrev = shadowMapCoordsPrev;
        shadowMapCoordsProjPrev.xyz /= shadowMapCoordsProjPrev.w;      
        float3 shadowMapCoordsBiasedPrev = 
          (float3(0.5)*shadowMapCoordsProjPrev.xyz) + float3(0.5);          
        
        float4x4 shadowMapTFNext = mul (flipY, lightPropsSM.shadowMapTF[i + 1]);
        float4 shadowMapCoordsNext = mul (shadowMapTFNext, shadowMapCoords);      
        float4 shadowMapCoordsProjNext = shadowMapCoordsNext;
        shadowMapCoordsProjNext.xyz /= shadowMapCoordsProjNext.w;      
        float3 shadowMapCoordsBiasedNext = 
          (float3(0.5)*shadowMapCoordsProjNext.xyz) + float3(0.5);
          
        float previousMap = getMapValue(i, shadowMapCoordsBiasedPrev.xy);
        float nextMap = getMapValue(i + 1, shadowMapCoordsBiasedNext.xy);   
   
        inLight = lerp(1 - previousMap, 1 - nextMap, (float) (gradient - previousSplit) 
          / (nextSplit - previousSplit) );
          
        inLight = inLight * (i != numSplits) + (1 - previousMap) * (i == numSplits) ;
          
        //inLight = 1 - previousMap;   
          
        break;
      }
      previousSplit = nextSplit;
    }
    
    return inLight;
  }
};

#endif // __SHADOW_OSM_CG_INC__
 
]]>
</include>