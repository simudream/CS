/*
    Copyright (C) 1998, 1999 by Nathaniel 'NooTe' Saint Martin
    Copyright (C) 1998, 1999 by Jorrit Tyberghein
    Written by Nathaniel 'NooTe' Saint Martin

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

#include <string.h>
#include "cssndldr/common/sndbuf.h"
#include "csparser/sndbufo.h"
#include "csobject/nameobj.h"

CSOBJTYPE_IMPL(csSoundBufferObject,csObject);

csSoundBufferObject::~csSoundBufferObject()
{ if (sndbuf) CHKB(delete sndbuf); }

csSoundBuffer* csSoundBufferObject::GetSound(csObject& csobj, const char* name)
{
  csObjIterator i = csobj.ObjGet(csSoundBufferObject::Type());
  while (!i.IsNull())
  {
    if (strcmp(name, csNameObject::GetName(*i)) == 0)
      return ((csSoundBufferObject&)(*i)).GetSound();
    ++i;
  }
  return NULL;
}
