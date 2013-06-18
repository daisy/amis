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
#include "../resource.h"
#include "gui/dialogs/AboutDialog.h"

#include "gui/AmisApp.h"

using namespace amis::gui::dialogs;

void AboutDialog::resolvePromptVariables(Prompt* pPrompt)
{
	USES_CONVERSION;
	for (int i = 0; i<pPrompt->getNumberOfItems(); i++)
	{
		PromptItemBase* p = pPrompt->getItem(i);
		if (p->getPromptItemType() == amis::gui::spoken::PROMPT_VARIABLE)
		{
			PromptVar* pvar = (PromptVar*)p;
			if (pvar->getName() == "VERSION_NUMBER")
			{
				wstring val = A2T(theApp.getVersion().c_str());
				pvar->setContents(val, "");
			}
			else if (pvar->getName() == "RELEASE_DATE")
			{
				wstring val = A2T(theApp.getReleaseDate().c_str());
				pvar->setContents(val, "");
			}
		}
	}
}
AboutDialog::AboutDialog() : AmisDialogBase(AboutDialog::IDD)
{
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
BOOL AboutDialog::OnInitDialog()
{
	USES_CONVERSION;
	CDialog::OnInitDialog();

	//IDC_VERSION
	 CStatic* p_version = (CStatic*)this->GetDlgItem(IDC_VERSION);
	 CString tmp = A2T(theApp.getVersion().c_str());
	 CString version;
	 p_version->GetWindowText(version);
	 version.Append(L" ");
	 version.Append(tmp);
	 p_version->SetWindowText(version);

    //IDC_VERSIONNOTE
	 CStatic* p_releasedate = (CStatic*)this->GetDlgItem(IDC_VERSIONNOTE);
	 tmp = A2T(theApp.getReleaseDate().c_str());
	 CString release_date;
	 p_releasedate->GetWindowText(release_date);
	 release_date.Append(L" ");
	 release_date.Append(tmp);
	 p_releasedate->SetWindowText(release_date);

	this->setFontOnAllControls();
		
	return TRUE;
}