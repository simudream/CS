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

#ifndef CSSPRITE_H
#define CSSPRITE_H

#include "csgeom/math3d.h"
#include "csgeom/math2d.h"
#include "csgeom/box.h"
#include "csgeom/polyint.h"
#include "csengine/csobjvec.h"
#include "csengine/rview.h"
#include "csengine/cscolor.h"
#include "csengine/texture.h"
#include "csengine/tranman.h"
#include "igraph3d.h"

class Dumper;
class csTextureList;
class csTextureHandle;
class csTriangleMesh;
class csLightHitsSprite;
class csSkeleton;
class csSkeletonState;
class csSprite3D;
class csBspContainer;
interface ITextureHandle;

/**
 * A frame for 3D sprite animation.
 */
class csFrame : public csBase
{
private:
  csVector3* vertices;
  csVector2* texels;
  csVector3* normals;
  char* name;
  int max_vertex;
  /// Bounding box in object space for this frame.
  csBox3 box;

public:
  ///
  csFrame (int max_vertices);
  ///
  virtual ~csFrame ();

  ///
  void SetVertex (int i, float x, float y, float z)
  {
    vertices[i].x = x;
    vertices[i].y = y;
    vertices[i].z = z;
  }

  ///
  void SetVertex (int i, const csVector3& v)
  {
    vertices[i].x = v.x;
    vertices[i].y = v.y;
    vertices[i].z = v.z;
  }

  ///
  void SetTexel (int i, float u, float v)
  {
    texels[i].x = u;
    texels[i].y = v;
  }

  ///
  void SetName (char * n);
  ///
  char* GetName () { return name; }

  ///
  csVector3& GetVertex (int i) { return vertices[i]; }
  ///
  csVector3* GetVertices () { return vertices; }
  ///
  csVector2& GetTexel (int i) { return texels[i]; }

  ///
  csVector3& GetNormal (int i) { return normals[i]; }
  /// Return true if this frame has calculated normals.
  bool HasNormals () { return normals != NULL; }

  ///
  void AddVertex (int num_vertices);
  ///
  int GetMaxVertices () { return max_vertex; }

  /**
   * Reorder vertices in this frame according to a mapping.
   * This is used after LOD precalculation to remap the vertices
   * in a more efficient ordering.
   * The index to 'mapping' is the old vertex number. The
   * result is the new number.
   */
  void RemapVertices (int* mapping, int num_vertices);

  /**
   * Compute all normals in this frame given the
   * mesh which connects the vertices in the frame.
   * The vertex array is also given. Note that the vertex
   * array from the frame is not used because it is possible
   * that the vertices are computed using a skeleton.
   */
  void ComputeNormals (csTriangleMesh* mesh, csVector3* object_verts, int num_vertices);

  /**
   * Compute the object space bounding box for this frame.
   * This has to be called after setting up the frame and before
   * using it.
   */
  void ComputeBoundingBox (int num_vertices);

  /**
   * Get the bounding box in object space.
   */
  void GetBoundingBox (csBox3& b) { b = box; }
};

/**
 * A Action frameset for a 3D sprite animation.
 */
class csSpriteAction : public csBase
{
public:
  /// Initialize a action object
  csSpriteAction ();
  /// Destroy this action object
  virtual ~csSpriteAction ();

  /// Add a frame to this action
  void AddFrame (csFrame * frame, int delay);
  /// Set action name
  void SetName (char *n);
  /// Get action name
  char * GetName ()
  { return name; }
  /// Get total number of frames in this action
  int GetNumFrames ()
  { return frames.Length (); }
  /// Query the frame number f
  csFrame* GetFrame (int f)
  { return (f < frames.Length ()) ? (csFrame *)frames [f] : (csFrame*)NULL; }
  /// Get delay for frame number f
  int GetFrameDelay (int f)
  { return (int)delays [f]; }

private:
  char *name;
  csVector frames;
  csVector delays;
};

/**
 * A 3D sprite based on a triangle mesh with a single texture.
 * Animation is done with frames.
 * This class represents a template from which a csSprite3D
 * class can be made.
 */
class csSpriteTemplate : public csObject
{
  friend class Dumper;

private:
  friend class csSprite3D;
  friend class csCollider;

  /// Texture handle as returned by ITextureManager.
  csTextureHandle* cstxt;

  /// The vertices.
  int num_vertices;

  /// The triangles.
  csTriangleMesh* base_mesh;

  /// An optional skeleton.
  csSkeleton* skeleton;

  /**
   * The order in which to introduce levels in order to get to a higher LOD.
   * The index of this array is the vertex number which is introduced.
   * The vertices of this template were reordered (by GenerateLOD()) so that
   * the first vertices are used in low-detail. The contents of this array
   * is the vertex number to emerge from.
   */
  int* emerge_from;

  /// The frames
  csObjVector frames;
  /// The actions (a vector of csSpriteAction objects)
  csObjVector actions;

public:
  /// Create the sprite template
  csSpriteTemplate ();
  /// Destroy the template
  virtual ~csSpriteTemplate ();

  /**
   * Create a new sprite for this template.
   * The 'default' action will be made default. If there is
   * no default action the first action will be made default.
   * The sprite will also be initialized (csSprite3D::InitSprite()).
   */
  csSprite3D* NewSprite ();

  /**
   * Get the base triangle mesh of this sprite.
   */
  csTriangleMesh* GetBaseMesh () { return base_mesh; }

  /// Set the skeleton for this sprite template.
  void SetSkeleton (csSkeleton* sk);

  /// Get the skeleton for this sprite template.
  csSkeleton* GetSkeleton () { return skeleton; }

  /// Get the 'emerge_from' array from which you can construct triangles.
  int* GetEmergeFrom () { return emerge_from; }

  /**
   * Generate the collapse order.
   * This function will also reorder all the vertices in the template.
   * So be careful!
   */
  void GenerateLOD ();

  /**
   * Compute the object space bounding box for all frames in this
   * template. This has to be called after setting up the template and before
   * using it.
   */
  void ComputeBoundingBox ();

  /// Set the number of vertices.
  void SetNumVertices (int v) { num_vertices = v; }

  /// Query the number of vertices.
  int GetNumVertices () { return num_vertices; }

  /// Create and add a new frame to the sprite.
  csFrame* AddFrame ();
  /// find a named frame into the sprite.
  csFrame* FindFrame (char * name);
  /// Query the number of frames
  int GetNumFrames () { return frames.Length (); }
  /// Query the frame number f
  csFrame* GetFrame (int f)
  { return (f < frames.Length ()) ? (csFrame *)frames [f] : (csFrame*)NULL; }

  /// Create and add a new action frameset to the sprite.
  csSpriteAction* AddAction ();
  /// find a named action into the sprite.
  csSpriteAction* FindAction (const char * name);
  /// Get the first action.
  csSpriteAction* GetFirstAction ()
  { return (csSpriteAction *)actions [0]; }
  /// Get number of actions in sprite
  int GetNumActions ()
  { return actions.Length (); }
  /// Get action number No
  csSpriteAction* GetAction (int No)
  { return (csSpriteAction *)actions [No]; }

  /// Get the texture
  csTextureHandle* GetTexture () const { return cstxt; }
  /// Get the texture handle.
  ITextureHandle* GetTextureHandle () const { return cstxt->GetTextureHandle (); }
  /// Set the texture used for this sprite
  void SetTexture (csTextureList* textures, char *texname);

  CSOBJTYPE;
};

/// A callback function for csSprite3D::Draw().
typedef void (csSpriteCallback) (csSprite3D* spr, csRenderView* rview);

/**
 * A 3D sprite based on a triangle mesh with a single texture.
 * Animation is done with frames (a frame may be controlled by
 * a skeleton).
 */
class csSprite3D : public csObject
{
  friend class Dumper;
  friend class csCollider;

private:
  /// Static vertex array.
  static csVector3* tr_verts;
  /// Static z array.
  static float* z_verts;
  /// Static uv array.
  static csVector2* uv_verts;
  /// The perspective corrected vertices.
  static csVector2* persp;
  /// Array which indicates which vertices are visible and which are not.
  static bool* visible;
  /// Array for lighting.
  static csLight** light_worktable;
  static int max_light_worktable;

  /// Update the above tables with a new size.
  static void UpdateWorkTables (int max_size);

  /// Update defered lighting.
  void UpdateDeferedLighting ();

  /**
   * Camera space bounding box is cached here.
   * GetCameraBoundingBox() will check the current cookie from the
   * transformation manager to see if it needs to recalculate this.
   */
  csBox3 camera_bbox;

  /// Current cookie for camera_bbox.
  csTranCookie camera_cookie;

public:
  /// List of sectors where this sprite is.
  csObjVector sectors;

  /**
   * Configuration value for global LOD. 0 is lowest detail, 1 is maximum.
   * If negative then the base mesh is used and no LOD reduction/computation
   * is done.
   */
  static float cfg_lod_detail;

  /**
   * Quality setting for sprite lighting. If true this uses
   * high quality lighting which is more accurate on the vertices.
   * Otherwise an approximation is used. This is a lot faster though.
   */
  static bool do_quality_lighting;

protected:
  UInt MixMode;

private:
  /// Object to world transformation.
  csVector3 v_obj2world;
  /// Object to world transformation.
  csMatrix3 m_obj2world;
  /// World to object transformation.
  csMatrix3 m_world2obj;

  /**
   * A mesh which contains a number of triangles as generated
   * by the LOD algorithm. This is static since it will likely
   * change every frame anyway. We hold it static also since
   * we don't want to allocate it again every time.
   */
  static csTriangleMesh mesh;

  /**
   * Array of colors for the vertices. If not set then this
   * sprite does not have colored vertices.
   */
  csColor* vertex_colors;

  /// The template.
  csSpriteTemplate* tpl;

  /// The texture handle as returned by ITextureManager.
  csTextureHandle* cstxt;

  /// The current frame number.
  int cur_frame;
  /// The current action.
  csSpriteAction* cur_action;

  /// The last frame time action
  int last_time;

  ///
  bool force_otherskin;

  /**
   * List of light-hits-sprites for this sprite.
   */
  csLightHitsSprite* dynamiclights;

  /// Skeleton state (optional).
  csSkeletonState* skeleton_state;

  /// Defered lighting. If > 0 then we have defered lighting.
  int defered_num_lights;

  /// Flags to use for defered lighting.
  int defered_lighting_flags;

  /// The callback which is called just before drawing.
  csSpriteCallback* draw_callback;

  /**
   * Flag which is set to true when the sprite is visible.
   * This is used by the c-buffer/bsp routines. The sprite itself
   * will not use this flag in any way at all. It is simply intended
   * for external visibility culling routines.
   */
  bool is_visible;

  /**
   * High quality version of UpdateLighting() which recalculates
   * the distance between the light and every vertex.
   */
  void UpdateLightingHQ (csLight** lights, int num_lights, csVector3* object_vertices);

  /**
   * Low quality version of UpdateLighting() which only
   * calculates the distance once (with the center of the sprite).
   */
  void UpdateLightingLQ (csLight** lights, int num_lights, csVector3* object_vertices);

public:
  ///
  csSprite3D ();
  ///
  virtual ~csSprite3D ();

  ///
  void SetTemplate (csSpriteTemplate* tmpl);

  ///
  csSpriteTemplate* GetTemplate () { return tpl; }

  /// Get the skeleton state for this sprite.
  csSkeletonState* GetSkeletonState () { return skeleton_state; }

  /// force a new texture skin other than default
  void SetTexture (char * name, csTextureList* textures);

  /// Mark this sprite as visible.
  void MarkVisible () { is_visible = true; }

  /// Mark this sprite as invisible.
  void MarkInvisible () { is_visible = false; }

  /// Return if this sprite is visible.
  bool IsVisible () { return is_visible; }

  /**
   * Set a color for a vertex.
   * As soon as you use this function this sprite will be rendered
   * using gouraud shading. Calling this function for the first time
   * will initialize all colors to black.
   */
  void SetVertexColor (int i, const csColor& col);

  /**
   * Add a color for a vertex.
   * As soon as you use this function this sprite will be rendered
   * using gouraud shading. Calling this function for the first time
   * will initialize all colors to black.
   */
  void AddVertexColor (int i, const csColor& col);

  /**
   * Reset the color list. If you call this function then the
   * sprite will no longer use gouraud shading.
   */
  void ResetVertexColors ();

  /**
   * Clamp all vertice colors to 2.0. This is called inside
   * csSprite3D::UpdateLighting() so that 3D renderer doesn't have
   * to deal with brightness lighter than 2.0
   */
  void FixVertexColors ();

  /**
   * Light sprite according to the given array of lights (i.e.
   * fill the vertex color array).
   * No shadow calculation will be done. This is assumed to have
   * been done earlier. This is a primitive lighting process
   * based on the lights which hit one point of the sprite (usually
   * the center). More elaborate lighting systems are possible
   * but this will do for now.
   */
  void UpdateLighting (csLight** lights, int num_lights);

  /**
   * Update lighting as soon as the sprite becomes visible.
   * This will call world->GetNearestLights with the supplied
   * parameters.
   */
  void DeferUpdateLighting (int flags, int num_lights);

  ///
  void UnsetTexture ()
  { force_otherskin = false; }

  /// Sets the mode that is used, when drawing that sprite.
  void SetMixmode (UInt m) { MixMode = m; }

  /**
   * Set the transformation vector to move sprite to some position.
   */
  void SetMove (const csVector3& v) { SetMove (v.x, v.y, v.z); }

  /**
   * Set the transformation vector to move sprite to some position.
   */
  void SetMove (float x, float y, float z);

  /**
   * Set the transformation matrix to rotate the sprite in some
   * orientation
   */
  void SetTransform (const csMatrix3& matrix);

  /**
   * Relative move
   */
  void Move (float dx, float dy, float dz);

  /**
   * Relative move
   */
  void Move (csVector3& v) { Move (v.x, v.y, v.z); }

  /**
   * Absolute move
   */
  bool MoveTo (const csVector3& v);

  /**
   * The same as above
   */
  bool MoveTo (float x, float y, float z) { return MoveTo (csVector3 (x, y, z)); }

  /**
   * Relative transform.
   */
  void Transform (csMatrix3& matrix);

  /**
   * Calculate a bounding box for this sprite in world space and add
   * the resulting polygons to the given container.
   */
  void AddBoundingBox (csBspContainer* container);

  /**
   * Fill the static mesh with the current sprite
   * for a given LOD level.
   */
  void GenerateSpriteLOD (int num_vts);

  /**
   * Get a 3D bounding box in object space.
   */
  void GetObjectBoundingBox (csBox3& boundingBox);

  /**
   * Get a 3D bounding box in camera space. This function is smart.
   * It will only recompute this information if needed. So if you call
   * this function several times in the same frame it will not recompute
   * the bounding box.
   */
  void GetCameraBoundingBox (const csCamera& camtrans, csBox3& boundingBox);

  /**
   * Get the coordinates of the sprite in screen coordinates.
   * Fills in the boundingBox with the X and Y locations
   * of the sprite.  Returns the max Z location of the sprite,
   * or -1 if the sprite is not on-screen.
   * If the sprite is not on-screen, the X and Y values are
   * not valid.
   */
  float GetScreenBoundingBox (const csCamera& camtrans, csBox& boundingBox);

  /**
   * Draw this sprite given a camera transformation.
   * If needed the skeleton state will first be updated.
   * Optionally update lighting if needed (DeferUpdateLighting()).
   */
  void Draw (csRenderView& rview);

  /**
   * Set a callback which is called just before the sprite is drawn.
   * This is useful to do some expensive computations which only need
   * to be done on a visible sprite.
   */
  void SetDrawCallback (csSpriteCallback* callback) { draw_callback = callback; }

  /**
   * Get the draw callback. If there are multiple draw callbacks you can
   * use this function to chain.
   */
  csSpriteCallback* GetDrawCallback () { return draw_callback; }

  /**
   * Go to the next frame depending on the current time in milliseconds.
   */
  bool NextFrame (long current_time, bool onestep = false, bool stoptoend = false);

  /**
   * Go to a specified frame.
   */
  void SetFrame (int f)
  {
    if (cur_action && f < cur_action->GetNumFrames ()) cur_frame = f;
  }

  /**
   * Get the current frame number.
   */
  int GetCurFrame () { return cur_frame; }

  /**
   * Get the current frame number.
   */
  csSpriteAction* GetCurAction () { return cur_action; }

  /**
   * Get the number of frames.
   */
  int GetNumFrames () { return cur_action->GetNumFrames (); }

  /**
   * Select an action.
   */
  void SetAction (const char * name)
  {
    csSpriteAction *act;
    if ((act = tpl->FindAction (name)) != NULL)
    {
      SetFrame (0);
      cur_action = act;
    }
  }

  /**
   * Initialize a sprite. This function is called automatically
   * from within 'load'. However you should call it directly
   * if you created the sprite on the fly (without 'load').
   */
  void InitSprite ();

  /// Get world to local transformation matrix
  inline csMatrix3 GetW2T () const { return m_world2obj; }
  /// Get world to local translation
  inline csVector3 GetW2TTranslation () const { return -v_obj2world; }

  /// Move this sprite to one sector (conveniance function).
  void MoveToSector (csSector* s);

  /// Remove this sprite from all sectors it is in (but not from the world).
  void RemoveFromSectors ();

  /**
   * Get an array of object vertices which is valid for the given frame.
   * This function correcty acounts for sprites which use skeletons. In
   * that case it will use the current transformation state of the skeleton
   * to compute object space vertices.<br>
   * Warning! The returned array should be used immediatelly or copied. It
   * points to a private static array in the sprite class and can be reused
   * if other calls to the sprite happen.
   */
  csVector3* GetObjectVerts (csFrame* fr);

  /**
   * Unlink a light-hits-sprite from the list.
   * Warning! This function does not test if it
   * is really on the list!
   */
  void UnlinkDynamicLight (csLightHitsSprite* lp);

  /**
   * Add a light-hits-sprite to the list.
   */
  void AddDynamicLight (csLightHitsSprite *lp);

  /**
   * Get the list of dynamic lights that hit this sprite.
   */
  csLightHitsSprite* GetDynamicLights () { return dynamiclights; }

  CSOBJTYPE;
};

#endif /*CSSPRITE_H*/
