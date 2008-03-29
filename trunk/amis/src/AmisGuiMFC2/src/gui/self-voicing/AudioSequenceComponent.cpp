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