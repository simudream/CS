/*
    Copyright (C) 1998,1999,2000 by Jorrit Tyberghein
    Largely rewritten by Ivan Avramovic <ivan@avramovic.com>
  
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

#ifndef __CS_BOX_H
#define __CS_BOX_H

#include "cstypes.h"	// for bool
#include "vector2.h"
#include "vector3.h"
#include "segment.h"

class csPlane3;

/**
 * The maximum value that a coordinate in the bounding box can use.
 * This is considered the 'infinity' value used for empty bounding boxes.
 */
#define CS_BOUNDINGBOX_MAXVALUE 1000000000.

/// For csBox2::GetCorner().
#define BOX_CORNER_xy 0
#define BOX_CORNER_xY 1
#define BOX_CORNER_Xy 2
#define BOX_CORNER_XY 3

/**
 * A bounding box in 2D space.
 * In order to operate correctly, this bounding box assumes that all values
 * entered or compared against lie within the range 
 * (-CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE).  It is not
 * recommended to use points outside of this range.
 */
class csBox2
{
protected:
  /// The top-left coordinate of the bounding box.
  csVector2 minbox;
  /// The bottom-right coordinate of the bounding box.
  csVector2 maxbox;

public:
  ///
  float MinX () const { return minbox.x; }
  ///
  float MinY () const { return minbox.y; }
  ///
  float MaxX () const { return maxbox.x; }
  ///
  float MaxY () const { return maxbox.y; }
  /// Get Min component for 0 (x) or 1 (y).
  float Min (int idx) const { return idx ? minbox.y : minbox.x; }
  /// Get Max component for 0 (x) or 1 (y).
  float Max (int idx) const { return idx ? maxbox.y : maxbox.x; }
  ///
  const csVector2& Min () const { return minbox; }
  ///
  const csVector2& Max () const { return maxbox; }

  /**
   * Return every corner of this bounding box from 0
   * to 3. This contrasts with Min() and Max() because
   * those are only the min and max corners.
   * Corner 0 = xy, 1 = xY, 2 = Xy, 3 = XY.
   * Use BOX_CORNER_??? defines.
   */
  csVector2 GetCorner (int corner) const;

  /**
   * Get the center of this box.
   */
  csVector2 GetCenter () const { return (minbox+maxbox)/2; }

  /**
   * Return every edge (segment) of this bounding box
   * from 0 to 3. The returned edge is undefined for any
   * other index.
   */
  csSegment2 GetEdge (int edge) const;

  /**
   * Test if a polygon if visible in the box. This
   * function does not test the case where the box is
   * fully contained in the polygon. But all other
   * cases are tested.
   */
  static bool Intersect (float minx, float miny, float maxx, float maxy,
    csVector2* poly, int num_poly);

  /**
   * Test if a polygon if visible in the box. This
   * function does not test the case where the box is
   * fully contained in the polygon. But all other
   * cases are tested.
   */
  static bool Intersect (const csVector2& minbox, const csVector2& maxbox,
    csVector2* poly, int num_poly)
  {
    return Intersect (minbox.x, minbox.y, maxbox.x, maxbox.y, poly, num_poly);
  }

  /**
   * Test if a polygon if visible in the box. This
   * function does not test the case where the box is
   * fully contained in the polygon. But all other
   * cases are tested.
   */
  bool Intersect (csVector2* poly, int num_poly)
  {
    return Intersect (minbox, maxbox, poly, num_poly);
  }

  /// Test if the given coordinate is in this box.
  bool In (float x, float y) const
  {
    if (x < minbox.x || x > maxbox.x) return false;
    if (y < minbox.y || y > maxbox.y) return false;
    return true;
  }

  /// Test if the given coordinate is in this box.
  bool In (const csVector2& v) const
  {
    return In (v.x, v.y);
  }

  /// Test if this box overlaps with the given box.
  bool Overlap (const csBox2& box) const
  {
    if (maxbox.x < box.minbox.x || minbox.x > box.maxbox.x) return false;
    if (maxbox.y < box.minbox.y || minbox.y > box.maxbox.y) return false;
    return true;
  }

  /// Test if this box is empty.
  bool Empty () const
  {
    if (minbox.x > maxbox.x) return true;
    if (minbox.y > maxbox.y) return true;
    return false;
  }

  /// Initialize this box to empty.
  void StartBoundingBox ()
  {
    minbox.x =  CS_BOUNDINGBOX_MAXVALUE;  minbox.y =  CS_BOUNDINGBOX_MAXVALUE;
    maxbox.x = -CS_BOUNDINGBOX_MAXVALUE;  maxbox.y = -CS_BOUNDINGBOX_MAXVALUE;
  }

  /// Initialize this box to one vertex.
  void StartBoundingBox (const csVector2& v)
  {
    minbox = v;
    maxbox = v;
  }

  /// Same but given some coordinates.
  void StartBoundingBox (float x, float y)
  {
    minbox.x = maxbox.x = x;
    minbox.y = maxbox.y = y;
  }

  /// Add a new vertex and recalculate the bounding box.
  void AddBoundingVertex (float x, float y)
  {
    if (x < minbox.x) minbox.x = x;  if (x > maxbox.x) maxbox.x = x;
    if (y < minbox.y) minbox.y = y;  if (y > maxbox.y) maxbox.y = y;
  }

  /// Add a new vertex and recalculate the bounding box.
  void AddBoundingVertex (const csVector2& v)
  {
    AddBoundingVertex (v.x, v.y);
  }

  /**
   * Add a new vertex and recalculate the bounding box.
   * This version is a little more optimal. It assumes however
   * that at least one point has been added to the bounding box.
   */
  void AddBoundingVertexSmart (float x, float y)
  {
    if (x < minbox.x) minbox.x = x; else if (x > maxbox.x) maxbox.x = x;
    if (y < minbox.y) minbox.y = y; else if (y > maxbox.y) maxbox.y = y;
  }

  /**
   * Add a new vertex and recalculate the bounding box.
   * This version is a little more optimal. It assumes however
   * that at least one point has been added to the bounding box.
   */
  void AddBoundingVertexSmart (const csVector2& v)
  {
    AddBoundingVertexSmart (v.x, v.y);
  }

  //-----
  // Maintenance Note: The csBox2 constructors and Set() appear at this point
  // in the file, rather than earlier, in order to appease the OpenStep 4.2
  // compiler.  Specifically, the problem is that the compiler botches code
  // generation if an unseen method (which is later declared inline) is
  // called from within another inline method.  For instance, if the
  // constructors were listed at the top of the file, rather than here, the
  // compiler would see calls to Empty() and StartBoundingBox() before seeing
  // declarations for them.  In such a situation, the buggy compiler
  // generates a broken object file.  The simple work-around of textually
  // reorganizing the file ensures that the declarations for Empty() and
  // StartBoundingBox() are seen before they are called.
  //-----

  /// Initialize this box to empty.
  csBox2 () : minbox (CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE),
	     maxbox (-CS_BOUNDINGBOX_MAXVALUE, -CS_BOUNDINGBOX_MAXVALUE) {}

  /// Initialize this box with one point.
  csBox2 (const csVector2& v) : minbox (v.x, v.y), maxbox (v.x, v.y) {}

  /// Initialize this box with the given values.
  csBox2 (float x1, float y1, float x2, float y2) :
    minbox (x1, y1), maxbox (x2, y2)
  { if (Empty ()) StartBoundingBox (); }

  /// Sets the bounds of the box with the given values.
  void Set (const csVector2& bmin, const csVector2& bmax)
  {
    minbox = bmin;
    maxbox = bmax;
  }

  /// Sets the bounds of the box with the given values.
  void Set (float x1, float y1, float x2, float y2)
  {
    if (x1>x2 || y1>y2) StartBoundingBox();
    else { minbox.x = x1;  minbox.y = y1;  maxbox.x = x2;  maxbox.y = y2; }
  }

  /// Compute the union of two bounding boxes.
  csBox2& operator+= (const csBox2& box);
  /// Compute the union of a point with this bounding box.
  csBox2& operator+= (const csVector2& point);
  /// Compute the intersection of two bounding boxes.
  csBox2& operator*= (const csBox2& box);

  /// Compute the union of two bounding boxes.
  friend csBox2 operator+ (const csBox2& box1, const csBox2& box2);
  /// Compute the union of a bounding box and a point.
  friend csBox2 operator+ (const csBox2& box, const csVector2& point);
  /// Compute the intersection of two bounding boxes.
  friend csBox2 operator* (const csBox2& box1, const csBox2& box2);

  /// Tests if two bounding boxes are equal.
  friend bool operator== (const csBox2& box1, const csBox2& box2);
  /// Tests if two bounding boxes are unequal.
  friend bool operator!= (const csBox2& box1, const csBox2& box2);
  /// Tests if box1 is a subset of box2.
  friend bool operator< (const csBox2& box1, const csBox2& box2);
  /// Tests if box1 is a superset of box2.
  friend bool operator> (const csBox2& box1, const csBox2& box2);
  /// Tests if a point is contained in a box.
  friend bool operator< (const csVector2& point, const csBox2& box);
};

/// For csBox3::GetCorner().
#define BOX_CORNER_xyz 0
#define BOX_CORNER_xyZ 1
#define BOX_CORNER_xYz 2
#define BOX_CORNER_xYZ 3
#define BOX_CORNER_Xyz 4
#define BOX_CORNER_XyZ 5
#define BOX_CORNER_XYz 6
#define BOX_CORNER_XYZ 7

/// For csBox3::GetSide().
#define BOX_SIDE_x 0
#define BOX_SIDE_X 1
#define BOX_SIDE_y 2
#define BOX_SIDE_Y 3
#define BOX_SIDE_z 4
#define BOX_SIDE_Z 5

/**
 * A bounding box in 3D space.
 * In order to operate correctly, this bounding box assumes that all values
 * entered or compared against lie within the range 
 * (-CS_BOUNDINGBOX_MAXVALUE, CS_BOUNDINGBOX_MAXVALUE).  It is not
 * recommended to use points outside of this range.
 */
class csBox3
{
protected:
  /// The top-left of this bounding box.
  csVector3 minbox;
  /// The bottom-right.
  csVector3 maxbox;

public:
  ///
  float MinX () const { return minbox.x; }
  ///
  float MinY () const { return minbox.y; }
  ///
  float MinZ () const { return minbox.z; }
  ///
  float MaxX () const { return maxbox.x; }
  ///
  float MaxY () const { return maxbox.y; }
  ///
  float MaxZ () const { return maxbox.z; }
  /// Get Min component for 0 (x), 1 (y), or 2 (z).
  float Min (int idx) const { return idx == 1 ? minbox.y : idx == 0 ? minbox.x : minbox.z; }
  /// Get Max component for 0 (x), 1 (y), or 2 (z).
  float Max (int idx) const { return idx == 1 ? maxbox.y : idx == 0 ? maxbox.x : maxbox.z; }
  ///
  const csVector3& Min () const { return minbox; }
  ///
  const csVector3& Max () const { return maxbox; }

  /**
   * Return every corner of this bounding box from 0
   * to 7. This contrasts with Min() and Max() because
   * those are only the min and max corners.
   * Corner 0 = xyz, 1 = xyZ, 2 = xYz, 3 = xYZ,
   *        4 = Xyz, 5 = XyZ, 6 = XYz, 7 = XYZ.
   * Use BOX_CORNER_??? defines.
   */
  csVector3 GetCorner (int corner) const;

  /**
   * Get the center of this box.
   */
  csVector3 GetCenter () const { return (minbox+maxbox)/2; }

  /**
   * Get a side of this box as a 2D box.
   * Use BOX_SIDE_??? defines.
   */
  csBox2 GetSide (int side) const;

  /**
   * Return every edge (segment) of this bounding box
   * from 0 to 11. The returned edge is undefined for any
   * other index.
   */
  csSegment3 GetEdge (int edge) const;

  /// Test if the given coordinate is in this box.
  bool In (float x, float y, float z) const
  {
    if (x < minbox.x || x > maxbox.x) return false;
    if (y < minbox.y || y > maxbox.y) return false;
    if (z < minbox.z || z > maxbox.z) return false;
    return true;
  }

  /// Test if the given coordinate is in this box.
  bool In (const csVector3& v) const
  {
    return In (v.x, v.y, v.z);
  }

  /// Test if this box overlaps with the given box.
  bool Overlap (const csBox3& box) const
  {
    if (maxbox.x < box.minbox.x || minbox.x > box.maxbox.x) return false;
    if (maxbox.y < box.minbox.y || minbox.y > box.maxbox.y) return false;
    if (maxbox.z < box.minbox.z || minbox.z > box.maxbox.z) return false;
    return true;
  }

  /// Test if this box is empty.
  bool Empty () const
  {
    if (minbox.x > maxbox.x) return true;
    if (minbox.y > maxbox.y) return true;
    if (minbox.z > maxbox.z) return true;
    return false;
  }

  /// Initialize this box to empty.
  void StartBoundingBox ()
  {
    minbox.x =  CS_BOUNDINGBOX_MAXVALUE;
    minbox.y =  CS_BOUNDINGBOX_MAXVALUE;
    minbox.z =  CS_BOUNDINGBOX_MAXVALUE;
    maxbox.x = -CS_BOUNDINGBOX_MAXVALUE;
    maxbox.y = -CS_BOUNDINGBOX_MAXVALUE;
    maxbox.z = -CS_BOUNDINGBOX_MAXVALUE;
  }

  /// Initialize this box to one vertex.
  void StartBoundingBox (const csVector3& v)
  {
    minbox = v; maxbox = v;
  }

  /// Add a new vertex and recalculate the bounding box.
  void AddBoundingVertex (float x, float y, float z)
  {
    if (x < minbox.x) minbox.x = x; if (x > maxbox.x) maxbox.x = x;
    if (y < minbox.y) minbox.y = y; if (y > maxbox.y) maxbox.y = y;
    if (z < minbox.z) minbox.z = z; if (z > maxbox.z) maxbox.z = z;
  }

  /// Add a new vertex and recalculate the bounding box.
  void AddBoundingVertex (const csVector3& v)
  {
    AddBoundingVertex (v.x, v.y, v.z);
  }

  /**
   * Add a new vertex and recalculate the bounding box.
   * This version is a little more optimal. It assumes however
   * that at least one point has been added to the bounding box.
   */
  void AddBoundingVertexSmart (float x, float y, float z)
  {
    if (x < minbox.x) minbox.x = x; else if (x > maxbox.x) maxbox.x = x;
    if (y < minbox.y) minbox.y = y; else if (y > maxbox.y) maxbox.y = y;
    if (z < minbox.z) minbox.z = z; else if (z > maxbox.z) maxbox.z = z;
  }

  /**
   * Add a new vertex and recalculate the bounding box.
   * This version is a little more optimal. It assumes however
   * that at least one point has been added to the bounding box.
   */
  void AddBoundingVertexSmart (const csVector3& v)
  {
    AddBoundingVertexSmart (v.x, v.y, v.z);
  }

  //-----
  // Maintenance Note: The csBox3 constructors and Set() appear at this point
  // in the file, rather than earlier, in order to appease the OpenStep 4.2
  // compiler.  Specifically, the problem is that the compiler botches code
  // generation if an unseen method (which is later declared inline) is
  // called from within another inline method.  For instance, if the
  // constructors were listed at the top of the file, rather than here, the
  // compiler would see calls to Empty() and StartBoundingBox() before seeing
  // declarations for them.  In such a situation, the buggy compiler
  // generated a broken object file.  The simple work-around of textually
  // reorganizing the file ensures that the declarations for Empty() and
  // StartBoundingBox() are seen before they are called.
  //-----

  /// Initialize this box to empty.
  csBox3 () :
    minbox ( CS_BOUNDINGBOX_MAXVALUE,
             CS_BOUNDINGBOX_MAXVALUE,
	     CS_BOUNDINGBOX_MAXVALUE),
    maxbox (-CS_BOUNDINGBOX_MAXVALUE,
            -CS_BOUNDINGBOX_MAXVALUE,
	    -CS_BOUNDINGBOX_MAXVALUE) {}

  /// Initialize this box with one point.
  csBox3 (const csVector3& v) : minbox (v), maxbox (v) { }

  /// Initialize this box with two points.
  csBox3 (const csVector3& v1, const csVector3& v2) :
  	minbox (v1), maxbox (v2)
  { if (Empty ()) StartBoundingBox (); }

  /// Initialize this box with the given values.
  csBox3 (float x1, float y1, float z1, float x2, float y2, float z2) :
    minbox (x1, y1, z1), maxbox (x2, y2, z2)
  { if (Empty ()) StartBoundingBox (); }

  /// Sets the bounds of the box with the given values.
  void Set (const csVector3& bmin, const csVector3& bmax)
  {
    minbox = bmin;
    maxbox = bmax;
  }

  /// Sets the bounds of the box with the given values.
  void Set (float x1, float y1, float z1, float x2, float y2, float z2)
  {
    if (x1>x2 || y1>y2 || z1>z2) StartBoundingBox();
    else
    {
      minbox.x = x1; minbox.y = y1; minbox.z = z1;
      maxbox.x = x2; maxbox.y = y2; maxbox.z = z2;
    }
  }

  /**
   * Test if this box is adjacent to the other on the X side.
   */
  bool AdjacentX (const csBox3& other) const;

  /**
   * Test if this box is adjacent to the other on the Y side.
   */
  bool AdjacentY (const csBox3& other) const;

  /**
   * Test if this box is adjacent to the other on the Z side.
   */
  bool AdjacentZ (const csBox3& other) const;

  /**
   * Test if this box is adjacent to the other one.
   */
  bool Adjacent (const csBox3& other) const
  {
    return AdjacentX (other) || AdjacentY (other) || AdjacentZ (other);
  }

  /**
   * Get a convex outline (not a polygon unless projected to 2D)
   * for for this box as seen from the given position.
   * The coordinates returned are world space coordinates.
   * Note that you need place for at least six vectors in the array.
   */
  void GetConvexOutline (const csVector3& pos,
  	csVector3* array, int& num_array) const;

  /**
   * Test if this box is between two others.
   */
  bool Between (const csBox3& box1, const csBox3& box2) const;

  /// Compute the union of two bounding boxes.
  csBox3& operator+= (const csBox3& box);
  /// Compute the union of a point with this bounding box.
  csBox3& operator+= (const csVector3& point);
  /// Compute the intersection of two bounding boxes.
  csBox3& operator*= (const csBox3& box);

  /// Compute the union of two bounding boxes.
  friend csBox3 operator+ (const csBox3& box1, const csBox3& box2);
  /// Compute the union of a bounding box and a point.
  friend csBox3 operator+ (const csBox3& box, const csVector3& point);
  /// Compute the intersection of two bounding boxes.
  friend csBox3 operator* (const csBox3& box1, const csBox3& box2);

  /// Tests if two bounding boxes are equal.
  friend bool operator== (const csBox3& box1, const csBox3& box2);
  /// Tests if two bounding boxes are unequal.
  friend bool operator!= (const csBox3& box1, const csBox3& box2);
  /// Tests if box1 is a subset of box2.
  friend bool operator< (const csBox3& box1, const csBox3& box2);
  /// Tests if box1 is a superset of box2.
  friend bool operator> (const csBox3& box1, const csBox3& box2);
  /// Tests if a point is contained in a box.
  friend bool operator< (const csVector3& point, const csBox3& box);
};

#endif // __CS_BOX_H
