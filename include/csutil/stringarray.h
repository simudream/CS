/*
  Crystal Space String Array
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

#ifndef __CS_STRINGARRAY_H__
#define __CS_STRINGARRAY_H__

#include <stdarg.h>
#include "csextern.h"
#include "util.h"
#include "array.h"

class csStringArrayElementHandler
{
public:
  static void Construct (const char** address, const char* const& src)
  {
    *address = csStrNew (src);
  }

  static void Destroy (const char** address)
  {
    delete[] (char*)*address;
  }

  static void InitRegion (const char** address, int count)
  {
    memset (address, 0, count*sizeof (const char*));
  }
};

/**
 * An array of strings. This array will properly make copies of the strings
 * and delete those copies using delete[] later.
 */
class csStringArray : public csArray<const char*, csStringArrayElementHandler>
{
  typedef csArray<const char*, csStringArrayElementHandler> superclass;
public:
  /**
   * Initialize object to hold initially 'ilimit' elements, and increase
   * storage by 'ithreshold' each time the upper bound is exceeded.
   */
  csStringArray (int ilimit = 0, int ithreshold = 0)
  	: superclass(ilimit, ithreshold)
  {
  }

  static int CaseSensitiveCompareKey (char const* const& s,
				      char const* const& k)
  {
    return strcmp (s, k);
  }

  static int CaseInsensitiveCompareKey (char const* const& s,
					char const* const& k)
  {
    return strcasecmp (s, k);
  }

  static int CaseSensitiveCompare (const char* const &item1,
				   const char* const &item2)
  {
    return strcmp (item1, item2);
  }

  static int CaseInsensitiveCompare (const char* const &item1,
				     const char* const &item2)
  {
    return strcasecmp (item1, item2);
  }

  /**
   * Sort array based on case sensitive string compare function.
   */
  void Sort (int (*compare)(char const* const&, char const* const&))
  {
    superclass::Sort (compare);
  }

  /**
   * Sort array based on case sensitive string compare function.
   */
  void Sort (bool case_sensitive = true)
  {
    if (case_sensitive)
      superclass::Sort (CaseSensitiveCompare);
    else
      superclass::Sort (CaseInsensitiveCompare);
  }

  /**
   * Find an element based on some key, using a csArrayCompareKeyFunction.
   * The array must be sorted. Returns -1 if element does not exist.
   */
  int FindSortedKey (char const* key,
    int (*comparekey)(char const* const& s, char const* const& k) =
    CaseSensitiveCompareKey, int* candidate = 0) const
  {
    return superclass::FindSortedKey(key, comparekey, candidate);
  }

  /**
   * Pop an element from tail end of array.  Caller is responsible for
   * invoking delete[] on the returned string when no longer needed.
   */
  char* Pop ()
  {
    CS_ASSERT (Length () > 0);
    int l = Length () - 1;
    char* ret = (char*)Get (l);
    InitRegion (l, 1);
    SetLength (l);
    return ret;
  }

  /**
   * Find a string, case-sensitive. Returns -1 if not found, else item index.
   * Works with unsorted arrays.  For sorted arrays, FindSortedKey() is faster.
   */
  int Find (const char* what) const
  {
    for (int i = 0; i < Length (); i++)
      if (! strcmp (Get (i), what))
        return i;
    return -1;
  }

  /**
   * Find a string, case-insensitive. Returns -1 if not found, else item index.
   * Works with unsorted arrays.  For sorted arrays, FindSortedKey() is faster.
   */
  int FindCaseInsensitive (const char* what) const
  {
    for (int i = 0; i < Length (); i++)
      if (!strcasecmp (Get (i), what))
        return i;
    return -1;
  }

  /**
   * Insert a string element at a sorted position, using a specialized
   * csArrayCompareFunction. Assumes array is already sorted.
   */
  int InsertSorted (const char* const &newstr,
    int (*compare)(char const* const&, char const* const&) =
    CaseSensitiveCompare, int* equal_index = 0)
  {
    return superclass::InsertSorted(newstr, compare, equal_index);
  }
};

#endif // __CS_STRINGARRAY_H__
