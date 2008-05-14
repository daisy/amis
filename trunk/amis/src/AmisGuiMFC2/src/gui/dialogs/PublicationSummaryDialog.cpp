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

#include "stdafx.h"
#include "gui/dialogs/PublicationSummaryDialog.h"
#include "../resource.h"
#include "AmisCore.h"
#include "dtb/Dtb.h"
#include "Media.h"
#include "ambulant/net/url.h"
#include "gui/MmView.h"
#include "gui/MainWndParts.h"
#include "util/FilePathTools.h"
#include "util/Log.h"
#include "gui/self-voicing/datamodel/DataTree.h"
#include "Preferences.h"
#include "gui/self-voicing/dialogs/PublicationSummaryDialogVoicing.h"

using namespace amis::gui::dialogs;

using namespace amis::gui::spoken;

amis::gui::dialogs::PublicationSummaryDialogVoicing * mpPublicationSummaryDialogVoicing = NULL;

BEGIN_MESSAGE_MAP(PublicationSummaryDialog, CDialog)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SUMMARYLIST, OnLvnItemchangedSummary)
END_MESSAGE_MAP()

PublicationSummaryDialog::PublicationSummaryDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(PublicationSummaryDialog::IDD)
{
	if (Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpPublicationSummaryDialogVoicing = new amis::gui::dialogs::PublicationSummaryDialogVoicing(this);
	}
}

PublicationSummaryDialog::~PublicationSummaryDialog()
{
	if (mpPublicationSummaryDialogVoicing != NULL)
	{
		delete mpPublicationSummaryDialogVoicing;
		mpPublicationSummaryDialogVoicing = NULL;
	}
}

void PublicationSummaryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL PublicationSummaryDialog::OnInitDialog() 
{
	if (mpBook == NULL)
	{
		amis::util::Log::Instance()->writeError(
			"Publication summary dialog cannot load because mpBook is NULL", 
			"PublicationSummaryDialog::OnInitDialog", "AmisGuiMFC2");
		return TRUE;
	}
	if (calculateData() == true)
	{
		CDialog::OnInitDialog();
		displayData();
	}

	return TRUE;
}

void PublicationSummaryDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Let the window do its default painting...
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	//call the base class on_paint function
	on_paint();
}


BOOL PublicationSummaryDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		CWnd* p_wnd = this->GetFocus();
		if (p_wnd)
		{
			int id = p_wnd->GetDlgCtrlID();

			if (id = IDC_SUMMARYLIST && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN))
			{
				return CDialog::PreTranslateMessage(pMsg);
			}
		}
	}
	return AmisDialogBase::PreTranslateMessage(pMsg);
}

void PublicationSummaryDialog::setBook(amis::dtb::Dtb* pBook)
{
	//clear all the data (it will get refreshed)
	mCreator.erase();
	mCurrentTimestamp.erase();
	mDate.erase();
	mDescription.erase();
	mFormat.erase();
	mNumPagesForSection = 0;
	mNumSubsections = 0;
	mPublisher.clear();
	mTimeRemaining.erase();
	mTotalNumPages = 0;
	mTotalTime.erase();
	mpCurrentPage = NULL;
	mpBook = NULL;
	mpSectionName = NULL;
	mpTitle = NULL;
	mMaxDepth = 0;
	mTocs = 0;

	mpBook = pBook;
}

bool PublicationSummaryDialog::calculateData()
{
	if (mpBook == NULL) return false;

	amis::dtb::Metadata* p_meta = mpBook->getMetadata();
	amis::dtb::DaisyVersion version = mpBook->getDaisyVersion();
	amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();

	mTocs = p_nav->getNavMap()->getNumberOfNavPoints();
	mMaxDepth = p_nav->getNavMap()->getMaxDepth();

	mpTitle = mpBook->getTitle();
	mCreator = p_meta->getMetadataContent("dc:creator");
	
	if (version == amis::dtb::DAISY_202)
	{
		mTotalTime = p_meta->getMetadataContent("ncc:totalTime");
		mNarrator = p_meta->getMetadataContent("ncc:narrator");
	}
	else if (version == amis::dtb::DAISY_2005)
	{
		mTotalTime = p_meta->getMetadataContent("dtb:totalTime");
		mNarrator = p_meta->getMetadataContent("dc:Narrator");
	}

	//TODO: get the full multimedia data (should be available directly from the nav data model)
	if (p_nav->getNavMap()->getLabel() != NULL)
	{
		mNavigableItems = p_nav->getNavMap()->getLabel()->getText()->getTextString();
		if (p_nav->hasPages())
		{	
			mNavigableItems.append(L", ");
			mNavigableItems.append(p_nav->getPageList()->getLabel()->getText()->getTextString());
		}
	}
	int sz = p_nav->getNumberOfNavLists();
	for (int i=0; i<sz; i++)
	{
		mNavigableItems.append(L", ");
		if (p_nav->getNavList(i)->getLabel() != NULL)
			mNavigableItems.append(p_nav->getNavList(i)->getLabel()->getText()->getTextString());
	}
	mPublisher = p_meta->getMetadataContent("dc:Publisher");
	mDescription = p_meta->getMetadataContent("dc:Description");
	mDate = p_meta->getMetadataContent("dc:Date");
	
	mFormat = p_meta->getMetadataContent("dc:Format");
	//also check for the deprecated "ncc:format"
	if (mFormat.size() == 0 && version == amis::dtb::DAISY_202)
		mFormat = p_meta->getMetadataContent("ncc:format");

	mTotalNumPages = p_nav->getPageList()->getLength();
	//the address for the currently-playing node
	const ambulant::net::url* p_url = MainWndParts::Instance()->mpMmView->getCurrentUrl();
	string id = p_url->get_ref();
	string file = amis::util::FilePathTools::getFileName(p_url->get_url());
	file = file + "#" + id;
	
	amis::dtb::nav::PageTarget* p_page = NULL;
	amis::dtb::nav::NavPoint* p_section = NULL;
	p_page = (amis::dtb::nav::PageTarget*)p_nav->getNodeForSmilId(file, mpBook->getNavModel()->getPageList());
	p_section = (amis::dtb::nav::NavPoint*)p_nav->getNodeForSmilId(file, mpBook->getNavModel()->getNavMap());

	if (p_page != NULL) mpCurrentPage = p_page->getLabel();
	if (p_section != NULL) 
	{
		mpSectionName = p_section->getLabel();
		mNumSubsections = p_section->getNumChildren();
		//the number of pages in a section is calculated using playorder, which doesn't exist in Zed 2002
		if (version == amis::dtb::DAISY_202 || version == amis::dtb::DAISY_2005)
		{
			int start_play_order, end_play_order;
			if (p_section->getFirstSibling() == NULL) end_play_order = -1;
			else end_play_order = p_section->getFirstSibling()->getPlayOrder();
			start_play_order = p_section->getPlayOrder();
			mNumPagesForSection = p_nav->getPageList()->countPagesInRange(start_play_order, end_play_order);
		}
	}
	else
	{
		mNumSubsections = 0;
		mNumPagesForSection = 0;
	}


	return true;
}

void PublicationSummaryDialog::displayData()
{
	CListCtrl* p_list = (CListCtrl*)GetDlgItem(IDC_SUMMARYLIST);
	p_list->DeleteAllItems();
	//these column headers don't get shown, so it doesn't matter that they're not localized
	p_list->InsertColumn(0, _T("Field"), LVCFMT_LEFT, 100);
	p_list->InsertColumn(1, _T("Value"), LVCFMT_LEFT, 400);

	CString title;
	title.LoadStringW(IDS_META_TITLE);
	CString author;
	author.LoadStringW(IDS_META_AUTHOR);
	CString publisher;
	publisher.LoadStringW(IDS_META_PUBLISHER);
	CString description;
	description.LoadStringW(IDS_META_DESCRIPTION);
	CString narrator;
	narrator.LoadStringW(IDS_META_NARRATOR);
	CString total_time;
	total_time.LoadStringW(IDS_META_TOTAL_DURATION);
	CString total_pages;
	total_pages.LoadStringW(IDS_META_TOTAL_PAGES);
	CString date;
	date.LoadStringW(IDS_META_DATE);
	CString format;
	format.LoadStringW(IDS_META_FORMAT);
	CString features;
	features.LoadStringW(IDS_META_FEATURES);
	CString curr_page;
	curr_page.LoadStringW(IDS_META_CURRENT_PAGE);
	CString curr_section;
	curr_section.LoadStringW(IDS_META_CURRENT_SECTION);
	CString section_contents;
	section_contents.LoadStringW(IDS_META_SECTION_CONTENTS);
	CString maxdepth;
	maxdepth.LoadStringW(IDS_META_MAX_SECTION_DEPTH);
	CString tocs;
	tocs.LoadStringW(IDS_META_TOC_ENTRIES);
	CString section_contents_list;
	section_contents_list.LoadStringW(IDS_META_SECTION_CONTENTS_LIST);

	CString value;
	wstring wvalue;	
	int idx = 0;

	wvalue = computeTitle();
	appendToList(p_list, title, wvalue);

	wvalue = computeAuthor();
	appendToList(p_list, author, wvalue);

	appendToList(p_list, publisher, mPublisher);
	appendToList(p_list, description, mDescription);
	appendToList(p_list, narrator, mNarrator);
	appendToList(p_list, total_time, mTotalTime);
	value.Format(_T("%d"), mTotalNumPages);
	wvalue = value;
	appendToList(p_list, total_pages, wvalue);
	appendToList(p_list, date, mDate);
	appendToList(p_list, format, mFormat);
	appendToList(p_list, features, mNavigableItems);
	if (mpCurrentPage != NULL)
		appendToList(p_list, curr_page, mpCurrentPage->getText()->getTextString());
	else 
		appendToList(p_list, curr_page, L"");
	if (mpSectionName != NULL)
	{
		appendToList(p_list, curr_section, mpSectionName->getText()->getTextString());
		value.Format(section_contents_list, mNumSubsections, mNumPagesForSection);
		wvalue = value;
		appendToList(p_list, section_contents, wvalue);
	} 
	else 
	{
		appendToList(p_list, curr_section, L"");
		appendToList(p_list, section_contents, L"");
	}
	value.Format(_T("%d"), mTocs);
	wvalue = value;
	appendToList(p_list, tocs, wvalue);
	value.Format(_T("%d"), mMaxDepth);
	wvalue = value;
	appendToList(p_list, maxdepth, wvalue);
}

CString PublicationSummaryDialog::computeAuthor() 
{
	CString value;
	
	if (mCreator.size() > 0) value = mCreator.c_str();
	else value.LoadStringW(IDS_NOT_AVAILABLE);

	return value;
}
CString PublicationSummaryDialog::computeTitle() 
{
	CString value;

	if (mpTitle != NULL) value = mpTitle->getText()->getTextString().c_str();
	else value.LoadStringW(IDS_NOT_AVAILABLE);
	
	return value;
}

void PublicationSummaryDialog::OnLvnItemchangedSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (amis::Preferences::Instance()->getIsSelfVoicing() == true)
	{
		mpPublicationSummaryDialogVoicing->OnLvnItemchangedSummary(pNMHDR, pResult);
	}
}

void PublicationSummaryDialog::resolvePromptVariables(Prompt* pPrompt)
{
	mpPublicationSummaryDialogVoicing->resolvePromptVariables(pPrompt);
	AmisDialogBase::resolvePromptVariables(pPrompt);
}

void PublicationSummaryDialog::appendToList(CListCtrl* pList, CString field, wstring value)
{
	//don't add empty items (no anymore: to match self-voicing exactly, and to support interactive navigation in the list)
	//if (value.size() == 0) return;
	int idx = pList->GetItemCount();
	pList->InsertItem(idx, _T(""));
	pList->SetItemText(idx, 0, field);
	pList->SetItemText(idx, 1, value.c_str());
}
