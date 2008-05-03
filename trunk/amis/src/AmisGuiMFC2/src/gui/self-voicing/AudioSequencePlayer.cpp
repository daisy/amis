

#include "gui/self-voicing/AudioSequencePlayer.h"

#include "gui/self-voicing/TTSPlayer.h"

#include "gui/AmisApp.h"

#include "util/FilePathTools.h"

#include "gui/self-voicing/directshow/dx_audio_player.h"


#include "Preferences.h"

#include <gui/MainWndParts.h>
#include "Media.h"

#include "gui/MmView.h"

//#include "ambulant/net/url.h"
#include "../../AmisGuiMFC2/resource.h"

#include "util/Log.h"

using namespace amis::tts;

using namespace amis::gui::spoken;

AudioSequencePlayer* AudioSequencePlayer::pinstance = 0;

#define CCS_ACTIVE



/*
unsigned __stdcall AudioSequencePlayer::ThreadProc(void* Arg) {
*/

DWORD WINAPI ThreadProc(LPVOID Arg) {
	USES_CONVERSION;

	_ASSERT(Arg != NULL);

	ULONG		nStream;
	AudioSequencePlayer*	pThis = (AudioSequencePlayer*) Arg;

	AudioSequenceComponent* comp = NULL;
	BOOL		bEmptyQueue;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr == S_FALSE) CoUninitialize();
	_ASSERT(hr == S_OK);

	while (! pThis->m_bAbort)
	{
		
	DWORD hr = WaitForSingleObject(pThis->m_hEventWakeup, INFINITE);
	switch (hr) {
	case WAIT_FAILED: {
		int i = 0;
		break;
					  }

		case WAIT_ABANDONED
			: {
				int i = 0;
				break;}

				case WAIT_OBJECT_0
					: {
						int i = 0;
						break;}

	case WAIT_TIMEOUT: 
		{
		int i = 0;
		break;
		}

	}
		
		/*
		if (pThis->m_wakeUpOriginator == 0) {
		break;
		} else if (pThis->m_wakeUpOriginator == 1) {

		if (AmisBrain::Instance()->isAudioClipPlaying()) {
		TRACE(L"### STOP AUDIO CLIP");
		ResetEvent(pThis->m_hEventWakeup);
		AmisBrain::Instance()->stopAudioClip();
		continue;
		} else 
		if (TTSPlayer::Instance()->IsSpeaking()) {	
		TRACE(L"### STOP TTS CLIP");
		ResetEvent(pThis->m_hEventWakeup);
		TTSPlayer::Instance()->Stop();
		//TTSPlayer::Instance()->WaitUntilDone();
		continue;
		} else {
		pThis->m_wakeUpOriginator = -1;
		}

		}
		*/

		/*
		if (pThis->m_wakeUpAction == 0) // INTERRUPT PROCESS BEGIN
		{
		if (pThis->m_wakeUpOriginator != 1) {
		TRACE(L"------------ ZZ_1");
		}

		pThis->m_wakeUpAction = 1; // TTS SHUTDOWN

		if (TTSPlayer::Instance()->IsSpeaking()) {	
		ResetEvent(pThis->m_hEventWakeup);
		TTSPlayer::Instance()->Stop();
		TTSPlayer::Instance()->WaitUntilDone();
		}

		//continue;
		}
		else if (pThis->m_wakeUpAction == 1) // TTS SHUTDOWN
		{

		if (pThis->m_wakeUpOriginator != 2) {
		TRACE(L"------------ ZZ_2");
		}

		pThis->m_wakeUpAction = 2; // AUDIO CLIP SHUTDOWN

		if (AmisBrain::Instance()->isAudioClipPlaying()) {
		ResetEvent(pThis->m_hEventWakeup);
		AmisBrain::Instance()->stopAudioClip();
		}

		//continue;
		}
		else if (pThis->m_wakeUpAction == 2) // AUDIO CLIP SHUTDOWN
		{

		if (pThis->m_wakeUpOriginator != 3) {
		TRACE(L"------------ ZZ_3");
		}
		pThis->m_wakeUpAction = 3; // Nothing to do. Just play normally.
		}*/


		if (pThis->m_wakeUpOriginator == 0) {
			pThis->m_bAbort = true;
			continue;
		}

		if (pThis->m_currentAudioSequence == NULL && pThis->m_nextAudioSequence == NULL) {continue;}
			
		bEmptyQueue = FALSE;
		{
#ifdef CCS_ACTIVE
		EnterCriticalSection(&pThis->m_csSequence);
#endif


			if (!pThis->m_bAbort && pThis->m_currentAudioSequence != NULL && pThis->m_currentAudioSequence->GetCount() > 0)
			{
				comp = pThis->m_currentAudioSequence->RemoveTail();

				/*if (comp!=NULL)
				{

					if (comp->m_isAudioClip) {
						if (AmisBrain::Instance()->isAudioClipPlaying()) {
							pThis->m_currentAudioSequence->AddTail(comp);
						}
					} else {
						if (TTSPlayer::Instance()->IsSpeaking()) {	
							pThis->m_currentAudioSequence->AddTail(comp);
						}
					}
				}*/

				if (pThis->m_currentAudioSequence->GetCount() == 0) {
					delete pThis->m_currentAudioSequence;
					pThis->m_currentAudioSequence = NULL;
				}
			}
			else
			{
				bEmptyQueue = TRUE;
			}

		if (!pThis->m_bAbort && !bEmptyQueue)
		{
			if (!pThis->m_bAbort && comp!=NULL)
			{

				if (!pThis->m_bAbort && comp->m_isAudioClip) {
					pThis->m_wakeUpAction = 3; // Nothing to do. Just play normally.
					/*if (TTSPlayer::Instance()->IsSpeaking()) {	
					ResetEvent(pThis->m_hEventWakeup);
					TTSPlayer::Instance()->Stop();
					TTSPlayer::Instance()->WaitUntilDone();
					}*/

					ResetEvent(pThis->m_hEventWakeup);
					TRACE(L"\n============ PLAY AUDIO CLIP\n");
					if (!pThis->m_bAbort) {
						/* while (ambulantX::gui::dx::audio_playerX::Instance()->isPlaying()) {
							TRACE(L"\nIS PLAYING...");
							Sleep(500);
						} */
						pThis->playAudioPrompt(comp->m_AudioClip);
					}
					delete comp;
					comp=NULL;
					/*
					if (!AmisBrain::Instance()->isAudioClipPlaying()) {
						AmisBrain::Instance()->playAudioPrompt(comp->m_AudioClip);
					} else {
						TRACE(L"THIS SHOULD NEVER HAPPEN ! 1");
					}*/

				} else if (!pThis->m_bAbort) {
					pThis->m_wakeUpAction = 3; // Nothing to do. Just play normally.

					/*if (AmisBrain::Instance()->isAudioClipPlaying()) {
					ResetEvent(pThis->m_hEventWakeup);
					AmisBrain::Instance()->stopAudioClip();
					}*/


					ResetEvent(pThis->m_hEventWakeup);
					CString strDebug;
					strDebug.Format(_T("============ PLAY AUDIO TTS: --%s--\n"), comp->m_String);
					TRACE(strDebug);
					if (!TTSPlayer::Instance()->IsSpeaking()) {	
						
						//LPCWSTR str = T2W(comp->m_String);
						//std::wstring str = (LPCTSTR)comp->m_String.GetBuffer();
						//comp->m_String.ReleaseBuffer();
						TTSPlayer::Instance()->Play(comp->m_String);
						delete comp;
						comp=NULL;
					} else {
						TRACE(L"THIS SHOULD NEVER HAPPEN ! 2");
					}
				}
			}
		} else if (!pThis->m_bAbort) {

			if (pThis->m_nextAudioSequence != NULL) {
				pThis->m_currentAudioSequence = pThis->m_nextAudioSequence;
				pThis->m_nextAudioSequence = NULL;
				SetEvent(pThis->m_hEventWakeup);
			} else {
				pThis->m_wakeUpAction = 3; // Nothing to do. Just play normally.
				ResetEvent(pThis->m_hEventWakeup);
				SetEvent(pThis->m_hEventEnd);
			}
		}
		
#ifdef CCS_ACTIVE
		LeaveCriticalSection(&pThis->m_csSequence);
#endif
		}
	}	// while ...

	CoUninitialize();
	   
	//_endthreadex( 0 );
    return 0;
}

void AudioSequencePlayer::TTSAudioEndNotify() {
				
	if (m_bAbort) {
		return;
	}

#ifdef CCS_ACTIVE
		//EnterCriticalSection(&m_csSequence);
#endif
	m_wakeUpOriginator = 2; // TTS END
	TRACE(L"\n ---- m_hEventWakeup TTS end\n");
	SetEvent(m_hEventWakeup);

#ifdef CCS_ACTIVE
		//LeaveCriticalSection(&m_csSequence);
#endif

	/*
	if (m_wakeUpAction != 0) {
	SetEvent(m_hEventWakeup);
	} else {
	m_nEndEventsToExpect--;
	if (m_nEndEventsToExpect <= 0) {
	SetEvent(m_hEventWakeup);
	}
	}*/
}


void AudioSequencePlayer::ClipAudioEndNotify() {
					
	if (m_bAbort) {
		return;
	}	
#ifdef CCS_ACTIVE
		EnterCriticalSection(&m_csSequence);
#endif
	m_wakeUpOriginator = 3; // AUDIO CLIP END
	TRACE(L"\n ---- m_hEventWakeup AUDIO end\n");
	SetEvent(m_hEventWakeup);

#ifdef CCS_ACTIVE
		LeaveCriticalSection(&m_csSequence);
#endif
	/*
	if (m_wakeUpAction != 0) {
	SetEvent(m_hEventWakeup);
	} else {
	m_nEndEventsToExpect--;
	if (m_nEndEventsToExpect <= 0) {
	SetEvent(m_hEventWakeup);
	}
	}*/
}

//The message callback function ... signals end of the tts
static void ttsFinishedCallback()
{
	if (AudioSequencePlayer::Instance()->bIgnoreTTSEnd) {
		AudioSequencePlayer::Instance()->bIgnoreTTSEnd = false;
		return;
	}
	AudioSequencePlayer::Instance()->checkEndSeq();
		//AudioSequencePlayer::Instance()->playNext(true);
	
		//HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
		MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
}
void AudioSequencePlayer::checkEndSeq() {
	if (m_currentAudioSequence != NULL && m_currentAudioSequence->GetCount() == 0) {SetEvent(m_hEventEnd);} 
}
//The message callback function ... signals end of the clip
static void clipFinishedCallback()
{
		//AudioSequencePlayer::Instance()->playNext(true);
	AudioSequencePlayer::Instance()->checkEndSeq();

		//HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
		MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
}
bool AudioSequencePlayer::InstanceExists() {
		
	  return pinstance != 0;
	}
AudioSequencePlayer* AudioSequencePlayer::Instance() {
		
		if (pinstance == 0) {  
    	  pinstance = new AudioSequencePlayer;
		}
	  return pinstance;
	}
AudioSequencePlayer::AudioSequencePlayer(void)
{
	m_previousAudioSequence = NULL;
	m_currentAudioSequence = NULL;
	m_nextAudioSequence = NULL;

	m_hEventWakeup = NULL;
	m_hEventEnd = NULL;
	m_hThread = NULL;
	m_bAbort = false;

	bIgnoreTTSEnd = false;

	m_wakeUpAction = 3;
	m_wakeUpOriginator = -1;
	m_nEndEventsToExpect = 0;

	ambulantX::gui::dx::audio_playerX::Instance()->setCallback((sendMessageCallbackFn)clipFinishedCallback);
	TTSPlayer::Instance()->setCallback((sendMessageCallbackFn)ttsFinishedCallback);


	InitializeCriticalSection(&m_csSequence);

	/* m_hEventWakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(m_hEventWakeup);
	ResetEvent(m_hEventWakeup); */

	m_hEventEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(m_hEventEnd);
	ResetEvent(m_hEventEnd);
	
	m_hThread = NULL;
	
	if (m_hThread == NULL) {
		
	/* 
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*) this, 0, &threadID);
		unsigned long threadID;
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &threadID);
	_ASSERT(m_hThread != NULL);
	 */
	}

}
void AudioSequencePlayer::waitForSequenceEnd()
{
	
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage("WAIT for SEQ end.");
TRACE("\nWAIT for SEQ end.\n");
	DWORD hr = WaitForSingleObject(m_hEventEnd, 3000);
	switch (hr) {
	case WAIT_FAILED: {
		int i = 0;
		break;}

		case WAIT_ABANDONED
			: {
				int i = 0;
				break;}

				case WAIT_OBJECT_0
					: {
						int i = 0;
						break;}

	case WAIT_TIMEOUT: {
		int i = 0;
		break;}

	}
	
					p_log->writeMessage("WAIT for SEQ end DONE.");
TRACE("\nWAIT for SEQ end DONE.\n");
}
void AudioSequencePlayer::DestroyInstance()
{
	if (pinstance != NULL)
	{
		m_bAbort = TRUE;
		Stop();
	ambulantX::gui::dx::audio_playerX::Instance()->DestroyInstance();
	TTSPlayer::Instance()->DestroyInstance();
		delete pinstance;
	}
}

AudioSequencePlayer::~AudioSequencePlayer(void)
{
	BOOL rc;
	//_ASSERT(m_hThread != NULL);

#ifdef CCS_ACTIVE
		EnterCriticalSection(&m_csSequence);
#endif

		if (m_previousAudioSequence) delete m_previousAudioSequence;
		if (m_currentAudioSequence) delete m_currentAudioSequence;
		if (m_nextAudioSequence) delete m_nextAudioSequence;


#ifdef CCS_ACTIVE
		LeaveCriticalSection(&m_csSequence);
#endif

	m_wakeUpOriginator = 0; // DESTRUCTOR
	/* rc = SetEvent(m_hEventWakeup);
	_ASSERT(rc); */

	rc = SetEvent(m_hEventEnd);
	_ASSERT(rc);
	
/*
DWORD hr = WaitForSingleObject(m_hThread, 2000);
	switch (hr) {
	case WAIT_FAILED: {
		int i = 0;
		break;}

		case WAIT_ABANDONED
			: {
				int i = 0;
				break;
			}
				case WAIT_OBJECT_0
					: {
						int i = 0;
						break;}

	case WAIT_TIMEOUT: {
		int i = 0;
		break;}

	}*/

	//CloseHandle(m_hThread);

	//CloseHandle(m_hEventWakeup);

	CloseHandle(m_hEventEnd);

	DeleteCriticalSection(&m_csSequence);

}

void AudioSequencePlayer::RepeatLast() {
	
	//AudioSequence* audioSequence = new AudioSequence();
	if (m_previousAudioSequence) {
		Play(m_previousAudioSequence);
	}
}

//unsigned __stdcall eventHandler(void* lpParam) {
DWORD __stdcall eventHandlerY(LPVOID lpParam) {
//UINT __cdecl eventHandlerX( LPVOID pParam ) {

		AudioSequencePlayer::Instance()->waitForSequenceEnd();

		//MmView *view = MainWndParts::Instance()->mpMmView;
		//view->OnFilePlay();
		try {
			MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, ID_AMBULANT_PLAY);
		} catch (exception e) {
			int i = 0;
		}

    //_endthreadex( 0 );
    return 0;
}
void AudioSequencePlayer::WaitForEndSeqAndRestartBook() {
	
	unsigned long lpdwThreadID;
		//hEventHandler = (HANDLE) _beginthreadex(NULL, 0, eventHandler, (void*) this, 0, &lpdwThreadID);
	HANDLE xeventHandler = CreateThread(NULL, 0, &eventHandlerY, 0, 0, &lpdwThreadID);
		//AfxBeginThread(eventHandlerX,0);
			//GetCurrentThreadId
		TRACE("\nTHREAD ID (WaitForEndSeqAndRestartBook): %x\n", lpdwThreadID);

		
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage("THREAD ID (WaitForEndSeqAndRestartBook)");
					char strID[10];
					sprintf(strID, "%x", lpdwThreadID);
					p_log->writeMessage(strID);

}

void AudioSequencePlayer::Stop(bool fromPlay)
{
SetEvent(m_hEventEnd);
	{
#ifdef CCS_ACTIVE
	if (!fromPlay)
		EnterCriticalSection(&m_csSequence);
#endif
			m_wakeUpAction = 0;
			m_nEndEventsToExpect = 0;

			if (m_currentAudioSequence) {delete m_currentAudioSequence; }
			m_currentAudioSequence = NULL;

			if (m_nextAudioSequence) {delete m_nextAudioSequence; }
			m_nextAudioSequence = NULL;

			bool bExpectEndEventForWakeUp = false;

					amis::util::Log* p_log = amis::util::Log::Instance();

			if (TTSPlayer::Instance()->IsSpeaking()) {	
				bIgnoreTTSEnd = true;
				m_nEndEventsToExpect++;
				//ResetEvent(pThis->m_hEventWakeup);
				TTSPlayer::Instance()->Stop();
				//TTSPlayer::Instance()->WaitUntilDone();
				TRACE(L"\n####### ++++ STOP TTS\n");
				
					p_log->writeMessage("####### ++++ STOP TTS");
			}

			if (ambulantX::gui::dx::audio_playerX::Instance()->is_playing()) {
				//bExpectEndEventForWakeUp = true;
				//m_nEndEventsToExpect++;
				//ResetEvent(pThis->m_hEventWakeup);
				ambulantX::gui::dx::audio_playerX::Instance()->stop(false);
				//m_wakeUpOriginator = 3; // AUDIO CLIP END
				//SetEvent(m_hEventWakeup);
				
					p_log->writeMessage("####### ++++ STOP AUDIO");
				TRACE(L"\n####### ++++ STOP AUDIO\n");
			}

			if (!bExpectEndEventForWakeUp) {
				//TRACE(L"####### SET EVENT");
				m_wakeUpOriginator = 1; // PLAY
				//SetEvent(m_hEventWakeup);
			}
		
#ifdef CCS_ACTIVE
			if (!fromPlay)
		LeaveCriticalSection(&m_csSequence);
#endif
	}
}
void AudioSequencePlayer::playNext(bool fromEndEvent) {

USES_CONVERSION;

	if (m_currentAudioSequence == NULL) return;

#ifdef CCS_ACTIVE
	if (fromEndEvent) EnterCriticalSection(&m_csSequence);
#endif
	
					amis::util::Log* p_log = amis::util::Log::Instance();

					p_log->writeMessage("*** PLAY NEXT");

					TRACE(L"\n*** PLAY NEXT\n");
	
if (m_currentAudioSequence->GetCount() == 0) {
					
					delete m_currentAudioSequence;
					m_currentAudioSequence = NULL;
					return;
				}

	AudioSequenceComponent* comp = m_currentAudioSequence->RemoveTail();

				
if (comp->m_isAudioClip) {

	if (fromEndEvent) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (hr == S_FALSE) CoUninitialize();
	_ASSERT(hr == S_FALSE);
	}

					p_log->writeMessage("============ PLAY AUDIO CLIP");
					TRACE(L"\n============ PLAY AUDIO CLIP\n");
						bool b = playAudioPrompt(comp->m_AudioClip);

						if (!b && !comp->m_String.IsEmpty()) {
					bIgnoreTTSEnd = false;
					TTSPlayer::Instance()->Play(comp->m_String);
						}
					
//if (fromEndEvent) {CoUninitialize();}

					delete comp;
					comp=NULL;

				} else {
					CString strDebug;
					strDebug.Format(_T("============ PLAY AUDIO TTS: --%s--\n"), comp->m_String);
					TRACE(strDebug);

					p_log->writeMessage("============ PLAY AUDIO TTS");
					p_log->writeMessage(W2CA(comp->m_String));

					bIgnoreTTSEnd = false;
					TTSPlayer::Instance()->Play(comp->m_String);
						delete comp;
						comp=NULL;
				}
#ifdef CCS_ACTIVE
			if (fromEndEvent) LeaveCriticalSection(&m_csSequence);
#endif
}

//unsigned __stdcall eventHandler(void* lpParam) {
DWORD __stdcall eventHandlerX(LPVOID lpParam) {

	AudioSequencePlayer *pPlayer = (AudioSequencePlayer*)lpParam;

	
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage("/// THREAD PLAY BEGIN");
	TRACE(L"\n/// THREAD PLAY BEGIN\n");

	pPlayer->playNext(true);

					p_log->writeMessage("/// THREAD PLAY END");
	TRACE(L"\n/// THREAD PLAY END\n");

    //_endthreadex( 0 );
    return 0;
}


void AudioSequencePlayer::Play(AudioSequence* audioSequence, bool doNotRegisterInHistory)
{
	if (audioSequence == NULL || audioSequence->GetCount() == 0) {
		int debugHere = 0;
	}
	{
#ifdef CCS_ACTIVE
		EnterCriticalSection(&m_csSequence);
#endif
		
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage("####### -- PLAY SEQ");
				TRACE(L"\n####### -- PLAY SEQ\n");

	Stop(true);

if (!doNotRegisterInHistory) {
				if (m_previousAudioSequence) {
					if (audioSequence != m_previousAudioSequence) { // Not replaying
						delete m_previousAudioSequence;
					}
				}
				m_previousAudioSequence = audioSequence->clone();
			}


			m_currentAudioSequence = audioSequence;
		ResetEvent(m_hEventEnd);


#ifdef CCS_ACTIVE
		LeaveCriticalSection(&m_csSequence);
#endif

		//HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
		MainWndParts::Instance()->mpMainFrame->SendMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
		//playNext(false);

		/*
		unsigned long lpdwThreadID;
		//hEventHandler = (HANDLE) _beginthreadex(NULL, 0, eventHandler, (void*) this, 0, &lpdwThreadID);
		HANDLE xeventHandler = CreateThread(NULL, 0, &eventHandlerX, this, 0, &lpdwThreadID);
		*/

	return;

		if (!m_bAbort)
		{
			m_wakeUpAction = 0;
			m_nEndEventsToExpect = 0;

			if (!doNotRegisterInHistory) {
				if (m_previousAudioSequence) {
					if (audioSequence != m_previousAudioSequence) {
						delete m_previousAudioSequence;
					}
				}
				m_previousAudioSequence = audioSequence->clone();
			}
			
			if (m_currentAudioSequence) delete m_currentAudioSequence;
			m_currentAudioSequence = NULL;

			m_nextAudioSequence = audioSequence;

			bool bExpectEndEventForWakeUp = false;

			if (TTSPlayer::Instance()->IsSpeaking()) {	
				bExpectEndEventForWakeUp = true;
				m_nEndEventsToExpect++;
				//ResetEvent(pThis->m_hEventWakeup);
				TTSPlayer::Instance()->Stop();
				//TTSPlayer::Instance()->WaitUntilDone();
				
					p_log->writeMessage("#######--- STOP TTS");
				TRACE(L"\n#######--- STOP TTS\n");
			}
			if (ambulantX::gui::dx::audio_playerX::Instance()->is_playing()) {
				bExpectEndEventForWakeUp = true;
				m_nEndEventsToExpect++;
				//ResetEvent(pThis->m_hEventWakeup);
				ambulantX::gui::dx::audio_playerX::Instance()->stop(false);
				//m_wakeUpOriginator = 3; // AUDIO CLIP END
				//SetEvent(m_hEventWakeup);
				
					p_log->writeMessage("#######--- STOP AUDIO");
				TRACE(L"\n#######--- STOP AUDIO\n");
			}

			ResetEvent(m_hEventEnd);

			if (!bExpectEndEventForWakeUp) {
				m_wakeUpOriginator = 1; // PLAY
				
		
					p_log->writeMessage(" ---- m_hEventWakeup PLAY");
	TRACE(L"\n ---- m_hEventWakeup PLAY\n");
				SetEvent(m_hEventWakeup);
			}
		} else {
			delete audioSequence;
		}
#ifdef CCS_ACTIVE
		LeaveCriticalSection(&m_csSequence);
#endif
	}
}



// DanToDo: this string function should move to utilities.
string AudioSequencePlayer::stringReplaceAll(string sourceStr, string searchStr, string replaceStr)
{
	int pos = 0;
	string source_str = sourceStr;

	while (pos > -1)
	{
		pos = source_str.find(searchStr);

		if (pos > -1)
		{
			source_str.replace(pos, searchStr.length(), replaceStr);
		}

	}

	return source_str;

}	
bool AudioSequencePlayer::playAudioPrompt(amis::AudioNode* pAudio)
{
	string str = pAudio->getSrc();
	if (str.length()==0) {
		return false;
	}

	string strFull = pAudio->getSrcExpanded();
	if (strFull.length()==0) {

		ambulant::net::url audio_src = ambulant::net::url::from_filename(str);

		amis::ModuleDescData* p_langpack_data = amis::Preferences::Instance()->getCurrentLanguageData();
		audio_src = audio_src.join_to_base(*p_langpack_data->getXmlFileName());

		pAudio->setSrcExpanded(audio_src.get_file());
		strFull = pAudio->getSrcExpanded();
	}

	return ambulantX::gui::dx::audio_playerX::Instance()->play(strFull.c_str()); 


	/*
	string clipBegin = "";
	string clipEnd = "";

	if (pAudio->getClipBegin().size() > 0)
	{
		clipBegin = stringReplaceAll(pAudio->getClipBegin(), "npt=", "");
	}

	if (pAudio->getClipEnd().size() > 0)
	{
		clipEnd = stringReplaceAll(pAudio->getClipEnd(), "npt=", "");
	}*/

			//(char*)clipBegin.c_str(),  (char*)clipEnd.c_str());
}



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Containers (menus).
void AudioSequencePlayer::playPromptFromUiId(int nItemID, AudioSequence* seq)
{
	USES_CONVERSION;
	
					amis::util::Log* p_log = amis::util::Log::Instance();

//amis::Preferences::Instance()->mbUseTTSNotAudio == true)
		if (nItemID != -1)
		{
			if (seq == NULL) {
				seq = new AudioSequence;
			}

			amis::BookList* books = theApp.getHistory();
			
			int nRecentBooks = (books ? books->getNumberOfEntries() : 0);
			string strCondition;

			if (ID_AMIS_PLAYPAUSE == nItemID) {

				MmView *view = MainWndParts::Instance()->mpMmView;
				bool b_BookIsPlaying = MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus || view->isPlaying(); // BOOK Audio Player

				strCondition = (b_BookIsPlaying ? "canPause" : "canPlay");

			} else if (ID_AMIS_RECENT_BOOKS_EMPTY == nItemID || (nItemID >= AMIS_RECENT_BOOK_BASE_ID && nItemID < AMIS_RECENT_BOOK_BASE_ID + nRecentBooks)) {
				if (nRecentBooks > 0) {
					strCondition = "AMIS_RECENT_BOOK_BASE_IDXX"; // Removing "XX" means that the list-repeat form AccessibleUiData.xml is used => the problem is that no prompt variable resolver exists for the list of recent books. Instead, it's all hardcoded below.
				} else {
					strCondition = "ID_AMIS_RECENT_BOOKS_EMPTY"; // This condition is never accepted by AccessibleDataUi.xml, so it's kinda of useless.
				}
			}

			
			//AppendAudioSequenceStringResource(seq, nItemID);

			UiItem* p_uiItem = DataTree::Instance()->findUiItemInContainers(nItemID, strCondition);
			if (p_uiItem != NULL && AMIS_PAGE_STYLE_BASE_ID != nItemID) { //TODO: temp fix to let the CSS file name be spoken via TTS

				Label* keyboardShortcutAccelerator = NULL;
				Label* mnemonic = NULL;
				if (p_uiItem->getUiItemType() == ACTION) {
					Action* action = (Action*)p_uiItem;
					keyboardShortcutAccelerator = action->getKeyboardAccelerator();
					mnemonic = action->getMnemonic();
				}
				else if (p_uiItem->getUiItemType() == CONTAINER) {
					Container* container = (Container*)p_uiItem;
					keyboardShortcutAccelerator = container->getKeyboardAccelerator();
					mnemonic = container->getMnemonic();
				} else {
					int here = 1;
				}
				//fillSequenceCaptionAndDescription(seq, p_uiItem);

				Label* label = p_uiItem->getCaption();
				if (label != NULL) {
					fillSequenceContentAndPrompt(seq, label, NULL);
				}
				if (mnemonic != NULL) {
					fillSequenceContentAndPrompt(seq, mnemonic, NULL);
				}
				if (keyboardShortcutAccelerator != NULL) {
					fillSequenceContentAndPrompt(seq, keyboardShortcutAccelerator, NULL);
				}
				label = p_uiItem->getDescription();
				if (label != NULL) {
					fillSequenceContentAndPrompt(seq, label, NULL);
				}
				LabelList * list = p_uiItem->getLabelList();
				if (list != NULL) {
					fillSequenceContentAndPrompt(seq, list, NULL);
				}
			}
			 else {

				// Default fallback TTS rendering, based on the widge's text.
				HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
				HMENU hMenu = ::GetMenu(hWnd);
	
				int nLen = ::GetMenuString(hMenu, nItemID, NULL, 0, MF_BYCOMMAND);

				if (nLen > 0) {
	
					LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));
					if (::GetMenuString(hMenu, nItemID, lpszText, nLen + 1, MF_BYCOMMAND) == nLen) {

						CString str;
						str.Append(lpszText);
						str.Replace(CString("&"), CString(""));

						CString strDebug;
						strDebug.Format(_T("+++ 1 --%s--\n"), str); //DAN_MENU_DEBUG
						TRACE(strDebug);

						
					p_log->writeMessage("+++ 1 --%s--");
					p_log->writeMessage(W2CA(str));

						if (! str.IsEmpty()) {
							seq->append(str);
						}

						//str.Empty();
					}
				}
				CString strText;
				strText.LoadString( nItemID );
		
				CString strDebug;
				strDebug.Format(_T("+++ 2 --%s--\n"), strText); //DAN_MENU_DEBUG
				TRACE(strDebug);


					p_log->writeMessage("+++ 1 --%s--");
					p_log->writeMessage(W2CA(strText));

				if (! strText.IsEmpty()) {
					seq->append(strText);
				}
				//strText.Empty();
			}

			 if (seq->GetCount() == 0) {
			delete seq;
			
			 } else {

				 AudioSequencePlayer::Instance()->Play(seq);
			 }
		}
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Containers (menus).
void AudioSequencePlayer::fillSequenceContainerPromptFromId(AudioSequence* seq, string id) {
	
	if (!id.empty())  {
		Container* p_container = DataTree::Instance()->findContainer(id);
		if (p_container != NULL) {
			
			fillSequenceCaptionAndDescription(seq, p_container, NULL, "default");

			Label* keyboardShortcutAccelerator = p_container->getKeyboardAccelerator();
			if (keyboardShortcutAccelerator != NULL) {
				fillSequenceContentAndPrompt(seq, keyboardShortcutAccelerator, NULL);
			}
			
			Label* menomonic = p_container->getMnemonic();
			if (menomonic != NULL) {
				fillSequenceContentAndPrompt(seq, menomonic, NULL);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Prompts.
void AudioSequencePlayer::playPromptFromStringId(string promptId)
{
	Prompt* p_prompt_ = DataTree::Instance()->findPrompt(promptId);

	if (p_prompt_ != NULL)
	{
		AudioSequence * seq = new AudioSequence;
		fillSequencePrompt(seq, p_prompt_, NULL);
		if (seq->GetCount() == 0) {
			delete seq;
		} else {
			AudioSequencePlayer::Instance()->Play(seq);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Prompts.
void AudioSequencePlayer::playPromptItemFromStringId(string promptId)
{
	

	
			
				AudioSequence  * seq = new AudioSequence;
			
	
	PromptItem* p_prompt_ = DataTree::Instance()->findPromptItem(promptId);

	if (p_prompt_ != NULL)
	{
		fillSequenceContents(seq, p_prompt_);
	}
	AudioSequencePlayer::Instance()->Play(seq);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Dialogs, plays the instructions.
void AudioSequencePlayer::playDialogInstructionsFromUiId(int nItemID) {

	if (nItemID != -1) {

		AudioSequence * seq = new AudioSequence;

		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL) {
			Prompt* prompt = p_dialog->getPrompt("instructions");
			if (prompt != NULL) {
				fillSequencePrompt(seq, prompt, NULL);
			}
		}
		if (seq->GetCount() == 0) {
			delete seq;
		}
		else {
			AudioSequencePlayer::Instance()->Play(seq, true);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Dialogs
void AudioSequencePlayer::playDialogTextControlsFromUiId(int nItemID, PromptResolver * presolver) {

	if (nItemID != -1) {
					
				AudioSequence * seq = new AudioSequence;
			
		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL) {
			fillSequenceCaptionAndDescription(seq, p_dialog, presolver, "default");
			fillSequenceSwitch(seq, p_dialog, presolver, "default");

			int count = p_dialog->getNumChildControls();
			for (int i = 0 ; i < count; i++) {
				DialogControl* control = p_dialog->getChildControl(i);
				string strCat = control->getWidgetCategories();
				int find = strCat.find("button");
				if (-1 == find) {
					fillSequenceCaptionAndDescription(seq, control, presolver, "default");
				}
			}
		}

		if (seq->GetCount()==0){delete seq;return;}else{ AudioSequencePlayer::Instance()->Play(seq);}
		
	}
}
void AudioSequencePlayer::playDialogWelcome(int nItemID, PromptResolver * presolver) {

	if (nItemID != -1) {
					
				AudioSequence * seq = new AudioSequence;
			
		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL) {
			fillSequenceCaptionAndDescription(seq, p_dialog, presolver, "default");
			
			Prompt* prompt = p_dialog->getPrompt("instructions");
			if (prompt != NULL) {
				fillSequencePrompt(seq, prompt, NULL);
			}

			int count = p_dialog->getNumChildControls();
			for (int i = 0 ; i < count; i++) {
				DialogControl* control = p_dialog->getChildControl(i);
				string strCat = control->getWidgetCategories();
				int find = strCat.find("autoplay");
				if (-1 != find) {
					fillSequenceCaptionAndDescription(seq, control, presolver, "default");
				}
			}
		}

		if (seq->GetCount()==0){delete seq;return;}else{ AudioSequencePlayer::Instance()->Play(seq);}
		
	}
}


void AudioSequencePlayer::fillOK_CANCEL(AudioSequence * seq, UINT ctrlId) {
	if (ctrlId == IDCANCEL) {
		PromptItem* p_prompt = DataTree::Instance()->findPromptItem("cancel_button_caption");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);
			//CString text = p_prompt->concatAllText().c_str();
			//seq->append(text);
		}
		p_prompt = DataTree::Instance()->findPromptItem("cancel_button_description");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);
			//CString text = p_prompt->concatAllText().c_str();
			//seq->append(text);
		}

	} else if (ctrlId == IDOK) {
		PromptItem* p_prompt = DataTree::Instance()->findPromptItem("ok_button_caption");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);
			//CString text = p_prompt->concatAllText().c_str();
			//seq->append(text);
		}
		p_prompt = DataTree::Instance()->findPromptItem("ok_button_description");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);
			//CString text = p_prompt->concatAllText().c_str();
			//seq->append(text);
		}


		/* Dialog* p_dialog = DataTree::Instance()->findDialog(IDD_ABOUTBOX);
		if (p_dialog != NULL) {
		DialogControl* control = p_dialog->findControl(IDOK);

		if (control != NULL) {
		fillSequenceCaptionAndDescription(seq, control);
		}
		}*/
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This works for Dialogs
AudioSequence * AudioSequencePlayer::playDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, bool playNow, string switchCondition) {

	if (
		(dlgID != -1 && ctrlId != -1)
		|| (dlgID == -1 && (ctrlId == IDOK || ctrlId == IDCANCEL))
		) {

			AudioSequence * seq = new AudioSequence;

			if (dlgID == -1) {
				fillOK_CANCEL(seq, ctrlId);
			} else {
				Dialog* p_dialog = DataTree::Instance()->findDialog(dlgID);
				if (p_dialog != NULL) {
					DialogControl* control = p_dialog->findControl(ctrlId);

					if (control != NULL) {
						fillSequenceCaptionAndDescription(seq, control, pResolver, switchCondition);
					}
				} else {
					fillOK_CANCEL(seq, ctrlId);
				}
			}

			if (seq != NULL && seq->GetCount() == 0) {
				delete seq;
				return NULL;

			} else if (playNow) {

				AudioSequencePlayer::Instance()->Play(seq);
				return seq;
			}
			else return seq;

		}
		
		return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequencePrompt(AudioSequence* seq, Prompt* prompt, PromptResolver* pResolver) {

	resolvePromptVariables(prompt, pResolver);

	int sz = prompt->getNumberOfItems();
	for (int i=0; i<sz; i++)
	{
		PromptItemBase* pib = prompt->getItem(i);
		switch(pib->getPromptItemType()) {
			case PROMPT_ITEM: {
				PromptItem* pi = (PromptItem*) pib;
				fillSequenceContents(seq, pi);
				break;
			}
			case PROMPT_VARIABLE: {
					PromptVar* pv = (PromptVar*) pib;
					//seq->append(pv->getName().c_str());
					fillSequenceContents(seq, pv);
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequenceCaptionAndDescription(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition) {

	Label* label = uiItem->getCaption();
	if (label != NULL) {
		fillSequenceContentAndPrompt(seq, label, pResolver);
	}

	LabelList * p_list = uiItem->getLabelList();

	label = uiItem->getDescription();
	if (p_list == NULL && label != NULL) {
		fillSequenceContentAndPrompt(seq, label, pResolver);
	}

	fillSequenceSwitch(seq, uiItem, pResolver, switchCondition);

	if (p_list != NULL)
	{
		fillSequenceContentAndPrompt(seq, p_list, pResolver);
		/*
		for (int i = 0; i<p_list->getNumberOfLabels(); i++)
		{
			Prompt* p_prompt = p_list->getLabel(i)->getPrompt();
			if (p_prompt != NULL)
			{
				fillSequencePrompt(seq, p_prompt, pResolver);
			}
		}*/
	}
	if (p_list != NULL && label != NULL) {
		fillSequenceContentAndPrompt(seq, label, pResolver);
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequenceSwitch(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition) {

	Switch* zwitch;
	if ((zwitch = uiItem->getSwitch()) != NULL) {

		switch(zwitch->getSwitchType()) {
			case ACTIONSWITCH: {
					Action* action = zwitch->getAction(switchCondition);
					if (action != NULL) {
						fillSequenceCaptionAndDescription(seq, action, pResolver, switchCondition);
					}
				break;
				}
			case CAPTIONSWITCH: {}
			case DESCRIPTIONSWITCH: {
					Label* label = zwitch->getLabel(switchCondition);
					if (label != NULL) {
						fillSequenceContentAndPrompt(seq, label, pResolver);
					}
				break;
				}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequenceContentAndPrompt(AudioSequence* seq, LabelList * p_list, PromptResolver * pResolver)
{
	for (int i = 0; i<p_list->getNumberOfLabels(); i++)
	{
		Prompt* p_prompt = p_list->getLabel(i)->getPrompt();
		if (p_prompt != NULL)
		{
			fillSequencePrompt(seq, p_prompt, pResolver);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequenceContentAndPrompt(AudioSequence* seq, Label* label, PromptResolver * pResolver)
{
	TextAudioPair* pair = label->getContents();
	if (pair != NULL) {
		amis::AudioNode* audio = pair->getAudio();
		
			TextNodeSV* textN = pair->getText();

		if (!Preferences::Instance()->getUseTTSNotAudio() && audio != NULL && audio->getSrc().length() != 0) 
		{
			seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));
		} 
		else 
		{
			if (textN != NULL) 
			{
				seq->append(textN->getTextString().c_str());
			}
		}
	}

	Prompt* prompt = label->getPrompt();
	if (prompt != NULL)
	{
		fillSequencePrompt(seq, prompt, pResolver);

		/*
		resolvePromptVariables(prompt, pResolver);

		int count = prompt->getNumberOfItems();
		for (int i=0; i<count; i++)
		{
			PromptItemBase* pib = prompt->getItem(i);
			switch(pib->getPromptItemType())
			{
				case PROMPT_ITEM:
					{
					PromptItem* pi = (PromptItem*) pib;

					pair = pi->getContents();
					if (pair != NULL)
					{
						amis::AudioNode* audio = pair->getAudio();
						
							TextNodeSV * textN = pair->getText();

						if (!Preferences::Instance()->getUseTTSNotAudio() && audio != NULL && audio->getSrc().length() != 0)
						{
							seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));
						} 
						else
						{
							if (textN != NULL)
							{
								seq->append(textN->getTextString().c_str());
							}
						}
					}
					break;
				}
				case PROMPT_VARIABLE: {
					PromptVar* pv = (PromptVar*) pib;

					//We assume variable value resolution happened before.

					pair = pv->getContents();
					if (pair != NULL)
					{
						amis::AudioNode* audio = pair->getAudio();
						
							TextNodeSV * textN = pair->getText();

						if (!Preferences::Instance()->getUseTTSNotAudio() && audio != NULL && audio->getSrc().length() != 0)
						{
							seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));
						} 
						else
						{
							if (textN != NULL)
							{
								seq->append(textN->getTextString().c_str());
							}
						}
					}
					break;
				}
			}
		}
		*/
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AudioSequencePlayer::fillSequenceContents(AudioSequence* seq, PromptItemBase* pi) {

	TextAudioPair* pair = pi->getContents();
	if (pair != NULL) {
		amis::AudioNode* audio = pair->getAudio();
			TextNodeSV * textN = pair->getText();

		if (!Preferences::Instance()->getUseTTSNotAudio() && audio != NULL && audio->getSrc().length() != 0) 
		{
			seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));
		} 
		else 
		{
			if (textN != NULL) 
			{
				seq->append(textN->getTextString().c_str());
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void AudioSequencePlayer::resolvePromptVariables(Prompt* pPrompt, PromptResolver* pResolver)
{
	USES_CONVERSION;

	PromptVar* p_var = NULL;

	for (int i=0; i<pPrompt->getNumberOfItems(); i++)
	{
		if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
		{
			p_var = (PromptVar*)pPrompt->getItem(i);

			if (p_var->getName().compare("BOOK_PATH") == 0)
			{
				//CString file_path = A2CW(mFilePath.c_str());
				//p_var->setContents(LPCTSTR(file_path), "");
				//DanToDo: is this variable actually used anywhere ? If yes, where do I get the current book path from ?
				//amis::dtb::DtbWithHooks::Instance()->
			}
		}
	}
	if (pResolver != NULL) {
		pResolver->resolvePromptVariables(pPrompt);
	}
}















