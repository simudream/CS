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

/*
 * pysimp - A simple application demonstrating the usage of the python plugin.
 */

#include "pysimp.h"

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

PySimple::PySimple ()
{
  SetApplicationName ("CrystalSpace.PySimp");
}

PySimple::~PySimple ()
{
}

bool PySimple::OnInitialize (int /*argc*/, char* /*argv*/[])
{
  if (!csInitializer::RequestPlugins (GetObjectRegistry(),
  	CS_REQUEST_VFS,
	CS_REQUEST_OPENGL3D,
	CS_REQUEST_ENGINE,
	CS_REQUEST_FONTSERVER,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_LEVELLOADER,
  CS_REQUEST_REPORTER,
  CS_REQUEST_REPORTERLISTENER,
	CS_REQUEST_PLUGIN( "crystalspace.script.python", iScript ),
	CS_REQUEST_END))
    return ReportError ("Couldn't init app!");

  csBaseEventHandler::Initialize (GetObjectRegistry());

  if (!RegisterQueue (GetObjectRegistry(), 
    csevAllEvents (GetObjectRegistry())))
    return ReportError("Failed to set up event handler!");

  return true;
}

void PySimple::OnExit()
{
  // Shut down the event handlers we spawned earlier.
  drawer.Invalidate();
  printer.Invalidate();
}

bool PySimple::Application()
{
  // Open the main system. This will open all the previously loaded plug-ins.
  // i.e. all windows will be opened.
  if (!OpenApplication(GetObjectRegistry()))
    return ReportError("Error opening system!");

  if (SetupModules())
  {
    // This calls the default runloop. This will basically just keep
    // broadcasting process events to keep the game going.
    Run();
  }

  return true;
}

bool PySimple::SetupModules ()
{
  // Now get the pointer to various modules we need. We fetch them
  // from the object registry. The RequestPlugins() call we did earlier
  // registered all loaded plugins with the object registry.
  g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
  if (!g3d) return ReportError("Failed to locate 3D renderer!");

  engine = csQueryRegistry<iEngine> (GetObjectRegistry());
  if (!engine) return ReportError("Failed to locate 3D engine!");

  vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry());
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  kbd = csQueryRegistry<iKeyboardDriver> (GetObjectRegistry());
  if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

  loader = csQueryRegistry<iLoader> (GetObjectRegistry());
  if (!loader) return ReportError("Failed to locate Loader!");

  // We need a View to the virtual world.
  view.AttachNew(new csView (engine, g3d));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  // We use the full window to draw the world.
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  iNativeWindow* nw = g2d->GetNativeWindow ();
  if (nw) nw->SetTitle ("Simple Crystal Space Python Application");

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  engine->SetLightingCacheMode (0);

  // Here we create our world.
  CreateRoom();

  // Let the engine prepare all lightmaps for use and also free all images 
  // that were loaded for the texture manager.
  engine->Prepare ();
  rm = engine->GetRenderManager();

  // these are used store the current orientation of the camera
  rotY = rotX = 0;

  // Now we need to position the camera in our world.
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));

  // We use some other "helper" event handlers to handle 
  // pushing our work into the 3D engine and rendering it
  // to the screen.
  //drawer.AttachNew(new FrameBegin3DDraw (GetObjectRegistry (), view));
  printer.AttachNew(new FramePrinter (GetObjectRegistry ()));

  return true;
}

void PySimple::CreateRoom ()
{
  // Create our world.
  ReportInfo ("Creating world!...");

  loader->LoadTexture ("stone", "/lib/std/stone4.gif");
  iSector *room = engine->CreateSector ("room");

  csRef<iPluginManager> plugin_mgr (
    csQueryRegistry<iPluginManager> (GetObjectRegistry()));
  // Initialize the python plugin.
  csRef<iScript> is = csQueryRegistry<iScript> (GetObjectRegistry());
  if (is)
  {
    char const* module = "pysimp";
    csRef<iCommandLineParser> cmd =
      csQueryRegistry<iCommandLineParser> (GetObjectRegistry());
    if (cmd.IsValid())
    {
      char const* file = cmd->GetName(0);
      if (file != 0)
        module = file;
    }

    // Load a python module.
    ReportInfo ("Loading script file `%s'...", module);
    if (!is->LoadModule (module))
      return;

    // Set up our room.
    // Execute one method defined in pysimp.py
    // This will create the polygons in the room.
    csString run;
    run << module << ".CreateRoom";
    // prepare arguments
    csRefArray<iScriptValue> args;
    args.Push(csRef<iScriptValue>(is->RValue("stone")));
    // run method
    csRef<iScriptValue> ret = is->Call(run,args);
    if(!ret.IsValid())
    {
      ReportError ("Failed running '%s.CreateRoom'...",module);
    }
  }
  else
    ReportError ("Could not load Python plugin");

  csRef<iLight> light;
  light = engine->CreateLight (0, csVector3 (0, 5, 0), 10,
    csColor (1, 0, 0));
  room->GetLights ()->Add (light);
}

void PySimple::OnCommandLineHelp ()
{
  csPrintf("\nTo load a Python script other than the default `pysimp.py',\n"
	   "specify its name (without the .py extension) as the one and only\n"
	   "argument to pysimp. The script must define a Python function\n"
	   "named CreateRoom() which accepts a material name as its only\n"
	   "argument, and which sets up the geometry for a `room' in the\n"
	   "sector named \"room\". The specified script will be `imported',\n"
	   "so it must be found in Python's search path (possibly augmented\n"
	   "by PYTHONPATH).\n\n");
}

void PySimple::Frame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();
  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.0) * (0.06 * 20);

  iCamera* c = view->GetCamera();

  if (kbd->GetKeyState (CSKEY_SHIFT))
  {
    // If the user is holding down shift, the arrow keys will cause
    // the camera to strafe up, down, left or right from it's
    // current position.
    if (kbd->GetKeyState (CSKEY_RIGHT))
      c->Move (CS_VEC_RIGHT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_LEFT))
      c->Move (CS_VEC_LEFT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_UP))
      c->Move (CS_VEC_UP * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN))
      c->Move (CS_VEC_DOWN * 4 * speed);
  }
  else
  {
    // left and right cause the camera to rotate on the global Y
    // axis; page up and page down cause the camera to rotate on the
    // _camera's_ X axis (more on this in a second) and up and down
    // arrows cause the camera to go forwards and backwards.
    if (kbd->GetKeyState (CSKEY_RIGHT))
      rotY += speed;
    if (kbd->GetKeyState (CSKEY_LEFT))
      rotY -= speed;
    if (kbd->GetKeyState (CSKEY_PGUP))
      rotX += speed;
    if (kbd->GetKeyState (CSKEY_PGDN))
      rotX -= speed;
    if (kbd->GetKeyState (CSKEY_UP))
      c->Move (CS_VEC_FORWARD * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN))
      c->Move (CS_VEC_BACKWARD * 4 * speed);
  }

  // We now assign a new rotation transformation to the camera.  You
  // can think of the rotation this way: starting from the zero
  // position, you first rotate "rotY" radians on your Y axis to get
  // the first rotation.  From there you rotate "rotX" radians on the
  // your X axis to get the final rotation.  We multiply the
  // individual rotations on each axis together to get a single
  // rotation matrix.  The rotations are applied in right to left
  // order .
  csMatrix3 rot = csXRotMatrix3 (rotX) * csYRotMatrix3 (rotY);
  csOrthoTransform ot (rot, c->GetTransform().GetOrigin ());
  c->SetTransform (ot);

  rm->RenderView (view);
}

bool PySimple::OnKeyboard (iEvent& ev)
{
  // We got a keyboard event.
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
  if (eventtype == csKeyEventTypeDown)
  {
    // The user pressed a key (as opposed to releasing it).
    utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
    if (code == CSKEY_ESC)
    {
      // The user pressed escape to exit the application.
      // The proper way to quit a Crystal Space application
      // is by broadcasting a csevQuit event. That will cause the
      // main runloop to stop. To do that we get the event queue from
      // the object registry and then post the event.
      csRef<iEventQueue> q = 
        csQueryRegistry<iEventQueue> (GetObjectRegistry());
      if (q.IsValid()) q->GetEventOutlet()->Broadcast(
        csevQuit(GetObjectRegistry()));
    }
  }
  return false;
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  return csApplicationRunner<PySimple>::Run (argc, argv);
}
