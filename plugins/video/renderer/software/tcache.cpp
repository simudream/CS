/*
    Copyright (C) 1998 by Jorrit Tyberghein
  
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

#include "cssysdef.h"
#include "qint.h"
#include "csutil/util.h"
#include "tcache.h"
#include "soft_g3d.h"
#include "soft_txt.h"
#include "ilghtmap.h"
#include "isystem.h"

static int hash_table [384];

//------------------------------------------------------------------------------

#define LM_NAME		csTextureCacheSoftware::create_lighted_texture_8
#define PI_INDEX8
#include "lightmap.inc"

#define LM_NAME		csTextureCacheSoftware::create_lighted_texture_555
#define PI_R5G5B5
#include "lightmap.inc"

#define LM_NAME		csTextureCacheSoftware::create_lighted_texture_565
#define PI_R5G6B5
#include "lightmap.inc"

#define LM_NAME		csTextureCacheSoftware::create_lighted_texture_888
#define PI_R8G8B8
#include "lightmap.inc"

//------------------------------------------------- csTextureCacheSoftware ---//

//  static void (*create_lighted_texture) (iPolygonTexture *pt,
//    SoftwareCachedTexture *ct, csTextureManagerSoftware *texman,
//    float u_min, float v_min, float u_max, float v_max);

static void compute_hash_table ()
{
  for (int i = 0; i < 384; i++)
    hash_table [i] = i + 64;
}

csTextureCacheSoftware::csTextureCacheSoftware (csTextureManagerSoftware *TexMan)
{
  head = tail = NULL;
  texman = TexMan;
  frameno = 0;
  Clear ();
  bytes_per_texel = texman->pfmt.PixelBytes;
  if (texman->pfmt.PixelBytes == 1)
    create_lighted_texture = &csTextureCacheSoftware::create_lighted_texture_8;
  else if (texman->pfmt.PixelBytes == 2)
    if (texman->pfmt.GreenBits == 5)
      create_lighted_texture = &csTextureCacheSoftware::create_lighted_texture_555;
    else
      create_lighted_texture = &csTextureCacheSoftware::create_lighted_texture_565;
  else if (texman->pfmt.PixelBytes == 4)
    create_lighted_texture = &csTextureCacheSoftware::create_lighted_texture_888;
  else
    abort (); // huh???
  compute_hash_table ();
}

csTextureCacheSoftware::~csTextureCacheSoftware ()
{
  Clear ();
}

void csTextureCacheSoftware::set_cache_size (long size)
{
  Clear ();
  cache_size = size;
}

void csTextureCacheSoftware::Clear ()
{
  while (head)
  {
    SoftwareCachedTexture *n = head->next;
    delete head;
    head = n;
  }

  head = tail = NULL;
  total_size = 0;
  total_textures = 0;
}

void csTextureCacheSoftware::uncache_texture (int MipMap, iPolygonTexture* pt)
{
  SoftwareCachedTexture* cached_texture;
  cached_texture = (SoftwareCachedTexture*)pt->GetCacheData (MipMap);
  if (!cached_texture) return;

  if (cached_texture->next)
    cached_texture->next->prev = cached_texture->prev;
  else
    tail = cached_texture->prev;
  if (cached_texture->prev)
    cached_texture->prev->next = cached_texture->next;
  else
    head = cached_texture->next;
  total_textures--;
  total_size -= cached_texture->size;

  delete cached_texture;
}

void csTextureCacheSoftware::uncache_texture (int MipMap, iTextureHandle *itexh)
{
  SoftwareCachedTexture *cached_texture, *next_cached_texture;
  cached_texture = next_cached_texture = head;
  while (cached_texture)
  {
    next_cached_texture = cached_texture->next;
    if ((cached_texture->mipmap == MipMap) && 
	(cached_texture->source->GetMaterialHandle ()->GetTexture () == itexh))
    {
      if (cached_texture->next)
	cached_texture->next->prev = cached_texture->prev;
      else
	tail = cached_texture->prev;
      if (cached_texture->prev)
	cached_texture->prev->next = cached_texture->next;
      else
	head = cached_texture->next;
      total_textures--;
      total_size -= cached_texture->size;

      delete cached_texture;
    }
    cached_texture = next_cached_texture;
  }   
}

SoftwareCachedTexture *csTextureCacheSoftware::cache_texture 
  (int MipMap, iPolygonTexture* pt)
{
  SoftwareCachedTexture *cached_texture =
    (SoftwareCachedTexture *)pt->GetCacheData (MipMap);

  if (cached_texture)
  {
    // Texture is already in the cache.
    // Unlink texture and put it in front (MRU).
    if (cached_texture != head)
    {
      if (cached_texture->prev)
        cached_texture->prev->next = cached_texture->next;
      else
        head = cached_texture->next;
      if (cached_texture->next)
        cached_texture->next->prev = cached_texture->prev;
      else
        tail = cached_texture->prev;

      cached_texture->prev = NULL;
      cached_texture->next = head;
      if (head)
        head->prev = cached_texture;
      else
        tail = cached_texture;
      head = cached_texture;
    }
    cached_texture->frameno = frameno;
  }
  else
  {
    // Texture is not in the cache.
    int lightmap_size = pt->GetLightMap ()->GetSize () * sizeof (ULong);
    int bitmap_w = (pt->GetWidth () >> MipMap);
    int bitmap_h = ((pt->GetHeight () + (1 << MipMap) - 1) >> MipMap);
    int bitmap_size = lightmap_size + bytes_per_texel * bitmap_w * (H_MARGIN * 2 + bitmap_h);

    total_textures++;
    total_size += bitmap_size;

    // Free lightmaps until we have less than cache_size bytes for the cache

    while (tail && (total_size > cache_size))
    {
      // Total size of textures in cache is too high. Remove the last one.
      cached_texture = tail;

      // thanks John Carmack: we miss you :-)
      // A possible improvement for the texture cache algorithm could be the
      // following: the problem with it currently is that it always frees the
      // least-recently-used texture, no matter when it was allocated. For
      // levels where all the textures visible in one frame do not fit in the
      // cache this is extremely ugly: the textures are computed all in chain
      // again and again because each new textures pushes the least recently
      // used (but also generated during this frame) texture out of cache and
      // so on. This could be improved by adding a "frame number" to each
      // texture, and to check if LRU texture has same frame number as the
      // current one; if so, we should free the MOSTLY allocated texture
      // instead. This way, we'll compute just several textures on each
      // frame instead of doing it for ALL of them.
      if (cached_texture->frameno == frameno)
#if 1
        // Hmm... this proven to work much better ... WHY??? :-)
        cached_texture = head;
#else
      {
        // Try to find the closest (by size) texture cached this frame
        int need_size = total_size - cache_size;
        int nearest_size = tail->size;
        for (SoftwareCachedTexture *cur = head; cur != tail; cur = cur->next)
          if (((cur->size >= need_size)
            && (cur->size < nearest_size))
           || ((nearest_size < need_size)
            && (nearest_size < cur->size)))
          {
            cached_texture = cur;
            nearest_size = cur->size;
          }
      }
#endif

      if (cached_texture->prev)
        cached_texture->prev->next = cached_texture->next;
      else
        head = cached_texture->next;
      if (cached_texture->next)
        cached_texture->next->prev = cached_texture->prev;
      else
        tail = cached_texture->prev;

      total_textures--;
      total_size -= cached_texture->size;

      delete cached_texture;
    }

    cached_texture = new SoftwareCachedTexture (MipMap, pt);
    cached_texture->frameno = frameno;

    int margin_size = H_MARGIN * bitmap_w * bytes_per_texel;
    UByte *data = new UByte [bitmap_size];
    memset (data, 0, lightmap_size);
    cached_texture->data = data;
    cached_texture->bitmap = data + lightmap_size + margin_size; // Skip margin.
    cached_texture->size = bitmap_size;

    // Add new texture to cache.
    cached_texture->next = head;
    cached_texture->prev = NULL;
    if (head)
      head->prev = cached_texture;
    else
      tail = cached_texture;
    head = cached_texture;
  }
  return cached_texture;
}

void csTextureCacheSoftware::fill_texture (int MipMap, iPolygonTexture* pt,
    csTextureHandleSoftware *tex_mm, float u_min, float v_min, float u_max, float v_max)
{
  // Recalculate the lightmaps
  // @@@ Note for Andrew: this function returns true if something
  // has changed with regards to the dynamic lights. It seems a bit
  // wasteful not to use this result. Or isn't it?
  pt->RecalculateDynamicLights ();

  // Now cache the texture
  SoftwareCachedTexture *cached_texture = cache_texture (MipMap, pt);

  // Compute the rectangle on the lighted texture, if it is dirty
  (this->*create_lighted_texture) (pt, cached_texture, tex_mm, texman, u_min, v_min, u_max, v_max);
}

#define SysPrintf iG3D->System->Printf

void csTextureCacheSoftware::dump (csGraphics3DSoftwareCommon *iG3D)
{
  SysPrintf (MSG_CONSOLE, "Textures in the cache: %d\n", total_textures);
  SysPrintf (MSG_CONSOLE, "Total size: %ld bytes\n", total_size);
  int mean = (total_textures == 0) ? 0 : total_size / total_textures;
  SysPrintf (MSG_CONSOLE, "Bytes per texture: %d\n", mean);
}
