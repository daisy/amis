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
#include "gui/dialogs/AmisFileDialog.h"
#include "../resource.h"
#include "Preferences.h"
#include "gui/AmisApp.h"

using namespace amis::gui::dialogs;

//this is just meant for opening files right now
//but you still have to pass the filter because it needs to have been constructed
//at runtime (for the localized strings to be correct)
AmisFileDialog::AmisFileDialog(CString filter)
	: CFileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)filter) //, AmisDialogBase(0)
{
}

AmisFileDialog::~AmisFileDialog(void)
{
}

std::string AmisFileDialog::showOpen()
{
	USES_CONVERSION;
	CString file_name;
	std::string str_filename = "";

	bool b = amis::gui::CAmisApp::beforeModalBox();

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		AudioSequencePlayer::playPromptFromStringId("OpenFile_notSelfVoicing");
	}
	if (this->DoModal() == IDOK)
	{
		file_name = this->GetPathName();
		str_filename = T2A(file_name);
	}
	
	theApp.afterModalBox(b);
	return str_filename;
}

CString AmisFileDialog::getFileOpenFilter()
{
	CString filter;
	filter.LoadString(IDS_FILE_OPEN_DAISY_BOOKS);
	filter += _T(" (");
	filter += _T("ncc.html; ncc.htm; *.opf");
	filter += _T(")|");
	filter += _T("ncc.html; ncc.htm; *.opf");
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	filter += _T("|DAISY Pdtb Keys (*.uak)|*.uak");
#endif
	filter += _T("||");

	return filter;
}