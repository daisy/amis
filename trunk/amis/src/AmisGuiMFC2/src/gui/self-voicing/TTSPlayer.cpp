/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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


#define _WIN32_DCOM
#define WINVER 0x0400

#include "gui/self-voicing/TTSPlayer.h"

#include <stdlib.h>
#include <sapi.h>
#include <sphelper.h>
#include <spuihelp.h>

#include "Preferences.h"
#include "../../AmisGuiMFC2/resource.h"
#include "util/Log.h"

#ifndef TRACE
#define TRACE ATLTRACE
#endif

using namespace amis::tts;

static void __stdcall SpkCallback(WPARAM wParam, LPARAM lParam);

static ISpVoice* m_iV; // Voice Interface

void TTSPlayer::setCallback(sendMessageCallbackFn pFunction)
{
	sendMessageCallback = pFunction;
}

TTSPlayer* TTSPlayer::pinstance = 0;

TTSPlayer* TTSPlayer::Instance()
{
	if (pinstance == 0)  // is it the first call?
	{  
		pinstance = new TTSPlayer; // create sole instance
	}
	return pinstance; // address of sole instance
}

// between -10 and 10. 0 is default rate (engine dependant).
// Error value is LONG_MAX.
long TTSPlayer::GetSpeechRate()
{
	long pRate;
	HRESULT hr = 0;
	hr = m_iV->GetRate(&pRate);
	if (SUCCEEDED(hr))
	{
		return pRate;
	}
	else if (hr == E_POINTER) 
	{
		return LONG_MAX;
	}
	else
	{
		_ASSERT(0);
		return LONG_MAX; // Should never happen !
	}
}

// between -10 and 10. 0 is default rate (engine dependant).
bool TTSPlayer::SetSpeechRate(long newRate)
{

	HRESULT hr = 0;
	hr = m_iV->SetRate(newRate);
	if (SUCCEEDED(hr))
	{
		return true;
	}
	else if (hr == E_INVALIDARG)
	{
		return false;
	}
	else
	{
		_ASSERT(0);
		return false; // Should never happen !
	}
}
#ifdef USE_WSTRING
void TTSPlayer::Play(std::wstring str)
{
#else
void TTSPlayer::Play(CString str)
{
	USES_CONVERSION;
#endif
	EnterCriticalSection(&m_csSequence);
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace(T2A(str), "TTSPlayer::Play");
	TRACE(str);
	TRACE("\n");
	if (IsSpeaking())
	{
		p_log->writeMessage("IsSpeaking", "TTSPlayer::Play");
		TRACE("IsSpeaking\n");
	}
	mbDoNotProcessEndEvent = true;

	m_iV->Speak(NULL, SPF_PURGEBEFORESPEAK, NULL);
	WaitUntilDone();

	mbDoNotProcessEndEvent = false;

#ifdef USE_WSTRING
	m_iV->Speak(str, SPF_ASYNC, NULL);	
#else

	//LPCWSTR str_ = T2W(str);
	m_iV->Speak(str, SPF_ASYNC, NULL);	
#endif
	LeaveCriticalSection(&m_csSequence);
}

void TTSPlayer::Stop()
{
	EnterCriticalSection(&m_csSequence);

	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("Stop TTS", "TTSPlayer::Stop");
	TRACE(_T("Stop TTS\r\n") );

	mbDoNotProcessEndEvent = true;
	m_iV->Speak(NULL, SPF_PURGEBEFORESPEAK, NULL);
	//m_iV->Speak(L"", SPF_PURGEBEFORESPEAK, NULL);
	//m_iV->Speak(L"", SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);

	WaitUntilDone();
	m_isSpeaking = FALSE;

	LeaveCriticalSection(&m_csSequence);
}

void TTSPlayer::WaitUntilDone()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("TTS before wait", "TTSPlayer::WaitUntilDone");
	TRACE(_T("//// TTS BEFORE WAIT \r\n") );
	m_iV->WaitUntilDone(INFINITE);

	p_log->writeTrace("TTS after wait", "TTSPlayer::WaitUntilDone");
	TRACE(_T("//// TTS AFTER WAIT \r\n") );
}

void __stdcall SpkCallback(WPARAM wParam, LPARAM lParam)
{
	((TTSPlayer *)lParam)->callback();
}

void TTSPlayer::callback()
{
	CSpEvent        event;  // helper class in sphelper.h for events that releases any 
	// allocated memory in it's destructor - SAFER than SPEVENT
	//SPVOICESTATUS   Stat;
	WPARAM          nStart;
	LPARAM          nEnd;
	int             i = 0;
	HRESULT 		hr = S_OK;

	//ISpVoice* m_iV = TTSPlayer::Instance()->m_iV;
	//ISpVoice* m_iV = ((TTSPlayer *)lParam)->m_iV;

	amis::util::Log* p_log = amis::util::Log::Instance();
	while( event.GetFrom(m_iV) == S_OK )
	{
		switch( event.eEventId )
		{
			case SPEI_START_INPUT_STREAM:
			{
				p_log->writeTrace("StartStream event", "TTSPlayer::callback");
				TRACE(_T("\nStartStream event\r\n") );
				m_isSpeaking = TRUE;
				break; 
			}
			case SPEI_END_INPUT_STREAM:
			{
				if (m_isSpeaking)
				{
					m_isSpeaking = FALSE;
					if (mbDoNotProcessEndEvent)
					{
						p_log->writeTrace("EndStream 1 mbDoNotProcessEndEvent", "TTSPlayer::callback");
						TRACE(_T("\nEndStream 1 mbDoNotProcessEndEvent\r\n") );
						mbDoNotProcessEndEvent = false;
					}
					else
					{
						p_log->writeTrace("EndStream 2 sendMessageCallback", "TTSPlayer::callback");
						TRACE(_T("\nEndStream 2 sendMessageCallback\r\n") );
						sendMessageCallback();
					}
				}			
				break;     
			}
			case SPEI_VOICE_CHANGE:
			{
				p_log->writeTrace("Voicechange event", "TTSPlayer::callback");
				TRACE(_T("\nVoicechange event\r\n") );
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

bool TTSPlayer::IsSpeaking(void)
{
	return m_isSpeaking;
}

TTSPlayer::TTSPlayer(void)
: m_currentVoiceNumber(-1)
, m_isSpeaking(FALSE)
{
	mbDoNotProcessEndEvent = false;
	InitializeCriticalSection(&m_csSequence);

	m_iV = NULL;
	HRESULT hr = 0;
#ifdef _DEBUG
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr == S_FALSE) CoUninitialize();
	assert(hr == S_FALSE);
#endif

	hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER,
		IID_ISpVoice, (void**) &m_iV);
	_ASSERT(hr == S_OK);	// If this assertion is hit then the Speech runtime is probably not installed

	//HWND hWnd = amis::gui::MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
	//m_iV->SetNotifyWindowMessage( hWnd, WM_TTSAPPCUSTOMEVENT, 0, 0 );
	m_iV->SetNotifyCallbackFunction(SpkCallback, 0, (LPARAM)this );
	m_iV->SetInterest( SPFEI_ALL_TTS_EVENTS, SPFEI_ALL_TTS_EVENTS );

	ChangeVoice(amis::Preferences::Instance()->getTTSVoiceIndex());

	m_iV->SetVolume(70);
}

void TTSPlayer::DestroyInstance()
{
	if (pinstance != NULL) delete pinstance;
}

TTSPlayer::~TTSPlayer(void)
{
	mbDoNotProcessEndEvent = false;
	if (m_iV != NULL) m_iV->Release();
	DeleteCriticalSection(&m_csSequence);
}

int TTSPlayer::initVoiceList(HWND hWnd)
{
	HRESULT hr = SpInitTokenComboBox(GetDlgItem(hWnd, IDC_TTSVOICES ), SPCAT_VOICES);
	//HRESULT hr = SpInitTokenComboBox( GetDlgItem( IDC_TTSVOICE )->GetSafeHwnd(), SPCAT_VOICES );
	if (SUCCEEDED(hr)) return m_currentVoiceNumber;
	else return -1;
}

void TTSPlayer::ChangeVoice(int index)
{
	m_currentVoiceNumber = index-1; // Because ++ will be applied in ChangeVoice(). 0 is the start index for both 'index' and 'm_currentVoiceNumber'
	ChangeVoice(false);
}


std::string TTSPlayer::ChangeVoice(bool speakNotify)
{
	USES_CONVERSION;

	HRESULT                             hr = S_OK;
	CComPtr<ISpObjectToken>             cpVoiceToken;
	CComPtr<IEnumSpObjectTokens>        cpEnum;
	ULONG                               ulCount = 0;
	CSpDynamicString*                szDescription;

	//ISpObjectToken                  *pToken = NULL;
	//WCHAR *pszCurTokenId = NULL;

	if(SUCCEEDED(hr))
		hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

	if(SUCCEEDED(hr))
		hr = cpEnum->GetCount(&ulCount);

	m_currentVoiceNumber ++;
	if (m_currentVoiceNumber >= ulCount)
	{
		m_currentVoiceNumber = 0;
	}
	szDescription = new CSpDynamicString [ulCount];

	ULONG counter = -1;

	while (SUCCEEDED(hr) && ulCount -- )
	{
		counter ++;
		cpVoiceToken.Release();

		if(SUCCEEDED(hr))
			hr = cpEnum->Next( 1, &cpVoiceToken, NULL );
		HRESULT hResult = SpGetDescription(cpVoiceToken, &szDescription[counter]);

		if (counter == m_currentVoiceNumber) break;
	}

	if(SUCCEEDED(hr))
		hr = m_iV->SetVoice(cpVoiceToken);

	cpVoiceToken.Release();

	CString str = W2T(szDescription[m_currentVoiceNumber]);
	string str2 = W2CA(str);

	if(SUCCEEDED(hr) && speakNotify)
		Play( L"This is my new voice !");

	delete [] szDescription;
	return str2;
}