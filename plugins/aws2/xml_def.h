/*
    Copyright (C) 2005 by Christopher Nelson

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

#ifndef __AWS_XML_DEF_PARSER_H__
#define __AWS_XML_DEF_PARSER_H__

#include "registry.h"
#include "csutil/xmltiny.h"
#include "csutil/scfstr.h"

using namespace aws;

namespace aws2
{
  /** Parses a definition file. */
  class defFile
  {
    /** Worker function, parses a node.  Creates subnodes if necessary. */
    void ParseNode (registry *reg, csRef<iDocumentNodeIterator> &pos);

  public:
    defFile() {}
    virtual ~defFile() {}

    /** Parses the given text into the given registry. */
    virtual bool Parse (const scfString &txt, registry &reg);
  };

} // end namespace

#endif
