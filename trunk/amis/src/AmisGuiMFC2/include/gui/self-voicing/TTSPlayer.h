#ifndef TTSPLAYER_H
#define TTSPLAYER_H

#pragma once

#include <string>

//#define USE_WSTRING

#ifdef USE_WSTRING
#include <string>
#else
#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif
#endif

namespace amis
{
namespace tts
{

class TTSPlayer
{
public:
	
	 typedef void (*sendMessageCallbackFn)(void);
	 
	 sendMessageCallbackFn sendMessageCallback;
	void setCallback(sendMessageCallbackFn pFunction);

	static void DestroyInstance();
	static TTSPlayer* Instance();	

#ifdef USE_WSTRING
	void Play(std::wstring str);
#else
	void Play(CString str);
#endif
	void Stop();
	
void callback();
void WaitUntilDone();

	long GetSpeechRate();
	bool SetSpeechRate(long newRate);
	
	std::string ChangeVoice(bool speakNotify);
	void ChangeVoice(int index);

	int initVoiceList(HWND);

bool IsSpeaking(void);

	bool m_isSpeaking;
	static TTSPlayer* pinstance;


	CRITICAL_SECTION m_csSequence;

	ULONG m_currentVoiceNumber;

	bool mbDoNotProcessEndEvent;
			TTSPlayer(void);
			~TTSPlayer(void);


};
}
}
#endif