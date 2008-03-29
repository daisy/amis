#ifndef AudioSequence_H
#define AudioSequence_H

#pragma once

#include "AudioSequenceComponent.h"


//#include <atlstr.h>
#include <atlcoll.h>


#include "Media.h"


namespace amis
{
namespace gui
{
namespace spoken
{

class AudioSequence
{
public:
	AudioSequence();
	~AudioSequence();
	UINT GetCount();
	bool IsEmpty();
	void appendAll(AudioSequence * seq);
	AudioSequenceComponent* RemoveTail();
	void AddTail(AudioSequenceComponent* comp);
	void append(CString strTTS);
	void prepend(CString strTTS);
	void append(amis::AudioNode* audioClip, CString strTTS);
	AudioSequence* clone();
	CAtlList<AudioSequenceComponent*> m_AudioSequence;
private:

	void append_prepend(CString strTTS, bool prepend) ;
};


}
}
}
#endif
