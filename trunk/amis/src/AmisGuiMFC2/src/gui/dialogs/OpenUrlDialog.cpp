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
#include "gui/dialogs/OpenUrlDialog.h"
#include "../resource.h"

#include "Preferences.h"

using namespace amis::gui::dialogs;

BEGIN_MESSAGE_MAP(OpenUrlDialog, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//IMPLEMENT_DYNAMIC(OpenUrlDialog, CDialog)

void OpenUrlDialog::resolvePromptVariables(Prompt* pPrompt)
{
	return;
}
OpenUrlDialog::OpenUrlDialog(CWnd* pParent /*=NULL*/)
	:  AmisDialogBase(OpenUrlDialog::IDD)
{
}

OpenUrlDialog::~OpenUrlDialog()
{
}

void OpenUrlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_URL, mUrl);
}


BOOL OpenUrlDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	this->setFontOnAllControls();
	CEdit* p_edit =	(CEdit*)GetDlgItem(IDC_URL);	
	p_edit->SetSel(0, 99);
	
	return TRUE;
}

CString OpenUrlDialog::getUrl()
{
	return mUrl;
}


void OpenUrlDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}

BOOL OpenUrlDialog::PreTranslateMessage(MSG* pMsg)
{
	return AmisDialogBase::PreTranslateMessageTextField(pMsg, IDC_URL);
}