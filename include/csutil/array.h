/*
  Crystal Space Generic Array Template
  Copyright (C) 2003 by Matze Braun

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
#ifndef __CSUTIL_ARRAY_H__
#define __CSUTIL_ARRAY_H__

#include "csutil/arraybase.h"

// hack: work around problems caused by #defining 'new'
#ifdef CS_EXTENSIVE_MEMDEBUG
# undef new
#endif
#include <new>

/**
 * A templated array class.  The objects in this class are constructed via
 * copy-constructor and are destroyed when they are removed from the array or
 * the array is destroyed.  Note: If you want to store reference-counted object
 * pointers (such as iSomething*), then you should look at csRefArray instead
 * of this class.
 */
template <class T>
class csArray : private csArrayBase<T>	// Note! Private inheritance here!
{
private:
  void ConstructElement (int n, T const& src)
  {
    new (static_cast<void*>(root + n)) T(src);
  }

  void DestroyElement (int n)
  {
    (root + n)->T::~T();
  }
 
public:
  // We take the following public functions from csArrayBase<T> and
  // make them public here.
  using csArrayBase<T>::Length;
  using csArrayBase<T>::Capacity;
  using csArrayBase<T>::Find;

  /// This function prototype is used for Sort()
  typedef int ArraySortCompareFunction (void const* item1,
	void const* item2);
  /// This function prototype is used for csArray::InsertSorted()
  typedef int ArrayCompareFunction (T const& item1, T const& item2);
  /// This function prototype is used for csArray::FindKey()
  typedef int ArrayCompareKeyFunction (T const& item1, void* item2);

  /**
   * Initialize object to have initial capacity of 'icapacity' elements, and to
   * increase storage by 'ithreshold' each time the upper bound is exceeded.
   */
  csArray (int icapacity = 0, int ithreshold = 0)
  	: csArrayBase<T> (icapacity, ithreshold)
  {
  }

  /// Copy constructor just copies all data.
  csArray (const csArray& other)
  {
    count = 0;
    capacity = 0;
    threshold = other.threshold;
    root = 0;
    SetLengthUnsafe (other.Length ());
    for (int i=0 ; i<other.Length() ; i++)
      ConstructElement (i, other[i]);
  }

  /**
   * Transfer the entire contents of one array to the other. The end
   * result will be that this array will be completely empty and the
   * other array will have all items that originally were in this array.
   * This operation is very efficient.
   */
  void TransferTo (csArray<T>& destination)
  {
    destination.DeleteAll ();
    destination.root = root;
    destination.count = count;
    destination.capacity = capacity;
    destination.threshold = threshold;
    root = 0;
    capacity = count = 0;
  }

  /// Assignment operator.
  csArray<T>& operator= (const csArray& other)
  {
    if (&other == this)
      return *this;

    DeleteAll ();
    SetLengthUnsafe (other.Length ());
    for (int i=0 ; i<other.Length() ; i++)
      ConstructElement (i, other[i]);
    return *this;
  }

  /// Clear entire vector, releasing all memory.
  void DeleteAll ()
  {
    for (int i = 0; i < count; i++)
      DestroyElement (i);
    DeleteRoot ();
  }

  /**
   * Truncate array to specified number of elements. The new number of
   * elements cannot exceed the current number of elements. Use SetLength()
   * for a more general way to enlarge the array.
   */
  void Truncate (int n)
  {
    CS_ASSERT(n >= 0);
    CS_ASSERT(n <= count);
    if (n < count)
    {
      for (int i = n; i < count; i++)
        DestroyElement(i);
      SetLengthUnsafe(n);
    }
  }

  /**
   * Set the actual number of items in this array. This can be used to
   * shrink an array (works like Truncate() then in which case it will properly
   * destroy all truncated objects) or to enlarge an array in which case
   * it will properly set the new capacity and construct all new items
   * based on the given item.
   */
  void SetLength (int n, T const& what)
  {
    if (n <= count)
    {
      Truncate (n);
    }
    else
    {
      int old_len = Length ();
      SetLengthUnsafe (n);
      for (int i = old_len ; i < n ; i++)
        ConstructElement (i, what);
    }
  }

  /**
   * Remove all elements.  Similar to DeleteAll(), but does not release memory
   * used by the array itself, thus making it more efficient for cases when the
   * number of contained elements will fluctuate.
   */
  void Empty()
  { Truncate(0); }


  /// Destroy the container.
  ~csArray ()
  {
    DeleteAll ();
  }

  /**
   * Set vector capacity to approximately 'n' elements.  Never sets the
   * capacity to fewer than the current number of elements in the array.  See
   * Truncate() or SetLength() if you need to adjust the number of actual
   * array elements.
   */
  void SetCapacity (int n)
  {
    if (n > Length ())
      AdjustCapacity (n);
  }

  /**
   * Make the array just as big as it needs to be. This is useful in cases
   * where you know the array isn't going to be modified anymore in order
   * to preserve memory.
   */
  void ShrinkBestFit ()
  {
    if (count == 0)
    {
      DeleteAll ();
    }
    else if (count != capacity)
    {
      capacity = count;
      root = (T*)realloc (root, capacity * sizeof(T));
    }
  }

  /// Get an element (const).
  T const& Get (int n) const
  {
    CS_ASSERT (n >= 0 && n < count);
    return root[n];
  }

  /// Get an element (non-const).
  T& Get (int n)
  {
    CS_ASSERT (n >= 0 && n < count);
    return root[n];
  }

  /// Get an element (const).
  T const& operator [] (int n) const
  {
    return Get(n);
  }

  /// Get an element (non-const).
  T& operator [] (int n)
  {
    return Get(n);
  }

  /**
   * Find an element based on some key.
   */
  int FindKey (void* key, ArrayCompareKeyFunction* comparekey) const
  {
    int i;
    for (i = 0 ; i < Length () ; i++)
      if (comparekey (root[i], key) == 0)
        return i;
    return -1;
  }


  /// Push an element onto the tail end of the array. Returns index of element.
  int Push (T const& what)
  {
    SetLengthUnsafe (count + 1);
    ConstructElement (count - 1, what);
    return (count - 1);
  }

  /*
   * Push a element onto the tail end of the array if not already present.
   * Returns index of newly pushed element or index of already present element.
   */
  int PushSmart (T const& what)
  {
    int const n = Find (what);
    return (n < 0) ? Push (what) : n;
  }

  /// Pop an element from tail end of array.
  T Pop ()
  {
    CS_ASSERT (count > 0);
    T ret(root [count - 1]);
    DestroyElement (count - 1);
    SetLengthUnsafe (count - 1);
    return ret;
  }

  /// Return the top element but do not remove it.
  T const& Top () const
  {
    CS_ASSERT (count > 0);
    return root [count - 1];
  }

  /// Delete element number 'n' from vector.
  bool DeleteIndex (int n)
  {
    if (n >= 0 && n < count)
    {
      int const ncount = count - 1;
      int const nmove = ncount - n;
      DestroyElement (n);
      if (nmove > 0)
        memmove (root + n, root + n + 1, nmove * sizeof(T));
      SetLengthUnsafe (ncount);
      return true;
    }
    else
      return false;
  }

  /**
   * Delete a given range (inclusive). This routine will clamp start and end
   * to the size of the array.
   */
  void DeleteRange (int start, int end)
  {
    if (start >= count) return;
    if (end < 0) return;
    if (start < 0) start = 0;
    if (end >= count) end = count-1;
    int i;
    for (i = start ; i < end ; i++)
      DestroyElement (i);

    int const range_size = end-start+1;
    int const ncount = count - range_size;
    int const nmove = count - end - 1;
    if (nmove > 0)
      memmove (root + start, root + start + range_size, nmove * sizeof(T));
    SetLengthUnsafe (ncount);
  }

  /// Delete the given element from vector.
  bool Delete (T const& item)
  {
    int const n = Find (item);
    if (n >= 0)
      return DeleteIndex (n);
    return false;
  }

  /// Insert element 'item' before element 'n'.
  bool Insert (int n, T const& item)
  {
    if (n <= count)
    {
      SetLengthUnsafe (count + 1); // Increments 'count' as a side-effect.
      int const nmove = (count - n - 1);
      if (nmove > 0)
        memmove (root + n + 1, root + n, nmove * sizeof(T));
      ConstructElement (n, item);
      return true;
    }
    else
      return false;
  }

  /**
   * Get the portion of the array between low and high inclusive
   */
  csArray<T> Section (int low, int high) const
  {
    CS_ASSERT (low >= 0 && high < count && high >= low);
    csArray<T> sect (high - low + 1);
    for (int i = low; i <= high; i++) sect.Push (root[i]);
    return sect;
  }

  /// The default ArrayCompareFunction for InsertSorted()
  static int DefaultCompare (T const &item1, T const &item2)
  {
    if (item1 < item2) return -1;
    else if (item1 > item2) return 1;
    else return 0;
  }

  /// The default ArrayCompareKeyFunction for FindKey()
  static int DefaultCompareKey (T const &item1, void *item2)
  {
    if (item1 < item2) return -1;
    else if (item1 > item2) return 1;
    else return 0;
  }

  /**
   * Find an element based on some key, using a csArrayCompareKeyFunction.
   * The array must be sorted. Returns -1 if element does not exist.
   */
  int FindSortedKey (void* key, ArrayCompareKeyFunction* comparekey
    = DefaultCompareKey, int *candidate = 0) const
  {
    int m = 0, l = 0, r = Length () - 1;
    while (l <= r)
    {
      m = (l + r) / 2;
      int cmp = comparekey (root [m], key);

      if (cmp == 0)
      {
        if (candidate) *candidate = -1;
        return m;
      }
      else if (cmp < 0)
        l = m + 1;
      else
        r = m - 1;
    }
    if (candidate) *candidate = m;
    return -1;
  }

  /**
   * Insert an element at a sorted position, using a csArrayCompareFunction.
   * Assumes array is already sorted.
   */
  int InsertSorted (const T &item, ArrayCompareFunction* compare
    = DefaultCompare, int* equal_index = 0)
  {
    int m = 0, l = 0, r = Length () - 1;
    while (l <= r)
    {
      m = (l + r) / 2;
      int cmp = compare (root [m], item);

      if (cmp == 0)
      {
	if (equal_index) *equal_index = m;
        Insert (++m, item);
        return m;
      }
      else if (cmp < 0)
        l = m + 1;
      else
        r = m - 1;
    }
    if (r == m)
      m++;
    if (equal_index) *equal_index = -1;
    Insert (m, item);
    return m;
  }

  /**
   * Sort array.
   */
  void Sort (ArraySortCompareFunction* compare)
  {
    qsort (root, Length (), sizeof (T), compare);
  }

  /** Iterator for the Array object */
  class Iterator
  {
  public:
    /** Returns true if the next Next() call will return an element */
    bool HasNext()
    { return currentelem < array.Length(); }

    /** Returns the next element in the array. */
    const T& Next()
    { return array.Get(currentelem++); }

  protected:
    Iterator(const csArray<T>& newarray)
	: currentelem(0), array(newarray)
    { }
    friend class csArray<T>;
    
  private:
    int currentelem;
    const csArray<T>& array;
  };

  /** Returns an Iterator which traverses the Array */
  Iterator GetIterator() const
  { return Iterator(*this); }
};

#ifdef CS_EXTENSIVE_MEMDEBUG
# define new CS_EXTENSIVE_MEMDEBUG_NEW
#endif

#endif
