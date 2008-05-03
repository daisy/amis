#include "../../../../resource.h"
#include "gui/self-voicing/datamodel/DataTree.h"
#include "gui/self-voicing/datamodel/Dialog.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include "gui/self-voicing/datamodel/Label.h"
#include "gui/self-voicing/Prompt.h"
#include "gui/self-voicing/UiMediaTypes.h"
#include "BookList.h"
#include "gui/self-voicing/dialogs/menuvoicing.h"
#include "gui/self-voicing/audiosequenceplayer.h"
#include "DtbWithHooks.h"
#include "gui/AmisApp.h"
#include "gui/MainWndParts.h"
#include "gui/MenuManip.h"

#include "util/Log.h"

namespace amis
{
	namespace gui
	{
		MenuVoicing::MenuVoicing(CMainFrame * frm)
		{
			mFrm= frm;
		}

		MenuVoicing::~MenuVoicing(void)
		{

		}

		//todo:	replace	this by	something like:
		// Container* container	= DataTree::Instance()->findContainerWhichHasFirstItem(id)
		// container.id
		std::string	MenuVoicing::computeRootMenuFromFirstChildID(unsigned int firstItemId, bool playPrompt)
		{

			
	USES_CONVERSION;

			CString	dbg;
			dbg.Format(_T("\n===firstItemId %d####\n"),	firstItemId);
			TRACE(dbg);

	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeMessage(W2CA(dbg));

			string str_id;

			if (firstItemId	== ID_AMIS_OPEN_FILE)
			{
				str_id = "file";
			}
			else if	(firstItemId ==	ID_AMIS_RECENT_BOOKS_EMPTY ||
				firstItemId == AMIS_RECENT_BOOK_BASE_ID) {
					str_id = "recentBooksList";
				}
				
			else if	(firstItemId ==	ID_AMIS_NO_PAGE_STYLES ||
				firstItemId == AMIS_PAGE_STYLE_BASE_ID) {
					str_id = "pageStylesList";
				}
			else if(firstItemId	== ID_AMIS_TOGGLE_SIDEBAR)
			{
				str_id = "view";
			}
			else if(firstItemId	== ID_AMIS_PLAYPAUSE)
			{
				str_id = "play";
			}
			else if(firstItemId	== ID_AMIS_PREVIOUS_SECTION)
			{
				str_id = "navigate";
			}
			else if(firstItemId	==	ID_AMIS_SECTION_DEPTH_EMPTY ||
				firstItemId	== AMIS_SECTION_DEPTH_BASE_ID + 1)
			{
				str_id = "sectiondepth";
			}

			else if(firstItemId	== ID_AMIS_ADD_BOOKMARK)
			{
				str_id = "bookmarks";
			}

			else if(firstItemId	== ID_AMIS_SHOW_READING_OPTIONS)
			{
				str_id = "info";
			}
			else if(firstItemId	== ID_AMIS_SHOW_HELP_CONTENTS)
			{
				str_id = "help";
			}
			else
			{
				str_id = "";
			}
			if (!str_id.empty()) {

				if (playPrompt)	{
					AudioSequence* seq = new AudioSequence();

					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, str_id);

					PromptItem* p_prompt_	= DataTree::Instance()->findPromptItem("menu");

					if (p_prompt_ != NULL)
					{
						AudioSequencePlayer::fillSequenceContents(seq, p_prompt_);
					}
					else {
						seq->append(_T("menu"));
					}		

					if (seq->GetCount() == 0) {
						delete seq;
					} else {
						AudioSequencePlayer::Instance()->Play(seq);
					}
				}
			}

			return str_id;
		}

		AudioSequence* MenuVoicing::createSeqPrepend(UINT nFlags) {
			
			AudioSequence* seq	= new AudioSequence();

			if ( nFlags	& MF_GRAYED	)
			{
				PromptItem* promptNotAvailable = DataTree::Instance()->findPromptItem("not_available");
				if (promptNotAvailable != NULL)
				{

					AudioSequencePlayer::fillSequenceContents(seq, promptNotAvailable);
				}
			}
			return seq;
		}

		void MenuVoicing::OnMenuSelect(UINT	nItemID, UINT nFlags, HMENU	hSysMenu)
		{

			USES_CONVERSION;

			//CMenu* pMenu = DYNAMIC_DOWNCAST(CMenu, (CObject*)hSysMenu);
			//TRACE("OnMenuSelect, hSysMenu=%p is %s\n", hSysMenu, ::IsMenu(hSysMenu) ? "HMENU" : pMenu ? "CMenu" : "unknown");

			bool isSysMenu = false;
			//HWND hWnd = MainWndParts::Instance()->mpMainFrame->GetSafeHwnd();
			CMenu * pSysMenu = mFrm->GetSystemMenu(FALSE);
			if (pSysMenu != NULL)
			{
				if (pSysMenu->m_hMenu == hSysMenu) {
					
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage("System Menu !");
					TRACE("System Menu !");

					isSysMenu = true;
				}
			}

			if (isSysMenu) {
				CString str;
				pSysMenu->GetMenuString(nItemID, str, MF_BYCOMMAND);

				str.Replace(L"&", L"");

				//str.Replace(CString("&"), CString(""));

				AudioSequence* seq = new AudioSequence;
				seq->append(str);
				AudioSequencePlayer::Instance()->Play(seq);


				//CMenu* p_menu = p_menu->GetSubMenu(nItemID);

				/*
				int nLen = ::GetMenuString(pSysMenu, nItemID, NULL, 0, MF_BYCOMMAND);
				if (nLen > 0) {

				LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));
				if (::GetMenuString(pSysMenu, nItemID, lpszText, nLen + 1, MF_BYCOMMAND) == nLen) {

				CString str;

				//pSysMenu->GetMenuString(nItemID, str, nFlags);
				//TRACE(str);
				str.Append(lpszText);

				str.Replace(L"&", L"");

				//str.Replace(CString("&"), CString(""));

				AudioSequence* seq = new AudioSequence;
				seq->append(str);
				AudioSequencePlayer::Instance()->Play(seq);


				}
				}
				*/
				return;
			}


			//don't	read separators
			if (nFlags & MF_SEPARATOR)
			{
				return;
			}

			CString	s;

			string prompt;


			if ( nFlags	& MF_POPUP )
			{
				if ( nFlags	& MF_BYPOSITION	)
				{
					//AmisBrain::Instance()->playPromptFromUiId(nItemID);
				}
				if ( nFlags	& MF_BYCOMMAND )
				{
					//AmisBrain::Instance()->playPromptFromUiId(nItemID);
				}


				if ((nItemID ==	8 || nItemID ==	2 || nItemID ==	3) && mFrm->m_lastOpenPopupMenu != NULL) { // 8 => "recent" (including separators), 2 => "show section depth"
					UINT nID = mFrm->m_lastOpenPopupMenu->GetMenuItemID(0);
					std::string	prompt = computeRootMenuFromFirstChildID(nID, false);

					s.Format(_T("\nMenuVoicing::OnMenuSelect(%d) '%s'\n"), nItemID, prompt.c_str());	 //DAN_MENU_DEBUG
					TRACE(s);

					
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage(W2CA(s));

			AudioSequence* seq = createSeqPrepend(nFlags);

					if (prompt.compare("file") == 0	|| prompt.compare("recentBooksList") ==	0) {
						AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "recentBooksList");

						PromptItem* p_prompt_	= DataTree::Instance()->findPromptItem("menu");

						if (p_prompt_ != NULL)
						{
							AudioSequencePlayer::fillSequenceContents(seq, p_prompt_);
						}
						else {
							seq->append(_T("menu"));
						}		

						if (seq->GetCount() == 0) {
							delete seq;
						} else {
							AudioSequencePlayer::Instance()->Play(seq);
						}
						return;
					} else if (prompt.compare("navigate") == 0 || prompt.compare("sectiondepth") ==	0) {
						AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "sectiondepth");


						PromptItem* p_prompt_	= DataTree::Instance()->findPromptItem("menu");

						if (p_prompt_ != NULL)
						{
							AudioSequencePlayer::fillSequenceContents(seq, p_prompt_);
						}
						else {
							seq->append(_T("menu"));
						}	

						if (seq->GetCount() == 0) {
							delete seq;
						} else {
							AudioSequencePlayer::Instance()->Play(seq);
						}
						return;
					} else if (prompt.compare("view") == 0 || prompt.compare("pageStylesList") ==	0) {
						AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "pageStylesList");


						PromptItem* p_prompt_	= DataTree::Instance()->findPromptItem("menu");

						if (p_prompt_ != NULL)
						{
							AudioSequencePlayer::fillSequenceContents(seq, p_prompt_);
						}
						else {
							seq->append(_T("menu"));
						}	

						if (seq->GetCount() == 0) {
							delete seq;
						} else {
							AudioSequencePlayer::Instance()->Play(seq);
						}
						return;
					} else {
						delete seq;
					}
				} else if ((nItemID >=0 && nItemID <=6) && (mFrm->m_lastOpenPopupMenu == NULL)) { //TODO:	Yuuk ! UGLY	! This is only a temporary workaround !	//(nFlags & MF_GRAYED)

					s.Format(_T("\nMenuVoicing::OnMenuSelect(%d)\n"), nItemID);	 //DAN_MENU_DEBUG
					TRACE(s);

		
					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage(W2CA(s));

			AudioSequence* seq = createSeqPrepend(nFlags);

					switch (nItemID) {
				case 0:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "file");
					break;
				case 1:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "view");
					break;
				case 2:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "play");
					break;
				case 3:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "navigate");
					break;
				case 4:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "bookmarks");
					break;
				case 5:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "info");
					break;
				case 6:
					AudioSequencePlayer::fillSequenceContainerPromptFromId(seq, "help");
					break;
					//default: seq = new AudioSequence();
					}
					PromptItem* p_prompt_	= DataTree::Instance()->findPromptItem("menu");

					if (p_prompt_ != NULL)
					{
						AudioSequencePlayer::fillSequenceContents(seq, p_prompt_);
					}
					else {
						seq->append(_T("menu"));

					}

					if (seq->GetCount() == 0) {
						delete seq;
					} else {
						AudioSequencePlayer::Instance()->Play(seq);
					}
					return;

				}
			}
			else
			{
				
					s.Format(_T("\nMenuVoicing::OnMenuSelect(%d is	selected)\n"), nItemID);	 //DAN_MENU_DEBUG
					
				TRACE(s);

					amis::util::Log* p_log = amis::util::Log::Instance();
					p_log->writeMessage(W2CA(s));

				amis::BookList* books = theApp.getHistory();
				int nRecentBooks = (books ? books->getNumberOfEntries() : 0);

				if	(nRecentBooks > 0 && (nItemID >= AMIS_RECENT_BOOK_BASE_ID && 
					nItemID < AMIS_RECENT_BOOK_BASE_ID + nRecentBooks))
				{
					int	idx	= nItemID -	AMIS_RECENT_BOOK_BASE_ID;

					amis::BookEntry* p_entry = theApp.getHistory()->getEntry(idx);

					//DanTodo: surely, there must be a way to get a title instead of rendering the URL with TTS ??! (see bookmark methd commented below)
					string str = p_entry->mPath.get_url();

			AudioSequence* seq = createSeqPrepend(nFlags);

					seq->append(A2T(str.c_str()));

					AudioSequencePlayer::Instance()->Play(seq);

					return;

					//CString cstr_filepath = A2T(filepath.c_str());

					/*
					amis::io::BookmarksFileIO bmk_file_io;
					bmk_file_io.readFromFile(&p_entry->mPath);

					amis::MediaGroup* p_title =	NULL;
					p_title	= bmk_file_io.getBookmarkSet()->getTitle();

					if (p_title	!= NULL)
					{
					if (p_title->hasAudio()	== true)
					{
					amis::AudioNode* p_audio = NULL;
					p_audio	= (amis::AudioNode*)p_title->getAudio(0)->clone();
					if (p_audio	!= NULL)
					{
					seq->append(p_audio);
					AudioSequencePlayer::Instance()->Play(seq);

					}


					}else if (p_title->hasText() == true)
					{
					amis::TextNode* p_text = NULL;
					p_text	= p_title->getText();
					if (p_text	!= NULL)
					{
					wstring str = p_text->getTextString();
					seq->append(str.c_str());
					AudioSequencePlayer::Instance()->Play(seq);
					}


					}

					} */
				}
				else
					if (nItemID	>= AMIS_BOOKMARKS_BASE_ID &&	nItemID	<= 
						AMIS_BOOKMARKS_BASE_ID + AMIS_MAX_ANYTHING)
					{
						int	idx	= nItemID -	AMIS_BOOKMARKS_BASE_ID;


						//amis::dtb::BookmarkSet* p_bmks = amis::dtb::DtbWithHooks::Instance()->getBookmarks();
						//amis::dtb::PositionMark* p_mark = p_bmks->getItem(idx);

			AudioSequence* seq = createSeqPrepend(nFlags);
						amis::MediaGroup* p_note = amis::dtb::DtbWithHooks::Instance()->getBookmarks()->getItem(idx)->mpNote;
						if (p_note!=NULL)
						{

							if (p_note->hasAudio() == true)
							{
								amis::AudioNode* p_audio = NULL;
								p_audio	= p_note->getAudio(0);
								if (p_audio	!= NULL)
								{
									amis::TextNode* p_text = NULL;
								p_text	= p_note->getText();
									seq->append(p_audio->clone(), (p_text != NULL ? p_text->getTextString().c_str() : L""));
									AudioSequencePlayer::Instance()->Play(seq);
									return;
								}
							} else if (p_note->hasText() == true)
							{
								//DanTodo: bmks generally have no audio, so how can we use the existing (open) book content to render audio voice here ?
								amis::TextNode* p_text = NULL;
								p_text	= p_note->getText();
								if (p_text	!= NULL)
								{
									wstring str = p_text->getTextString();
									seq->append(str.c_str());
									AudioSequencePlayer::Instance()->Play(seq);
									return;
								}


							}

						}
					} else 

						if (nItemID > AMIS_SECTION_DEPTH_BASE_ID 
							&& nItemID <= (AMIS_SECTION_DEPTH_BASE_ID + AMIS_MAX_ANYTHING))
						{
							string strPrompt;
							switch (nItemID) {
					case (AMIS_SECTION_DEPTH_BASE_ID+1): {
						strPrompt = "numeric_one";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+2): {
						strPrompt = "numeric_two";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+3): {
						strPrompt = "numeric_three";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+4): {
						strPrompt = "numeric_four";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+5): {
						strPrompt = "numeric_five";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+6): {
						strPrompt = "numeric_six";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+7): {
						strPrompt = "numeric_seven";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+8): {
						strPrompt = "numeric_eight";
						break;
														 }
					case (AMIS_SECTION_DEPTH_BASE_ID+9): {
						strPrompt = "numeric_nine";
						break;
														 }
							}
							if (!strPrompt.empty()) {
								//seq->append(strPrompt.c_str()); TEST ONLY
								AudioSequencePlayer::playPromptItemFromStringId(strPrompt);

								return;
							}

						}
						//is this a	view menu item (sections/prodnotes/etc)
						else if	(nItemID >=	AMIS_VIEW_MENU_BASE_ID && nItemID < AMIS_VIEW_MENU_BASE_ID + 
						MenuManip::Instance()->getNumberOfViewMenuNavContainers())
						{

						amis::dtb::nav::NavModel* p_model = amis::dtb::DtbWithHooks::Instance()->getNavModel();
	

						//first	item in	the	list is	always "sections"
						if (nItemID	== AMIS_VIEW_MENU_BASE_ID)
						{
							AudioSequencePlayer::playPromptItemFromStringId("sections");
						}
						//then "pages",	if exists
						else if	(nItemID ==	AMIS_VIEW_MENU_BASE_ID + 1 && 
						p_model != NULL && p_model->hasPages())
						{
							AudioSequencePlayer::playPromptItemFromStringId("pages");
						}
						else if (p_model != NULL)
						{
						//otherwise	it's a nav list	so ask NavParse	for	the	nav	list ID
						//subtract 1 because the menu items	include	sections, pages, navlists
						//we want just the index relative to nav lists
						int	idx	= nItemID -	AMIS_VIEW_MENU_BASE_ID - 1;

						//adjust again by subtracting 1	if there are pages in this book
						if (p_model->hasPages()	== true)
						{
							idx--;
						}

						std::string	item_id	= p_model->getNavList(idx)->getId();

						if (item_id.compare("prodnote")	== 0 ||
						item_id.compare("optional-prodnote") ==	0 )
						{
						AudioSequencePlayer::playPromptItemFromStringId("prodnotes");
						}
						else if	(item_id.compare("sidebar")	== 0)
						{
						AudioSequencePlayer::playPromptItemFromStringId("sidebars");
						}
						else if	(item_id.compare("footnote") ==	0 ||
						item_id.compare("noteref") == 0)
						{
						AudioSequencePlayer::playPromptItemFromStringId("noterefs");
						}
						else 
						{
						//we can't identify	it,	so read	its	name with TTS
						AudioSequencePlayer::playPromptFromUiId(nItemID);
						}
						}

						}

						else 
						{
							AudioSequence* seq = createSeqPrepend(nFlags);
							AudioSequencePlayer::playPromptFromUiId(nItemID, seq);
							return;
						}
			}
		}

	}
}
