/*
    Copyright (C) 1998,1999 by Jorrit Tyberghein
    Overhauled and re-engineered by Eric Sunshine <sunshine@sunshineco.com>
  
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
		
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "sysdef.h"
#include "isystem.h"
#include "cs2d/openglbe/glbe2d.h"
#include "cs2d/openglbe/CrystGLWindow.h"
#include "cssys/be/beitf.h"

CrystGLView::CrystGLView(BRect frame, IBeLibSystemDriver* isys) :
	BGLView(frame, "", B_FOLLOW_NONE, 0, BGL_RGB | BGL_DEPTH | BGL_DOUBLE),
	be_system(isys)
{
	be_system->AddRef();
}

CrystGLView::~CrystGLView()
{
	be_system->Release();
}

void CrystGLView::ProcessUserEvent() const
{
	be_system->ProcessUserEvent(Looper()->CurrentMessage());
}

void CrystGLView::KeyDown(char const* bytes, int32 numBytes)
{
	ProcessUserEvent();
}

void CrystGLView::KeyUp(char const* bytes, int32 numBytes)
{
	ProcessUserEvent();
}

void CrystGLView::MouseMoved(BPoint point, uint32 transit, const BMessage* m)
{
	ProcessUserEvent();
}

void CrystGLView::MouseDown(BPoint point)
{
	ProcessUserEvent();
	if (!IsFocus())
		MakeFocus();
}

void CrystGLView::MouseUp(BPoint point)
{
	ProcessUserEvent();
}

void CrystGLView::AttachedToWindow()
{
	LockGL(); 
	BGLView::AttachedToWindow(); 
	UnlockGL();
}

CrystGLWindow::CrystGLWindow(BRect frame, const char* name, CrystGLView *v,
	csGraphics2DGLBe *piBeG2D, ISystem* isys, IBeLibSystemDriver* bsys) :
//	BWindow(frame,name, B_TITLED_WINDOW, B_NOT_RESIZABLE,0),
//	BGLScreen(name, B_16_BIT_640x480, 0, &res, 0),
	BDirectWindow(frame,name, B_TITLED_WINDOW, B_NOT_RESIZABLE, 0),
//	BWindowScreen(name, B_8_BIT_640x480, res, 0),
	view(v), cs_system(isys), be_system(bsys)
{
	cs_system->AddRef();
	be_system->AddRef();

	// Initialise local flags
#if 0
	fConnected = false;
	fConnectionDisabled = false;
	fDrawingThreadSuspended=false;
	locker = new BLocker(); // dh:remove for conventional DirectConnected
#endif
	
	// Cache the pointer to the 2D graphics driver object
	pi_BeG2D = piBeG2D;
	
	view->SetViewColor(0, 0, 0);
	AddChild(view);

	// Add a shortcut to switch in and out of fullscreen mode.
	AddShortcut('f', B_COMMAND_KEY, new BMessage('full'));
	
	// As we said before, the window shouldn't get wider than 2048 in any
	// direction, so those limits will do.
	SetSizeLimits(40.0, 2000.0, 40.0, 2000.0);
}

CrystGLWindow::~CrystGLWindow()
{
	Hide();
	Flush();
	be_system->Release();
	cs_system->Release();
}

bool CrystGLWindow::QuitRequested()
{
	pi_BeG2D->dpy->EnableDirectMode(false);
	cs_system->Shutdown();
	// FIXME: Don't destroy window before "LoopThread" has finished.
	return true;
}

void CrystGLWindow::MessageReceived(BMessage* m)
{
	switch(m->what) {
		case 'full':
			SetFullScreen(!IsFullScreen());
			break;
		default:
			BWindow::MessageReceived(m);
			break;
	}
}

void CrystGLWindow::DirectConnected(direct_buffer_info *info)
{
#if 0
printf("Entered CrystWindow::DirectConnected \n");
	if (!fConnected && fConnectionDisabled) {
		return S_OK;
	}
	locker->Lock();

	switch (info->buffer_state & B_DIRECT_MODE_MASK) {
		case B_DIRECT_START:
//			printf("DirectConnected: B_DIRECT_START \n");
			fConnected = true;
			if (fDrawingThreadSuspended)	{
				status_t retval;
				bool notdone=true;
				while (resume_thread(find_thread("LoopThread")) == B_BAD_THREAD_STATE)	{
					//	this is done to cope with thread setting fDrawingThreadSuspended then getting
					//	rescheduled before it can suspend itself.  It just makes repeated attempts to
					//	resume that thread.
					snooze(1000);
				}
				fDrawingThreadSuspended = false;
			}
				
		case B_DIRECT_MODIFY:
		break;
		
		case B_DIRECT_STOP:
//			printf("DirectConnected: B_DIRECT_STOP \n");
			fConnected = false;
		break;
	}
	
	locker->Unlock();
//    printf("leaving IXBeLibGraphicsInfo::DirectConnected \n");
	*/
//	let's try doing it with BGLView's DirectConnected method
	if (pi_BeG2D->dpy) {
		pi_BeG2D->dpy->DirectConnected(info);
		}
	pi_BeG2D->dpy->EnableDirectMode( true );// is this necessary
	
//	this bit just keeps conventional window behaviour until I've sorted out DirectConnected
//BDirectWindow::DirectConnected(info);
#endif
}
