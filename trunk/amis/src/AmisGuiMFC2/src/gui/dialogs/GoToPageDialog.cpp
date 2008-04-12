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

#include "stdafx.h"
#include "gui/dialogs/GoToPageDialog.h"
#include "../resource.h"

#include "Preferences.h"

using namespace amis::gui::dialogs;

BEGIN_MESSAGE_MAP(GoToPageDialog, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void GoToPageDialog::resolvePromptVariables(Prompt* pPrompt) 
{
	return;
}

GoToPageDialog::GoToPageDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(GoToPageDialog::IDD)
{
	m_PageNumber = _T("");
}

GoToPageDialog::~GoToPageDialog()
{
}

void GoToPageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PAGENUM, m_PageNumber);
}

BOOL GoToPageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//set focus to the field
	CWnd* p_control = NULL;
	p_control = GetDlgItem(IDC_PAGENUM);
	if (p_control)
	{
		p_control->SetFocus();
		return FALSE;
	}
	// return TRUE unless you set the focus to a control
	else return TRUE;
}

std::wstring GoToPageDialog::getPageNumber()
{
	return m_PageNumber.Trim().GetBuffer();
}


void GoToPageDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}

BOOL GoToPageDialog::PreTranslateMessage(MSG* pMsg)
{
	return AmisDialogBase::PreTranslateMessageTextField(pMsg, IDC_PAGENUM);

	/*
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* cwnd = this->GetFocus();
		if (cwnd) {
			//int id = cwnd->GetDlgCtrlID();
			CEdit* p_edit = (CEdit*)GetDlgItem(IDC_PAGENUM);
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
						mCommonPreTranslateMessageHandler->handle(pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()),false,true,str, strFULL, false);
						return CDialog::PreTranslateMessage(pMsg);
					}
				}
			}

			mCommonPreTranslateMessageHandler->handle(pMsg, (cwnd == NULL ? -1 : cwnd->GetDlgCtrlID()));
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
	*/
}