/*
    Copyright (C) 2001 by Jorrit Tyberghein

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
#define SYSDEF_PATH
#include "cssysdef.h"
#include "csver.h"
#include "csutil/scf.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/plane3.h"
#include "csgeom/box.h"
#include "bugplug.h"
#include "isys/system.h"
#include "isys/vfs.h"
#include "isys/event.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivaria/conout.h"
#include "iobject/object.h"
#include "imesh/object.h"
#include "iengine/engine.h"
#include "iengine/sector.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"

IMPLEMENT_FACTORY (csBugPlug)

EXPORT_CLASS_TABLE (bugplug)
  EXPORT_CLASS (csBugPlug, "crystalspace.utilities.bugplug",
    "Debugging utility")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csBugPlug)
  IMPLEMENTS_INTERFACE (iPlugIn)
IMPLEMENT_IBASE_END

#define SysPrintf System->Printf

csBugPlug::csBugPlug (iBase *iParent)
{
  CONSTRUCT_IBASE (iParent);
  Engine = NULL;
  System = NULL;
  G3D = NULL;
  G2D = NULL;
  Conout = NULL;
  VFS = NULL;
  mappings = NULL;
  process_next_key = false;
  initialized = false;
}

csBugPlug::~csBugPlug ()
{
  if (Engine) Engine->DecRef ();
  if (G3D) G3D->DecRef ();
  if (Conout) Conout->DecRef ();
  if (VFS) VFS->DecRef ();
  while (mappings)
  {
    csKeyMap* n = mappings->next;
    delete mappings;
    mappings = n;
  }
}

bool csBugPlug::Initialize (iSystem *system)
{
  System = system;
  if (!System->CallOnEvents (this, CSMASK_Nothing|CSMASK_KeyUp|CSMASK_KeyDown))
    return false;

  return true;
}

void csBugPlug::SetupPlugin ()
{
  if (initialized) return;

  if (!Engine)
    Engine = QUERY_PLUGIN_ID (System, CS_FUNCID_ENGINE, iEngine);
  if (!Engine)
  {
    printf ("No engine!\n");
    return;
  }

  if (!G3D)
    G3D = QUERY_PLUGIN_ID (System, CS_FUNCID_VIDEO, iGraphics3D);
  if (!G3D)
  {
    printf ("No G3D!\n");
    return;
  }

  if (!G2D)
    G2D = G3D->GetDriver2D ();
  if (!G2D)
  {
    printf ("No G2D!\n");
    return;
  }

  if (!VFS)
    VFS = QUERY_PLUGIN_ID (System, CS_FUNCID_VFS, iVFS);
  if (!VFS)
  {
    printf ("No VFS!\n");
    return;
  }

  if (!Conout)
    Conout = QUERY_PLUGIN_ID (System, CS_FUNCID_CONSOLE, iConsoleOutput);

  ReadKeyBindings ("/config/bugplug.cfg");

  initialized = true;

  System->Printf (MSG_CONSOLE, "BugPlug loaded...\n");

  //---------------------------------------------------------------------
  do_clear = false;
  //---------------------------------------------------------------------
}

void csBugPlug::ToggleG3DState (G3D_RENDERSTATEOPTION op, const char* name)
{
  bool v;
  v = G3D->GetRenderState (op);
  v = !v;
  if (G3D->SetRenderState (op, v))
  {
    System->Printf (MSG_CONSOLE, "BugPlug %s %s.\n",
	v ? "enabled" : "disabled", name);
  }
  else
  {
    System->Printf (MSG_CONSOLE, "%s not supported for this renderer!\n",
    	name);
  }
}

bool csBugPlug::EatKey (iEvent& event)
{
  SetupPlugin ();
  int key = event.Key.Code;
  int down = (event.Type == csevKeyDown);
  bool shift = (event.Key.Modifiers & CSMASK_SHIFT) != 0;
  bool alt = (event.Key.Modifiers & CSMASK_ALT) != 0;
  bool ctrl = (event.Key.Modifiers & CSMASK_CTRL) != 0;
  // Get command.
  int cmd = GetCommandCode (key, shift, alt, ctrl);
  if (down)
  {
    // First we check if it is the 'debug enter' key.
    if (cmd == DEBUGCMD_DEBUGENTER)
    {
      process_next_key = !process_next_key;
      if (process_next_key)
      {
        System->Printf (MSG_CONSOLE, "Press debug key...\n");
      }
      return true;
    }
  }

  // Return false if we are not processing our own keys.
  if (!process_next_key) return false;
  if (down)
  {
    switch (cmd)
    {
      case DEBUGCMD_UNKNOWN:
        return true;
      case DEBUGCMD_QUIT:
        System->Printf (MSG_CONSOLE, "Nah nah! I will NOT quit!\n");
        break;
      case DEBUGCMD_STATUS:
        System->Printf (MSG_CONSOLE, "I'm running smoothly, thank you...\n");
        break;
      case DEBUGCMD_HELP:
        System->Printf (MSG_CONSOLE, "Sorry, cannot help you yet.\n");
        break;
      case DEBUGCMD_DUMPENG:
        System->Printf (MSG_CONSOLE,
		"Dumping entire engine contents to debug.txt.\n");
	Dump (Engine);
	System->Printf (MSG_DEBUG_0F, "\n");
        break;
      case DEBUGCMD_DUMPSEC:
        System->Printf (MSG_CONSOLE, "Not implemented yet.\n");
        break;
      case DEBUGCMD_CLEAR:
        do_clear = !do_clear;
        System->Printf (MSG_CONSOLE, "BugPlug %s screen clearing.\n",
	  	do_clear ? "enabled" : "disabled");
        break;
      case DEBUGCMD_EDGES:
        ToggleG3DState (G3DRENDERSTATE_EDGES, "edge drawing");
        break;
      case DEBUGCMD_TEXTURE:
        ToggleG3DState (G3DRENDERSTATE_TEXTUREMAPPINGENABLE, "texture mapping");
        break;
      case DEBUGCMD_BILINEAR:
        ToggleG3DState (G3DRENDERSTATE_BILINEARMAPPINGENABLE,
		"bi-linear filtering");
	break;
      case DEBUGCMD_TRILINEAR:
        ToggleG3DState (G3DRENDERSTATE_TRILINEARMAPPINGENABLE,
		"tri-linear filtering");
	break;
      case DEBUGCMD_LIGHTING:
        ToggleG3DState (G3DRENDERSTATE_LIGHTINGENABLE, "lighting");
	break;
      case DEBUGCMD_GOURAUD:
        ToggleG3DState (G3DRENDERSTATE_GOURAUDENABLE, "gouraud shading");
	break;
      case DEBUGCMD_ILACE:
        ToggleG3DState (G3DRENDERSTATE_INTERLACINGENABLE, "interlaced mode");
	break;
      case DEBUGCMD_MMX:
        ToggleG3DState (G3DRENDERSTATE_MMXENABLE, "mmx mode");
	break;
      case DEBUGCMD_TRANSP:
        ToggleG3DState (G3DRENDERSTATE_TRANSPARENCYENABLE, "transp mode");
        break;
      case DEBUGCMD_CACHECLEAR:
        G3D->ClearCache ();
        System->Printf (MSG_CONSOLE, "BugPlug cleared the texture cache.\n");
        break;
      case DEBUGCMD_CACHEDUMP:
        G3D->DumpCache ();
        break;
      case DEBUGCMD_MIPMAP:
        {
	  char* choices[6] = { "on", "off", "1", "2", "3", NULL };
	  long v = G3D->GetRenderState (G3DRENDERSTATE_MIPMAPENABLE);
	  v = (v+1)%5;
	  G3D->SetRenderState (G3DRENDERSTATE_MIPMAPENABLE, v);
	  System->Printf (MSG_CONSOLE, "BugPlug set mipmap to '%s'\n",
	  	choices[v]);
  	}
	break;
      case DEBUGCMD_INTER:
	{
	  char* choices[5] = { "smart", "step32", "step16", "step8", NULL };
	  long v = G3D->GetRenderState (G3DRENDERSTATE_INTERPOLATIONSTEP);
	  v = (v+1)%4;
	  G3D->SetRenderState (G3DRENDERSTATE_INTERPOLATIONSTEP, v);
	  System->Printf (MSG_CONSOLE, "BugPlug set interpolation to '%s'\n",
	  	choices[v]);
	}
	break;
    }
    process_next_key = false;
  }
  return true;
}

bool csBugPlug::HandleStartFrame (iEvent& /*event*/)
{
  SetupPlugin ();
  if (do_clear)
  {
    G3D->BeginDraw (CSDRAW_2DGRAPHICS);
    int bgcolor_clear = G3D->GetTextureManager ()->FindRGB (0, 255, 255);
    G2D->Clear (bgcolor_clear);
  }
  return false;
}

bool csBugPlug::HandleEndFrame (iEvent& /*event*/)
{
  SetupPlugin ();
  return false;
}

int csBugPlug::GetKeyCode (const char* keystring, bool& shift, bool& alt,
	bool& ctrl)
{
  shift = alt = ctrl = false;
  char* dash = strchr (keystring, '-');
  while (dash)
  {
    if (!strncmp (keystring, "shift", int (dash-keystring))) shift = true;
    else if (!strncmp (keystring, "alt", int (dash-keystring))) alt = true;
    else if (!strncmp (keystring, "ctrl", int (dash-keystring))) ctrl = true;
    keystring = dash+1;
    dash = strchr (keystring, '-');
  }

  int keycode = -1;
  if (!strcmp (keystring, "tab")) keycode = CSKEY_TAB;
  else if (!strcmp (keystring, "space")) keycode = ' ';
  else if (!strcmp (keystring, "esc")) keycode = CSKEY_ESC;
  else if (!strcmp (keystring, "enter")) keycode = CSKEY_ENTER;
  else if (!strcmp (keystring, "bs")) keycode = CSKEY_BACKSPACE;
  else if (!strcmp (keystring, "up")) keycode = CSKEY_UP;
  else if (!strcmp (keystring, "down")) keycode = CSKEY_DOWN;
  else if (!strcmp (keystring, "right")) keycode = CSKEY_RIGHT;
  else if (!strcmp (keystring, "left")) keycode = CSKEY_LEFT;
  else if (!strcmp (keystring, "pgup")) keycode = CSKEY_PGUP;
  else if (!strcmp (keystring, "pgdn")) keycode = CSKEY_PGDN;
  else if (!strcmp (keystring, "home")) keycode = CSKEY_HOME;
  else if (!strcmp (keystring, "end")) keycode = CSKEY_END;
  else if (!strcmp (keystring, "ins")) keycode = CSKEY_INS;
  else if (!strcmp (keystring, "del")) keycode = CSKEY_DEL;
  else if (!strcmp (keystring, "f1")) keycode = CSKEY_F1;
  else if (!strcmp (keystring, "f2")) keycode = CSKEY_F2;
  else if (!strcmp (keystring, "f3")) keycode = CSKEY_F3;
  else if (!strcmp (keystring, "f4")) keycode = CSKEY_F4;
  else if (!strcmp (keystring, "f5")) keycode = CSKEY_F5;
  else if (!strcmp (keystring, "f6")) keycode = CSKEY_F6;
  else if (!strcmp (keystring, "f7")) keycode = CSKEY_F7;
  else if (!strcmp (keystring, "f8")) keycode = CSKEY_F8;
  else if (!strcmp (keystring, "f9")) keycode = CSKEY_F9;
  else if (!strcmp (keystring, "f10")) keycode = CSKEY_F10;
  else if (!strcmp (keystring, "f11")) keycode = CSKEY_F11;
  else if (!strcmp (keystring, "f12")) keycode = CSKEY_F12;
  else if (*(keystring+1) != 0) return -1;
  else if ((*keystring >= 'A' && *keystring <= 'Z')
  	|| strchr ("!@#$%^&*()_+", *keystring))
  {
    shift = 1;
    keycode = *keystring;
  }
  else
    keycode = *keystring;

  return keycode;
}

int csBugPlug::GetCommandCode (const char* cmd)
{
  if (!strcmp (cmd, "debugenter"))	return DEBUGCMD_DEBUGENTER;
  if (!strcmp (cmd, "quit"))		return DEBUGCMD_QUIT;
  if (!strcmp (cmd, "status"))		return DEBUGCMD_STATUS;
  if (!strcmp (cmd, "help"))		return DEBUGCMD_HELP;

  if (!strcmp (cmd, "dumpeng"))		return DEBUGCMD_DUMPENG;
  if (!strcmp (cmd, "dumpsec"))		return DEBUGCMD_DUMPSEC;
  if (!strcmp (cmd, "edges"))		return DEBUGCMD_EDGES;
  if (!strcmp (cmd, "clear"))		return DEBUGCMD_CLEAR;
  if (!strcmp (cmd, "cacheclear"))	return DEBUGCMD_CACHECLEAR;
  if (!strcmp (cmd, "cachedump"))	return DEBUGCMD_CACHEDUMP;
  if (!strcmp (cmd, "texture"))		return DEBUGCMD_TEXTURE;
  if (!strcmp (cmd, "bilinear"))	return DEBUGCMD_BILINEAR;
  if (!strcmp (cmd, "trilinear"))	return DEBUGCMD_TRILINEAR;
  if (!strcmp (cmd, "lighting"))	return DEBUGCMD_LIGHTING;
  if (!strcmp (cmd, "gouraud"))		return DEBUGCMD_GOURAUD;
  if (!strcmp (cmd, "ilace"))		return DEBUGCMD_ILACE;
  if (!strcmp (cmd, "mmx"))		return DEBUGCMD_MMX;
  if (!strcmp (cmd, "transp"))		return DEBUGCMD_TRANSP;
  if (!strcmp (cmd, "mipmap"))		return DEBUGCMD_MIPMAP;
  if (!strcmp (cmd, "inter"))		return DEBUGCMD_INTER;

  return DEBUGCMD_UNKNOWN;
}

int csBugPlug::GetCommandCode (int key, bool shift, bool alt, bool ctrl)
{
  csKeyMap* m = mappings;
  while (m)
  {
    if (m->key == key && m->shift == shift && m->alt == alt && m->ctrl == ctrl)
      return m->cmd;
    m = m->next;
  }
  return DEBUGCMD_UNKNOWN;
}

void csBugPlug::AddCommand (const char* keystring, const char* cmdstring)
{
  bool shift, alt, ctrl;
  int keycode = GetKeyCode (keystring, shift, alt, ctrl);
  // Check if valid key name.
  if (keycode == -1) return;

  int cmdcode = GetCommandCode (cmdstring);
  // Check if valid command name.
  if (cmdcode == DEBUGCMD_UNKNOWN) return;

  // Check if key isn't already defined.
  if (GetCommandCode (keycode, shift, alt, ctrl) != DEBUGCMD_UNKNOWN) return;

  // Make new key assignment.
  csKeyMap* map = new csKeyMap ();
  map->key = keycode;
  map->shift = shift;
  map->alt = alt;
  map->ctrl = ctrl;
  map->cmd = cmdcode;
  map->next = mappings;
  if (mappings) mappings->prev = map;
  map->prev = NULL;
  mappings = map;
}

bool csBugPlug::ReadLine (iFile* file, char* buf, int nbytes)
{
  if (!file)
    return false;

  char c = '\n';
  while (c == '\n' || c == '\r')
    if (!file->Read (&c, 1))
      break;

  if (file->AtEOF())
    return false;

  char* p = buf;
  const char* plim = p + nbytes - 1;
  while (p < plim)
  {
    if (c == '\n' || c == '\r')
      break;
    *p++ = c;
    if (!file->Read (&c, 1))
      break;
  }
  *p = '\0';
  return true;
}

void csBugPlug::ReadKeyBindings (const char* filename)
{
  iFile* f = VFS->Open (filename, VFS_FILE_READ);
  if (f)
  {
    char buf[256];
    while (ReadLine (f, buf, 255))
    {
      char* del = strchr (buf, '=');
      if (del)
      {
        *del = 0;
	AddCommand (buf, del+1);
      }
      else
      {
        System->Printf (MSG_WARNING,
    	  "BugPlug hit a badly formed line in '%s'!\n", filename);
	f->DecRef ();
        return;
      }
    }
    f->DecRef ();
  }
  else
  {
    System->Printf (MSG_WARNING,
    	"BugPlug could not read '%s'!\n", filename);
  }
}

void csBugPlug::Dump (iEngine* engine)
{
  System->Printf (MSG_DEBUG_0, "===========================================\n");
  System->Printf (MSG_DEBUG_0,
    "%d sectors, %d mesh factories, %d mesh objects\n",
    engine->GetSectorCount (),
    engine->GetNumMeshFactories (),
    engine->GetNumMeshObjects ());
  int i;
  for (i = 0 ; i < engine->GetSectorCount () ; i++)
  {
    iSector* sector = engine->GetSector (i);
    Dump (sector);
  }
  for (i = 0 ; i < engine->GetNumMeshFactories () ; i++)
  {
    iMeshFactoryWrapper* meshfact = engine->GetMeshFactory (i);
    Dump (meshfact);
  }
  for (i = 0 ; i < engine->GetNumMeshObjects () ; i++)
  {
    iMeshWrapper* mesh = engine->GetMeshObject (i);
    Dump (mesh);
  }
  System->Printf (MSG_DEBUG_0, "===========================================\n");
}

void csBugPlug::Dump (iSector* sector)
{
  const char* sn = sector->QueryObject ()->GetName ();
  System->Printf (MSG_DEBUG_0, "    Sector '%s' (%08lx)\n",
  	sn ? sn : "?", sector);
  System->Printf (MSG_DEBUG_0, "    %d meshes, %d lights\n",
  	sector->GetMeshCount (), sector->GetLightCount ());
  int i;
  for (i = 0 ; i < sector->GetMeshCount () ; i++)
  {
    iMeshWrapper* mesh = sector->GetMesh (i);
    const char* n = mesh->QueryObject ()->GetName ();
    System->Printf (MSG_DEBUG_0, "        Mesh '%s' (%08lx)\n",
    	n ? n : "?", mesh);
  }
}

void csBugPlug::Dump (iMeshWrapper* mesh)
{
  const char* mn = mesh->QueryObject ()->GetName ();
  System->Printf (MSG_DEBUG_0, "    Mesh wrapper '%s' (%08lx)\n",
  	mn ? mn : "?", mesh);
  iMeshObject* obj = mesh->GetMeshObject ();
  if (!obj)
  {
    System->Printf (MSG_DEBUG_0, "        Mesh object missing!\n");
  }
  else
  {
    iFactory* fact = QUERY_INTERFACE (obj, iFactory);
    if (fact)
    {
      System->Printf (MSG_DEBUG_0, "        Plugin '%s'\n",
  	  fact->QueryDescription () ? fact->QueryDescription () : "NULL");
      fact->DecRef ();
    }
    csBox3 bbox;
    obj->GetObjectBoundingBox (bbox);
    System->Printf (MSG_DEBUG_0, "        Object bounding box:\n");
    Dump (8, bbox);
  }
  iMovable* movable = mesh->GetMovable ();
  if (!movable)
  {
    System->Printf (MSG_DEBUG_0, "        Mesh object missing!\n");
  }
  else
  {
    csReversibleTransform& trans = movable->GetTransform ();
    Dump (8, trans.GetOrigin (), "Movable origin");
    Dump (8, trans.GetO2T (), "Movable O2T");
    int cnt = movable->GetSectorCount ();
    int i;
    for (i = 0 ; i < cnt ; i++)
    {
      iSector* sec = movable->GetSector (i);
      const char* sn = sec->QueryObject ()->GetName ();
      System->Printf (MSG_DEBUG_0, "        In sector '%s'\n",
      	sn ? sn : "?");
    }
  }
}

void csBugPlug::Dump (iMeshFactoryWrapper* meshfact)
{
  const char* mn = meshfact->QueryObject ()->GetName ();
  System->Printf (MSG_DEBUG_0, "        Mesh factory wrapper '%s' (%08lx)\n",
  	mn ? mn : "?", meshfact);
}

void csBugPlug::Dump (int indent, const csMatrix3& m, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0, "%sMatrix '%s':\n", ind, name);
  System->Printf (MSG_DEBUG_0, "%s/\n", ind);
  System->Printf (MSG_DEBUG_0, "%s| %3.2f %3.2f %3.2f\n",
  	ind, m.m11, m.m12, m.m13);
  System->Printf (MSG_DEBUG_0, "%s| %3.2f %3.2f %3.2f\n",
  	ind, m.m21, m.m22, m.m23);
  System->Printf (MSG_DEBUG_0, "%s| %3.2f %3.2f %3.2f\n",
  	ind, m.m31, m.m32, m.m33);
  System->Printf (MSG_DEBUG_0, "%s\\\n", ind);
}

void csBugPlug::Dump (int indent, const csVector3& v, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0,
  	"%sVector '%s': (%f,%f,%f)\n", ind, name, v.x, v.y, v.z);
}

void csBugPlug::Dump (int indent, const csVector2& v, char const* name)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0, "%sVector '%s': (%f,%f)\n",
  	ind, name, v.x, v.y);
}

void csBugPlug::Dump (int indent, const csPlane3& p)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0, "%sA=%2.2f B=%2.2f C=%2.2f D=%2.2f\n",
            ind, p.norm.x, p.norm.y, p.norm.z, p.DD);
}

void csBugPlug::Dump (int indent, const csBox2& b)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0, "%s(%2.2f,%2.2f)-(%2.2f,%2.2f)\n", ind,
  	b.MinX (), b.MinY (), b.MaxX (), b.MaxY ());
}

void csBugPlug::Dump (int indent, const csBox3& b)
{
  char ind[255];
  int i;
  for (i = 0 ; i < indent ; i++) ind[i] = ' ';
  ind[i] = 0;
  System->Printf (MSG_DEBUG_0, "%s(%2.2f,%2.2f,%2.2f)-(%2.2f,%2.2f,%2.2f)\n",
  	ind, b.MinX (), b.MinY (), b.MinZ (), b.MaxX (), b.MaxY (), b.MaxZ ());
}


bool csBugPlug::HandleEvent (iEvent& event)
{
  if (event.Type == csevKeyDown)
  {
    return EatKey (event);
  }
  else if (event.Type == csevKeyUp)
  {
    return EatKey (event);
  }
  else if (event.Type == csevBroadcast)
  {
    if (event.Command.Code == cscmdPreProcess)
    {
      return HandleStartFrame (event);
    }
    if (event.Command.Code == cscmdPostProcess)
    {
      return HandleEndFrame (event);
    }
  }

  return false;
}

