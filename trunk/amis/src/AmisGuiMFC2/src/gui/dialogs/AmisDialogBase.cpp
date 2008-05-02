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
#include "gui/dialogs/AmisDialogBase.h"
#include "Preferences.h"
#include "gui/MainWndParts.h"

#include "gui/AmisApp.h"

using namespace amis::gui::dialogs;


IMPLEMENT_DYNAMIC(AmisDialogBase, CDialog)
//IMPLEMENT_DYNCREATE(AmisDialogBase, CDialog)

void AmisDialogBase::resolvePromptVariables(Prompt* pPrompt) {
	return;
}

void AmisDialogBase::triggerVirtualKeyStroke(CWnd* cwnd) {

	if (amis::Preferences::Instance()->getIsSelfVoicing() == false) {
		
		return;
	}
	MSG * msg = new MSG();

	msg->message = WM_KEYDOWN;
	msg->hwnd = cwnd->GetSafeHwnd();
	msg->lParam = 0;
	msg->wParam = VK_UP;
	
	//PreTranslateMessage(msg);
	mCommonPreTranslateMessageHandler->handle(this, msg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()));
	
	msg->message = WM_KEYUP;
	
	//PreTranslateMessage(msg);
	mCommonPreTranslateMessageHandler->handle(this, msg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()));

	delete msg;
}
BOOL AmisDialogBase::PreTranslateMessageTextField(MSG* pMsg, UINT id)
{
	if (Preferences::Instance()->getIsSelfVoicing() == false) 
		return CDialog::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* cwnd = this->GetFocus();
		if (cwnd) 
		{
			//int id = cwnd->GetDlgCtrlID();
			CEdit* p_edit = (CEdit*)GetDlgItem(id);
			if (cwnd == p_edit) {

				CString str;
				p_edit->GetWindowText(str);


				if (!str.IsEmpty()) {

					CString strFULL = str;

					int nStartChar = -1;
					int nEndChar = -1;
					p_edit->GetSel(nStartChar, nEndChar);

					str = mCommonPreTranslateMessageHandler->normalizeTextEntry(str, nStartChar, nEndChar);

					if (!str.IsEmpty()) { // && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)) {
						mCommonPreTranslateMessageHandler->handle(this, pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()),false,true,str, strFULL, false);
						return CDialog::PreTranslateMessage(pMsg);
					}
				}
			}

			mCommonPreTranslateMessageHandler->handle(this, pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()));
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
BOOL AmisDialogBase::PreTranslateMessage(MSG* pMsg)
{
	if (Preferences::Instance()->getIsSelfVoicing() == false) 
		return CDialog::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* cwnd = this->GetFocus();
		if (cwnd) 
		{
			mCommonPreTranslateMessageHandler->handle
				(this, pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()));
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

INT_PTR AmisDialogBase::do_modal()
{
	
	bool b = amis::gui::CAmisApp::beforeModalBox();
	INT_PTR ret = DoModal();
	theApp.afterModalBox(b);


	return ret;
}

AmisDialogBase::AmisDialogBase(int id) : CDialog(id)
{
	mId = id;
	mCommonPreTranslateMessageHandler = new PreTranslateMessageHandler(mId);
}
AmisDialogBase::~AmisDialogBase()
{
	delete mCommonPreTranslateMessageHandler;
}
void AmisDialogBase::setFirstDraw(bool val)
{
	mbFlag_FirstDraw = val;
}

void AmisDialogBase::on_init()
{
	mbFlag_FirstDraw = true;
}

void AmisDialogBase::on_paint()
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == false)
	{
		mbFlag_FirstDraw = false;
		return;
	}

	if(mbFlag_FirstDraw)
	{
		if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
		{
			AudioSequencePlayer::playDialogWelcome(mId, this);

			//AudioSequencePlayer::playDialogTextControlsFromUiId(mId, this);
			//AudioSequencePlayer::Instance()->playDialogInstructionsFromUiId(mId);
		}
		mbFlag_FirstDraw = false;
	}	
}

BEGIN_MESSAGE_MAP(AmisDialogBase, CDialog)
	//{{AFX_MSG_MAP(AmisDialogBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()