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
#ifndef PUBLICATIONSUMMARY_H
#define PUBLICATIONSUMMARY_H

#include "stdafx.h"
#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "AmisCore.h"
#include "Media.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
class PublicationSummaryDialog : public AmisDialogBase
{
//friend class amis::gui::spoken::PublicationSummaryDialogVoicing;
friend class PublicationSummaryDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);
	PublicationSummaryDialog(CWnd* pParent = NULL);
	~PublicationSummaryDialog();
	virtual BOOL PreTranslateMessage(MSG*);
	enum { IDD = IDD_PUBLICATIONSUMMARY };
	//call this first
	void setBook(amis::dtb::Dtb*);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	
	afx_msg void OnLvnItemchangedSummary(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DoDataExchange(CDataExchange*);
	DECLARE_MESSAGE_MAP()

private:
	CString computeTitle();
	CString computeAuthor();
	bool calculateData();
	void displayData();
	void appendToList(CListCtrl*, CString, wstring);

	amis::dtb::Dtb* mpBook;
	amis::MediaGroup* mpTitle;
	wstring mCreator;
	amis::MediaGroup* mpCurrentPage;
	int mTotalNumPages;
	wstring mCurrentTimestamp;
	string mTimeRemaining;
	amis::MediaGroup* mpSectionName;
	int mNumSubsections;
	int mNumPagesForSection;
	int mMaxDepth;
	int mTocs;
	wstring mTotalTime;
	wstring mPublisher;
	wstring mDescription;
	wstring mDate;
	wstring mFormat;
	wstring mNarrator;
	wstring mNavigableItems;
};
}
}
}
#endif
