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
#include "gui/dialogs/MultipleBooksOnVolumeDialog.h"
#include "../resource.h"
#include "AmisCore.h"

using namespace amis::gui::dialogs;

BEGIN_MESSAGE_MAP(MultipleBooksOnVolumeDialog, CDialog)
	ON_WM_PAINT()
	ON_LBN_DBLCLK(IDC_BOOKLIST, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_OPENBOOK, OnOpenbook)
END_MESSAGE_MAP()


void MultipleBooksOnVolumeDialog::resolvePromptVariables(Prompt* pPrompt)
{
	return;
}

MultipleBooksOnVolumeDialog::MultipleBooksOnVolumeDialog(CWnd* pParent /*=NULL*/, amis::UrlList* pUrlList)
	: AmisDialogBase(MultipleBooksOnVolumeDialog::IDD)
{
	mpUrlList = pUrlList;
}

MultipleBooksOnVolumeDialog::~MultipleBooksOnVolumeDialog()
{
}

BOOL MultipleBooksOnVolumeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	populateListControl();

	return TRUE;
}

//TODO: fill in titles, not URLs
void MultipleBooksOnVolumeDialog::populateListControl()
{
	USES_CONVERSION;
	CListBox* p_filelist = NULL;
	p_filelist = (CListBox*)this->GetDlgItem(IDC_BOOKLIST);

	amis::UrlList::iterator it;
	int i = 0;
	for (it = mpUrlList->begin(); it != mpUrlList->end(); ++it)
	{
		CString result;
		result = A2T(it->get_file().c_str());
		p_filelist->AddString(result);
		i++;
	}
	
	p_filelist->SetFocus();
	CButton* p_button = (CButton*)this->GetDlgItem(IDC_OPENBOOK);
	p_button->EnableWindow(TRUE);
}

void MultipleBooksOnVolumeDialog::loadBook()
{
	CListBox* p_list = NULL;
	p_list = (CListBox*)this->GetDlgItem(IDC_BOOKLIST);
	int sel = p_list->GetCurSel();
	if (sel > -1 && sel < mpUrlList->size()) mLoadBookOnDialogClose = (*mpUrlList)[sel];
	this->EndDialog(1);
}

ambulant::net::url MultipleBooksOnVolumeDialog::getBookToLoad()
{
	return mLoadBookOnDialogClose;
}

void MultipleBooksOnVolumeDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}

void MultipleBooksOnVolumeDialog::OnOpenbook() 
{
	loadBook();
}

void MultipleBooksOnVolumeDialog::OnDblclkFilelist() 
{
	loadBook();
}

BOOL MultipleBooksOnVolumeDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* p_wnd = this->GetFocus();
		if (p_wnd)
		{
			int id = p_wnd->GetDlgCtrlID();
			//inexplicably, the default behavior for pressing enter in a dialog is to close it.
			//we're overriding this here.
			if (pMsg->wParam == VK_RETURN)
			{
				 //if the files list has focus, open the selected book
				 if (id == IDC_BOOKLIST)
				 {
					 pMsg->wParam = NULL;
					 loadBook();
				 }
			}	
		}
	}
	return AmisDialogBase::PreTranslateMessage(pMsg);
}
