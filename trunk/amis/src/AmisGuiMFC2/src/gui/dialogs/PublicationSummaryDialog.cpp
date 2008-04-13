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

using namespace amis::gui::dialogs;

BEGIN_MESSAGE_MAP(PublicationSummaryDialog, CDialog)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SUMMARYLIST, OnLvnItemchangedSummary)
END_MESSAGE_MAP()

PublicationSummaryDialog::PublicationSummaryDialog(CWnd* pParent /*=NULL*/)
	: AmisDialogBase(PublicationSummaryDialog::IDD)
{
}

PublicationSummaryDialog::~PublicationSummaryDialog()
{
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
		return FALSE;
	}
	if (calculateData() == true)
		displayData();
	return CDialog::OnInitDialog();
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


			//inexplicably, the default behavior for pressing enter in a dialog is to close it.
			//we're overriding this here.
			if (pMsg->wParam == VK_RETURN)
			{
				//do nothing
				//return CDialog::PreTranslateMessage(pMsg);
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
	
	//TODO: figure out current timestamp

	return true;
}

//this will be taken over by the accessible ui list/prompt data
//for now, it's just temporary to a. see how it looks and b. see if the data is correct
void PublicationSummaryDialog::displayData()
{
	CListCtrl* p_list = (CListCtrl*)GetDlgItem(IDC_SUMMARYLIST);
	p_list->DeleteAllItems();
	//these column headers don't get shown, so it doesn't matter that they're not localized
	p_list->InsertColumn(0, _T("Field"), LVCFMT_LEFT, 100);
	p_list->InsertColumn(1, _T("Value"), LVCFMT_LEFT, 400);

	CString title = _T("Title:");
	CString author = _T("Author:");
	CString publisher = _T("Publisher:");
	CString description = _T("Description:");
	CString narrator = _T("Narrator:");
	CString total_time = _T("Total duration:");
	CString total_pages = _T("Total pages:");
	CString date = _T("Date:");
	CString format = _T("Format:");
	CString features = _T("Features:");
	CString curr_page = _T("Current page:");
	CString curr_time = _T("Current time:");
	CString curr_section = _T("Current section:");
	CString section_contents = _T("Section contents:");
	CString maxdepth = _T("Max section depth:");
	CString tocs = _T("TOC entries:");


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
	wvalue = L"TODO";
	appendToList(p_list, curr_time, wvalue);
	if (mpSectionName != NULL)
	{
		appendToList(p_list, curr_section, mpSectionName->getText()->getTextString());
		value.Format(_T("%d subsections and %d pages"), mNumSubsections, mNumPagesForSection);
		wvalue = value;
		appendToList(p_list, section_contents, wvalue);
	} else {
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

CString PublicationSummaryDialog::computeAuthor() {
	CString value;
	
	if (mCreator.size() > 0) value = mCreator.c_str();
	else value = _T("not available");

	return value;
}
CString PublicationSummaryDialog::computeTitle() {
	CString value;

	if (mpTitle != NULL) value = mpTitle->getText()->getTextString().c_str();
	else value = _T("not available");
	
	return value;
}

void PublicationSummaryDialog::OnLvnItemchangedSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	if (amis::Preferences::Instance()->getIsSelfVoicing() == false) {
		
	*pResult = 0;
		return;
	}
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem != -1) {
		if (pNMLV->uNewState == 3) {
			
			CListCtrl* p_ctrl_list = (CListCtrl*)this->GetDlgItem(IDC_SUMMARYLIST);
			//int i = p_ctrl_list->GetSelectionMark(); // Should = pNMLV->iItem
			int i = pNMLV->iItem;

			Dialog* p_dlg = DataTree::Instance()->findDialog(IDD_PUBLICATIONSUMMARY);

			if (p_dlg != NULL)
			{
				DialogControl* p_ctrl = p_dlg->findControl(IDC_SUMMARYLIST);
				if (p_ctrl != NULL)
				{
					LabelList* p_list = p_ctrl->getLabelList();
					if (p_list != NULL)
					{
						Prompt* p_prompt = p_list->getLabel(i)->getPrompt();
						if (p_prompt != NULL)
						{
							AudioSequence* seq = new AudioSequence();
							AudioSequencePlayer::Instance()->fillSequencePrompt(seq, p_prompt, this);
							if (seq->IsEmpty()) {delete seq;}else{ 
								AudioSequencePlayer::Instance()->Play(seq);
							}
						}
					}
				}
			}	
		}
	}
	*pResult = 0;
}

void PublicationSummaryDialog::resolvePromptVariables(Prompt* pPrompt) {

	PromptVar* p_var = NULL;
	PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");

	for (int i=0; i<pPrompt->getNumberOfItems(); i++)
	{
		if (pPrompt->getItem(i)->getPromptItemType() == PROMPT_VARIABLE)
		{
			p_var = (PromptVar*)pPrompt->getItem(i);

			if (p_var->getName().compare("META_TITLE") == 0)
			{
				amis::AudioNode * node = mpTitle->getAudio(0);

				wstring str;
				str = computeTitle();

				if (node == NULL) {
					if (str.length() != 0) {
						p_var->setContents(str, "");
					}
					else if (promptNotAvailable != NULL)
					{
						p_var->setContents(promptNotAvailable->getContents()->clone());
					}
				} else {
					p_var->setContents(str, node->clone());
				}
			}
			else if (p_var->getName().compare("META_CREATOR") == 0)
			{
				wstring str;
				str = computeAuthor();
				if (str.length() != 0) {
					p_var->setContents(str, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			
			else if (p_var->getName().compare("META_PUBLISHER") == 0)
			{
				if (mPublisher.length() != 0) {
					p_var->setContents(mPublisher, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("META_DESC") == 0)
			{
				if (mDescription.length() != 0) {
					p_var->setContents(mDescription, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("META_NARRATOR") == 0)
			{
				if (mNarrator.length() != 0) {
					p_var->setContents(mNarrator, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("META_TOTALTIME") == 0)
			{
				if (mTotalTime.length() != 0) {
					p_var->setContents(mTotalTime, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("META_NUMPAGES") == 0)
			{
				CString value;
				value.Format(_T("%d"), mTotalNumPages);
				wstring str;
				str = value;
				p_var->setContents(str, "");
			} 
			else if (p_var->getName().compare("META_DATE") == 0)
			{
				if (mDate.length() != 0) {
					p_var->setContents(mDate, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("META_FORMAT") == 0)
			{
				if (mFormat.length() != 0) {
					p_var->setContents(mFormat, "");
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			
			else if (p_var->getName().compare("META_TOCITEMS") == 0)
			{
				amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
				int max = p_nav->getNavMap()->getNumberOfNavPoints();
				CString value;
				value.Format(_T("%d"), max);
				wstring str;
				str = value;
				p_var->setContents(str, "");
				//p_var->setContents(promptNotAvailable->getContents()->clone());
			} 
			else if (p_var->getName().compare("META_DEPTH") == 0)
			{
				amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
				int max = p_nav->getNavMap()->getMaxDepth();
				CString value;
				value.Format(_T("%d"), max);
				wstring str;
				str = value;
				p_var->setContents(str, "");
				//p_var->setContents(promptNotAvailable->getContents()->clone());
			} 
			else if (p_var->getName().compare("TOC_CATEGORIES") == 0)
			{
				if (mNavigableItems.length() != 0) {

					p_var->setContents(mNavigableItems, "");

					/*
					amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
					if (p_nav->getNavMap()->getLabel() != NULL)
					{
						amis::AudioNode * node = p_nav->getNavMap()->getLabel()->getAudio(0);
						p_var->setContents(p_nav->getNavMap()->getLabel()->getText()->getTextString(), node);
					} else {
						p_var->setContents(mNavigableItems, "");
					}
					*/
				}
				else if (promptNotAvailable != NULL)
				{
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}

				/*
				//TODO: get the full multimedia data (should be available directly from the nav data model)
				
	amis::dtb::nav::NavModel* p_nav = mpBook->getNavModel();
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
	} */
			} 
			
			else if (p_var->getName().compare("CURRENT_PAGE") == 0)
			{
				if (mpCurrentPage != NULL) {
					amis::AudioNode * node = mpCurrentPage->getAudio(0);
					p_var->setContents(mpCurrentPage->getText()->getTextString(), (node != NULL ? node->clone() : NULL));
				} else if (promptNotAvailable != NULL) {
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			else if (p_var->getName().compare("CURRENT_TIME") == 0)
			{
					p_var->setContents(promptNotAvailable->getContents()->clone());
			} 

			else if (p_var->getName().compare("CURRENT_SECTION_TITLE") == 0)
			{
				if (mpSectionName != NULL)
				{
					amis::AudioNode * node = mpSectionName->getAudio(0);
					p_var->setContents(mpSectionName->getText()->getTextString(), (node != NULL ? node->clone() : NULL));
				} else if (promptNotAvailable != NULL) {
					p_var->setContents(promptNotAvailable->getContents()->clone());
				}
			} 
			
			else if (p_var->getName().compare("CURRENT_SECTION_NUM_SUBSECTIONS") == 0)
			{
				CString value;
				value.Format(_T("%d"), mNumSubsections);
				wstring str;
				str = value;
				p_var->setContents(str, "");
			} 
			
			else if (p_var->getName().compare("CURRENT_SECTION_NUM_PAGES") == 0)
			{
				CString value;
				value.Format(_T("%d"), mNumPagesForSection);
				wstring str;
				str = value;
				p_var->setContents(str, "");
			} 
	
		}
	}
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
