/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef OPENURLDIALOG_H
#define OPENURLDIALOG_H

#include <string>
#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
class OpenUrlDialog : public AmisDialogBase
{
	//DECLARE_DYNAMIC(OpenUrlDialog)

public:
	void resolvePromptVariables(Prompt*);

	OpenUrlDialog(CWnd* pParent = NULL);
	virtual ~OpenUrlDialog();
	virtual BOOL OnInitDialog();
	CString getUrl();
	enum { IDD = IDD_OPENURL };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	CString mUrl;
	
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
}
}
}
#endif