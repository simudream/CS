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

#ifndef LIGHT_H
#define LIGHT_H

#include "csgeom/transfrm.h"
#include "csobject/csobj.h"
#include "csengine/cscolor.h"
#include "lightdef.h"

class csSector;
class csPolygon3D;
class csDynLight;
class Dumper;
class csThing;

/**
 * Superclass of all positional lights.
 * A light subclassing from this has a color, a position
 * and a radius.<P>
 *
 * First some terminology about all the several types of lights
 * that Crystal Space supports:
 * <ul>
 * <li>Static light. This is a normal static light that cannot move
 *     and cannot change intensity/color. All lighting information from
 *     all static lights is collected in one static lightmap.
 * <li>Pseudo-dynamic light. This is a static light that still cannot
 *     move but the intensity/color can change. The shadow information
 *     from every pseudo-dynamic light is kept in a seperate shadow-map.
 *     Shadowing is very accurate with pseudo-dynamic lights since they
 *     use the same algorithm as static lights.
 * <li>Dynamic light. This is a light that can move and change
 *     intensity/color. These lights are the most flexible. All lighting
 *     information from all dynamic lights is collected in one dynamic
 *     lightmap (seperate from the pseudo-dynamic shadow-maps).
 *     Shadows for dynamic lights will be less accurate because things
 *     will not cast accurate shadows (due to computation speed limitations).
 * </ul>
 * Note that static and pseudo-dynamic lights are represented by the
 * same csStatLight class.
 */
class csLight : public csObject
{
protected:
  /// Home sector of the light.
  csSector* sector;
  /// Position of the light.
  csVector3 center;
  /// Radius of light.
  float dist;
  /// Squared radius of light.
  float sqdist;
  /// Color.
  csColor color;

  /// If a halo is enabled for this light.
  bool halo_enabled;
  /// the current intensity of the attached halo.
  float halo_intensity;
  /// the maximum intensity of the attached halo.
  float halo_max_intensity;
  /// the reference count (for halo fading purposes).
  int halo_ref_count;
  /// whether this light is in the halo queue or not.
  bool in_halo_queue;
  
public:
  /// Config value: ambient red value.
  static int ambient_red;
  /// Config value: ambient green value.
  static int ambient_green;
  /// Config value: ambient blue value.
  static int ambient_blue;
  /// Config value: ambient white value.
  static int ambient_white;

public:
  /**
   * Construct a light at a given position. With
   * a given radius, a given color, a given name and
   * type. The light will not have a halo by default.
   */
  csLight (float x, float y, float z, float dist,
  	 float red, float green, float blue);

  /**
   * Destroy the light. Note that destroying a light
   * may not have the expected effect. Static lights result
   * in changes in the lightmaps. Removing them will not automatically
   * update those lightmaps as that is a time-consuming process.
   */
  virtual ~csLight ();

  /**
   * Set the current sector for this light.
   */
  virtual void SetSector (csSector* sector) { csLight::sector = sector; }

  /**
   * Get the current sector for this light.
   */
  csSector* GetSector () { return sector; }

  /**
   * Get the center position.
   */
  csVector3& GetCenter () { return center; }

  /**
   * Get the radius.
   */
  float GetRadius () { return dist; }

  /**
   * Get the squared radius.
   */
  float GetSquaredRadius () { return sqdist; }

  /**
   * Get the light color.
   */
  csColor& GetColor () { return color; }

  /**
   * Set the light color. Note that setting the color
   * of a light may not always have an immediate visible effect.
   * Static lights are precalculated into the lightmaps and those
   * lightmaps are not automatically updated when calling this function
   * as that is a time consuming process.
   */
  virtual void SetColor (const csColor& col) { color = col; }

  /**
   * Enable halo.
   */
  void EnableHalo () { halo_enabled = true; }

  /**
   * Disable halo.
   */
  void DisableHalo () { halo_enabled = false; }

  /**
   * Is the halo enabled?
   */
  bool IsHaloEnabled () { return halo_enabled; }

  /**
   * Return the maximum intensity of the halo.
   */
  float GetHaloMaxIntensity () { return halo_max_intensity; }

  /**
   * Set the intensity of the halo.
   */
  void SetHaloIntensity (float newI) { halo_intensity = newI; }

  /**
   * Get the intensity of the halo.
   */
  float GetHaloIntensity () { return halo_intensity; }

  /**
   * Return the reference count. This counter keeps track
   * of the number of times a halo is visible. If 0 the
   * halo will slowly fade away.
   */
  int GetReferenceCount () { return halo_ref_count; }

  /**
   * Add a reference to the halo. As long as there are references
   * to the halo it is visible.
   */
  void AddReference () { halo_ref_count++; }

  /**
   * Remove a reference to the halo. When all references
   * are gone the halo will slowly fade away.
   */
  void RemoveReference () { if (halo_ref_count > 0) halo_ref_count--; }

  /**
   * Set whether or not the halo is in the queue.
   */
  void SetHaloInQueue (bool bNew) { in_halo_queue = bNew; }

  /**
   * Query if the halo is in the queue.
   */
  bool GetHaloInQueue () { return in_halo_queue; }
  
  /**
   * Change the given r, g, b value to the current mixing mode
   * (TRUE_RGB or NOCOLOR). In NOCOLOR mode this function will
   * take the average of the three colors to return a grayscale
   * value.
   */
  static void CorrectForNocolor (unsigned char* rp, unsigned char* gp, unsigned char* bp);

  /**
   * Change the given r, g, b value to the current mixing mode
   * (TRUE_RGB or NOCOLOR). In NOCOLOR mode this function will
   * take the average of the three colors to return a grayscale
   * value.
   */
  static void CorrectForNocolor (float* rp, float* gp, float* bp);

  CSOBJTYPE;
};

/**
 * Class for a static light. These lights cast shadows (against
 * sector boundaries and with things), they support three different
 * colors (R,G,B). They cannot move and they can only vary in
 * intensity with some memory trade-offs (in which case we call
 * it a pseudo-dynamic light).
 */
class csStatLight : public csLight
{
private:
  /**
   * The following three variables are used if the light intensity
   * can vary. 'dynamic' is set to true in that case.
   * 'num_polygon' and 'polygons' indicate all polygons that are
   * possibly lit by this light.
   */
  bool dynamic;

  /// Number of polygons affected by this dynamic light.
  int num_polygon;

  /// List of polygons that are affected by this dynamic light.
  csPolygon3D** polygons;

public:
  /**
   * Construct a static light at a given position. With
   * a given radius and a given color. If 'dynamic' is
   * true we have a pseudo-dynamic light which can change
   * intensity and color (but not move).
   * The light will not have a halo by default.
   */
  csStatLight (float x, float y, float z, float dist,
  	 float red, float green, float blue,
	 bool dynamic);
  /**
   * Destroy the light. Note that destroying a light
   * may not have the expected effect. Static lights result
   * in changes in the lightmaps. Removing them will not automatically
   * update those lightmaps as that is a time-consuming process.
   */
  virtual ~csStatLight ();

  /**
   * Return true if this light is pseudo-dynamic.
   */
  bool IsDynamic () { return dynamic; }

  /**
   * Set the light color. Note that setting the color
   * of a light may not always have an immediate visible effect.
   * Static lights are precalculated into the lightmaps and those
   * lightmaps are not automatically updated when calling this function
   * as that is a time consuming process.
   * However, this function works as expected for pseudo-dynamic
   * lights. In this case the lightmaps will be correctly updated
   * the next time they become visible.
   */
  virtual void SetColor (const csColor& col);

  /**
   * Register a polygon for a pseudo-dynamic light.
   * Every polygon which is interested in updating its
   * lightmaps as this light changes should register itself
   * to the light.
   */
  void RegisterPolygon (csPolygon3D* poly);

  /**
   * Shine this light on all polygons visible from the light.
   * This routine will update the lightmaps of all polygons.
   * It correctly takes pseudo-dynamic lights into account and will then
   * update the corresponding shadow map.
   */
  void ShineLightmaps ();

  /**
   * Shine this light on all polygons of the csThing.
   * Only backface culling is used. The light is assumed
   * to be in the same sector as the csThing.
   */
  void ShineLightmaps (csThing* th);

  /**
   * This is a debugging function that will show the outlines
   * on all polygons that are hit by this light.
   * It will draw perspective correct outlines so it is meant to
   * be called with a camera transformation.
   * @@@ REMOVE FROM ENGINE?
   */
  void DumpFrustrum (csTransform& t);

  CSOBJTYPE;
};

/**
 * A light patch. This is a 3D polygon which fits on a world level 3D
 * polygon and defines where the light hits the polygon.
 * There is a list of light patches in every polygon (all dynamic lights
 * hitting a polygon will give rise to a seperate light patch) and there
 * is a list of light patches in every dynamic light (representing all
 * polygons that are hit by that particular light).
 */
class csLightPatch
{
  friend class csPolygon3D;
  friend class csPolyTexture;
  friend class csDynLight;
  friend class Dumper;

private:
  csLightPatch* next_poly;
  csLightPatch* prev_poly;
  csLightPatch* next_light;
  csLightPatch* prev_light;

  int num_vertices;
  csVector3* vertices;

  /// Polygon that this light patch is for.
  csPolygon3D* polygon;
  /// Light that this light patch originates from.
  csDynLight* light;

public:
  /**
   * Create an empty light patch (infinite frustrum).
   */
  csLightPatch ();

  /**
   * Unlink this light patch from the polygon and the light
   * and then destroy.
   */
  ~csLightPatch ();

  /**
   * Get the polygon that this light patch belongs too.
   */
  csPolygon3D* GetPolygon () { return polygon; }

  /**
   * Get the light that this light patch belongs too.
   */
  csDynLight* GetLight () { return light; }

  /**
   * Get next light patch as seen from the standpoint
   * of the polygon.
   */
  csLightPatch* GetNextPoly () { return next_poly; }

  /**
   * Get the next light patch as seen from the standpoint
   * of the light.
   */
  csLightPatch* GetNextLight () { return next_light; }
};

/**
 * Class for a dynamic light. These lights only cast shadows
 * for sectors/portals and not for things. However, they can
 * freely move and change color intensity.
 */
class csDynLight : public csLight
{
private:
  csDynLight* next;
  csDynLight* prev;

  /// List of light patches for this dynamic light.
  csLightPatch* lightpatches;

public:
  /**
   * Create a new dynamic light at the given position and with the
   * given radius and color. Initially the light will
   * not be visible. You need to set the current
   * sector and call 'Setup()' first.
   */
  csDynLight (float x, float y, float z, float dist,
  	 float red, float green, float blue);

  /**
   * Remove the dynamic light from all polygons (i.e.
   * remove all light patches) and then destroy the light itself.
   */
  virtual ~csDynLight ();

  /**
   * Initial placement of the light. This routine generates a view
   * frustrum as seen from the light. The clipped polygons that
   * result from this are light patches and are put in the
   * lightpatches list. This routine needs to be called whenever
   * the light moves.
   */
  void Setup ();

  /**
   * Move the light. This will NOT automatically recalculate the
   * view frustrum. You still need to call Setup() after this.
   */
  void Move (csSector* sector, csVector3& v) { Move (sector, v.x, v.y, v.z); }

  /**
   * Move the light. This will NOT automatically recalculate the
   * view frustrum. You still need to call Setup() after this.
   */
  void Move (csSector* sector, float x, float y, float z);

  /**
   * Resize the light. This will NOT automatically recalculate the
   * view frustrum. You still need to call Setup() after this.
   */
  void Resize (float radius) { dist = radius; sqdist = dist*dist; }

  /**
   * Unlink a light patch from the light patch list.
   * Warning! This function does not test if the light patch
   * is really on the list!
   */
  void UnlinkLightpatch (csLightPatch* lp);

  /**
   * Add a light patch to the light patch list.
   */
  void AddLightpatch (csLightPatch* lp);

  ///
  void SetNext (csDynLight* n) { next = n; }
  ///
  void SetPrev (csDynLight* p) { prev = p; }
  ///
  csDynLight* GetNext () { return next; }
  ///
  csDynLight* GetPrev () { return prev; }

  CSOBJTYPE;
};

#endif /*LIGHT_H*/
