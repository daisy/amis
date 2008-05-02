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
#ifndef FINDINTEXTDIALOG_H
#define FINDINTEXTDIALOG_H

#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"


namespace amis
{
namespace gui
{
namespace dialogs
{
class FindInTextDialog : public AmisDialogBase
{
public:

	FindInTextDialog(CWnd* pParent = NULL);
	~FindInTextDialog();
	enum { IDD = IDD_FIND };
	CString getUserSearchString();
	int getSearchDirection();
	void forceSearchValue(CString);
	void resolvePromptVariables(Prompt*);

private:
	CString mUserData;
	int mDir;

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedOk();

	afx_msg void OnPaint();
	afx_msg void OnEnChangeSearchstring();
	afx_msg void OnSetfocusSearchString();
	afx_msg void OnSetfocusOk();
	afx_msg void OnSetfocusCancel();
	DECLARE_MESSAGE_MAP()
};
}
}
}
#endif
