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
#ifndef SEARCHFORBOOKSDIALOG_H
#define SEARCHFORBOOKSDIALOG_H

#include "util/SearchForFilesMFC.h"
#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "ambulant/net/url.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
enum SearchStatus 
{
	SEARCHING, READY, STOPPED, NONE_FOUND, ONE_FOUND, MANY_FOUND
};
class SearchForBooksDialog : public AmisDialogBase
{
//friend class amis::gui::spoken::SearchForBooksDialogVoicing;
friend class SearchForBooksDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);

	SearchForBooksDialog(CWnd* pParent = NULL);
	~SearchForBooksDialog();
	ambulant::net::url getBookToLoad();
	enum { IDD = IDD_SEARCHDAISY };
protected:
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnDblclkFilelist();
	afx_msg void OnOpenbook();
	afx_msg void OnPaint();
	afx_msg void OnBrowse();
	afx_msg void OnStartsearch();
	afx_msg void OnStopsearch();
	afx_msg void OnSelchangeFilelist();
	DECLARE_MESSAGE_MAP()

private:
	int mFilesFound;
	void announceStatus(CString, SearchStatus); 
	void populateListControl();
	void loadBook();

	bool mbShouldStopSearching;
	amis::util::SearchForFilesMFC mSearcher;
	ambulant::net::url mLoadBookOnDialogClose;

	CString mCaptionWhileSearching;
	CString mCaptionDefault;
	CString mCaptionSearchAborted;
	CString mCaptionSearchCompleteNoFilesFound;
	CString mCaptionSearchCompleteOneFileFound;
	CString mCaptionSearchCompleteFilesFound;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
}
}
}
#endif
