/*
    Crystal Space input library
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Andrew Zabolotny <bit@eltech.ru>
  
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

#ifndef __CSINPUT_H__
#define __CSINPUT_H__

/**
 * These are the low-level interfaces to input devices like keyboard
 * and mouse (possibly joystick will be added in the future).
 * System-dependent code should inherit system-specific classes from
 * those defined below, implementing as much functionality as possible.
 */

#include "cscom/com.h"
#include "csutil/csbase.h"
#include "csinput/csevent.h"
#include "csinput/cseventq.h"

interface ISystem;

/**
 * Keyboard driver maintains a structure containing the current state of
 * major control keys (see below). It is NOT (unlike in releases before 008)
 * the responsability of the keyboard driver to maintain this structure,
 * it is maintained by do_keypress and do_keyrelease methods. Instead, it
 * is highly desirable (read: required) for driver to call do_keypress and
 * do_keyrelease with appropiate key codes (see CSKEY_XXX constants below).
 */
struct csKey
{
  bool up;
  bool down;
  bool left;
  bool right;
  bool pgup;
  bool pgdn;
  bool esc;
  bool alt;
  bool ctrl;
  bool shift;
  bool home;
  bool end;
  bool ins;
  bool del;
  bool enter;
  bool tab;
  bool backspace;
};

/**
 * This is the lowest-level interface to keyboard.<p>
 * Keyboard driver should generate events and put them into the
 * event queue which is passed to object on Open().
 */
class csKeyboardDriver
{
protected:
  /// Event queue which keyboard driver is bound to
  csEventQueue *EventQueue;

public:
  /// Partial key state: arrows, pgup/pgdn, ins/del, home/end, esc etc.
  csKey Key;
  /// Alphanumeric key state
  bool aKey[256];

  /// Initialize keyboard interface
  csKeyboardDriver ();
  /// Deinitialize keyboard interface
  virtual ~csKeyboardDriver ();

  /// Start receiving keyboard events
  virtual bool Open (csEventQueue *EvQueue);
  /// Finish receiving keyboard events
  virtual void Close ();

  /// Check if keyboard driver is ready
  virtual bool Ready ()
  { return (EventQueue != NULL); }

  /**
   * Call to release all key down flags
   * (when focus switches from application window, for example)
   */
  virtual void Reset ();

  /// Call this routine to add a key down event to queue
  void do_keypress (time_t evtime, int key);
  /// Call this routine to add a key up event to queue
  void do_keyrelease (time_t evtime, int key);

protected:
  /**
   * Set key state. For example SetKey (CSKEY_UP, true). Called
   * automatically by do_keypress and do_keyrelease.
   */
  void SetKey (int key, bool state);
};

/**
 * This is the lowest-level interface to mouse.<p>
 * Mouse driver should generate events and put them into the
 * event queue which is passed to object on Open().
 */
class csMouseDriver
{
protected:
  /// Event queue which keyboard driver is bound to
  csEventQueue *EventQueue;
  /// Last "mouse down" event time
  time_t LastClickTime;
  /// Last "mouse down" event button
  int LastClickButton;
  /// Last "mouse down" event position
  int LastClickX, LastClickY;
  /// Last mouse position
  int LastMouseX, LastMouseY;

public:
  /// Mouse buttons state (only 9 supported :-))
  bool Button[10];
  /// Mouse double click max interval in 1/1000 seconds
  static time_t DoubleClickTime;
  /// Mouse double click max distance
  static size_t DoubleClickDist;

  /// Initialize mouse interface
  csMouseDriver ();
  /// Deinitialize mouse interface
  virtual ~csMouseDriver ();

  /// Start receiving mouse events
  virtual bool Open (ISystem* System, csEventQueue *EvQueue);
  /// Finish receiving mouse events
  virtual void Close ();

  /// Check if mouse driver is ready
  virtual bool Ready ()
  { return (EventQueue != NULL); }

  /**
   * Call to release all mouse buttons
   * (when focus switches from application window, for example)
   */
  virtual void Reset ();

  // Query last mouse X position
  int GetLastX () { return LastMouseX; }
  // Query last mouse Y position
  int GetLastY () { return LastMouseY; }

  /// Call this to add a 'mouse button down' event to queue
  void do_buttonpress (time_t evtime, int button, int x, int y, bool shift, bool alt, bool ctrl);
  /// Call this to add a 'mouse button up' event to queue
  void do_buttonrelease (time_t evtime, int button, int x, int y);
  /// Call this to add a 'mouse moved' event to queue
  void do_mousemotion (time_t evtime, int x, int y);
};

#endif // __CSINPUT_H__
