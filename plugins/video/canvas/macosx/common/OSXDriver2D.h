//
//  OSXDriver2D.h
//
//
//  Created by mreda on Wed Oct 31 2001.
//  Copyright (c) 2001 Matt Reda. All rights reserved.
//


#ifndef __CS_OSXDRIVER2D_H__
#define __CS_OSXDRIVER2D_H__

#include "csgeom/csrect.h"
#include "csutil/macosx/OSXAssistant.h"
#include "iutil/eventh.h"
#include "ivideo/graph2d.h"
#include "plugins/video/canvas/common/graph2d.h"

#import <Cocoa/Cocoa.h>
#import <CoreFoundation/CoreFoundation.h>

@class OSXDelegate2D;
@class OSXCanvasView;

// Table for storing gamma values
struct GammaTable
{
  float r[256];
  float g[256];
  float b[256];
};


class OSXDriver2D
{
public:
  // Constructor
  OSXDriver2D(csGraphics2D *inCanvas);

  // Destructor
  virtual ~OSXDriver2D();

  // Initialize 2D plugin
  virtual bool Initialize(iObjectRegistry *reg);

  // Open graphics system (set mode, open window, etc)
  virtual bool Open();

  // Close graphics system
  virtual void Close();

  // Flip video page (or dump to framebuffer) - pure virtual
  virtual void Print(csRect const* area = 0) = 0;

  // Pure virtual function - the driver must invlude code to handle resizing
  virtual bool Resize(int w, int h) = 0;

  // Handle an event
  virtual bool HandleEvent(iEvent &ev);

  // Dispatch an event to the assistant
  void DispatchEvent(NSEvent *ev, OSXCanvasView *view);

  // Show/Hide the mouse
  virtual void HideMouse();
  virtual void ShowMouse();
  bool MouseIsHidden();
  
  // Event handler
  struct EventHandler : public iEventHandler
  {
  private:
    OSXDriver2D *parent;
  public:
    SCF_DECLARE_IBASE;
    EventHandler(OSXDriver2D *p)
    {
      SCF_CONSTRUCT_IBASE(0);
      parent = p;
    };
    virtual ~EventHandler()
    {
      SCF_DESTRUCT_IBASE();
    };
    virtual bool HandleEvent (iEvent& e) { return parent->HandleEvent(e); }
  } *scfiEventHandler;

protected:
  // Initialize pixel format for 16 bit depth
  void Initialize16();

  // Initialize pixel format for 32 bit depth
  void Initialize32();

  // Switch to fullscreen mode
  bool EnterFullscreenMode();

  // Switch out of fullscreen mode, to mode stored in originalMode
  void ExitFullscreenMode();

  // Toggle current state of fullscreen
  virtual bool ToggleFullscreen();

  // Uses CoreGraphics to fade to a given color 
  void FadeToRGB(CGDirectDisplayID disp, float r, float g, float b);
  
  // Fade to a given gamma table
  void FadeToGammaTable(CGDirectDisplayID disp, GammaTable table);
  
  // Save the current gamma values to the given table
  void SaveGamma(CGDirectDisplayID disp, GammaTable &table);

  // Choose which display to use
  void ChooseDisplay();

  // Set the window's title
  void SetTitle(char *newTitle);

  // Set the mouse position
  bool SetMousePosition(int x, int y);

  // Set the mouse cursor
  bool OSXDriver2D::SetMouseCursor(csMouseCursorID cursor);
  
  // Create the window with the specified properties
  BOOL openWindow(char *winTitle, int w, int h, int d, BOOL fs, 
                  CGDirectDisplayID display, int screen);
  
  CFDictionaryRef originalMode;		// Original display mode
  GammaTable originalGamma;		// Original gamma values
  bool inFullscreenMode;		// In full-screen mode
  CGDirectDisplayID display;		// Screen to display on
  uint32_t screen;			// Screen number to display on
  
  int origWidth, origHeight;		// Original dimensions kept so they can
					// be restored when switching modes

  OSXDelegate2D *delegate;		// Delegate
  csGraphics2D *canvas;			// Canvas (parent class)

  csRef<iOSXAssistant> assistant;	// Assistant for dispatching events
  iObjectRegistry *objectReg;		// Object registry
  
  BOOL hideMouse;                       // YES if mouse is not visible

  // Window - created even in fullscreen mode to get events (but with a different style)
  // Window can have one of two titles - Paused or active
  NSWindow *window;
  int style;
  NSString *title, *pausedTitle;

  // Is window paused (out of focus, etc)
  BOOL isPaused;

  // Last processed event type.
  int lastEventType;
  
};

#endif // __CS_OSXDRIVER2D_H__
