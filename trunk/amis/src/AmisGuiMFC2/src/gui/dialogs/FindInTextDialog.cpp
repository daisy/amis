/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2008  DAISY for All Project

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

using namespace amis::gui::dialogs;

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
	mUserData.Empty();
	mDir = 0;
}

FindInTextDialog::~FindInTextDialog()
{
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
	
	PromptVar* p_var = NULL;
	PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

	for (int i=0; i<pPrompt->getNumberOfItems(); i++)
	{
		if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
		{
			p_var = (PromptVar*)pPrompt->getItem(i);

			if (p_var->getName().compare("SELECTED_OR_DESELECTED") == 0)
			{
				string strSelect;
				
	CButton* p_check_prev =	(CButton*)GetDlgItem(IDC_FINDPREVIOUS);

					if (p_check_prev->GetCheck() == 1)
					{
						strSelect = "item_selected";
					}
					else
					{
						strSelect = "item_deselected";
					}

					amis::gui::spoken::PromptItem* pi = NULL;

					pi = amis::gui::spoken::DataTree::Instance()->findPromptItem(strSelect);
					if (pi != NULL)
					{
						p_var->setContents(pi->getContents()->clone());
					}
			}
		}
	}
	AmisDialogBase::resolvePromptVariables(pPrompt);
}