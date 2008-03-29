#pragma once

#include "AudioSequencePlayer.h"
#include "promptresolver.h"

namespace amis
{
namespace gui
{
namespace spoken
{

class PreTranslateMessageHandler
{
public:
	bool PreTranslateMessageHandler::isControlPressed();

	PreTranslateMessageHandler(int idUiDialog);
	~PreTranslateMessageHandler(void);

	CString normalizeTextEntry(CString str, int nStartChar, int nEndChar);
	void handle(PromptResolver * pResolver, 
							MSG* pMsg,
							int idUiFocus,
							bool ignoreSpaceKey = false,
							bool ignoreArrowKeys = false,
							CString strTextField = CString(L""),
							CString strTextFieldFULL = CString(L""),
							bool ignoreCharKeys = false);
	
void handleCommand(WPARAM cmdid);

private:
	
	CString convertKeystrokeToSpeakableString(MSG* pMsg, bool ignoreSpaceKey = false, bool ignoreArrowKeys = false, bool ignoreCharKeys = false);
	CString convertKeystrokeToSpeakableString(WPARAM ch, bool ignoreSpaceKey = false, bool ignoreArrowKeys = false, bool ignoreCharKeys = false);

	int m_instructionsDialogID;

	bool mbKeyControl;
	WPARAM mLastKeyDown;

	//bool processGeneralDialogFeedback(bool playNow, AudioSequence*, MSG* pMsg, int idUiFocus, bool ignoreSpaceKey, bool ignoreArrowKeys, CString strTextField);
	//void processGeneralKeyPressFeedback(bool playNow, AudioSequence* ,MSG* pMsg, bool ignoreSpaceKey, bool ignoreArrowKeys);
};
}
}
}