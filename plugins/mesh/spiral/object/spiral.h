/*
    Copyright (C) 2000 by W.C.A. Wijngaards

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

#ifndef __CS_SPIRAL_H__
#define __CS_SPIRAL_H__

#include "csgeom/vector3.h"
#include "csgeom/box.h"
#include "csutil/cscolor.h"
#include "plugins/mesh/partgen/partgen.h"
#include "imesh/spiral.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"

/**
 * This class has a set of particles that act like a spiraling
 * particle fountain.
 */
class csSpiralMeshObject : public csNewtonianParticleSystem
{
protected:
  int max;
  int time_before_new_particle; // needs to be signed.
  csVector3 source;
  int last_reuse;

  void SetupObject ();

public:
  /// Constructor.
  csSpiralMeshObject (iObjectRegistry* object_reg, iMeshObjectFactory* factory);
  /// Destructor.
  virtual ~csSpiralMeshObject ();

  /// Set the number of particles to use.
  void SetParticleCount (int num) { initialized = false; max = num; }
  /// Get the number of particles.
  int GetParticleCount () const { return max; }

  /// Set the source.
  void SetSource (const csVector3& source)
  {
    initialized = false;
    csSpiralMeshObject::source = source;
  }
  /// Get the source.
  const csVector3& GetSource () const { return source; }

  /// Update the particle system.
  virtual void Update (csTicks elapsed_time);

  /// For iMeshObject.
  virtual void HardTransform (const csReversibleTransform& t);
  virtual bool SupportsHardTransform () const { return true; }

  SCF_DECLARE_IBASE_EXT (csParticleSystem);

  //------------------------- iSpiralState implementation ----------------
  class SpiralState : public iSpiralState
  {
    SCF_DECLARE_EMBEDDED_IBASE (csSpiralMeshObject);
    virtual void SetParticleCount (int num)
    {
      scfParent->SetParticleCount (num);
    }
    virtual void SetSource (const csVector3& source)
    {
      scfParent->SetSource (source);
    }
    virtual int GetParticleCount () const
    {
      return scfParent->GetParticleCount();
    }
    virtual const csVector3& GetSource () const
    {
      return scfParent->GetSource();
    }
  } scfiSpiralState;
  friend class SpiralState;
};

/**
 * Factory for spiral.
 */
class csSpiralMeshObjectFactory : public iMeshObjectFactory
{
private:
  iObjectRegistry* object_reg;
  iBase* logparent;

public:
  /// Constructor.
  csSpiralMeshObjectFactory (iBase *pParent, iObjectRegistry* object_reg);

  /// Destructor.
  virtual ~csSpiralMeshObjectFactory ();

  //------------------------ iMeshObjectFactory implementation --------------
  SCF_DECLARE_IBASE;

  virtual csPtr<iMeshObject> NewInstance ();
  virtual void HardTransform (const csReversibleTransform&) { }
  virtual bool SupportsHardTransform () const { return false; }
  virtual void SetLogicalParent (iBase* lp) { logparent = lp; }
  virtual iBase* GetLogicalParent () const { return logparent; }
};

/**
 * Spiral type. This is the plugin you have to use to create instances
 * of csSpiralMeshObjectFactory.
 */
class csSpiralMeshObjectType : public iMeshObjectType
{
private:
  iObjectRegistry* object_reg;

public:
  SCF_DECLARE_IBASE;

  /// Constructor.
  csSpiralMeshObjectType (iBase*);

  /// Destructor.
  virtual ~csSpiralMeshObjectType ();

  virtual csPtr<iMeshObjectFactory> NewFactory ();

  struct eiComponent : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE(csSpiralMeshObjectType);
    virtual bool Initialize(iObjectRegistry* p)
    { scfParent->object_reg = p; return true; }
  } scfiComponent;
  friend struct eiComponent;
};

#endif // __CS_SPIRAL_H__
