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
#include "../resource.h"
#include "gui/dialogs/AboutDialog.h"

using namespace amis::gui::dialogs;

void AboutDialog::resolvePromptVariables(Prompt* pPrompt) {
	return;
}
AboutDialog::AboutDialog() : AmisDialogBase(AboutDialog::IDD)
{
}


void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL AboutDialog::OnInitDialog() 
{
	USES_CONVERSION;

	CDialog::OnInitDialog();

	CStatic* p_ctrl_version = NULL;
	CStatic* p_ctrl_version_note = NULL;

	p_ctrl_version = (CStatic*)this->GetDlgItem(IDC_VERSION);
	p_ctrl_version_note = (CStatic*)this->GetDlgItem(IDC_VERSIONNOTE);

	if (p_ctrl_version != NULL && p_ctrl_version_note != NULL)
	{
		p_ctrl_version->SetWindowText(mVersion);
		p_ctrl_version_note->SetWindowText(mVersionNote);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(AboutDialog, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void AboutDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}
