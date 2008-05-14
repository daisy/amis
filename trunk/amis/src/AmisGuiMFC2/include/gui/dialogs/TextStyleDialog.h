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
#ifndef TEXTSTYLEDIALOG_H
#define TEXTSTYLEDIALOG_H

#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "util/Color.h"

BOOL CALLBACK EnumFontCallback (LPLOGFONT, LPTEXTMETRIC, DWORD, LPARAM);

namespace amis
{
namespace gui
{
namespace dialogs
{
class TextStyleDialog : public AmisDialogBase
{
//friend class amis::gui::spoken::TextStyleDialogVoicing;
friend class TextStyleDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);
	virtual void DoDataExchange(CDataExchange*);

	TextStyleDialog(CWnd* pParent = NULL);
	~TextStyleDialog();
	enum { IDD = IDD_TEXTSTYLE };
protected:

	void initializeCombos();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeFont();
	afx_msg void OnCbnSetfocusFont();
	afx_msg void OnCbnSelchangeHighlightForeground();
	afx_msg void OnCbnSetfocusHighlightForeground();
	afx_msg void OnCbnSelchangeHighlightBackground();
	afx_msg void OnCbnSetfocusHighlightBackground();
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
};
}
}
}
#endif