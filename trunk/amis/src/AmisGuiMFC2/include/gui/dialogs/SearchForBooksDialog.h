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
#ifndef SEARCHFORBOOKSDIALOG_H
#define SEARCHFORBOOKSDIALOG_H

#include "util/SearchForFilesMFC.h"
#include "gui/dialogs/AmisDialogBase.h"
#include "../resource.h"
#include "ambulant/net/url.h"

#include "util/ThreadYielder.h"

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
class SearchForBooksDialog : public AmisDialogBase, public ThreadYielder
{
//friend class amis::gui::spoken::SearchForBooksDialogVoicing;
friend class SearchForBooksDialogVoicing;

public:
	void resolvePromptVariables(Prompt*);
	void peekAndPump();
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
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CString search_string;
	//static UINT ThreadProc(LPVOID lpParam);
	//DWORD __stdcall ThreadProc(LPVOID lpParam);
		void SearchDone();
public:
		void SearchLoop();

private:
	int mFilesFound;
	void announceStatus(CString, SearchStatus); 
	void populateListControl();
	void loadBook();
	amis::BookList* resolveTitles(amis::UrlList*);
	bool mbShouldStopSearching;
	amis::util::SearchForFilesMFC mSearcher;
	ambulant::net::url mLoadBookOnDialogClose;
	//the list of books is here, with the title text + audio
	amis::BookList* mpBooks;

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
