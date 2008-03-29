#ifndef AudioSequenceComponent_H
#define AudioSequenceComponent_H

#pragma once

#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif


#include "Media.h"


namespace amis
{
namespace gui
{
namespace spoken
{

// --------------------------------
class AudioSequenceComponent
{
public:
	AudioSequenceComponent();
	~AudioSequenceComponent();
	AudioSequenceComponent* clone();
	BOOL m_isAudioClip;
	
	CString m_String;
	amis::AudioNode* m_AudioClip;
};


}
}
}
#endif