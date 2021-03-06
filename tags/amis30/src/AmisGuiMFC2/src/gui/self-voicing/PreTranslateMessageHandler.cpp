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

#include "gui/self-voicing/pretranslatemessagehandler.h"

#include "gui/self-voicing/datamodel/UiItem.h"
#include "gui/self-voicing/datamodel/DataTree.h"

#include "../../AmisGuiMFC2/resource.h"

#include "util/Log.h"

PreTranslateMessageHandler::PreTranslateMessageHandler(int idUiDialog)
{
	m_instructionsDialogID = idUiDialog;
	mbKeyControl = false;
	mLastKeyDown = -1;
}

PreTranslateMessageHandler::~PreTranslateMessageHandler(void)
{
}

bool PreTranslateMessageHandler::isControlPressed()
{
	return mbKeyControl;
}

CString PreTranslateMessageHandler::normalizeTextEntry(CString str, int nStartChar, int nEndChar)
{

	CString strNew = str;
	if (nStartChar == nEndChar && nStartChar != -1)
	{
		if (nStartChar >= str.GetLength())
		{
			//str = L"";
		}
		else
		{
			strNew = str.Mid(nStartChar, 1);
		}
	}
	else if (nStartChar != -1 && nEndChar != -1)
	{
		strNew = str.Mid(nStartChar, nEndChar-nStartChar);
	}
	return strNew;
}

CString PreTranslateMessageHandler::convertKeystrokeToSpeakableString(WPARAM ch, bool ignoreSpaceKey, bool ignoreArrowKeys, bool ignoreCharKeys)
{
	if (!ignoreCharKeys)
	{
		switch (ch) 
		{ 
		case 0x08: 

			// Process a backspace. 

			break; 

		case 0x0A: 

			// Process a linefeed. 

			break; 

		case 0x1B: 

			// Process an escape. 

			break; 

		case 0x09: 

			// Process a tab. 

			break; 

		case 0x0D: 

			// Process a carriage return. 

			break; 

		default: 

			// Process displayable characters. 

			//echo keystrokes
			return CString((char)ch);
		}
	}
	return L"";
}

CString PreTranslateMessageHandler::convertKeystrokeToSpeakableString(MSG* pMsg, bool ignoreSpaceKey, bool ignoreArrowKeys, bool ignoreCharKeys)
{
	WPARAM ch = pMsg->wParam;

	/*if (pMsg->message == WM_CHAR)
	{
	cstr = convertKeystrokeToSpeakableString(ch, ignoreSpaceKey, ignoreArrowKeys, ignoreCharKeys);
	}
	else*/
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		if (ch == VK_BACK)
		{ 
			return L"backspace";
		} else if (ch == VK_CONTROL)
		{ 
			return L"control";
		} else if (ch == VK_SHIFT)
		{ 
			//return L"shift";
			return L""; // Because when this key is used in combination with TAB to reverse-navigate, it overrides the prompt for the dialog that is reached.
		} else if (ch == VK_LEFT)
		{ 
			if (!ignoreArrowKeys) return L"left";
		} else if (ch == VK_UP)
		{ 
			if (!ignoreArrowKeys) return L"up";
		} else if (ch == VK_RIGHT)
		{ 
			if (!ignoreArrowKeys) return L"right";
		} else if (ch == VK_DOWN)
		{ 
			if (!ignoreArrowKeys) return L"down";
		}  else if (ch == VK_ESCAPE)
		{ 
			//return L"escape";
			return L""; // Because this key activates the corresponding MFC action (i.e. close a dialog) on KEY_DOWN, as opposed to KEY_UP, which means that the "dialogClosed" prompt would be overridden. The SPACE key does not have this problem.
		} else if (ch == VK_RETURN)
		{ 
			//return L"return";
			return L""; // Because this key activates the corresponding MFC action (i.e. close a dialog) on KEY_DOWN, as opposed to KEY_UP, which means that the "dialogClosed" prompt would be overridden. The SPACE key does not have this problem.
		} else if (ch == VK_HOME)
		{ 
			if (!ignoreArrowKeys) return L"home";
		} else if (ch == VK_END)
		{ 
			if (!ignoreArrowKeys) return L"end";
		}
		else if (ch == VK_TAB)
		{ 
			return L"tab";
		}
		else if (ch == VK_SPACE)
		{ 
			if (!ignoreSpaceKey) return L"space";
		}
		else if (ch == VK_DELETE)
		{ 
			return L"delete";
		}
		else if (ch == VK_F1)
		{ 
			return L"f one";
		}
		else if (ch == VK_F2)
		{ 
			return L"f two";
		}
		else if (ch == VK_F3)
		{ 
			return L"f three";
		}
		else if (ch == VK_F4)
		{ 
			return L"f four";
		}
		else if (ch == VK_F5)
		{ 
			return L"f five";
		}
		else if (ch == VK_F6)
		{ 
			return L"f six";
		}
		else if (ch == VK_F7)
		{ 
			return L"f seven";
		}
		else if (ch == VK_F8)
		{ 
			return L"f eight";
		}
		else if (ch == VK_F9)
		{ 
			return L"f nine";
		}
		else if (ch == VK_F10)
		{ 
			return L"f ten";
		}
		else if (ch == VK_F11)
		{ 
			return L"f eleven";
		}
		else if (ch == VK_F12)
		{ 
			return L"f twelve";
		} else {
			return convertKeystrokeToSpeakableString(ch, ignoreSpaceKey, ignoreArrowKeys, ignoreCharKeys);
		}
	}

	return L"";
}


void PreTranslateMessageHandler::handleCommand(WPARAM cmdid)
{
	if (SELF_VOICING_PLAY_NEXT == cmdid)
	{
		AudioSequencePlayer::Instance()->playNext(true);
	}
}
void PreTranslateMessageHandler::handle(PromptResolver * pResolver, MSG* pMsg, int idUiFocus, bool ignoreSpaceKey, bool ignoreArrowKeys, CString strTextField, CString strTextFieldFULL, bool ignoreCharKeys)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (mLastKeyDown == pMsg->wParam)
		{
			return;
		}
		mLastKeyDown = pMsg->wParam;

		if (pMsg->wParam == VK_CONTROL)
		{
			mbKeyControl = true;
		}	
		
		return;		
	}
	else if (pMsg->message == WM_KEYUP)
	{
		bool wasSameKey = false;
		if (mLastKeyDown == pMsg->wParam)
		{
			//mLastKeyDown = 0;
			wasSameKey = true;
		}
		if (pMsg->wParam == VK_CONTROL)
		{
			if (mbKeyControl && wasSameKey && mLastKeyDown == VK_CONTROL)
			{
				//if (seq != NULL && !(seq->IsEmpty()) && playNow) AudioSequencePlayer::Instance()->Play(seq, true);
				//else AudioSequencePlayer::Instance()->Stop();
				TRACE(L"\n @@@ CONTROL KEY STOP\n");
				amis::util::Log* p_log = amis::util::Log::Instance();
				p_log->writeTrace("CONTROL KEY STOP", "PreTranslateMessageHandler::handle");
				AudioSequencePlayer::Instance()->Stop();
			}
			mbKeyControl = false;
			return;
		}
		else if (pMsg->wParam == VK_SHIFT)
		{
			return;
		}
		else if (pMsg->wParam == VK_F1 && wasSameKey)
		{
			AudioSequencePlayer::playDialogWelcome(m_instructionsDialogID, NULL, false);
			return;
		}
		else  if(pMsg->wParam == 'I' && mbKeyControl == true)
		{
			mbKeyControl = false;
			//repeat the instructions
			//AudioSequencePlayer::Instance()->playDialogInstructionsFromUiId(m_instructionsDialogID);
			
			AudioSequencePlayer::playDialogWelcome(m_instructionsDialogID, NULL, false);
			return;
		}
		else if(pMsg->wParam == 'R' && mbKeyControl == true)
		{
			mbKeyControl = false;
			AudioSequencePlayer::Instance()->RepeatLast();
			return;
		}
		else if(pMsg->wParam == 'T' && mbKeyControl == true)
		{
			mbKeyControl = false;
			if (!strTextFieldFULL.IsEmpty())
			{
				AudioSequence * mSeq = new AudioSequence();
				Prompt* p_prompt_ = DataTree::Instance()->findPrompt("textFieldEntry");

				if (p_prompt_ != NULL)
				{
					AudioSequencePlayer::Instance()->fillSequencePrompt(mSeq, p_prompt_, pResolver);
				}
				if (strTextFieldFULL.GetLength() == 1)
				{
					strTextFieldFULL = convertKeystrokeToSpeakableString(strTextFieldFULL.GetAt(0));
				}
				mSeq->append(strTextFieldFULL);
				AudioSequencePlayer::Instance()->Play(mSeq);
			}

			return;
		}
		else 
		{
			if (pMsg->wParam == VK_TAB || (!ignoreArrowKeys && (pMsg->wParam == VK_HOME || pMsg->wParam == VK_END || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)))
			{
				if (idUiFocus == IDOK)
				{
					AudioSequencePlayer::Instance()->playDialogControlFromUiIds(m_instructionsDialogID, IDOK, pResolver, true, "default");
					return;
				}
				else if (idUiFocus == IDCANCEL)
				{
					AudioSequencePlayer::Instance()->playDialogControlFromUiIds(m_instructionsDialogID, IDCANCEL, pResolver, true, "default");
					return;

				}
				else if (idUiFocus >= 0)
				{
					AudioSequence * mSeq = AudioSequencePlayer::Instance()->playDialogControlFromUiIds(m_instructionsDialogID, idUiFocus, pResolver, false, "default");
					if (mSeq == NULL)
					{
						mSeq = new AudioSequence();
					}

					if (! strTextField.IsEmpty())
					{
						if (strTextField.GetLength() == 1)
						{
							strTextField = convertKeystrokeToSpeakableString(strTextField.GetAt(0));
						}
						mSeq->append(strTextField);
					}
					
					if (! strTextFieldFULL.IsEmpty() && strTextField.Compare(strTextFieldFULL) != 0)
					{
						if (strTextFieldFULL.GetLength() == 1)
						{
							strTextFieldFULL = convertKeystrokeToSpeakableString(strTextFieldFULL.GetAt(0));
						}
						mSeq->append(strTextFieldFULL);
					}
					if (mSeq->GetCount() == 0)
					{
						delete mSeq;
					}
					else
					{
						AudioSequencePlayer::Instance()->Play(mSeq);
					}
					return;
				}  	
			}
			else
			{
				AudioSequence * mSeq = new AudioSequence();
				if (!(pMsg->wParam == VK_SHIFT || pMsg->wParam == VK_HOME || pMsg->wParam == VK_END || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
				{
					if (pMsg->wParam == VK_SPACE)
					{
						if (! strTextField.IsEmpty())
						{
							mSeq->append(convertKeystrokeToSpeakableString(pMsg, false, true, false));
						}
					}
					else
					{
						if (! strTextField.IsEmpty())
						{
							mSeq->append(convertKeystrokeToSpeakableString(pMsg, false, true, false));
						}
					}
				}
				if (! strTextField.IsEmpty())
				{
					if (strTextField.GetLength() == 1)
					{
						strTextField = convertKeystrokeToSpeakableString(strTextField.GetAt(0));
					}
					mSeq->append(strTextField);
					if (strTextField.Compare(strTextFieldFULL) != 0)
					{
						Prompt* p_prompt_ = DataTree::Instance()->findPrompt("textFieldEntry");
						if (p_prompt_ != NULL)
						{
							AudioSequencePlayer::Instance()->fillSequencePrompt(mSeq, p_prompt_, pResolver);
						}
						mSeq->append(strTextFieldFULL);
					}
					//if (playNow) AudioSequencePlayer::Instance()->Play(seq);
				}
				if (!(mSeq->IsEmpty()))  // && wasSameKey)
				{
					AudioSequencePlayer::Instance()->Play(mSeq, true);
					return;
				}
				else
				{
					delete mSeq;
				}
			}
		}
	}
}
