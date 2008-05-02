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
//#include "AudioSequenceComponent.h"
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
string stringReplaceAll(string sourceStr, string searchStr, string replaceStr);
bool playAudioPrompt(amis::AudioNode* pAudio);


	static bool InstanceExists();
	static AudioSequencePlayer* Instance();
	void Play(AudioSequence* audioSequence, bool doNotRegisterInHistory = false);

	void Stop(bool fromPlay = false);
	void playNext(bool fromEndEvent);

	void RepeatLast();
	void TTSAudioEndNotify();
	void ClipAudioEndNotify();
		void DestroyInstance();

	int m_wakeUpAction;
	int m_wakeUpOriginator;
	int m_nEndEventsToExpect;
	AudioSequence* m_previousAudioSequence;
	AudioSequence* m_currentAudioSequence;
	AudioSequence* m_nextAudioSequence;
	static AudioSequencePlayer* pinstance;
	AudioSequencePlayer(void);
	~AudioSequencePlayer(void);

	HANDLE m_hThread;
	HANDLE m_hEventWakeup;

	HANDLE m_hEventEnd;
	CRITICAL_SECTION m_csSequence;
	bool m_bAbort;
	bool bIgnoreTTSEnd;

void checkEndSeq();

void WaitForEndSeqAndRestartBook();
	void waitForSequenceEnd();

	//static void AppendAudioSequenceStringResource(AudioSequence* seq, int nItemID);
	//static void AppendAudioSequenceStringID(AudioSequence* seq, string);

	static void playPromptFromUiId(int nItemID, AudioSequence* seq = NULL);
	
	static void playPromptFromStringId(string);
	static void playPromptItemFromStringId(string);
	static void playDialogInstructionsFromUiId(int nItemID);
	static void playDialogTextControlsFromUiId(int nItemID, PromptResolver* pResolver);
	static void playDialogWelcome(int nItemID, PromptResolver* pResolver);
	static AudioSequence * playDialogControlFromUiIds(int dlgID, int ctrlId, PromptResolver* pResolver, bool playNow, string switchCondition);
	
	static void fillSequenceContainerPromptFromId(AudioSequence* seq, string id);
	static void fillSequencePrompt(AudioSequence* seq, Prompt* prompt, PromptResolver* pResolver);
	static void fillSequenceCaptionAndDescription(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition);
	static void fillSequenceSwitch(AudioSequence* seq, UiItem* uiItem, PromptResolver * pResolver, string switchCondition);

	static void fillSequenceContentAndPrompt(AudioSequence* seq, Label* label, PromptResolver * pResolver);
	static void fillSequenceContentAndPrompt(AudioSequence* seq, LabelList * p_list, PromptResolver * pResolver);
	
	static void fillSequenceContents(AudioSequence* seq, PromptItemBase* pi);
	
	static void resolvePromptVariables(Prompt*, PromptResolver* pResolver);

private:
	static void fillOK_CANCEL(AudioSequence * seq, UINT ctrlId);
};

}
}
}
#endif