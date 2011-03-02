/*
  Copyright (C) 2011 Christian Van Brussel, Communications and Remote
      Sensing Laboratory of the School of Engineering at the 
      Universite catholique de Louvain, Belgium
      http://www.tele.ucl.ac.be

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "cstool/animnodetmpl.h"

using namespace CS::Animation;

// ------------------------------   csSkeletonAnimNodeFactory   ------------------------------

csSkeletonAnimNodeFactory::csSkeletonAnimNodeFactory (const char* name)
  : name (name)
{}

const char* csSkeletonAnimNodeFactory::GetNodeName () const
{
  return name;
}

// ------------------------------   csSkeletonAnimNodeFactorySingle   ------------------------------

csSkeletonAnimNodeFactorySingle::csSkeletonAnimNodeFactorySingle (const char* name)
  : csSkeletonAnimNodeFactory (name)
{}

void csSkeletonAnimNodeFactorySingle::SetChildNode (iSkeletonAnimNodeFactory* factory)
{
  childNodeFactory = factory;
}

iSkeletonAnimNodeFactory* csSkeletonAnimNodeFactorySingle::GetChildNode () const
{
  return childNodeFactory;
}

// ------------------------------   csSkeletonAnimNodeSingle   ------------------------------

SkeletonAnimNodeSingle::SkeletonAnimNodeSingle (iSkeleton* skeleton)
  : skeleton (skeleton), isPlaying (false), playbackSpeed (1.0f)
{}

iSkeletonAnimNode* SkeletonAnimNodeSingle::GetChildNode () const
{
  return childNode;
}

void SkeletonAnimNodeSingle::Play ()
{
  if (isPlaying)
    return;

  if (childNode)
    childNode->Play ();

  isPlaying = true;
}

void SkeletonAnimNodeSingle::Stop ()
{
  if (!isPlaying)
    return;

  if (childNode)
    childNode->Stop ();

  isPlaying = false;
}

void SkeletonAnimNodeSingle::SetPlaybackPosition (float time)
{
  if (childNode)
    childNode->SetPlaybackPosition (time);
}

float SkeletonAnimNodeSingle::GetPlaybackPosition () const
{
  if (childNode)
    return childNode->GetPlaybackPosition ();

  return 0.0f;
}

float SkeletonAnimNodeSingle::GetDuration () const
{
  if (childNode)
    return childNode->GetDuration ();

  return 0.0f;
}

void SkeletonAnimNodeSingle::SetPlaybackSpeed (float speed)
{
  playbackSpeed = speed;

  if (childNode)
    childNode->SetPlaybackSpeed (speed);
}

float SkeletonAnimNodeSingle::GetPlaybackSpeed () const
{
  return playbackSpeed;
}

void SkeletonAnimNodeSingle::BlendState (csSkeletalState* state,
					   float baseWeight)
{
  if (childNode)
    childNode->BlendState (state, baseWeight);
}

void SkeletonAnimNodeSingle::TickAnimation (float dt)
{
  if (childNode)
    childNode->TickAnimation (dt);
}

bool SkeletonAnimNodeSingle::IsActive () const
{
  return isPlaying;
}

void SkeletonAnimNodeSingle::AddAnimationCallback (iSkeletonAnimCallback* callback)
{
  if (childNode)
    childNode->AddAnimationCallback (callback);
}

void SkeletonAnimNodeSingle::RemoveAnimationCallback (iSkeletonAnimCallback* callback)
{
  if (childNode)
    childNode->RemoveAnimationCallback (callback);
}

// ------------------------------   csSkeletonAnimNodeFactoryMulti   ------------------------------

csSkeletonAnimNodeFactoryMulti::csSkeletonAnimNodeFactoryMulti (const char* name)
  : csSkeletonAnimNodeFactory (name)
{}

void csSkeletonAnimNodeFactoryMulti::AddChildNode (iSkeletonAnimNodeFactory* factory)
{
  childNodeFactories.Push (factory);
}

void csSkeletonAnimNodeFactoryMulti::RemoveChildNode (iSkeletonAnimNodeFactory* factory)
{
  childNodeFactories.Delete (factory);
}

void csSkeletonAnimNodeFactoryMulti::ClearChildNodes ()
{
  childNodeFactories.DeleteAll ();
}

iSkeletonAnimNodeFactory* csSkeletonAnimNodeFactoryMulti::GetChildNode (size_t index) const
{
  CS_ASSERT (index < childNodeFactories.GetSize ());

  return childNodeFactories[index];
}

