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

#ifndef SKIPDIALOG_H
#define SKIPDIALOG_H


#include "dtb/CustomTest.h"
#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"


namespace amis
{
namespace gui
{
namespace dialogs
{
class SkipDialog : public AmisDialogBase
{
//friend class amis::gui::spoken::SkipDialogVoicing;
friend class SkipDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);

	SkipDialog(CWnd* pParent = NULL);
	~SkipDialog();
	void initializeData(amis::dtb::CustomTestSet*);
	//Dialog Data
	enum { IDD = IDD_READING_OPTIONS };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void fillListBox();
	void saveChanges();
	amis::dtb::CustomTestSet* mpCustomTests;
	
	CButton m_Ok;
	
	void setDescriptiveText(bool);

protected:
	CCheckListBox	m_ListSkips;
public:
	afx_msg void OnSelchangeSkips();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
}
}
}
#endif