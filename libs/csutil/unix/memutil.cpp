/*
Copyright (C) 2008 by Scott Johnson <scottj@cs.umn.edu>

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
#include "csutil/csstring.h"
#include "csutil/sysfunc.h"
#include "../memutil.h"
#include <sys/sysinfo.h>


namespace CS {
  namespace Platform {
    namespace Implementation {

      size_t GetPhysicalMemorySize()
      {
#if defined(CS_HAVE_SYSINFO)
        struct sysinfo s_info;
        int error = sysinfo(&s_info);

        if (error == 0)
        {
          return (s_info.totalram/1024);
        }
#elif defined (CS_HAVE_PROC_MEMINFO)
	FILE* f = fopen("/proc/meminfo", "r");
	if (f != 0)
	{
	  size_t totalMem = 0;
	  csString line, key, val;
	  char buff[ 1024 ];
	  while (fgets(buff, sizeof(buff) - 1, f) != 0)
	  {
	    line = buff;
	    size_t pos = line.Find(":");
	    if (pos > 0)
	    {
	      key = line.Slice(0, pos);
	      if (key.CompareNoCase("memtotal"))
	      {
		val = line.Slice(pos + 1);
		totalMem = (size_t)atol(val.GetData());
		break;
	      }
	    }
	  }
	  fclose(f);
	  return totalMem;
	}
#endif
        // everything seems to have failed, so output some information
        // and return 0
        csPrintfErr("ERROR: GetPhysicalMemorySize(): Unable to determine mechanism for finding physical memory size.\n");
        return 0;
      }
    } // End namespace Implementation
  } // End namespace Platform
} // End namespace CS

