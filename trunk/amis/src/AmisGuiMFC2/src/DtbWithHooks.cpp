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
#include "DtbWithHooks.h"
#include "gui/MainWndParts.h"
#include "util/FilePathTools.h"
#include "gui/sidebar/AmisSidebarLoader.h"
#include "BookList.h"
#include "io/BookListFileIO.h"
#include "gui/AmisApp.h"
#include "gui/MenuManip.h"
#include "gui/TextRenderBrain.h"
#include "ambulant/net/url.h"
#include "ambulant/smil2/test_attrs.h"
#include "util/Log.h"
#include "gui/self-voicing/audiosequenceplayer.h"
#include "Preferences.h"
#include "gui/self-voicing/TTSPlayer.h"

using namespace amis::dtb;

DtbWithHooks* DtbWithHooks::pinstance = 0;

DtbWithHooks* DtbWithHooks::Instance()
{
	if (pinstance == 0) pinstance = new DtbWithHooks(); 
    return pinstance;
}

//this is sort of a hack but sort of makes sense
//this function tells if we're using DtbWithHooks or not
bool DtbWithHooks::isNull()
{
	if (pinstance == NULL) return true;
	else return false;
}
void DtbWithHooks::DestroyInstance()
{
	delete pinstance;
	pinstance = NULL;
}

DtbWithHooks::DtbWithHooks():Dtb(theApp.getAppPath())
{
	mpFileSearcherTmp = NULL;
	mpFileSearcherTmp = new amis::util::SearchForFilesMFC();
	setFileSearcher(mpFileSearcherTmp);
	mbIsWaitingForLastmarkNode = false;
	//setTTSNextPhraseFlag(false);
	setCacheIndex(Preferences::Instance()->getCacheIndex());
}

DtbWithHooks::~DtbWithHooks()
{
	if (mpFileSearcherTmp != NULL) delete mpFileSearcherTmp;
}

bool DtbWithHooks::open(const ambulant::net::url* filename, const ambulant::net::url* bookmarksPath, bool saveInHistory)
{
	std::wstring str = amis::gui::CAmisApp::emitMessage("loading", true);
	
	CString strx(str.c_str());
	amis::gui::MainWndParts::Instance()->updateTitleBar(amis::gui::MainWndParts::TITLEBAR_BOOKTITLE, strx);

	setThreadYielder(amis::gui::MainWndParts::Instance());
	
	getFileSearcher()->clearSearchResults();
	mbIsWaitingForLastmarkNode = false;
	BookList* p_history = NULL;
	if (saveInHistory == true)
		p_history = theApp.getHistory();

	//call the base class	
	if (!Dtb::open(filename, bookmarksPath, p_history))
	{
		amis::gui::MainWndParts::Instance()->updateTitleBar(amis::gui::MainWndParts::TITLEBAR_BOOKTITLE, L"");
		return false;
	}

	makeAllLabelsHumanReadable();
	
	
	if (this->getTitle() != NULL)
	{
		amis::TextNode* p_txt = NULL;
		p_txt = this->getTitle()->getText();
		if (p_txt != NULL)
		{
			CString title = p_txt->getTextString().c_str();
			title.Trim();
			amis::gui::MainWndParts::Instance()->updateTitleBar(amis::gui::MainWndParts::TITLEBAR_BOOKTITLE, title);
		}
	}

	if (saveInHistory == true) addToHistory();
	amis::dtb::BookmarkSet* p_bmks = NULL;
	p_bmks = this->getBookmarks();

	amis::gui::MenuManip::Instance()->setupNavigationOptions();
	amis::gui::MenuManip::Instance()->addNavContainersToViewMenu();
	amis::gui::MenuManip::Instance()->loadBookmarks(p_bmks);

	// The DONE message is likely to be "skipped" because
	// the loadNavigationData() happens right after and
	// generates the prompt "Sidebar has focus" (which interrupts the previous one).
	// After testing with a few books, this behaviour seems fine.
	// We should leave the DONE prompt here anyway, just in case the self-voicing logic
	// associated to loadNavigationData() changes in the future.
	amis::gui::CAmisApp::emitMessage("done");

	amis::gui::sidebar::AmisSidebarLoader::Instance()->loadNavigationData
		(this->getNavModel(), &amis::gui::MainWndParts::Instance()->mpSidebar->m_wndDlg);

	return true;
}

//for skippable structures
//set playAll = true if all options should be forced to be turned on
void DtbWithHooks::updateCustomTestStates(bool playAll)
{
	amis::dtb::CustomTestSet* p_tests = this->getCustomTestSet();
	if (p_tests == NULL) return;

	//turn on all options in ambulant (they are off by default)
	for (int i = 0; i<p_tests->getLength(); i++)
	{
		std::string test_id = p_tests->getCustomTest(i)->getId();
		//append "-on" if this is a Daisy 202 book
		if (this->getDaisyVersion() == DAISY_202) test_id.append("-on");
		bool state = p_tests->getCustomTest(i)->getCurrentState() | playAll;
		if (this->getDaisyVersion() == amis::dtb::DAISY_202)
			ambulant::smil2::test_attrs::set_current_system_component_value(test_id, state);
		else
			ambulant::smil2::test_attrs::set_current_custom_test_value(test_id, state);
		p_tests->getCustomTest(i)->setCurrentState(state);
	}
}

smil::SmilMediaGroup* DtbWithHooks::startReading(bool loadLastmark)
{
	USES_CONVERSION;
#ifdef WITH_EXTERNAL_SMIL_PLAYER
	const ambulant::net::url* smilfile = NULL;
	amis::dtb::PositionData* p_lastmark = NULL;
	if (getBookmarks()) p_lastmark = getBookmarks()->getLastmark();

	//if we're not using the lastmark or it is unavailable, just load from the beginning
	if (loadLastmark == false || p_lastmark == NULL)
	{
		smilfile = getSpine()->getFirstFile();
		mIdOfLastmarkNode = "";
		setIsWaitingForLastmarkNode(false);
	}
	else
	{
		smilfile = &getBookmarks()->getLastmark()->mUri;
		//set a flag to stop highlighting of all text nodes until we hit the one belonging to the smil node
		//with ref = smilfile->get_ref()
		mIdOfLastmarkNode = smilfile->get_ref();
		setIsWaitingForLastmarkNode(true);
	}

	loadSmilFromUrl(smilfile);
	
	return NULL;
#else
	smil::SmilMediaGroup* pData = Dtb::startReading(loadLastmark);
	
	if (amis::gui::MainWndParts::Instance()->mpMmDoc != NULL)
	{
		ambulant::net::url current_uri = calculateUriOfData(pData);
		amis::gui::MainWndParts::Instance()->mpMmDoc->OnOpenDocument(A2T(current_uri.get_url().c_str()));
	}
	return pData;
#endif
}
smil::SmilMediaGroup* DtbWithHooks::nextPhrase()
{
	//this should reduce some stuttering
	if (this->hasAudio() == false) stopTTS();
	amis::gui::MainWndParts::Instance()->mpMmView->nextPhrase();
	return NULL;
}

smil::SmilMediaGroup* DtbWithHooks::previousPhrase()
{
	//this should reduce some stuttering
	if (this->hasAudio() == false) stopTTS();
	amis::gui::MainWndParts::Instance()->mpMmView->prevPhrase();
	return NULL;
}

void DtbWithHooks::nextSmilDocument()
{
	if (theApp.isBookOpen() == false) return;

	const ambulant::net::url* filepath = this->getSpine()->getNextFile();
	if (filepath) 
	{
		string str_filepath = amis::util::ambulantUrlToString(filepath);
		amis::gui::MainWndParts::Instance()->mpMmView->ScheduleReplaceDoc(str_filepath);
	}
}

void DtbWithHooks::previousSmilDocument()
{
	if (theApp.isBookOpen() == false) return;

	const ambulant::net::url* filepath = this->getSpine()->getPreviousFile();
	
	if (filepath)
	{
		string str_filepath = amis::util::ambulantUrlToString(filepath);
		amis::gui::MainWndParts::Instance()->mpMmView->ScheduleReplaceDoc(str_filepath);
	}
}

void DtbWithHooks::loadNavNode(nav::NavNode* pNav)
{
	if (pNav == NULL) 
	{
		amis::util::Log::Instance()->writeWarning("Tried to load NULL nav node", "DtbWithHooks::loadNavNode");
		return;
	}
	amis::util::Log::Instance()->writeMessage("Loading nav node", "DtbWithHooks::loadNavNode");
	ambulant::net::url rel_path;
	if (this->getFileSet()->getBookDirectory()->is_local_file())
		rel_path = ambulant::net::url::from_filename(pNav->getContent(), true);
	else
		rel_path = ambulant::net::url::from_url(pNav->getContent());
	loadSmilFromUrl(&rel_path);
}

void DtbWithHooks::addToHistory()
{
	amis::BookList* p_history = NULL;
	p_history = theApp.getHistory();

	if (p_history != NULL)
	{
		amis::BookEntry* p_entry = new amis::BookEntry();
		p_entry->mbIsLastRead = true;
		p_entry->mBmkPath = *this->getFileSet()->getBookmarksFilepath();
		p_entry->mUid.assign(this->getUid());
		if (getDaisyVersion() == DAISY_202)
			p_entry->mPath = *this->getFileSet()->getNavFilepath();
		else if (getDaisyVersion() == DAISY_2005)
			p_entry->mPath = *this->getFileSet()->getOpfFilepath();

		if (this->getTitle()->hasText() == true)
			p_entry->setTitleText(this->getTitle()->getText()->getTextString());
		if (this->getTitle()->hasAudio() == true)
		{
			amis::AudioNode* p_audio = this->getTitle()->getAudio(0);
			p_entry->setTitleAudio(p_audio->getPath(), p_audio->getClipBegin(), p_audio->getClipEnd());
		}
		p_history->addEntry(p_entry);

		amis::io::BookListFileIO io;
		io.writeToFile(p_history->getFilepath(), p_history);

		amis::gui::MenuManip::Instance()->refreshRecentBooksListMenu();
	}
}


void DtbWithHooks::makeAllLabelsHumanReadable()
{
	USES_CONVERSION;

	//update the navigation containers
	nav::NavMap* p_map = getNavModel()->getNavMap();
	//make a dummy label if there's nothing there
	//it will get replaced in the next step
	if (p_map->getLabel() == NULL)
	{
		amis::MediaGroup* p_media = new amis::MediaGroup();
		amis::TextNode* p_text = new amis::TextNode();
		wstring txt = L"sections";
		p_text->setTextString(txt);
		p_media->setText(p_text);
		p_map->setLabel(p_media);
	}
	makeLabelHumanReadable(getNavModel()->getNavMap()->getLabel(), 
		getNavModel()->getNavMap()->getId());

	if (getNavModel()->hasPages()) 
	{
		nav::PageList* p_pages = NULL;
		p_pages = getNavModel()->getPageList();
		//make a dummy label if there's nothing there
		//it will get replaced in the next step
		if (p_pages->getLabel() == NULL)
		{
			amis::MediaGroup* p_media = new amis::MediaGroup();
			amis::TextNode* p_text = new amis::TextNode();
			wstring txt = L"pages";
			p_text->setTextString(txt);
			p_media->setText(p_text);
			p_pages->setLabel(p_media);
		}
		makeLabelHumanReadable(getNavModel()->getPageList()->getLabel(),
			getNavModel()->getPageList()->getId());
	}

	nav::NavListList* p_lists = NULL;
	p_lists = getNavModel()->getNavLists();
	if (p_lists != NULL)
	{
		for (int i = 0; i<p_lists->size(); i++)
		{
			//make a dummy label if there's nothing there
			//it will get replaced in the next step
			if (((*p_lists)[i])->getLabel() == NULL)
			{
				amis::MediaGroup* p_media = new amis::MediaGroup();
				amis::TextNode* p_text = new amis::TextNode();
				wstring txt = L"navlist";
				p_text->setTextString(txt);
				p_media->setText(p_text);
				((*p_lists)[i])->setLabel(p_media);
			}
			nav::NavList* p_list = ((nav::NavListList)(*p_lists))[i];
			makeLabelHumanReadable(p_list->getLabel(), p_list->getId());
		}
	}
	
	amis::dtb::CustomTestSet* p_custom_tests = getCustomTestSet();
	if (p_custom_tests != NULL)
	{
		for (int i=0; i<p_custom_tests->getLength(); i++)
		{
			amis::dtb::CustomTest* p_custom_test = p_custom_tests->getCustomTest(i);
			amis::MediaGroup* p_label = new amis::MediaGroup();
			amis::TextNode* p_text = new amis::TextNode();
			p_text->setTextString(A2T(p_custom_test->getId().c_str()));
			p_label->setText(p_text);
			p_custom_test->setLabel(p_label);
			makeLabelHumanReadable(p_label, p_custom_test->getId());
		}
	}
}
//used to make NCC labels consistent
void DtbWithHooks::makeLabelHumanReadable(amis::MediaGroup* pLabel, std::string id)
{
	std::string amis_xml_item_id;
	std::wstring item_name = pLabel->getText()->getTextString().c_str();

	//replace NCC identifiers with more human-readable names
	if (id.compare("sidebar") == 0 ||
		id.compare("sidebars") == 0)
	{
		amis_xml_item_id = "sidebars";
		CString temp;
		temp.LoadStringW(IDS_SIDEBARS);
		item_name = temp;
	}
	else if (id.compare("optional-prodnote") == 0 ||
		id.compare("optional-prodnotes") == 0 ||
		id.compare("prodnote") == 0 ||
		id.compare("prodnotes") == 0)
	{
		amis_xml_item_id = "prodnotes";
		CString temp;
		temp.LoadStringW(IDS_PRODUCER_NOTES);
		item_name = temp;
	}
	else if (id.compare("noteref") == 0 ||
		id.compare("noterefs") == 0 ||
		id.compare("footnote") == 0 ||
		id.compare("footnotes") == 0)
	{
		amis_xml_item_id = "noterefs";
		CString temp;
		temp.LoadStringW(IDS_FOOTNOTES);
		item_name = temp;
	}
	else if (id.compare("group") == 0 ||
		id.compare("groups") == 0)
	{
		amis_xml_item_id = "groups";
		CString temp;
		temp.LoadStringW(IDS_GROUPED_ITEMS);
		item_name = temp;
	}
	else if (id.compare("page") == 0 ||
		id.compare("pages") == 0 ||
		id.compare("pagenumber") == 0 ||
		id.compare("pagenumbers") == 0)
	{
		amis_xml_item_id = "pages";
		CString temp;
		temp.LoadStringW(IDS_PAGES);
		item_name = temp;
	}
	else if (id.compare("section") == 0 ||
		id.compare("sections") == 0)
	{
		amis_xml_item_id = "sections";
		CString temp;
		temp.LoadStringW(IDS_SECTIONS);
		item_name = temp;
	}
	
	pLabel->getText()->setTextString(item_name);
}

amis::dtb::Bookmark* DtbWithHooks::addBookmark()
{
	USES_CONVERSION;

	amis::MediaGroup* p_note = new amis::MediaGroup();
	amis::TextNode* p_note_text = new amis::TextNode();
	
	wstring curr_text = amis::gui::TextRenderBrain::Instance()->getCurrentText();
	if (curr_text.size() == 0)
	{
		//the word "bookmark 1...n" can be the name for a bookmark with no text
		CString bmk_word;
		bmk_word.LoadStringW(IDS_BOOKMARK);
		curr_text = bmk_word;
		int count = getBookmarks()->getNumberOfItems() + 1;
		curr_text.append(_T(" "));
		wchar_t ch[3];
		_itow(count, ch, 10);
		curr_text.append(ch);
	}
	else
	{
		//use the first 20 chars of the current text for the bookmark's note field
		if (curr_text.size() > 20) curr_text = curr_text.substr(0, 20);
	}
	
	//temporarily convert to CString because the Replace function is better
	CString cstr_curr_text = curr_text.c_str();
	cstr_curr_text.Replace(_T("<br/>"), _T(""));
	curr_text = cstr_curr_text;

	p_note_text->setTextString(curr_text);
	p_note->setText(p_note_text);

	//get the audio from ambulant's most recent audio node
	const ambulant::lib::node* amb_node = MainWndParts::Instance()->mpMmView->m_recent_audio_node;
	if (amb_node != NULL)
	{
		amis::AudioNode* p_audio = new amis::AudioNode();
		std::string src = amb_node->get_attribute("src");
		ambulant::net::url url_src = ambulant::net::url::from_filename(src);
		url_src = url_src.join_to_base(*this->getFileSet()->getSmilFile());

		p_audio->setPath(url_src.get_file());

		const char* clip_begin = amb_node->get_attribute("clipBegin");
		const char* clip_end = amb_node->get_attribute("clipEnd");

		if (!clip_begin) clip_begin = amb_node->get_attribute("clip-begin");
		if (!clip_end) clip_end = amb_node->get_attribute("clip-end");
	
		if (clip_begin) p_audio->setClipBegin(clip_begin);
		if (clip_begin) p_audio->setClipEnd(clip_end);

		//add the audio clip of the current position to the bookmark's note's audio field
		p_note->addAudioClip(p_audio);
	}
	return amis::dtb::Dtb::addBookmark(p_note);
}

void DtbWithHooks::loadBookmark(int index)
{
	amis::dtb::BookmarkSet* p_bmks = NULL;
	amis::dtb::PositionMark* p_mark = NULL;
	
	p_bmks = getBookmarks();
	if (!p_bmks) return;

	p_mark = p_bmks->getItem(index);
	if (!p_mark) return;
	amis::util::Log::Instance()->writeMessage("Loading bookmark", "DtbWitHooks::loadBookmark");
	loadSmilFromUrl(&p_mark->mpStart->mUri);
}

amis::dtb::smil::SmilMediaGroup* DtbWithHooks::loadSmilFromUrl(const ambulant::net::url* pUri)
{
	USES_CONVERSION;
	assert(pUri);
	ambulant::net::url full_path = pUri->join_to_base(*getFileSet()->getBookDirectory());
	
	//set the spine at this file.  this also makes sure that we're going to a file that is in the book.
	if (getSpine()->goToFile(&full_path))
	{
		LPCTSTR str_ = A2T(full_path.get_url().c_str());
		string log_msg = "Loading SMIL from URL: " + full_path.get_url();
		amis::util::Log::Instance()->writeMessage("Loading SMIL from URL", &full_path, 
			"DtbWithHooks::loadSmilFromUrl");

		if (this->hasAudio())
		{
			MmView *view = MainWndParts::Instance()->mpMmView;
			if (view != NULL) view->OnFilePause();
		}
//#ifndef AVOID_SELF_VOICING_COM_STUFF
		else
		{
			if (!Preferences::Instance()->getSafeMode())
				stopTTS();	
		}
//#endif

		if(amis::gui::MainWndParts::Instance()->mpMmView->getCurrentUrl()->same_document(full_path))
		{
			std::string *strcopy = new std::string(full_path.get_ref());
			amis::gui::MainWndParts::Instance()->mpMmView->PostMessage(WM_MY_GOTO_ID, 0, (LPARAM)strcopy);
		}
		else
		{
			amis::gui::MainWndParts::Instance()->mpMmDoc->OnOpenDocument(str_);
		}
	}
	return NULL;
}

void DtbWithHooks::previousSection()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	nav::NavNode* p_node = NULL;
	p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();

		int sidebar_depth = MainWndParts::Instance()->mpSidebar->m_wndDlg.getExposedDepth();
		if (sidebar_depth != getNavModel()->getNavMap()->getMaxDepth())
			p_node = getNavModel()->previousSection(p_node->getPlayOrder(), sidebar_depth);
		else
			p_node = getNavModel()->previousSection(p_node->getPlayOrder());
		
		if (p_node != NULL) 
		{
			loadNavNode(p_node);
		}
		else 
		{
			std::string msg = "Previous section is NULL.  Reloading current. Current nav ID# = " + id;
			p_log->writeMessage(msg, "DtbWithHooks::previousSection");
			loadNavNode(getCurrentNavNode());
		}
	}
	else
	{
		p_log->writeWarning("current nav node is NULL", "DtbWithHooks::previousSection");
	}
}
void DtbWithHooks::nextSection()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	nav::NavNode* p_node = NULL;
	p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		int sidebar_depth = MainWndParts::Instance()->mpSidebar->m_wndDlg.getExposedDepth();
		if (sidebar_depth != getNavModel()->getNavMap()->getMaxDepth())
			p_node = getNavModel()->nextSection(p_node->getPlayOrder(), sidebar_depth);
		else
			p_node = getNavModel()->nextSection(p_node->getPlayOrder());
		
		if (p_node != NULL) 
		{
			loadNavNode(p_node);
		}
		else 
		{
			std::string msg = "Next section is NULL.  Current nav ID# = " + id;
			p_log->writeMessage(msg, "DtbWithHooks::nextSection");
		}
	}
	else
	{
		p_log->writeWarning("current nav node is NULL", "DtbWithHooks::nextSection");
	}
}
void DtbWithHooks::previousPage()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	nav::NavNode* p_node = NULL;
	p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->previousPage(p_node->getPlayOrder());
		if (p_node != NULL) 
		{
			loadNavNode(p_node);
		}
		else 
		{
			std::string msg = "Previous page is NULL.  Current nav ID# = " + id;
			p_log->writeMessage(msg, "DtbWithHooks::previousPage");
		}
	}
	else
	{
		p_log->writeWarning("current nav node is NULL", "DtbWithHooks::previousPage");
	}
}
void DtbWithHooks::nextPage()
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	nav::NavNode* p_node = NULL;
	p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->nextPage(p_node->getPlayOrder());
		if (p_node != NULL)
		{
			loadNavNode(p_node);
		}
		else 
		{
			std::string msg = "Next page is NULL.  Current nav ID# = " + id;
			p_log->writeMessage(msg, "DtbWithHooks::nextPage");
		}
	}
	else
	{
		//go to the first page
		if (getNavModel()->hasPages() == true)
		{
			p_node = getNavModel()->getPageList()->getAtIndex(0);
			if (p_node != NULL)
				loadNavNode(p_node);
		}
		else
		{
			p_log->writeWarning("current nav node is NULL and no page found", "DtbWithHooks::nextPage");
		}
	}
}
//always true until the nextPhrase + document_stopped event can be untangled in MmView
//to find out if we really have a next phrase or not
bool DtbWithHooks::canGoToNextPhrase()
{
	return true;	
}
//always true until the previousPhrase + document_stopped event can be untangled in MmView
//to find out if we really have a previous phrase or not
bool DtbWithHooks::canGoToPreviousPhrase()
{
	return true;
}
bool DtbWithHooks::canGoToNextSection()
{
	amis::dtb::nav::NavNode* p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->nextSection(p_node->getPlayOrder());
		if (p_node != NULL) return true;
		else return false;
	}
	return false;
}
bool DtbWithHooks::canGoToPreviousSection()
{
	return true;
	/*
	amis::dtb::nav::NavNode* p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->previousSection(p_node->getPlayOrder());
		if (p_node != NULL) return true;
	}
	return false;*/
}
bool DtbWithHooks::canGoToNextPage()
{
	amis::dtb::nav::NavNode* p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->nextPage(p_node->getPlayOrder());
		if (p_node != NULL) return true;
		else return false;
	}
	//there actually is a case where there is front matter outside the NavModel, but
	//the book has pages, so you should be able to go to the next page (i.e., the first page)
	else
	{
		if (getNavModel()->hasPages() == true)
			return true;
	}
	return false;
}
bool DtbWithHooks::canGoToPreviousPage()
{
	amis::dtb::nav::NavNode* p_node = getCurrentNavNode();
	if (p_node != NULL)
	{
		std::string id = p_node->getId();
		p_node = getNavModel()->previousPage(p_node->getPlayOrder());
		if (p_node != NULL) return true;
		else return false;
	}
	return false;
}

bool DtbWithHooks::getIsWaitingForLastmarkNode()
{
	return mbIsWaitingForLastmarkNode;
}
void DtbWithHooks::setIsWaitingForLastmarkNode(bool val)
{
	mbIsWaitingForLastmarkNode = val;
}

string DtbWithHooks::getIdOfLastmarkNode()
{
	return mIdOfLastmarkNode;
}

bool DtbWithHooks::hasAudio()
{
	if (MainWndParts::Instance() != 0 && MainWndParts::Instance()->mpMmView != 0)
	return MainWndParts::Instance()->mpMmView->expectingAudio();
	else return false;
}

bool DtbWithHooks::isPlaying()
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	if (view==NULL) return false;
	if (this->hasAudio())
		return view->isPlaying();
	else
	{
//#ifndef AVOID_SELF_VOICING_COM_STUFF
		if (!Preferences::Instance()->getSafeMode())
		{
			return amis::tts::TTSPlayer::InstanceTwo()->IsPlaying();
		}
//#else
		else
		{
			return false;
		}
//#endif
	}
}


void DtbWithHooks::pause()
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	if (view==NULL) return;
	
	//CAmisApp* pApp = (CAmisApp *) AfxGetApp(); 
	//pApp->setPauseState(true);
		
	if (this->hasAudio())
		view->OnFilePause();
	else
	{
		amis::gui::MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_SET_PAUSE_STATE, true);
		pauseTTS();
	}
}

void DtbWithHooks::resume()
{
	MmView *view = MainWndParts::Instance()->mpMmView;
	if (view==NULL) return;

	if (this->hasAudio())
		view->OnFilePlay();
	else
	{
		amis::gui::MainWndParts::Instance()->mpMainFrame->PostMessageW(WM_MY_SET_PAUSE_STATE, false);
		resumeTTS();
	}
}
void DtbWithHooks::stopTTS()
{
	amis::tts::TTSPlayer::InstanceTwo()->Stop();
}
void DtbWithHooks::pauseTTS()
{
	amis::tts::TTSPlayer::InstanceTwo()->Pause();
}
void DtbWithHooks::resumeTTS()
{
	amis::tts::TTSPlayer::InstanceTwo()->Resume();
}
void DtbWithHooks::speakTTS(wstring str)
{
	if (theApp.getShouldNotRenderAudio() == true) return;
	TRACE(_T("Speaking %s\n"), str.c_str());
	amis::tts::TTSPlayer::InstanceTwo()->Play(str.c_str());
}

//this is for TTSPlayer::InstanceTwo
void DtbWithHooks::ttsTwoDone()
{
	DtbWithHooks* p_inst = amis::dtb::DtbWithHooks::Instance();
	MainWndParts::Instance()->mpMainFrame->SendMessage(WM_COMMAND, (WPARAM)ID_AMIS_NEXT_PHRASE, (LPARAM)0);
	//p_inst->nextPhrase();
}
