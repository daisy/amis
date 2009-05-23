/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2004-2009  DAISY Consortium

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "gui/self-voicing/AudioSequenceComponent.h"

using namespace amis::gui::spoken;

AudioSequenceComponent::AudioSequenceComponent(){
		
	}
AudioSequenceComponent::~AudioSequenceComponent(){

	m_String.Empty();
		
			delete m_AudioClip;
		
		m_AudioClip = NULL;
	}

AudioSequenceComponent* AudioSequenceComponent::clone() {
		AudioSequenceComponent* comp = new AudioSequenceComponent;
		comp->m_AudioClip = (m_AudioClip ? (amis::AudioNode*)m_AudioClip->clone() : NULL);
		comp->m_String = m_String;
		comp->m_isAudioClip = m_isAudioClip;
		
		return comp;
}