/*
    Copyright (C) 2010 by Mike Gist

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

#ifndef PARTICLESASSET_H__
#define PARTICLESASSET_H__

#include "assetbase.h"

struct iParticleBuiltinEffectorForce;
struct iParticleBuiltinEffectorLinColor;
struct iParticleBuiltinEffectorVelocityField;
struct iParticleBuiltinEffectorLinear;
struct iParticleBuiltinEmitterSphere;
struct iParticleBuiltinEmitterCone;
struct iParticleBuiltinEmitterBox;
struct iParticleBuiltinEmitterCylinder;
struct iParticleSystem;
struct iParticleSystemFactory;

class ParticlesAsset : public AssetBase
{
private:
  csRef<iParticleSystem> object;
  csRef<iParticleSystemFactory> factory;

  template<typename T>
  csPtr<iStringArray> GetCommonProps(T* emitter);
  csPtr<iStringArray> GetSphereProps(iParticleBuiltinEmitterSphere* emitter);
  csPtr<iStringArray> GetConeProps(iParticleBuiltinEmitterCone* emitter);
  csPtr<iStringArray> GetBoxProps(iParticleBuiltinEmitterBox* emitter);
  csPtr<iStringArray> GetCylinderProps(iParticleBuiltinEmitterCylinder* emitter);

  csPtr<iStringArray> GetForceProps(iParticleBuiltinEffectorForce* effector);
  csPtr<iStringArray> GetLinColorProps(iParticleBuiltinEffectorLinColor* effector);
  csPtr<iStringArray> GetVelFieldProps(iParticleBuiltinEffectorVelocityField* effector);
  csPtr<iStringArray> GetLinearProps(iParticleBuiltinEffectorLinear* effector);

public:
  static bool Support(iMeshWrapper* mesh);

  ParticlesAsset(iObjectRegistry* obj_reg, iMeshWrapper* mesh);
  virtual ~ParticlesAsset();

  // Animations
  virtual bool SupportsAnimations() { return false; }

  // Sockets
  virtual bool SupportsSockets() { return false; }

  // SubMeshes
  virtual bool SupportsSubMeshes() { return false; }

  // MorphTargets
  virtual bool SupportsMorphTargets() { return false; }

  // Particles
  virtual bool SupportsParticles() { return true; }

  virtual csPtr<iStringArray> GetEmitters();

  virtual csPtr<iStringArray> GetEffectors();

  virtual iParticleEmitter* AddEmitter(uint type);

  virtual bool DeleteEmitter(uint idx);

  virtual csPtr<iStringArray> GetEmitterProps(uint indx);

  virtual csPtr<iStringArray> GetEmitterProps(iParticleEmitter* emitter);

  virtual iParticleEffector* AddEffector(uint type);

  virtual bool DeleteEffector(uint idx);

  virtual csPtr<iStringArray> GetEffectorProps(uint indx);

  virtual csPtr<iStringArray> GetEffectorProps(iParticleEffector* effector);
};

#endif // PARTICLESASSET_H__
