/*
    Copyright (C) 2000 by Jorrit Tyberghein
  
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

#ifndef MOVABLE_H
#define MOVABLE_H

#include "csgeom/transfrm.h"
#include "csutil/csvector.h"
#include "imovable.h"

class csSector;

/**
 * This class represents an entity that can move in the world.
 * It has a list of sectors and a position (a list of sectors
 * because an entity can overlap several sectors at the same time
 * through portals). This class itself does not have geometry.
 * It is only responsible for managing movement.
 */
class csMovable : public iBase
{
private:
  /// World to object transformation.
  csReversibleTransform obj;
  /// List of sectors.
  csVector sectors;

public:
  /**
   * Create a default movable.
   */
  csMovable ();

  /// Destructor.
  virtual ~csMovable ();

  /**
   * Initialize the list of sectors to one sector where
   * this thing is. This is a conveniance funcion.
   * This function does not update the corresponding list in
   * the sector. It only does a local update here.
   */
  void SetSector (csSector* sector)
  {
    sectors.SetLength (0);
    sectors.Push (sector);
  }

  /**
   * Clear the list of sectors.
   */
  void ClearSectors ()
  {
    sectors.SetLength (0);
  }

  /**
   * Add a sector to the list of sectors.
   */
  void AddSector (csSector* sector)
  {
    sectors.Push (sector);
  }

  /**
   * Get list of sectors for this entity.
   */
  csVector& GetSectors ()
  {
    return sectors;
  }

  /**
   * Get the specified sector where this entity lives.
   * (conveniance function).
   */
  csSector* GetSector (int idx) { return (csSector*)GetSectors ()[idx]; }

  /**
   * Return true if we are placed in a sector.
   */
  bool InSector ()
  {
    return sectors.Length () > 0;
  }

  /**
   * Set the transformation vector and sector to move to
   * some position.
   */
  void SetPosition (csSector* home, const csVector3& v);

  /**
   * Get the current position.
   */
  const csVector3& GetPosition () { return obj.GetOrigin (); }

  /**
   * Set the transformation matrix for this entity.
   */
  void SetTransform (const csMatrix3& matrix);

  /**
   * Set the world to object tranformation.
   */
  void SetTransform (const csReversibleTransform& t) { obj = t; }

  /**
   * Get the world to object tranformation.
   */
  csReversibleTransform& GetTransform () { return obj; }

  /**
   * Relative move.
   */
  void MovePosition (const csVector3& v);

  /**
   * Relative transform.
   */
  void Transform (csMatrix3& matrix);

  DECLARE_IBASE;

  //------------------------- iMovable interface -------------------------------
  struct eiMovable : public iMovable
  {
    DECLARE_EMBEDDED_IBASE (csMovable);
    virtual void SetSector (iSector* sector);
    virtual void ClearSectors ()
    {
      scfParent->ClearSectors ();
    }
    virtual void AddSector (iSector* sector);
    virtual csVector& GetSectors ()
    {
      return scfParent->GetSectors ();
    }
    virtual iSector* GetSector (int idx);
    virtual bool InSector ()
    {
      return scfParent->InSector ();
    }
    virtual void SetPosition (iSector* home, const csVector3& v);
    virtual const csVector3& GetPosition ()
    {
      return scfParent->GetPosition ();
    }
    virtual void SetTransform (const csMatrix3& matrix)
    {
      scfParent->SetTransform (matrix);
    }
    virtual void SetTransform (const csReversibleTransform& t)
    {
      scfParent->SetTransform (t);
    }
    virtual csReversibleTransform& GetTransform ()
    {
      return scfParent->GetTransform ();
    }
    virtual void MovePosition (const csVector3& v)
    {
      scfParent->MovePosition (v);
    }
    virtual void Transform (csMatrix3& matrix)
    {
      scfParent->Transform (matrix);
    }
  } scfiMovable;
};

#endif /*MOVABLE_H*/
