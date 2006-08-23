/*
    Copyright (C) 2003 by Jorrit Tyberghein, John Harger, Daniel Duhprey

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

#ifndef __CS_PARTICLES_H__
#define __CS_PARTICLES_H__

#include "cstool/objmodel.h"
#include "csgeom/transfrm.h"
#include "csgfx/shadervar.h"
#include "csgfx/shadervarcontext.h"
#include "csutil/array.h"
#include "csutil/cscolor.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/flags.h"
#include "csutil/leakguard.h"
#include "csutil/ref.h"
#include "csutil/scf_implementation.h"
#include "csutil/weakref.h"

#include "iengine/material.h"
#include "imesh/object.h"
#include "imesh/particles.h"
#include "iutil/comp.h"
#include "ivideo/graph3d.h"
#include "ivideo/rendermesh.h"

/**
 * Particles type, instantiates factories which create meshes
 */
class csParticlesType :
  public scfImplementation2<csParticlesType, iMeshObjectType, iComponent>
{
private:
  iObjectRegistry *object_reg;
  iBase* parent;

public:
  csParticlesType (iBase* p);
  virtual ~csParticlesType ();

  csPtr<iMeshObjectFactory> NewFactory();

  bool Initialize (iObjectRegistry* p)
  { this->object_reg = p; return true; }
};


/**
 * The factory only stores initial settings used for creating many
 * instances of the same particle system type
 */
class csParticlesFactory :
  public scfImplementation2<csParticlesFactory,
    iMeshObjectFactory, iParticlesFactoryState>
{
  friend class csParticlesObject;
private:
  iMeshFactoryWrapper* logparent;
  csParticlesType* particles_type;
  iObjectRegistry *object_reg;

  csWeakRef<iGraphics3D> g3d;
  csWeakRef<iShaderManager> shmgr;
  csRef<iMaterialWrapper> material;

  csParticleEmitType emit_type;
  float emit_size_1;
  float emit_size_2;
  float emit_size_3;

  csParticleForceType force_type;

  csVector3 force_direction;
  csVector3 force_direction_variation;
  float force_range;
  csParticleFalloffType force_falloff;
  float force_cone_radius;
  csParticleFalloffType force_cone_radius_falloff;

  float force_amount;
  float particle_mass;
  float mass_variation;
  float dampener;

  bool autostart;
  bool transform_mode;

  int particles_per_second;
  int initial_particles;

  csVector3 gravity;

  float emit_time;
  float time_to_live;
  float time_variation;

  float diffusion;

  float particle_radius;

  csString physics_plugin;
  bool physics_zsort;

  csArray<csColor4> gradient_colors;

  float loop_time;
  float base_heat;
  csColor4 constant_color;
  csParticleColorMethod color_method;
  csRef<iParticlesColorCallback> color_callback;
  csFlags flags;

  uint mixmode;
  bool zsort_enabled;

public:
  CS_LEAKGUARD_DECLARE (csParticlesFactory);

  csParticlesFactory (csParticlesType* p, iObjectRegistry* objreg);
  virtual ~csParticlesFactory ();

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> NewInstance ();
  virtual csPtr<iMeshObjectFactory> Clone () { return 0; }
  virtual void HardTransform (const csReversibleTransform&) {}
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetMeshFactoryWrapper (iMeshFactoryWrapper* lp)
  {
    logparent = lp;
  }
  virtual iMeshFactoryWrapper* GetMeshFactoryWrapper () const
  {
    return logparent;
  }
  virtual iMeshObjectType* GetMeshObjectType () const { return particles_type; }
  virtual iObjectModel* GetObjectModel () { return 0; }

  void SetMaterial (iMaterialWrapper *mat)
  { material = mat; }
  void SetParticlesPerSecond (int count)
  { particles_per_second = count; }
  void SetInitialParticleCount (int count)
  { initial_particles = count; }
  void SetPointEmitType ()
  {
    emit_type = CS_PART_EMIT_SPHERE;
    emit_size_1 = 0.0015f;
    emit_size_2 = 0.001f;
  }
  void SetSphereEmitType (float outer_radius, float inner_radius)
  {
    emit_type = CS_PART_EMIT_SPHERE;
    emit_size_1 = outer_radius;
    emit_size_2 = inner_radius;
  }
  void SetPlaneEmitType (float x_size, float y_size)
  {
    emit_type = CS_PART_EMIT_PLANE;
    emit_size_1 = x_size;
    emit_size_2 = y_size;
  }
  void SetBoxEmitType (float x_size, float y_size, float z_size)
  {
    emit_type = CS_PART_EMIT_BOX;
    emit_size_1 = x_size;
    emit_size_2 = y_size;
    emit_size_3 = z_size;
  }
  void SetCylinderEmitType (float radius, float height)
  {
    emit_type = CS_PART_EMIT_CYLINDER;
    emit_size_1 = radius;
    emit_size_2 = height;
  }
  void SetRadialForceType (float range, csParticleFalloffType falloff)
  {
    force_type = CS_PART_FORCE_RADIAL;
    force_range = range;
    force_falloff = falloff;
  }
  void SetLinearForceType (const csVector3 &direction,
  	const csVector3& direction_variation, float range,
	csParticleFalloffType falloff)
  {
    force_type = CS_PART_FORCE_LINEAR;
    force_direction = direction;
    force_direction_variation = direction_variation;
    force_range = range;
    force_falloff = falloff;
  }
  void SetConeForceType (const csVector3 &direction,
  	const csVector3& direction_variation, float range,
	csParticleFalloffType falloff, float radius,
	csParticleFalloffType radius_falloff)
  {
    force_type = CS_PART_FORCE_CONE;
    force_direction = direction;
    force_direction_variation = direction_variation;
    force_range = range;
    force_falloff = falloff;
    force_cone_radius = radius;
    force_cone_radius_falloff = radius_falloff;
  }
  void SetForce (float force)
  { force_amount = force; }
  void SetDiffusion (float size)
  { diffusion = size; }
  void SetGravity (const csVector3 &gravity)
  { csParticlesFactory::gravity = gravity; }
  void SetEmitTime (float time)
  { emit_time = time; }
  void SetTimeToLive (float time)
  { time_to_live = time; }
  void SetTimeVariation (float variation)
  { time_variation = variation; }

  void AddColor (csColor4 color)
  {
    gradient_colors.Push(color);
  }
  void ClearColors ()
  { gradient_colors.DeleteAll (); }
  void SetConstantColorMethod (csColor4 color)
  {
    color_method = CS_PART_COLOR_CONSTANT;
    constant_color = color;
  }
  void SetLinearColorMethod ()
  { color_method = CS_PART_COLOR_LINEAR; }
  void SetLoopingColorMethod (float seconds)
  {
    color_method = CS_PART_COLOR_LOOPING;
    loop_time = seconds;
  }
  void SetHeatColorMethod (int base_temp)
  {
    color_method = CS_PART_COLOR_HEAT;
    base_heat = base_temp;
  }
  void SetColorCallback (iParticlesColorCallback* callback)
  {
    CS_ASSERT(callback != 0);
    color_method = CS_PART_COLOR_CALLBACK;
    color_callback = callback;
  }
  iParticlesColorCallback* GetColorCallback ()
  {
    return color_callback;
  }
  void SetParticleRadius (float rad)
  { particle_radius = rad; }
  int GetParticlesPerSecond ()
  { return particles_per_second; }
  int GetInitialParticleCount ()
  { return initial_particles; }
  csParticleEmitType GetEmitType ()
  { return emit_type; }
  float GetEmitSize1 ()
  { return emit_size_1; }
  float GetEmitSize2 ()
  { return emit_size_2; }
  float GetEmitSize3 ()
  { return emit_size_3; }
  csParticleForceType GetForceType ()
  { return force_type; }
  void GetFalloffType(csParticleFalloffType &force,
    csParticleFalloffType &cone)
  {
    force = force_falloff;
    cone = force_cone_radius_falloff;
  }
  float GetForceRange ()
  { return force_range; }
  void GetForceDirection (csVector3 &dir)
  { dir = force_direction; }
  void GetForceDirectionVariation (csVector3 &dirvar)
  { dirvar = force_direction_variation; }
  float GetForceConeRadius ()
  { return force_cone_radius; }
  float GetForce ()
  { return force_amount; }
  float GetDiffusion ()
  { return diffusion; }
  void GetGravity (csVector3 &gravity)
  { gravity = csParticlesFactory::gravity; }
  float GetEmitTime ()
  { return emit_time; }
  float GetTimeToLive ()
  { return time_to_live; }
  float GetTimeVariation ()
  { return time_variation; }
  float GetParticleRadius ()
  { return particle_radius; }
  csParticleColorMethod GetParticleColorMethod ()
  { return color_method; }
  void GetConstantColor (csColor4& color)
  { color = constant_color; }
  float GetColorLoopTime ()
  { return loop_time; }
  float GetBaseHeat ()
  { return base_heat; }
  const csArray<csColor4> &GetGradient ()
  { return gradient_colors; }
  void SetDampener (float damp)
  { dampener = damp; }
  float GetDampener ()
  { return dampener; }
  void SetMass(float mass)
  { particle_mass = mass; }
  void SetMassVariation (float variation)
  { mass_variation = variation; }
  float GetMass()
  { return particle_mass; }
  float GetMassVariation ()
  { return mass_variation; }
  void SetAutoStart (bool a)
  { autostart = a; }
  void SetTransformMode (bool transform)
  { transform_mode = transform; }
  bool GetTransformMode ()
  { return transform_mode; }
  void SetPhysicsPlugin (const char *plugin)
  { physics_plugin = plugin; }
  virtual void SetMixMode (uint mode)
  { mixmode = mode; }
  virtual uint GetMixMode () const
  { return mixmode; }
  void EnableZSort (bool en) { zsort_enabled = en; }
  bool IsZSortEnabled () const { return zsort_enabled; }
  virtual bool SetMaterialWrapper (iMaterialWrapper* mat)
  {
    SetMaterial (mat);
    return true;
  }
  virtual iMaterialWrapper* GetMaterialWrapper () const { return material; }

  virtual float GetSphereEmitInnerRadius ()
  { return this->GetEmitSize2 (); }
  virtual float GetSphereEmitOuterRadius ()
  { return this->GetEmitSize1 (); }
  virtual float GetEmitXSize ()
  { return this->GetEmitSize1 (); }
  virtual float GetEmitYSize ()
  { return this->GetEmitSize2 (); }
  virtual float GetEmitZSize ()
  { return this->GetEmitSize3 (); }
};


/**
 * Particles object instance
 */
class csParticlesObject :
  public scfImplementationExt3<csParticlesObject, csObjectModel,
    iMeshObject, iParticlesObjectState, iRenderBufferAccessor>
{
private:
  iMeshWrapper* logparent;
  csParticlesFactory* pFactory;
  iMeshObjectDrawCallback* vis_cb;
  csRef<csShaderVariableContext> svcontext;
  csRef<csRenderBufferHolder> bufferHolder;
  csRef<iParticlesPhysics> physics;

  csRef<iMaterialWrapper> matwrap;
  csRenderMesh *mesh;
  csRenderMesh **meshpp;
  int meshppsize;

  csReversibleTransform tr_o2c;
  csMatrix3 rotation_matrix;
  int tricount;

  csStringID vertex_name;
  csStringID color_name;
  csStringID texcoord_name;
  csStringID index_name;
  csStringID radius_name;
  csStringID scale_name;

  int camera_fov;
  int camera_pixels;

  csColor basecolor;

  csParticleEmitType emit_type;
  float emit_size_1;
  float emit_size_2;
  float emit_size_3;

  csParticleForceType force_type;

  csVector3 force_direction;
  csVector3 force_direction_variation;
  float force_range;
  csParticleFalloffType force_falloff;
  float force_cone_radius;
  csParticleFalloffType force_cone_radius_falloff;

  float force_amount;

  int particles_per_second;
  int initial_particles;

  csVector3 gravity;

  float emit_time;
  float time_to_live;
  float time_variation;

  float particle_mass;
  float mass_variation;
  float dampener;

  bool autostart;
  bool running;
  bool transform_mode;

  float diffusion;

  float particle_radius;
  bool radius_changed;

  csArray<csColor4> gradient_colors;
  float loop_time;
  float base_heat;
  csColor4 constant_color;
  csParticleColorMethod color_method;
  csRef<iParticlesColorCallback> color_callback;

  const csArray<csParticlesData> *point_data;
  struct i_vertex
  {
    csVector3 position;
    csVector4 color;
  };
  csDirtyAccessArray<i_vertex> vertex_data;

  size_t buffer_length;

  bool point_sprites;

  csRef<iRenderBuffer> masterBuffer;
  csRef<iRenderBuffer> vertex_buffer;
  csRef<iRenderBuffer> color_buffer;
  csRef<iRenderBuffer> texcoord_buffer;
  csRef<iRenderBuffer> index_buffer;

  csVector3 corners[4];

  csRandomGen rng;
  csVector3 emitter;
  float radius;
  csBox3 obj_bbox;

  csFlags flags;

  uint mixmode;
  bool zsort_enabled;

public:
  CS_LEAKGUARD_DECLARE (csParticlesObject);

  csParticlesObject (csParticlesFactory* f);
  virtual ~csParticlesObject ();

  /// Returns a point to the factory that made this
  iMeshObjectFactory* GetFactory () const { return pFactory; }

  virtual csFlags& GetFlags () { return flags; }
  virtual csPtr<iMeshObject> Clone ();

  /// Updates the lighting
  void UpdateLighting (iLight** lights, int num_lights, iMovable* movable);

  /// Returns the mesh, ready for rendering
  csRenderMesh** GetRenderMeshes (int& n, iRenderView* rview,
    iMovable* movable, uint32 frustum_mask);

  void SetVisibleCallback (iMeshObjectDrawCallback* cb) { vis_cb = cb; }
  iMeshObjectDrawCallback* GetVisibleCallback () const { return vis_cb; }

  /// For creating the quads when necessary
  void NextFrame (csTicks ticks, const csVector3&, uint);

  /// Unsupported
  void HardTransform (const csReversibleTransform&) {}

  /// Shows that HardTransform is not supported by this mesh
  bool SupportsHardTransform () const { return false; }

  /// Check if hit by the beam
  bool HitBeamOutline (const csVector3& start, const csVector3& end,
	csVector3& isect, float* pr);
  /// Find exact position of a beam hit
  bool HitBeamObject (const csVector3& start, const csVector3& end,
	csVector3& isect, float* pr, int* polygon_idx = 0,
	iMaterialWrapper** material = 0);

  /// Set/Get logical parent
  void SetMeshWrapper (iMeshWrapper* lp) { logparent = lp; }
  iMeshWrapper* GetMeshWrapper () const { return logparent; }

  /// Gets the object model
  iObjectModel *GetObjectModel () { return this; }

  /// Set the constant base color
  bool SetColor (const csColor& c) { basecolor = c; return true; }
  /// Get the constant base color
  bool GetColor (csColor &c) const { c = basecolor; return true; }

  iRenderBuffer *GetRenderBuffer (csRenderBufferName name);

  /// Set the material wrapper
  bool SetMaterialWrapper (iMaterialWrapper* m)
  { matwrap = m; return true; }
  /// Get the material wrapper
  iMaterialWrapper* GetMaterialWrapper () const { return matwrap; }
  void InvalidateMaterialHandles () {}

  void GetObjectBoundingBox (csBox3& bbox);
  const csBox3& GetObjectBoundingBox ();
  void SetObjectBoundingBox (const csBox3& bbox);
  void GetRadius (float& rad, csVector3& c);
  iTerraFormer* GetTerraFormerColldet () { return 0; }

  bool LoadPhysicsPlugin (const char *plugin_id);

  void SetParticlesPerSecond (int count)
  { particles_per_second = count; }
  void SetInitialParticleCount (int count)
  { initial_particles = count; }
  void SetPointEmitType ()
  {
    emit_type = CS_PART_EMIT_SPHERE;
    emit_size_1 = 0.0015f;
    emit_size_2 = 0.001f;
  }
  void SetSphereEmitType (float outer_radius, float inner_radius)
  {
    emit_type = CS_PART_EMIT_SPHERE;
    emit_size_1 = outer_radius;
    emit_size_2 = inner_radius;
  }
  void SetPlaneEmitType (float x_size, float y_size)
  {
    emit_type = CS_PART_EMIT_PLANE;
    emit_size_1 = x_size;
    emit_size_2 = y_size;
  }
  void SetBoxEmitType (float x_size, float y_size, float z_size)
  {
    emit_type = CS_PART_EMIT_BOX;
    emit_size_1 = x_size;
    emit_size_2 = y_size;
    emit_size_3 = z_size;
  }
  void SetCylinderEmitType (float radius, float height)
  {
    emit_type = CS_PART_EMIT_CYLINDER;
    emit_size_1 = radius;
    emit_size_2 = height;
  }
  void SetRadialForceType (float range, csParticleFalloffType falloff)
  {
    force_type = CS_PART_FORCE_RADIAL;
    force_range = range;
    force_falloff = falloff;
  }
  void SetLinearForceType (const csVector3 &direction,
  	const csVector3& direction_variation, float range,
	csParticleFalloffType falloff)
  {
    force_type = CS_PART_FORCE_LINEAR;
    force_direction = direction;
    force_direction_variation = direction_variation;
    force_range = range;
    force_falloff = falloff;
  }
  void SetConeForceType (const csVector3 &direction,
  	const csVector3& direction_variation, float range,
	csParticleFalloffType falloff, float radius,
	csParticleFalloffType radius_falloff)
  {
    force_type = CS_PART_FORCE_CONE;
    force_direction = direction;
    force_direction_variation = direction_variation;
    force_range = range;
    force_falloff = falloff;
    force_cone_radius = radius;
    force_cone_radius_falloff = radius_falloff;
  }
  void SetForce (float force)
  { force_amount = force; }
  void SetDiffusion (float size)
  { diffusion = size; }
  void SetGravity (const csVector3 &gravity)
  { csParticlesObject::gravity = gravity; }
  void SetEmitTime (float time)
  { emit_time = time; }
  void SetTimeToLive (float time)
  { time_to_live = time; }
  void SetTimeVariation (float variation)
  { time_variation = variation; }
  void AddColor (csColor4 color)
  {
    gradient_colors.Push(color);
  }
  void ClearColors ()
  {
    gradient_colors.DeleteAll ();
  }
  void SetConstantColorMethod (csColor4 color)
  {
    color_method = CS_PART_COLOR_CONSTANT;
    constant_color = color;
  }
  void SetLinearColorMethod ()
  { color_method = CS_PART_COLOR_LINEAR; }
  void SetLoopingColorMethod (float seconds)
  {
    color_method = CS_PART_COLOR_LOOPING;
    loop_time = seconds;
  }
  void SetHeatColorMethod (int base_temp)
  {
    color_method = CS_PART_COLOR_HEAT;
    base_heat = base_temp;
  }
  void SetColorCallback (iParticlesColorCallback* callback)
  {
    CS_ASSERT(callback != 0);
    color_method = CS_PART_COLOR_CALLBACK;
    color_callback = callback;
  }
  iParticlesColorCallback* GetColorCallback ()
  {
    return color_callback;
  }

  void SetParticleRadius (float rad);

  int GetParticlesPerSecond ()
  { return particles_per_second; }
  int GetInitialParticleCount ()
  { return initial_particles; }
  void GetEmitPosition (csVector3 &position)
  { position = emitter; }
  csParticleEmitType GetEmitType ()
  { return emit_type; }
  float GetEmitSize1 ()
  { return emit_size_1; }
  float GetEmitSize2 ()
  { return emit_size_2; }
  float GetEmitSize3 ()
  { return emit_size_3; }
  csParticleForceType GetForceType ()
  { return force_type; }
  void GetFalloffType(csParticleFalloffType &force,
    csParticleFalloffType &cone)
  {
    force = force_falloff;
    cone = force_cone_radius_falloff;
  }
  float GetForceRange ()
  { return force_range; }
  void GetForceDirection (csVector3 &dir)
  { dir = force_direction; }
  void GetForceDirectionVariation (csVector3 &dirvar)
  { dirvar = force_direction_variation; }
  float GetForceConeRadius ()
  { return force_cone_radius; }
  float GetForce ()
  { return force_amount; }
  float GetDiffusion ()
  { return diffusion; }
  void GetGravity (csVector3 &gravity)
  { gravity = csParticlesObject::gravity; }
  float GetEmitTime ()
  { return emit_time; }
  float GetTimeToLive ()
  { return time_to_live; }
  float GetTimeVariation ()
  { return time_variation; }
  float GetParticleRadius ()
  { return particle_radius; }
  void SetDampener (float damp)
  { dampener = damp; }
  float GetDampener ()
  { return dampener; }
  void SetMass(float mass)
  { particle_mass = mass; }
  void SetMassVariation (float variation)
  { mass_variation = variation; }
  float GetMassVariation ()
  { return mass_variation; }
  float GetMass ()
  { return particle_mass; }
  csParticleColorMethod GetParticleColorMethod ()
  { return color_method; }
  void GetConstantColor (csColor4& color)
  { color = constant_color; }
  float GetColorLoopTime ()
  { return loop_time; }
  float GetBaseHeat ()
  { return base_heat; }
  const csArray<csColor4> &GetGradient ()
  { return gradient_colors; }
  void SetTransformMode (bool transform)
  { transform_mode = transform; }
  bool GetTransformMode ()
  { return transform_mode; }
  csReversibleTransform GetCameraTranform ()
  { return tr_o2c; }
  const csMatrix3 &GetRotation ()
  { return rotation_matrix; }

  void Start ();
  void Stop ();
  bool IsRunning ()
  { return running; }
  void SetMixMode (uint mode)
  { mixmode = mode; }
  uint GetMixMode () const
  { return mixmode; }
  void EnableZSort (bool en);
  bool IsZSortEnabled () const { return zsort_enabled; }

  virtual void PositionChild (iMeshObject* /*child*/, csTicks /*current_time*/) {}

  virtual float GetSphereEmitInnerRadius ()
  { return this->GetEmitSize2 (); }
  virtual float GetSphereEmitOuterRadius ()
  { return this->GetEmitSize1 (); }
  virtual float GetEmitXSize ()
  { return this->GetEmitSize1 (); }
  virtual float GetEmitYSize ()
  { return this->GetEmitSize2 (); }
  virtual float GetEmitZSize ()
  { return this->GetEmitSize3 (); }
  virtual csReversibleTransform GetObjectToCamera ()
  { return this->GetCameraTranform (); }
  virtual void ChangePhysicsPlugin (const char *plugin)
  { this->LoadPhysicsPlugin (plugin); }

  void PreGetBuffer (csRenderBufferHolder* holder, csRenderBufferName buffer);
};

#endif // __CS_PARTICLES_H__
