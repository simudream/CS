/*
    Crystal Space Windowing System: Windowing System Texture class
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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

#ifndef __CSWSTEX_H__
#define __CSWSTEX_H__

#include "csgfxldr/csimage.h"
#include "csutil/csvector.h"

scfInterface iTextureHandle;
scfInterface iTextureManager;

/**
 * Texture representation within the windowing system.
 * The application keeps an array of such objects, and they are registered
 * with the 3D and 2D drivers as soon as app->RegisterTextures() is called.
 */
class csWSTexture
{
  // Reference count
  int RefCount;
  // texture image
  iImageFile *Image;
  // Will be this texture used for 3D and/or for 2D operations?
  bool for2D, for3D;
  // Red,Green and Blue components of transparent color
  int tr, tg, tb;
  // has texture transparent areas?
  bool IsTransp;
  // Has transparent color changed?
  bool TranspChanged;
  // texture handle for the 3D/2D driver
  iTextureHandle *Handle;
  // texture name
  char *Name;
  // VFS file name
  char *FileName;
  // A pointer to the texture manager (WARNING: NO INCREF/DECREF)
  iTextureManager *TexMan;

public:
  /// Create the 2D texture
  csWSTexture (const char *iName, iImageFile *iImage, bool i2D, bool i3D);
  /// Destroy the texture object
  ~csWSTexture ();
  /// Set texture transparent color
  void SetTransparent (int iR, int iG, int iB);
  /// Set whenever texture has transparent holes or not
  void SetTransparent (bool iTransparent);
  /// Get texture transparent color
  void GetTransparent (int &oR, int &oG, int &oB)
  { oR = tr; oG = tg; oB = tb; }
  /// Query whenever texture has transparent areas
  bool IsTransparent ()
  { return IsTransp; }
  /// Register the texture with texture manager
  void Register (iTextureManager *iTexMan);
  /// Unregister the texture
  void Unregister ();
  /// Define texture name
  void SetName (const char *iName);
  /// Get texture name
  const char *GetName ()
  { return Name; }
  iTextureHandle *GetHandle ()
  { return Handle; }
  /// Increment reference count to this texture
  void IncRef ()
  { RefCount++; }
  /// Delete a reference to this texture
  void DecRef ()
  { RefCount--; }
  /// Return reference count
  int GetRefCount () const
  { return RefCount; }
  /// Get texture file name
  const char *GetFileName () const
  { return FileName; }
  /// Set texture file name
  void SetFileName (const char *iFileName);
  /// Find nearest transparent color in image
  void FixTransparency ();
  /// Tell texture that transparent color has already been fixed
  void DontFixTransparency ()
  { TranspChanged = false; }
};

/// This class is a vector of csWSTexture's
class csWSTexVector : public csVector
{
public:
  /// Initialize the texture vector
  csWSTexVector ();
  /// Destroy the object
  virtual ~csWSTexVector ();
  /// Free a texture element
  virtual bool FreeItem (csSome Item);
  /// Compare texture with name; used in FindKey ()
  virtual int CompareKey (csSome Item, csConstSome Key, int Mode) const;
  /// Get texture by index
  csWSTexture *Get (int idx)
  { return (csWSTexture *)csVector::Get (idx); }
  /// Find a texture by name
  csWSTexture *FindTexture (const char *iName)
  {
    int idx = FindKey (iName);
    return idx >= 0 ? (csWSTexture *)Get (idx) : NULL;
  }
};

#endif // __CSWSTEX_H__
