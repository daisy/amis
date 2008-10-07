/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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

#ifndef AudioSequencePlayer_H
#define AudioSequencePlayer_H

#pragma once

//#include <process.h>
#include <stdlib.h>

#ifndef TRACE
#define TRACE ATLTRACE
#endif

#include "datamodel/UiItem.h"
#include "promptresolver.h"
#include "AudioSequence.h"

using namespace amis::gui::spoken;

namespace amis
{
	namespace gui
	{
		namespace spoken
		{

			class AudioSequencePlayer
			{

			public:
				AudioSequencePlayer(void);
				~AudioSequencePlayer(void);
				
				static bool InstanceExists();
				static AudioSequencePlayer* Instance();

				string stringReplaceAll(string sourceStr, string searchStr, string replaceStr);

				std::string computeExpandedSrc(amis::AudioNode* pAudio, std::string langID = "");

				bool playAudioPrompt(amis::AudioNode* pAudio);
				void Play(AudioSequence* audioSequence, bool doNotRegisterInHistory = false, bool repeat = false, bool repeating = false);

				void Stop(bool fromPlay = false);

				void playNext(bool fromEndEvent);

				void RepeatLast();

				void DestroyInstance();

				bool checkEndSeq();

				void WaitForEndSeqAndRestartBook();
				void waitForSequenceEnd();

				static void playPromptFromUiId(int nItemID, AudioSequence* seq = NULL, bool verbose = true);

				
				static std::wstring getTextForDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, string switchCondition);
	
				static std::wstring getTextForPromptFromStringId(string promptId);
				static std::wstring getTextForPromptItemFromStringId(string promptId);
				
				static bool playPromptFromStringId(string, bool repeat = false);
				static bool playPromptItemFromStringId(string, bool repeat = false);

				static bool playDialogInstructionsFromUiId(int nItemID);
				static bool playDialogTextControlsFromUiId(int nItemID, PromptResolver* pResolver);
				static bool playDialogWelcome(int nItemID, PromptResolver* pResolver, bool playfull);

				static AudioSequence * playDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, bool playNow, string switchCondition);

				static void fillSequenceContainerPromptFromId(AudioSequence* seq, string id);
				static void fillSequencePrompt(AudioSequence* seq, Prompt* prompt, PromptResolver* pResolver);
				static void fillSequenceCaptionAndDescription(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition);
				static void fillSequenceSwitch(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition);

				static void fillSequenceContentAndPrompt(AudioSequence* seq, Label* label, PromptResolver * pResolver);
				static void fillSequenceContentAndPrompt(AudioSequence* seq, LabelList * p_list, PromptResolver * pResolver);

				static void fillSequenceContents(AudioSequence* seq, PromptItemBase* pi);

				static void resolvePromptVariables(Prompt*, PromptResolver* pResolver);

				HANDLE m_hEventWakeup;
				bool m_bAbort;
				
			
				AudioSequence* m_previousAudioSequence;
				AudioSequence* m_currentAudioSequence;
				AudioSequence* m_nextAudioSequence;

				
				CRITICAL_SECTION m_csSequence;
				

				HANDLE m_hThread;

				HANDLE m_hEventEnd;
				
				bool bIgnoreTTSEnd;

			private:
				bool mRepeatLoop;
				static AudioSequencePlayer* pinstance;
				static void fillOK_CANCEL(AudioSequence * seq, UINT ctrlId);
			};
		}
	}
}
#endif //AudioSequencePlayer_H