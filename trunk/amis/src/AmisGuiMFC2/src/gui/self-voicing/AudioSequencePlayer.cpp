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


#include "gui/self-voicing/AudioSequencePlayer.h"
#include "gui/self-voicing/TTSPlayer.h"
#include "gui/AmisApp.h"
#include "util/FilePathTools.h"
#include "gui/self-voicing/directshow/dx_audio_player.h"
#include "Preferences.h"
#include <gui/MainWndParts.h>
#include "Media.h"
#include "gui/MmView.h"
#include "../../AmisGuiMFC2/resource.h"
#include "util/Log.h"

using namespace amis::tts;
using namespace amis::gui::spoken;

AudioSequencePlayer* AudioSequencePlayer::pinstance = 0;

#define CCS_ACTIVE


//The message callback function ... signals end of the tts
static void ttsFinishedCallback()
{
	/*if (AudioSequencePlayer::Instance()->bIgnoreTTSEnd)
	{
		AudioSequencePlayer::Instance()->bIgnoreTTSEnd = false;
		return;
	}*/
	AudioSequencePlayer::Instance()->checkEndSeq();
	MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
}
bool AudioSequencePlayer::checkEndSeq() 
{
	if (m_currentAudioSequence != NULL && m_currentAudioSequence->GetCount() == 0)
		SetEvent(m_hEventEnd);
	
	return mRepeatLoop;
}
//The message callback function ... signals end of the clip
static void clipFinishedCallback()
{
	AudioSequencePlayer::Instance()->checkEndSeq();
	MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
}
bool AudioSequencePlayer::InstanceExists()
{
	return pinstance != 0;
}
AudioSequencePlayer* AudioSequencePlayer::Instance()
{
	if (pinstance == 0)	pinstance = new AudioSequencePlayer;
	return pinstance;
}
AudioSequencePlayer::AudioSequencePlayer(void)
{
	mRepeatLoop = false;

	m_previousAudioSequence = NULL;
	m_currentAudioSequence = NULL;
	m_nextAudioSequence = NULL;

	m_hEventWakeup = NULL;
	m_hEventEnd = NULL;
	m_hThread = NULL;
	m_bAbort = false;

	//bIgnoreTTSEnd = false;

	if (!Preferences::Instance()->getMustAvoidDirectX())
		ambulantX::gui::dx::audio_playerX::Instance()->setCallback((sendMessageCallbackFn)clipFinishedCallback);

	if (!Preferences::Instance()->getMustAvoidTTS())
		TTSPlayer::InstanceOne()->setCallback((sendMessageCallbackFn)ttsFinishedCallback);

	InitializeCriticalSection(&m_csSequence);

	m_hEventEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(m_hEventEnd);
	ResetEvent(m_hEventEnd);

	m_hThread = NULL;
}
void AudioSequencePlayer::waitForSequenceEnd()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("Wait for seq end.", "AudioSequencePlayer::waitForSequenceEnd");
	TRACE("\nWAIT for SEQ end.\n");
	DWORD hr = WaitForSingleObject(m_hEventEnd, 3000);
	switch (hr) 
	{
		case WAIT_FAILED:
		{
			int i = 0;
			break;
		}
		case WAIT_ABANDONED:
		{
			int i = 0;
			break;
		}
		case WAIT_OBJECT_0:
		{
			int i = 0;
			break;
		}
		case WAIT_TIMEOUT:
		{
			int i = 0;
			break;
		}
	}

	p_log->writeTrace("Wait for seq end: DONE." "AudioSequencePlayer::waitForSequenceEnd");
	TRACE("\nWAIT for SEQ end DONE.\n");
}
void AudioSequencePlayer::DestroyInstance()
{
	if (pinstance != NULL)
	{
		m_bAbort = TRUE;
		Stop();
		if (!Preferences::Instance()->getMustAvoidDirectX())
			ambulantX::gui::dx::audio_playerX::Instance()->DestroyInstance();
		if (!Preferences::Instance()->getMustAvoidTTS())
			TTSPlayer::DestroyInstanceOne();
		delete pinstance;
	}
}

AudioSequencePlayer::~AudioSequencePlayer(void)
{
	BOOL rc;

#ifdef CCS_ACTIVE
	EnterCriticalSection(&m_csSequence);
#endif

	if (m_previousAudioSequence) delete m_previousAudioSequence;
	if (m_currentAudioSequence) delete m_currentAudioSequence;
	if (m_nextAudioSequence) delete m_nextAudioSequence;

#ifdef CCS_ACTIVE
	LeaveCriticalSection(&m_csSequence);
#endif

	rc = SetEvent(m_hEventEnd);
	_ASSERT(rc);

	CloseHandle(m_hEventEnd);
	DeleteCriticalSection(&m_csSequence);
}

void AudioSequencePlayer::RepeatLast()
{
	if (m_previousAudioSequence)
		Play(m_previousAudioSequence);
}

DWORD __stdcall thread_WaitAndPlayBook(LPVOID lpParam)
{
	AudioSequencePlayer::Instance()->waitForSequenceEnd();
	try
	{
		MainWndParts::Instance()->mpMainFrame->PostMessage(WM_COMMAND, ID_AMBULANT_PLAY);
	}
	catch (exception e)
	{
		//TODO: trace the exception?
	}

	return 0;
}
void AudioSequencePlayer::WaitForEndSeqAndRestartBook()
{
	USES_CONVERSION;
	unsigned long lpdwThreadID;
	HANDLE xeventHandler = CreateThread(NULL, 0, &thread_WaitAndPlayBook, 0, 0, &lpdwThreadID);
	TRACE("\nTHREAD ID (WaitForEndSeqAndRestartBook): %x\n", lpdwThreadID);
	amis::util::Log* p_log = amis::util::Log::Instance();
	CString cstr_log_msg;
	cstr_log_msg.Format(_T("Thread ID: %d"), lpdwThreadID);
	string log_msg = T2A(cstr_log_msg);
	p_log->writeTrace(log_msg, "AudioSequencePlayer::WaitForEndSeqAndRestartBook");
}

void AudioSequencePlayer::Stop(bool fromPlay)
{
	SetEvent(m_hEventEnd);
	{
#ifdef CCS_ACTIVE
		if (!fromPlay) EnterCriticalSection(&m_csSequence);
#endif
		if (m_currentAudioSequence) delete m_currentAudioSequence;
		m_currentAudioSequence = NULL;

		if (m_nextAudioSequence) delete m_nextAudioSequence;
		m_nextAudioSequence = NULL;

		amis::util::Log* p_log = amis::util::Log::Instance();

		if (!Preferences::Instance()->getMustAvoidTTS())
		{
			if (TTSPlayer::InstanceOne()->IsPlaying())
			{	
				//bIgnoreTTSEnd = true;
				TTSPlayer::InstanceOne()->Stop();
				p_log->writeTrace("Stop TTS", "AudioSequencePlayer::Stop");
				TRACE(L"STOP TTS\n");
			}
		}

		if (!Preferences::Instance()->getMustAvoidDirectX())
		{
			if (ambulantX::gui::dx::audio_playerX::Instance()->is_playing())
			{
				ambulantX::gui::dx::audio_playerX::Instance()->stop(false);
				p_log->writeTrace("Stop Audio", "AudioSequencePlayer::Stop");
				TRACE(L"STOP AUDIO\n");
			}
		}

#ifdef CCS_ACTIVE
		if (!fromPlay) LeaveCriticalSection(&m_csSequence);
#endif
	}
}

void AudioSequencePlayer::playNext(bool fromEndEvent)
{
	USES_CONVERSION;
	if (m_currentAudioSequence == NULL) return;

#ifdef CCS_ACTIVE
	if (fromEndEvent) EnterCriticalSection(&m_csSequence);
#endif

	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("Play next", "AudioSequencePlayer::playNext");
	TRACE(L"PLAY NEXT\n");

	if (m_currentAudioSequence->GetCount() == 0)
	{
		delete m_currentAudioSequence;
		m_currentAudioSequence = NULL;
		
		if (mRepeatLoop && m_previousAudioSequence)
		{
			Sleep(2000);
			Play(m_previousAudioSequence, false, true, true);
		}
		return;
	}

	AudioSequenceComponent* comp = m_currentAudioSequence->RemoveTail();
	if (comp->m_isAudioClip)
	{
		p_log->writeTrace("Play audio clip", "AudioSequencePlayer::playNext");
		TRACE(L"PLAY AUDIO CLIP\n");
		bool b = playAudioPrompt(comp->m_AudioClip);

		if (!b && !comp->m_String.IsEmpty())
		{
			//bIgnoreTTSEnd = false;
			if (!Preferences::Instance()->getMustAvoidTTS())
				TTSPlayer::InstanceOne()->Play(comp->m_String);
		}
		delete comp;
		comp=NULL;
	} 
	else 
	{
		CString strDebug;
		strDebug.Format(_T("Play audio TTS: %s\n"), comp->m_String);
		TRACE(strDebug);
		string log_msg = T2A(strDebug);
		p_log->writeTrace(log_msg, "AudioSequencePlayer::playNext");
	
		//bIgnoreTTSEnd = false;
		if (!Preferences::Instance()->getMustAvoidTTS())
			TTSPlayer::InstanceOne()->Play(comp->m_String);
		delete comp;
		comp=NULL;
	}
#ifdef CCS_ACTIVE
	if (fromEndEvent) LeaveCriticalSection(&m_csSequence);
#endif
}


void AudioSequencePlayer::Play(AudioSequence* audioSequence, bool doNotRegisterInHistory, bool repeat, bool repeating)
{
	mRepeatLoop = repeat;
	{
#ifdef CCS_ACTIVE
		EnterCriticalSection(&m_csSequence);
#endif
		amis::util::Log* p_log = amis::util::Log::Instance();
		p_log->writeTrace("Play seq", "AudioSequencePlayer::Play");
		TRACE(L"PLAY SEQ\n");
		Stop(true);
		if (!doNotRegisterInHistory)
		{
			if (m_previousAudioSequence && audioSequence != m_previousAudioSequence)
				delete m_previousAudioSequence;
			m_previousAudioSequence = audioSequence->clone();
		}
		m_currentAudioSequence = audioSequence;
		ResetEvent(m_hEventEnd);

#ifdef CCS_ACTIVE
		LeaveCriticalSection(&m_csSequence);
#endif
		MainWndParts::Instance()->mpMainFrame->SendMessage(WM_COMMAND, (WPARAM)SELF_VOICING_PLAY_NEXT, (LPARAM)0);
	}
}

bool AudioSequencePlayer::playAudioPrompt(amis::AudioNode* pAudio)
{
	if (Preferences::Instance()->getMustAvoidDirectX()) return false;

	string strFull = pAudio->getPath();
	if (strFull.length() == 0) return false;

	string clipBegin = "";
	string clipEnd = "";

	if (pAudio->getClipBegin().size() > 0)
		clipBegin = stringReplaceAll(pAudio->getClipBegin(), "npt=", "");

	if (pAudio->getClipEnd().size() > 0)
		clipEnd = stringReplaceAll(pAudio->getClipEnd(), "npt=", "");

	return ambulantX::gui::dx::audio_playerX::Instance()->play(strFull.c_str(), (char*)clipBegin.c_str(), (char*)clipEnd.c_str());
}

void AudioSequencePlayer::playPromptFromUiId(int nItemID, AudioSequence* seq, bool verbose)
{
	USES_CONVERSION;

	amis::util::Log* p_log = amis::util::Log::Instance();

	if (nItemID != -1)
	{
		if (seq == NULL) seq = new AudioSequence;
		amis::BookList* books = theApp.getHistory();
		int nRecentBooks = (books ? books->getNumberOfEntries() : 0);
		string strCondition;

		if (ID_AMIS_PLAYPAUSE == nItemID)
		{
			MmView *view = MainWndParts::Instance()->mpMmView;
			bool b_BookIsPlaying = MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus || view->isPlaying(); // BOOK Audio Player
			strCondition = (b_BookIsPlaying ? "canPause" : "canPlay");
		} 
		else if (ID_AMIS_RECENT_BOOKS_EMPTY == nItemID || (nItemID >= AMIS_RECENT_BOOK_BASE_ID && nItemID < AMIS_RECENT_BOOK_BASE_ID + nRecentBooks))
		{
			if (nRecentBooks > 0)
			{
				strCondition = "AMIS_RECENT_BOOK_BASE_IDXX"; // Removing "XX" means that the list-repeat form AccessibleUiData.xml is used => the problem is that no prompt variable resolver exists for the list of recent books. Instead, it's all hardcoded below.
			} 
			else 
			{
				strCondition = "ID_AMIS_RECENT_BOOKS_EMPTY"; // This condition is never accepted by AccessibleDataUi.xml, so it's kinda of useless.
			}
		}
		UiItem* p_uiItem = DataTree::Instance()->findUiItemInContainers(nItemID, strCondition);
		if (p_uiItem != NULL && AMIS_PAGE_STYLE_BASE_ID != nItemID)  //TODO: temp fix to let the CSS file name be spoken via TTS
		{

			Label* keyboardShortcutAccelerator = NULL;
			Label* mnemonic = NULL;
			if (p_uiItem->getUiItemType() == ACTION)
			{
				Action* action = (Action*)p_uiItem;
				keyboardShortcutAccelerator = action->getKeyboardAccelerator();
				mnemonic = action->getMnemonic();
			}
			else if (p_uiItem->getUiItemType() == CONTAINER)
			{
				Container* container = (Container*)p_uiItem;
				keyboardShortcutAccelerator = container->getKeyboardAccelerator();
				mnemonic = container->getMnemonic();
			} 
			else 
			{
				int debug = 1;
			}

			Label* label = p_uiItem->getCaption();
			if (label != NULL)
			{
				fillSequenceContentAndPrompt(seq, label, NULL);
			}
			if (verbose)
			{
				if (mnemonic != NULL)
				{
					fillSequenceContentAndPrompt(seq, mnemonic, NULL);
				}
				if (keyboardShortcutAccelerator != NULL)
				{
					fillSequenceContentAndPrompt(seq, keyboardShortcutAccelerator, NULL);
				}
				label = p_uiItem->getDescription();
				if (label != NULL)
				{
					fillSequenceContentAndPrompt(seq, label, NULL);
				}
				LabelList * list = p_uiItem->getLabelList();
				if (list != NULL)
				{
					fillSequenceContentAndPrompt(seq, list, NULL);
				}
			}
		}
		else 
		{
			// Default fallback TTS rendering, based on the widge's text.
			HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
			HMENU hMenu = ::GetMenu(hWnd);

			int nLen = ::GetMenuString(hMenu, nItemID, NULL, 0, MF_BYCOMMAND);

			if (nLen > 0)
			{
				LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));
				if (::GetMenuString(hMenu, nItemID, lpszText, nLen + 1, MF_BYCOMMAND) == nLen)
				{
					CString str;
					str.Append(lpszText);
					str.Replace(CString("&"), CString(""));

					CString strDebug;
					strDebug.Format(_T("%s\n"), str);
					TRACE(strDebug);
					p_log->writeTrace(T2A(strDebug), "AudioSequencePlayer::playPromptFromUiId");
					
					if (! str.IsEmpty()) seq->append(str);
				}
			}
			CString strText;
			strText.LoadString( nItemID );

			CString strDebug;
			strDebug.Format(_T("%s\n"), strText);
			TRACE(strDebug);
			p_log->writeTrace(T2A(strDebug), "AudioSequencePlayer::playPromptFromUiId");

			if (! strText.IsEmpty()) seq->append(strText);
		}
		if (seq->GetCount() == 0) delete seq;
		else AudioSequencePlayer::Instance()->Play(seq);
	}
}


void AudioSequencePlayer::fillSequenceContainerPromptFromId(AudioSequence* seq, string id)
{
	if (!id.empty())  
	{
		Container* p_container = DataTree::Instance()->findContainer(id);
		if (p_container != NULL)
		{
			fillSequenceCaptionAndDescription(seq, p_container, NULL, "default");
			Label* keyboardShortcutAccelerator = p_container->getKeyboardAccelerator();
			if (keyboardShortcutAccelerator != NULL)
				fillSequenceContentAndPrompt(seq, keyboardShortcutAccelerator, NULL);
	
			Label* menomonic = p_container->getMnemonic();
			if (menomonic != NULL)
				fillSequenceContentAndPrompt(seq, menomonic, NULL);
		}
	}
}

std::wstring AudioSequencePlayer::getTextForDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, string switchCondition)
{
	//todo: not a high priority now, but the code below is far too incomplete to be usable generically (there is only one use of it right now, and it searches through the switch caption only).
	std::wstring str;
	if ((dlgID != -1 && ctrlId != -1) 
		|| (dlgID == -1 && (ctrlId == IDOK || ctrlId == IDCANCEL)))
	{
		Dialog* p_dialog = DataTree::Instance()->findDialog(dlgID);
		if (p_dialog != NULL)
		{
			DialogControl* uiItem = p_dialog->findControl(ctrlId);
			if (uiItem != NULL)
			{
				Switch* zwitch;
				if ((zwitch = uiItem->getSwitch()) != NULL)
				{
					switch(zwitch->getSwitchType())
					{
						case ACTIONSWITCH:
						{
							Action* action = zwitch->getAction(switchCondition);
							if (action != NULL)
							{
								;//
							}
							break;
						}
						case CAPTIONSWITCH: {}
						case DESCRIPTIONSWITCH:
						{
							Label* label = zwitch->getLabel(switchCondition);
							if (label != NULL)
							{									
								TextAudioPair* pair = label->getContents();
								if (pair != NULL)
								{
									TextNodeSV* textN = pair->getText();
									if (textN != NULL) 
									{
										str.append(textN->getTextString());
									}	
								}
							}
							break;
						}
					}
				}
			}
		}	
	}
	return str;
}

std::wstring AudioSequencePlayer::getTextForPromptItemFromStringId(string promptId)
{
	std::wstring str;

	PromptItem* pi = DataTree::Instance()->findPromptItem(promptId);

	if (pi != NULL)
	{
		TextAudioPair* pair = pi->getContents();
		if (pair != NULL)
		{
			TextNodeSV * textN = pair->getText();
			if (textN != NULL) str.append(textN->getTextString());
		}
	}
	return str;
}

std::wstring AudioSequencePlayer::getMenuCaption(int nItemID)
{
	UiItem* p_uiItem = DataTree::Instance()->findUiItemInContainers(nItemID, "");
	if (p_uiItem != NULL)
	{
		Label* label = p_uiItem->getCaption();
		if (label != NULL)
		{
			TextAudioPair * pair = label->getContents();
			if (pair != NULL)
			{
				TextNodeSV * textNode = pair->getText();
				if (textNode != NULL)
				{
					return textNode->getTextString();
				}
			}
		}
	}
}

std::wstring AudioSequencePlayer::getTextForPromptFromStringId(string promptId)
{
	std::wstring str;
	Prompt* prompt = DataTree::Instance()->findPrompt(promptId);
	if (prompt != NULL)
	{
		int sz = prompt->getNumberOfItems();
		for (int i=0; i<sz; i++)
		{
			PromptItemBase* pi = prompt->getItem(i);
			TextAudioPair* pair = pi->getContents();
			if (pair != NULL)
			{
				TextNodeSV * textN = pair->getText();
				if (textN != NULL) str.append(textN->getTextString());
			}
		}
	}
	
	// Fallback on PromptItem instead of Prompt
	if (str.length() == 0)	str = getTextForPromptItemFromStringId(promptId);
	return str;
}

bool AudioSequencePlayer::playPromptFromStringId(string promptId, bool repeat)
{
	Prompt* p_prompt_ = DataTree::Instance()->findPrompt(promptId);
	if (p_prompt_ != NULL)
	{
		AudioSequence * seq = new AudioSequence;
		fillSequencePrompt(seq, p_prompt_, NULL);
		if (seq->GetCount() == 0)
		{
			delete seq;
		}
		else
		{
			AudioSequencePlayer::Instance()->Play(seq, false, repeat);
			return true;
		}
	}
	else
	{
		// Fallback on PromptItem instead of Prompt
		return playPromptItemFromStringId(promptId, repeat);
	}
	return false;
}

bool AudioSequencePlayer::playPromptItemFromStringId(string promptId, bool repeat)
{		
	PromptItem* p_prompt_ = DataTree::Instance()->findPromptItem(promptId);

	if (p_prompt_ != NULL)
	{
		AudioSequence  * seq = new AudioSequence;
		fillSequenceContents(seq, p_prompt_);
		if (seq->GetCount() == 0)
		{
			delete seq;
		}
		else
		{
			AudioSequencePlayer::Instance()->Play(seq, false, repeat);
			return true;
		}
	}	
	return false;
}

bool AudioSequencePlayer::playDialogInstructionsFromUiId(int nItemID)
{
	if (nItemID != -1)
	{
		AudioSequence * seq = new AudioSequence;
		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL)
		{
			Prompt* prompt = p_dialog->getPrompt("instructions");
			if (prompt != NULL) fillSequencePrompt(seq, prompt, NULL);
		}
		if (seq->GetCount() == 0)
		{
			delete seq;
		}
		else
		{
			AudioSequencePlayer::Instance()->Play(seq, true);
			return true;
		}
	}
	return false;
}

bool AudioSequencePlayer::playDialogTextControlsFromUiId(int nItemID, PromptResolver * presolver)
{
	if (nItemID != -1)
	{
		AudioSequence * seq = new AudioSequence;
		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL)
		{
			fillSequenceCaptionAndDescription(seq, p_dialog, presolver, "default");
			fillSequenceSwitch(seq, p_dialog, presolver, "default");

			int count = p_dialog->getNumChildControls();
			for (int i = 0 ; i < count; i++)
			{
				DialogControl* control = p_dialog->getChildControl(i);
				string strCat = control->getWidgetCategories();
				int find = strCat.find("button");
				if (-1 == find) 
				{
					fillSequenceCaptionAndDescription(seq, control, presolver, "default");
				}
			}
		}

		if (seq->GetCount()==0)
		{
			delete seq;
		}
		else
		{
			AudioSequencePlayer::Instance()->Play(seq);
			return true;
		}
	}
	return false;
}
bool AudioSequencePlayer::playDialogWelcome(int nItemID, PromptResolver * presolver, bool playfull)
{
	if (nItemID != -1)
	{
		AudioSequence * seq = new AudioSequence;
		Dialog* p_dialog = DataTree::Instance()->findDialog(nItemID);
		if (p_dialog != NULL)
		{
			fillSequenceCaptionAndDescription(seq, p_dialog, presolver, "default");
			Prompt* prompt = p_dialog->getPrompt("instructions");
			if (prompt != NULL)
			{
				fillSequencePrompt(seq, prompt, NULL);
			}
			if (playfull || nItemID == IDD_ABOUTBOX)
			{
				int count = p_dialog->getNumChildControls();
				for (int i = 0 ; i < count; i++)
				{
					DialogControl* control = p_dialog->getChildControl(i);
					string strCat = control->getWidgetCategories();
					int find = strCat.find("autoplay");
					if (-1 != find)
					{
						fillSequenceCaptionAndDescription(seq, control, presolver, "default");
					}
				}
			}
		}

		if (seq->GetCount()==0)
		{
			delete seq;
		}
		else
		{
			AudioSequencePlayer::Instance()->Play(seq);
			return true;
		}
	}
	return false;
}


void AudioSequencePlayer::fillOK_CANCEL(AudioSequence * seq, UINT ctrlId)
{
	if (ctrlId == IDCANCEL)
	{
		PromptItem* p_prompt = DataTree::Instance()->findPromptItem("cancel_button_caption");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);

		}
		p_prompt = DataTree::Instance()->findPromptItem("cancel_button_description");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);

		}
	}
	else if (ctrlId == IDOK)
	{
		PromptItem* p_prompt = DataTree::Instance()->findPromptItem("ok_button_caption");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);

		}
		p_prompt = DataTree::Instance()->findPromptItem("ok_button_description");
		if (p_prompt != NULL)
		{
			fillSequenceContents(seq, p_prompt);

		}
	}
}

AudioSequence * AudioSequencePlayer::playDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, bool playNow, string switchCondition, bool verbose)
{
	if ((dlgID != -1 && ctrlId != -1)
		|| (dlgID == -1 && (ctrlId == IDOK || ctrlId == IDCANCEL)))
	{
		AudioSequence * seq = new AudioSequence;
		if (dlgID == -1)
		{
			fillOK_CANCEL(seq, ctrlId);
		}
		else
		{
			Dialog* p_dialog = DataTree::Instance()->findDialog(dlgID);
			if (p_dialog != NULL)
			{
				DialogControl* control = p_dialog->findControl(ctrlId);
				if (control != NULL)
				{
					fillSequenceCaptionAndDescription(seq, control, pResolver, switchCondition, verbose);
				}
			}
			else
			{
				fillOK_CANCEL(seq, ctrlId);
			}
		}
		if (seq != NULL && seq->GetCount() == 0)
		{
			delete seq;
			return NULL;

		}
		else if (playNow)
		{
			AudioSequencePlayer::Instance()->Play(seq);
			return seq;
		}
		else return seq;
	}
	return NULL;
}

void AudioSequencePlayer::fillSequencePrompt(AudioSequence* seq, Prompt* prompt, PromptResolver* pResolver)
{
	resolvePromptVariables(prompt, pResolver);

	int sz = prompt->getNumberOfItems();
	for (int i=0; i<sz; i++)
	{
		PromptItemBase* pib = prompt->getItem(i);
		switch(pib->getPromptItemType())
		{
			case PROMPT_ITEM: 
			{
				PromptItem* pi = (PromptItem*) pib;
				fillSequenceContents(seq, pi);
				break;
			}
			case PROMPT_VARIABLE:
			{
				PromptVar* pv = (PromptVar*) pib;
				
				if (pv->hasSubItems())
				{
					for (int j = 0; j < pv->subItemsCount(); j++)
					{
						PromptItem * pi = pv->getSubItem(j);
						fillSequenceContents(seq, pi);
					}
					pv->clearSubItems();
				}
				else
				{
					fillSequenceContents(seq, pv);
				}
				break;
			}
		}
	}
}


void AudioSequencePlayer::fillSequenceCaptionAndDescription(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition, bool verbose)
{
	Label* label = uiItem->getCaption();

	if (verbose)
	{
		if (label != NULL) fillSequenceContentAndPrompt(seq, label, pResolver);
	}
	
	LabelList * p_list = uiItem->getLabelList();

	label = uiItem->getDescription();
	
	if (verbose)
	{
		if (p_list == NULL && label != NULL) fillSequenceContentAndPrompt(seq, label, pResolver);
	}
	
	fillSequenceSwitch(seq, uiItem, pResolver, switchCondition);

	if (p_list != NULL) fillSequenceContentAndPrompt(seq, p_list, pResolver);
		
	if (verbose)
	{
		if (p_list != NULL && label != NULL) fillSequenceContentAndPrompt(seq, label, pResolver);
	}
}


void AudioSequencePlayer::fillSequenceSwitch(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition)
{
	Switch* zwitch;
	if ((zwitch = uiItem->getSwitch()) != NULL)
	{
		switch(zwitch->getSwitchType())
		{
			case ACTIONSWITCH:
			{
				Action* action = zwitch->getAction(switchCondition);
				if (action != NULL) fillSequenceCaptionAndDescription(seq, action, pResolver, switchCondition);
				break;
			}
			case CAPTIONSWITCH: {}
			case DESCRIPTIONSWITCH:
			{
				Label* label = zwitch->getLabel(switchCondition);
				if (label != NULL)	fillSequenceContentAndPrompt(seq, label, pResolver);
				break;
			}
		}
	}
}

void AudioSequencePlayer::fillSequenceContentAndPrompt(AudioSequence* seq, LabelList * p_list, PromptResolver * pResolver)
{
	for (int i = 0; i<p_list->getNumberOfLabels(); i++)
	{
		Prompt* p_prompt = p_list->getLabel(i)->getPrompt();
		if (p_prompt != NULL) fillSequencePrompt(seq, p_prompt, pResolver);
	}
}

void AudioSequencePlayer::fillSequenceContentAndPrompt(AudioSequence* seq, Label* label, PromptResolver * pResolver)
{
	TextAudioPair* pair = label->getContents();
	if (pair != NULL)
	{
		amis::AudioNode* audio = pair->getAudio();
		TextNodeSV* textN = pair->getText();
		if (!Preferences::Instance()->getIsSelfVoicingTTSOnly() && 
			audio != NULL && 
			audio->getPath().length() != 0) 
		{
			seq->append(audio->clone(), (textN != NULL ? textN->getTextString().c_str() : L""));
		} 
		else 
		{
			if (textN != NULL) seq->append(textN->getTextString().c_str());
		}
	}

	Prompt* prompt = label->getPrompt();
	if (prompt != NULL) fillSequencePrompt(seq, prompt, pResolver);
}

void AudioSequencePlayer::fillSequenceContents(AudioSequence* seq, PromptItemBase* pi)
{
	TextAudioPair* pair = pi->getContents();
	if (pair != NULL)
	{
		amis::AudioNode* audio = pair->getAudio();
		TextNodeSV * textN = pair->getText();

		if (!Preferences::Instance()->getIsSelfVoicingTTSOnly() && 
			audio != NULL && 
			audio->getPath().length() != 0) 
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
	if (pResolver != NULL)
	{
		pResolver->resolvePromptVariables(pPrompt);
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
		if (pos > -1) source_str.replace(pos, searchStr.length(), replaceStr);
	}
	return source_str;
}