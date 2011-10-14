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
#ifndef AMISDIALOGBASE_H
#define AMISDIALOGBASE_H

#include <string>

#include "gui/self-voicing/PreTranslateMessageHandler.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
class AmisDialogBase : public CDialog, public PromptResolver
{
public:
	void resolvePromptVariables(Prompt*);
	AmisDialogBase(int);
	~AmisDialogBase();
	void on_paint();
	INT_PTR do_modal();

	void triggerVirtualKeyStroke(CWnd* cwnd);
	BOOL PreTranslateMessageTextField(MSG* pMsg, UINT id);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void setFontOnAllControls();
protected:
	bool mbFlag_FirstDraw;
	static BOOL CALLBACK setFontOnControl(HWND, LPARAM);
	//DECLARE_DYNCREATE(AmisDialogBase)
	DECLARE_DYNAMIC(AmisDialogBase)

	//{{AFX_MSG(AmisDialogBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:	
	int mId;
	PreTranslateMessageHandler* mCommonPreTranslateMessageHandler;
private:
	CFont mFont;
};
}
}
}
#endif
