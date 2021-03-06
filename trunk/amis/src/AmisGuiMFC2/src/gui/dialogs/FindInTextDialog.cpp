/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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
#include "gui/dialogs/FindInTextDialog.h"
#include "../resource.h"

#include "Preferences.h"

#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/datamodel/Dialog.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include "gui/self-voicing/datamodel/Label.h"
#include "gui/self-voicing/Prompt.h"
#include "gui/self-voicing/UiMediaTypes.h"

#include "gui/self-voicing/dialogs/FindInTextDialogVoicing.h"

using namespace amis::gui::dialogs;
using namespace amis::gui::spoken;

amis::gui::dialogs::FindInTextDialogVoicing * mpFindInTextDialogVoicing = NULL;

BEGIN_MESSAGE_MAP(FindInTextDialog, CDialog)
	ON_WM_PAINT()
	ON_BN_SETFOCUS(IDOK, OnSetfocusOk)
	ON_BN_CLICKED(IDOK,	OnBnClickedOk)
	ON_BN_CLICKED(IDC_FINDPREVIOUS,	OnBnClickedTick)
	ON_EN_SETFOCUS(IDC_SEARCHSTRING, OnSetfocusSearchString)
	ON_EN_CHANGE(IDC_SEARCHSTRING, OnEnChangeSearchstring)
	ON_BN_SETFOCUS(IDCANCEL, OnSetfocusCancel)
END_MESSAGE_MAP()


FindInTextDialog::FindInTextDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(FindInTextDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpFindInTextDialogVoicing = new amis::gui::dialogs::FindInTextDialogVoicing(this);
	}
	else
	{
		mpFindInTextDialogVoicing = NULL;
	}
	mUserData.Empty();
	mDir = 0;
}

FindInTextDialog::~FindInTextDialog()
{
	if (mpFindInTextDialogVoicing != NULL)
	{
		delete mpFindInTextDialogVoicing;
		mpFindInTextDialogVoicing = NULL;
	}
}

CString	FindInTextDialog::getUserSearchString()
{
	return mUserData;
}

int	FindInTextDialog::getSearchDirection()
{
	return mDir;
}

BOOL FindInTextDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* p_edit =	(CEdit*)GetDlgItem(IDC_SEARCHSTRING);
	p_edit->SetWindowText(mUserData);

	CButton* p_check_prev =	(CButton*)GetDlgItem(IDC_FINDPREVIOUS);
	if (mDir ==	1 || mDir == 0)
	{
		p_check_prev->SetCheck(0);
	}
	else
	{
		p_check_prev->SetCheck(1);
	}

	this->setFontOnAllControls();
	p_edit->SetSel(0, 99);
		
	return TRUE;
}

void FindInTextDialog::forceSearchValue(CString searchString)
{
	mUserData =	searchString;
}

void FindInTextDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}
BOOL FindInTextDialog::PreTranslateMessage(MSG* pMsg)
{
	return AmisDialogBase::PreTranslateMessageTextField(pMsg, IDC_SEARCHSTRING);
}

void FindInTextDialog::OnEnChangeSearchstring()
{	
}
void FindInTextDialog::OnSetfocusSearchString() 
{
}
void FindInTextDialog::OnBnClickedOk()
{
	CEdit* p_edit =	(CEdit*)GetDlgItem(IDC_SEARCHSTRING);
	p_edit->GetWindowText(mUserData);

	CButton* p_check = (CButton*)GetDlgItem(IDC_FINDPREVIOUS);
	if (p_check->GetCheck()	== 1) mDir = -1;
	else mDir = 1;

	OnOK();
}
void FindInTextDialog::OnBnClickedTick()
{
	CWnd* cwnd = this->GetFocus();
	triggerVirtualKeyStroke(cwnd);
}
void FindInTextDialog::OnSetfocusOk() 
{
}
void FindInTextDialog::OnSetfocusCancel() 
{
}
void FindInTextDialog::resolvePromptVariables(Prompt* pPrompt) 
{
	mpFindInTextDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}