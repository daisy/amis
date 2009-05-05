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

#include "stdafx.h"

#include "gui/dialogs/SkipDialog.h"

#include "../resource.h"
#include "Preferences.h"

#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/datamodel/Dialog.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include "gui/self-voicing/datamodel/Label.h"
#include "gui/self-voicing/Prompt.h"
#include "gui/self-voicing/UiMediaTypes.h"
#include "gui/self-voicing/dialogs/SkipDialogVoicing.h"

using namespace amis::gui::dialogs;
using namespace amis::gui::spoken;

amis::gui::dialogs::SkipDialogVoicing * mpSkipDialogVoicing = NULL;


BEGIN_MESSAGE_MAP(SkipDialog, CDialog)
	ON_LBN_SELCHANGE(IDC_OPTIONS_LIST, OnSelchangeSkips)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void SkipDialog::resolvePromptVariables(Prompt* pPrompt) 
{
	mpSkipDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}

SkipDialog::SkipDialog(CWnd* pParent /*=NULL*/)
	 : AmisDialogBase(SkipDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpSkipDialogVoicing = new amis::gui::dialogs::SkipDialogVoicing(this);
	}
	else
	{
		mpSkipDialogVoicing = NULL;
	}
	mpCustomTests = NULL;
}

SkipDialog::~SkipDialog()
{	
	if (mpSkipDialogVoicing != NULL)
	{
		delete mpSkipDialogVoicing;
		mpSkipDialogVoicing = NULL;
	}
}
BOOL SkipDialog::PreTranslateMessage(MSG* pMsg)
{
	if (Preferences::Instance()->getIsSelfVoicing() == false)
	{
		return CDialog::PreTranslateMessage(pMsg);
	}

	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* cwnd = this->GetFocus();
		if (cwnd) 
		{
			int id = cwnd->GetDlgCtrlID();
			mCommonPreTranslateMessageHandler->handle(this, pMsg, id, (id == IDC_OPTIONS_LIST ? true : false), (id == IDC_OPTIONS_LIST ? true : false), L"", L"", (id == IDC_OPTIONS_LIST ? true : false));

			if (pMsg->message == WM_KEYUP)
			{
				if (id == IDC_OPTIONS_LIST && (pMsg->wParam == ' ' || pMsg->wParam == VK_SPACE)) 
				{
					OnSelchangeSkips();
				}
			}
			return CDialog::PreTranslateMessage(pMsg);
		}
	}

	return AmisDialogBase::PreTranslateMessage(pMsg);
}

void SkipDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTIONS_LIST, m_ListSkips);
	DDX_Control(pDX, IDOK, m_Ok);
}


void SkipDialog::initializeData(amis::dtb::CustomTestSet* pTests)
{
	if (pTests == NULL) return;
	mpCustomTests = pTests;
}

BOOL SkipDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//clear the list box
	int sz = m_ListSkips.GetCount();
	for (int i=sz-1; i>=0; i--) m_ListSkips.DeleteString(i);

	fillListBox();

	/* 
	//if the list has something in it, set focus to the listbox; else to the OK button
	if (mpCustomTests != NULL && mpCustomTests->getLength() > 0) m_ListSkips.SetFocus();
	else m_Ok.SetFocus();
	*/
	this->setFontOnAllControls();

	return TRUE;
}

void SkipDialog::fillListBox()
{
	USES_CONVERSION;
	
	//populate the list box
	amis::dtb::CustomTest* p_test = NULL;
		
	for (unsigned int i=0; i<mpCustomTests->getLength(); i++)
	{
		p_test = mpCustomTests->getCustomTest(i);

		if (p_test != NULL)
		{
			CString entry;
			entry = p_test->getLabel()->getText()->getTextString().c_str();
			int idx = m_ListSkips.AddString(entry);
			m_ListSkips.SetItemData(idx,(DWORD)p_test);

			//false = skip this item (display as unchecked)
			if (p_test->getCurrentState() == false) m_ListSkips.SetCheck(idx, 0);
			else m_ListSkips.SetCheck(idx, 1);
		}
	}
}

//the user clicked a checkbox in a list item
void SkipDialog::OnSelchangeSkips() 
{
	int i = m_ListSkips.GetCurSel();
	if (i < 0) return;

	amis::dtb::CustomTest* p_test = NULL;
	p_test = (amis::dtb::CustomTest*)m_ListSkips.GetItemData(i);
	if (p_test == NULL) return;

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpSkipDialogVoicing->OnSelchangeSkips();
	}

	//if it's checked, it means we want to play it
	if (m_ListSkips.GetCheck(i)) p_test->setCurrentState(true);
	else p_test->setCurrentState(false);
}


void SkipDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}