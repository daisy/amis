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
#include "gui/dialogs/SearchForBooksDialog.h"
#include "../resource.h"
#include "util/SearchForFilesMFC.h"
#include "io/TitleAuthorParse.h"
#include "AmisCore.h"
#include "Preferences.h"
#include "gui/dialogs/PathDialog.h"
#include "gui/MainWndParts.h"
#include "BookList.h"

#include "gui/AmisApp.h"

#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/dialogs/SearchForBooksDialogVoicing.h"

//#define USE_THREAD_FOR_SEARCH

using namespace amis::gui::dialogs;
using namespace amis::gui::spoken;

amis::gui::dialogs::SearchForBooksDialogVoicing * mpSearchForBooksDialogVoicing = NULL;

BEGIN_MESSAGE_MAP(SearchForBooksDialog, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_STARTSEARCH, OnStartsearch)
	ON_BN_CLICKED(IDC_STOPSEARCH, OnStopsearch)
	ON_LBN_DBLCLK(IDC_FILESFOUND, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_OPENBOOK, OnOpenbook)
	ON_LBN_SELCHANGE(IDC_FILESFOUND, OnSelchangeFilelist)
END_MESSAGE_MAP()

void SearchForBooksDialog::OnSelchangeFilelist() 
{
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpSearchForBooksDialogVoicing->OnSelchangeFilelist();
	}
}

void SearchForBooksDialog::resolvePromptVariables(Prompt* pPrompt)
{
	mpSearchForBooksDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}

SearchForBooksDialog::SearchForBooksDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(SearchForBooksDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpSearchForBooksDialogVoicing = new amis::gui::dialogs::SearchForBooksDialogVoicing(this);
	}
	else
	{
		mpSearchForBooksDialogVoicing = NULL;
	}

	mCaptionWhileSearching.LoadStringW(IDS_SEARCHING);
	mCaptionDefault.LoadStringW(AFX_IDS_IDLEMESSAGE);
	mCaptionSearchAborted.LoadStringW(IDS_SEARCH_STOPPED);
	mCaptionSearchCompleteNoFilesFound.LoadStringW(IDS_NO_FILES_FOUND);
	mCaptionSearchCompleteOneFileFound.LoadStringW(IDS_ONE_FILE_FOUND);
	mCaptionSearchCompleteFilesFound.LoadStringW(IDS_FILES_FOUND);
	mFilesFound = 0;

	mbShouldStopSearching = false;
}

SearchForBooksDialog::~SearchForBooksDialog()
{
	if (mpSearchForBooksDialogVoicing != NULL)
	{
		delete mpSearchForBooksDialogVoicing;
		mpSearchForBooksDialogVoicing = NULL;
	}
}

BOOL SearchForBooksDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CButton* p_button = (CButton*)this->GetDlgItem(IDC_STOPSEARCH);
	p_button->EnableWindow(FALSE);

	p_button = (CButton*)this->GetDlgItem(IDC_OPENBOOK);
	p_button->EnableWindow(FALSE);
	this->setFontOnAllControls();
	mpBooks = NULL;

	return TRUE;
}


void SearchForBooksDialog::announceStatus(CString msg, SearchStatus status)
{
	CWnd* p_label_widget = NULL;
	p_label_widget = this->GetDlgItem(IDC_SEARCHING);
	if (p_label_widget)
	{
		p_label_widget->SetWindowText(msg);
		p_label_widget->ShowWindow(SW_SHOW);
	}
	if (amis::Preferences::Instance()->getIsSelfVoicing() == true) 
	{
		mpSearchForBooksDialogVoicing->announceStatus(msg, status);
	}
}

void SearchForBooksDialog::populateListControl()
{
	USES_CONVERSION;
	CListBox* p_filelist = NULL;
	p_filelist = (CListBox*)this->GetDlgItem(IDC_FILESFOUND);

	amis::UrlList* p_search_results = mSearcher.getSearchResults();
	mpBooks = resolveTitles(p_search_results);
	int sz = mpBooks->getNumberOfEntries();
	for (int i=0; i<mpBooks->getNumberOfEntries(); i++)
	{
		CString result;
		result = mpBooks->getEntry(i)->getTitleText().c_str();
		p_filelist->AddString(result.Trim());
	}
	
	if (p_search_results->size() == 0)
	{
		CEdit* p_edit = (CEdit*)this->GetDlgItem(IDC_SEARCHPATH);
		p_edit->SetWindowText(_T(""));
		p_edit->SetFocus();
	}
}
amis::BookList* SearchForBooksDialog::resolveTitles(amis::UrlList* pUrls)
{
	USES_CONVERSION;
	amis::BookList* p_list = new amis::BookList();
	amis::io::TitleAuthorParse parser;
	int sz = pUrls->size();
	for (int i=0; i<pUrls->size(); i++)
	{
		const ambulant::net::url* url = &(*pUrls)[i];
		if (parser.readFromFile(url) == true)
		{
			amis::BookEntry* p_entry = new amis::BookEntry();
			//p_entry now owns the title info data and must delete it
			p_entry->setTitleMedia(parser.getTitleInfo());
			p_entry->mPath = *url;
			p_list->addEntry(p_entry);
		}
		else
		{
			amis::BookEntry* p_entry = new amis::BookEntry();
			//for some reason, this couldn't be read, so just display the URL
			p_entry->setTitleText(A2T(url->get_file().c_str()));
			p_entry->mPath = *url;
			p_list->addEntry(p_entry);
		}
	}
	return p_list;
}

void SearchForBooksDialog::loadBook()
{
	CListBox* p_list = NULL;
	p_list = (CListBox*)this->GetDlgItem(IDC_FILESFOUND);
	int sel = p_list->GetCurSel();
	if (sel > -1 && sel < mpBooks->getNumberOfEntries()) 
		mLoadBookOnDialogClose = mpBooks->getEntry(sel)->mPath;
	//clear the books list
	if (mpBooks != NULL) delete mpBooks;
	this->EndDialog(1);
}

ambulant::net::url SearchForBooksDialog::getBookToLoad()
{
	return mLoadBookOnDialogClose;
}

void SearchForBooksDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}

void SearchForBooksDialog::OnBrowse() 
{
	CString sel_folder_title;
	CString sel_folder_instr;
	CString curr_path;

	sel_folder_title.LoadString(IDS_SELFOLDER_TITLE);
	sel_folder_instr.LoadString(IDS_SELFOLDER_INSTR);

	CPathDialog path_dlg(sel_folder_title, sel_folder_instr, curr_path, NULL);

	bool b = amis::gui::CAmisApp::beforeModalBox();

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		AudioSequencePlayer::playPromptFromStringId("BrowseForFolder_notSelfVoicing");
	}

	if (path_dlg.DoModal() == IDOK)
	{
		curr_path = path_dlg.GetPathName();
		CEdit* p_edit = NULL;
		p_edit = (CEdit*)this->GetDlgItem(IDC_SEARCHPATH);
		p_edit->SetWindowText(curr_path);
	}
	
	theApp.afterModalBox(b);
}

#ifdef USE_THREAD_FOR_SEARCH
//unsigned __stdcall ThreadProc(void* lpParam) {
DWORD __stdcall ThreadProc(LPVOID lpParam) 
{
	SearchForBooksDialog *pThis = (SearchForBooksDialog*)lpParam;
	pThis->SearchLoop();

	TRACE(L"%s", L"\n== ThreadProc END\n");

	//_endthreadex( 0 );
	return 0;
}
#endif //USE_THREAD_FOR_SEARCH

//UINT SearchForBooksDialog::ThreadProc(LPVOID lpParam)
//{
//    SearchForBooksDialog *pThis = reinterpret_cast<SearchForBooksDialog *>(lpParam);
//    pThis->SearchLoop();
//    return 1;
//}

void SearchForBooksDialog::OnStartsearch() 
{
	CButton* p_button = (CButton*)this->GetDlgItem(IDC_OPENBOOK);
	p_button->EnableWindow(FALSE);

	mbShouldStopSearching = false;
	
	//clear the file list
	CListBox* p_filelist = NULL;
	p_filelist = (CListBox*)this->GetDlgItem(IDC_FILESFOUND);
	int sz = p_filelist->GetCount();
	for (int i=sz - 1; i>=0; i--) p_filelist->DeleteString(0);
	
	CEdit* p_edit = NULL;
	p_edit = (CEdit*)this->GetDlgItem(IDC_SEARCHPATH);
	
	p_edit->GetWindowText(search_string);
	search_string.TrimLeft();
	search_string.TrimRight();

	if (search_string.IsEmpty() == TRUE) return;
	
	//change the text on the side to say "searching"
	announceStatus(mCaptionWhileSearching, SEARCHING);

	peekAndPump();

	//gray-out the search button and enable the stop button
	p_button = (CButton*)this->GetDlgItem(IDC_STARTSEARCH);
	p_button->EnableWindow(FALSE);

	p_button = (CButton*)this->GetDlgItem(IDC_STOPSEARCH);
	p_button->EnableWindow(TRUE);
	p_button->SetFocus();

	//prepare the search tool
	mSearcher.clearAll();
	mSearcher.addSearchCriteria("ncc.htm");
	mSearcher.addSearchCriteria(".opf");
	//sometimes I see these temp files on my drive .. excluding them just to be safe
	mSearcher.addSearchExclusionCriteria("_ncc.html");
	
	// SVN folders are interfering
	mSearcher.addSearchExclusionCriteria(".svn-base");

	mSearcher.setRecursiveSearch(true);

#ifdef USE_THREAD_FOR_SEARCH
	//AfxBeginThread(ThreadProc, reinterpret_cast<LPVOID>(this), THREAD_PRIORITY_NORMAL);

	unsigned long lpdwThreadID;
	//hEventHandler = (HANDLE) _beginthreadex(NULL, 0, eventHandler, (void*) this, 0, &lpdwThreadID);
	HANDLE hEventHandler = ::CreateThread(NULL, 0, &ThreadProc, this, 0, &lpdwThreadID);
	//GetCurrentThreadId
	TRACE("\nTHREAD ID (SEARCH BOOKS): %x\n", lpdwThreadID);
	
	amis::util::Log* p_log = amis::util::Log::Instance();
	string log_msg = "Thread ID: ";
	char strID[10];
	sprintf(strID, "%x", lpdwThreadID);			
	log_msg.append(strID);
	p_log->writeTrace(log_msg, "SearchForBooksDialog::OnStartsearch");
#else
	mSearcher.setThreadYielder(this);
#endif //USE_THREAD_FOR_SEARCH

	SearchLoop();
}

//BOOL SearchForBooksDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
//{
//	//if (wParam == BOOK_SEARCH_DONE)
//	//{
//	//
//	//}
//	//else if (wParam = SELF_VOICING_PLAY_NEXT)
//	//{
//	//	int debug = 1;
//	//}
//	
//	return AmisDialogBase::OnCommand(wParam, lParam);
//}



void SearchForBooksDialog::SearchLoop() 
{
	/*for (int i = 0; i < 1000; i++)
	{
		Sleep(10);
		peekAndPump();
	}*/

	USES_CONVERSION;

	// START LONG OP
	mFilesFound = mSearcher.startSearch(T2A(search_string));
	// END LONG OP

#ifdef USE_THREAD_FOR_SEARCH
	this->PostMessageW(WM_COMMAND, (WPARAM)BOOK_SEARCH_DONE, (LPARAM)0);
#else
	SearchDone();
#endif //USE_THREAD_FOR_SEARCH
}

void SearchForBooksDialog::SearchDone() 
{
	populateListControl();
			
	CListBox* p_filelist = NULL;
	p_filelist = (CListBox*)this->GetDlgItem(IDC_FILESFOUND);
	p_filelist->SetFocus();

	peekAndPump();

	if (mFilesFound == 0) announceStatus(this->mCaptionSearchCompleteNoFilesFound, NONE_FOUND);
	else if (mFilesFound == 1) announceStatus(this->mCaptionSearchCompleteOneFileFound, ONE_FOUND);
	else
	{
		CString temp;
		temp.Format(mCaptionSearchCompleteFilesFound, mFilesFound);
		announceStatus(temp, MANY_FOUND);
	}

	//reverse the gray-out button states
	CButton* p_button = (CButton*)this->GetDlgItem(IDC_STARTSEARCH);
	p_button->EnableWindow(TRUE);

	p_button = (CButton*)this->GetDlgItem(IDC_STOPSEARCH);
	p_button->EnableWindow(FALSE);

	p_button = (CButton*)this->GetDlgItem(IDC_OPENBOOK);
	p_button->EnableWindow(TRUE);
}

void SearchForBooksDialog::peekAndPump()
{
	amis::gui::MainWndParts::Instance()->peekAndPump(this->m_hWnd);
}

void SearchForBooksDialog::OnStopsearch() 
{
	mbShouldStopSearching = true;
	mSearcher.stopSearch();
	
	//reverse the gray-out button states
	CButton* p_button = (CButton*)this->GetDlgItem(IDC_STARTSEARCH);
	p_button->EnableWindow(TRUE);

	p_button = (CButton*)this->GetDlgItem(IDC_STOPSEARCH);
	p_button->EnableWindow(FALSE);

	p_button = (CButton*)this->GetDlgItem(IDC_OPENBOOK);
	p_button->EnableWindow(TRUE);

	announceStatus(mCaptionSearchAborted, STOPPED);
}

void SearchForBooksDialog::OnOpenbook() 
{
	loadBook();
}

void SearchForBooksDialog::OnDblclkFilelist() 
{
	loadBook();
}

BOOL SearchForBooksDialog::PreTranslateMessage(MSG* pMsg)
{
	
#ifdef USE_THREAD_FOR_SEARCH
	if (pMsg->wParam == BOOK_SEARCH_DONE)
	{
		SearchDone();
	}
#endif //USE_THREAD_FOR_SEARCH
	
	//else if (pMsg->wParam = SELF_VOICING_PLAY_NEXT)
	//{
	//	int debug = 1;
	//}
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* p_wnd = this->GetFocus();
		
		if (p_wnd)
		{
			int id = p_wnd->GetDlgCtrlID();

			if (id == IDC_FILESFOUND && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN))
			{
				return CDialog::PreTranslateMessage(pMsg);
			}

			//inexplicably, the default behavior for pressing enter in a dialog is to close it.
			//we're overriding this here.
			if (pMsg->wParam == VK_RETURN)
			{
				CEdit* p_edit =	(CEdit*)GetDlgItem(IDC_SEARCHPATH);

				//if the textbox has focus and the user pressed enter, start searching
				if (id == IDC_SEARCHPATH && pMsg->hwnd == p_wnd->m_hWnd && p_edit->m_hWnd == pMsg->hwnd) // making sure that we're not capturing events from the Japanese IME
				 {
					pMsg->wParam = NULL;
					OnStartsearch();
					return CDialog::PreTranslateMessage(pMsg);
				 }
				 //if the files list has focus, open the selected book
				 else if (id == IDC_FILESFOUND)
				 {
					 pMsg->wParam = NULL;
					 loadBook();
					 return CDialog::PreTranslateMessage(pMsg);
				 }
			}	
		}
	}
	return AmisDialogBase::PreTranslateMessageTextField(pMsg, IDC_SEARCHPATH);
}
