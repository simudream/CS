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

#if !defined(__CSSOUNDLISTENERDS3D_H__)
#define __CSSOUNDLISTENERDS3D_H__

#include "cssndldr/common/sndbuf.h"
#include "isndrdr.h"
#include "isndlstn.h"

class csSoundListenerDS3D  : public ISoundListener
{
public:
	csSoundListenerDS3D();
	virtual ~csSoundListenerDS3D();

	STDMETHODIMP SetDirection(float fx, float fy, float fz, float tx, float ty, float tz);
	STDMETHODIMP SetPosition(float x, float y, float z);
  STDMETHODIMP SetVelocity(float x, float y, float z);
	STDMETHODIMP SetDistanceFactor(float factor);
  STDMETHODIMP SetRollOffFactor(float factor);
  STDMETHODIMP SetDopplerFactor(float factor);
  STDMETHODIMP SetHeadSize(float size);
  STDMETHODIMP SetEnvironment(SoundEnvironment env);

  STDMETHODIMP GetInfoListener(csSoundListenerInfo *info);

	DECLARE_IUNKNOWN()
	DECLARE_INTERFACE_TABLE(csSoundListenerDS3D)

public:
  csSoundListenerInfo info;

  int CreateListener(ISoundRender *render);
	int DestroyListener();

  LPDIRECTSOUNDBUFFER		m_pDS3DPrimaryBuffer;
	LPDIRECTSOUND3DLISTENER	m_pDS3DListener;
  LPDIRECTSOUND		m_p3DAudioRenderer;
};

#endif // __CSSOUNDLISTENERDS3D_H__
