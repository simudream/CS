/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein

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

#ifndef __WENTITY_H__
#define __WENTITY_H__

#include "csutil/dataobj.h"
#include "csgeom/vector3.h"
#include "csgeom/transfrm.h"
#include "csutil/cscolor.h"
#include "csutil/csvector.h"

struct iLight;
struct iMeshWrapper;
struct iMovable;
struct iPortal;

SCF_VERSION (csWalkEntity, 0, 0, 1);

/**
 * A general WalkTest entity.
 */
class csWalkEntity : public csObject
{
public:
  /// Activate this entity.
  virtual void Activate () = 0;

  /// Handle next frame.
  virtual void NextFrame (float elapsed_time) = 0;

  SCF_DECLARE_IBASE_EXT (csObject);

  virtual ~csWalkEntity ()
  {}
};

/**
 * An object controlling an animating portal.
 */
class csAnimatedPortal : public csWalkEntity
{
private:
  iPortal* portal;
  csVector3 center;
  csVector3 to;
  csReversibleTransform orig_trans;
  float cur_angle;
  float max_angle;
  float speed;
  int xyz;
  int cur_dir;

public:
  bool visible;

public:
  /// Create this object.
  csAnimatedPortal (iPortal* p,
	int xyz, float max_angle, float speed);

  /// Activate this entity.
  virtual void Activate ();

  /// Handle next frame.
  virtual void NextFrame (float elapsed_time);
};

#endif // __WENTITY_H__

