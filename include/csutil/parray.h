/*
  Crystal Space Pointer Array
  Copyright (C) 2003 by Jorrit Tyberghein

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

#ifndef __CS_PTRARR_H__
#define __CS_PTRARR_H__

#include "csextern.h"
#include "array.h"

template <class T>
class csPDelArrayElementHandler
{
public:
  static void Construct (T* address, T const& src)
  {
    *address = src;
  }

  static void Destroy (T* address)
  {
    delete *address;
  }

  static void InitRegion (T* address, int count)
  {
    memset (address, 0, count*sizeof (T));
  }
};

/**
 * An array of pointers. No ref counting is done on the elements in this
 * array. Use csRefArray if you want ref counting to happen.
 * This array will delete elements (using 'delete') as needed.
 * This array properly initializes new elements in the array to 0 (the NULL
 * pointer).
 */
template <class T>
class csPDelArray : public csArray<T*, csPDelArrayElementHandler<T*> >
{
  typedef csArray<T*, csPDelArrayElementHandler<T*> > superclass;

private:
  csPDelArray (const csPDelArray&);            // Illegal; unimplemented.
  csPDelArray& operator= (const csPDelArray&); // Illegal; unimplemented.

public:
  /**
   * Initialize object to hold initially 'ilimit' elements, and increase
   * storage by 'ithreshold' each time the upper bound is exceeded.
   */
  csPDelArray (int ilimit = 0, int ithreshold = 0) :
    csArray<T*, csPDelArrayElementHandler<T*> > (ilimit, ithreshold) {}

  /**
   * Get and clear the element 'n' from vector. This spot in the array
   * will be set to 0. You are responsible for deleting the returned
   * pointer later.
   */
  T* GetAndClear (int n)
  {
    T* ret = Get (n);
    InitRegion (n, 1);
    return ret;
  }

  /**
   * Extract element number 'n' from vector. The element is deleted
   * from the array and returned. You are responsible for deleting the
   * pointer later.
   */
  T* Extract (int n)
  {
    T* ret = GetAndClear (n);
    DeleteIndex (n);
    return ret;
  }

  /// Pop an element from tail end of array.
  T* Pop ()
  {
    CS_ASSERT (Length () > 0);
    T* ret = GetAndClear (Length () - 1);
    Truncate (Length () - 1);
    return ret;
  }

  /// Variant of SetLength() which copies the pointed-to object instead of
  /// the actual pointer.
  void SetLength (int n, T const &what)
  {
    if (n <= Length ())
    {
      Truncate (n);
    }
    else
    {
      int old_len = Length ();
      superclass::SetLength (n);
      for (int i = old_len ; i < n ; i++) Get(i) = new T (what);
    }
  }

  /// Call csArray<T*>::SetLength(n, w).
  void SetLength (int n, T* const &w)
  {
    superclass::SetLength(n, w);
  }

  /// Call csArray<T*>::SetLength(n).
  void SetLength (int n)
  {
    superclass::SetLength(n);
  }

  /// Find an element based on some key.
  template <class K>
  int FindKey (K const& key, int(*comparekey)(T* const&, K const&)) const
  {
    return superclass::FindKey (key, comparekey);
  }

  /**
   * Find an element based on some key, using a comparison function.
   * The array must be sorted. Returns -1 if element does not exist.
   */
  template <class K>
  int FindSortedKey (K const& key,
    int (*comparekey)(T* const&, K const&), int* candidate = 0) const
  {
    return superclass::FindSortedKey (key, comparekey, candidate);
  }

  /**
   * Insert an element at a sorted position, using an element comparison
   * function.  Assumes array is already sorted.
   */
  int InsertSorted (T const* item,
    int (*compare)(T* const&, T* const&),
    int* equal_index = 0)
  {
    return superclass::InsertSorted((T*)item, compare, equal_index);
  }

  /// Sort array.
  void Sort (int (*compare)(T* const&, T* const&))
  {
    superclass::Sort(compare);
  }
};

#endif // __CS_PTRARR_H__
