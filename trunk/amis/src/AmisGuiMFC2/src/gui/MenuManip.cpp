/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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
#include "../../resource.h"

#include "gui/MenuManip.h"
#include "gui/MainWndParts.h"
#include "gui/AmisApp.h"
#include "DtbWithHooks.h"
#include "BookList.h"
#include "dtb/nav/NavModel.h"
#include "gui/TextRenderBrain.h"
#include "util/FilePathTools.h"
#include "Preferences.h" 
using namespace amis::gui;

MenuManip* MenuManip::pinstance = 0;

MenuManip* MenuManip::Instance()
{
	if (pinstance == 0)
	{
		pinstance = new MenuManip;
	}

	return pinstance;
}

void MenuManip::DestroyInstance()
{
	delete pinstance;
}

MenuManip::MenuManip()
{
	mCountViewMenuNavContainers = 0;
}

MenuManip::~MenuManip()
{
}

void MenuManip::refreshRecentBooksListMenu()
{
	USES_CONVERSION;
	if (theApp.getHistory() == NULL) return;
	int num_entries = theApp.getHistory()->getNumberOfEntries();

	CMenu* p_main_menu = NULL;
	p_main_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();

	//exit if no menu is present..this could mean we are in basic view mode
	if (p_main_menu == NULL) return;

	if (num_entries > 0)
	{
		CMenu* p_menu = NULL;
		//get "File"
		p_menu = p_main_menu->GetSubMenu(0);
		assert(p_menu);
		//get "Recent"
		p_menu = p_menu->GetSubMenu(8);
		assert(p_menu);
		
		UINT num_menu_items = p_menu->GetMenuItemCount();

		//clear it
		for (int i=num_menu_items-1; i>=0; i--)	p_menu->RemoveMenu(i, MF_BYPOSITION);
			
		//re-add all entries
		amis::BookEntry* p_entry = NULL;
		int max = AMIS_MAX_RECENT_BOOKS;
		if (num_entries < max) max = num_entries;
		//get the last X entries
		for (int i=num_entries-1; i>=num_entries-max; i--)
		{
			p_entry = theApp.getHistory()->getEntry(i);
			CString menu_text;
			wstring title_text = p_entry->getTitleText();
			if (title_text.length() > 0)
			{
				menu_text = title_text.c_str();
			}
			else
			{
				string filepath = p_entry->mPath.get_url();
				menu_text = A2T(filepath.c_str());
			}
			BOOL ret = p_menu->AppendMenu(MF_STRING, AMIS_RECENT_BOOK_BASE_ID + i, menu_text);
		}		
	}
}

//this is called each time a book is opened
//it adds the section depth options and greys-out or enables the page navigation
void MenuManip::setupNavigationOptions()
{
	amis::dtb::nav::NavModel* p_model = NULL;
	p_model = amis::dtb::DtbWithHooks::Instance()->getNavModel();
	if (p_model == NULL) return;

	int max_depth = p_model->getNavMap()->getMaxDepth();

	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();

	//exit if no menu is present..this could mean we are in basic view mode
	if (p_menu == NULL) return;
	
	//get "Navigate"
	p_menu = p_menu->GetSubMenu(3);

	//get "Show Section Depth" popup
	p_menu = p_menu->GetSubMenu(2);

	//clear all menu items
	int num_entries = p_menu->GetMenuItemCount();
	for (int i=num_entries-1; i>=0; i--)
		p_menu->RemoveMenu(i, MF_BYPOSITION);

	//add 1...max_depth items
	for (int i=1; i<=max_depth; i++)
	{
		CString menu_label;
		menu_label.Format(_T("&%d"), i);
		BOOL ret = p_menu->AppendMenu(MF_STRING, AMIS_SECTION_DEPTH_BASE_ID + i, menu_label);
	}
	//remember that other enabling/disabling is done in CMainFrame's ON_UPDATE_COMMAND_UI handlers
}
int MenuManip::getNumberOfViewMenuNavContainers() 
{
	return mCountViewMenuNavContainers;
}

//add a list of navigation containers to the view menu
void MenuManip::addNavContainersToViewMenu()
{
	amis::dtb::nav::NavModel* p_model = NULL;
	p_model = amis::dtb::DtbWithHooks::Instance()->getNavModel();
	if (p_model == NULL) return;
	
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	//exit if no menu is present..this could mean we are in basic view mode
	if (p_menu == NULL) return;
	
	//get "View"
	p_menu = p_menu->GetSubMenu(1);

	//remove old stuff (anything below "Text style...", whose index is 6)
	int len = p_menu->GetMenuItemCount();
	for (int i=len-1; i>6; i--)
		p_menu->RemoveMenu(i, MF_BYPOSITION);

	
	//add a separator
	p_menu->AppendMenu(MF_SEPARATOR);

	//add the nav map (sections)
	int count = 0;
	CString label = p_model->getNavMap()->getLabel()->getText()->getTextString().c_str();
	p_menu->AppendMenu(MF_STRING, AMIS_VIEW_MENU_BASE_ID, label);
	count++;
	if (p_model->hasPages() == true)
	{
		label = p_model->getPageList()->getLabel()->getText()->getTextString().c_str();
		p_menu->AppendMenu(MF_STRING, AMIS_VIEW_MENU_BASE_ID + count, label);
		count++;
	}
	for (int i = 0; i<p_model->getNumberOfNavLists(); i++)
	{
		amis::dtb::nav::NavList* p_list = NULL;
		p_list = p_model->getNavList(i);
		if (p_list == NULL) continue;
		label = p_list->getLabel()->getText()->getTextString().c_str();
		p_menu->AppendMenu(MF_STRING, AMIS_VIEW_MENU_BASE_ID + count, label);
		count++;
	}
	mCountViewMenuNavContainers = count;
	//put a checkmark by the "sections" item
	setCheckmarkOnForNavigationContainer(0);
}
void MenuManip::setItemCheckmark(MainMenu menuName, bool isChecked, UINT itemId, SubMenu submenuName)
{
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	assert(p_menu);
	//get the specified menu
	p_menu = p_menu->GetSubMenu(menuName);
	assert(p_menu);
	if (submenuName != AMIS_SUBMENU_NONE)
		p_menu = p_menu->GetSubMenu(submenuName);
	
	if (isChecked == true) p_menu->CheckMenuItem(itemId, MF_CHECKED);
	else p_menu->CheckMenuItem(itemId, MF_UNCHECKED);
}

void MenuManip::setSectionDepthCheckmark(UINT itemId)
{
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	assert(p_menu);
	//get "Navigate"
	p_menu = p_menu->GetSubMenu(3);
	//get "Show Section Depth" popup
	p_menu = p_menu->GetSubMenu(2);
	assert(p_menu);
	
	//turn off the checkmark for all navigation container items
	//all the navigation containers are below a separator whose index is 7)
	int count = p_menu->GetMenuItemCount();
	for (int i = 0; i<count; i++)
		setItemCheckmark(AMIS_NAVIGATE, false, AMIS_SECTION_DEPTH_BASE_ID + i, AMIS_SECTION_DEPTH);

	//turn on the checkmark for the given index
	setItemCheckmark(AMIS_NAVIGATE, true, itemId, AMIS_SECTION_DEPTH);
}

void MenuManip::addBookmark(amis::dtb::PositionMark* pBmk)
{
	USES_CONVERSION;
	
	if (pBmk == NULL) return;
	
	CMenu* p_menu = NULL;
	CString menu_label;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	//get "bookmarks"
	p_menu = p_menu->GetSubMenu(4);

	amis::TextNode* p_text = NULL;

	if (pBmk->mbHasNote == true && pBmk->mpNote->hasText() == true)
	{
		//add the text of the note as the menu caption
		p_text = pBmk->mpNote->getText();
		menu_label = p_text->getTextString().c_str();
		menu_label.TrimLeft();
		menu_label.TrimRight();
	}
	//use the URI 
	else
	{
		menu_label = A2CW(pBmk->mpStart->mUri.get_url().c_str());
	}
	
	int bmk_index = amis::dtb::DtbWithHooks::Instance()->getBookmarks()->getIndexOf(pBmk);
	if (bmk_index != -1)
		p_menu->AppendMenu(MF_STRING, AMIS_BOOKMARKS_BASE_ID + bmk_index, menu_label);
}

void MenuManip::loadBookmarks(amis::dtb::BookmarkSet* pBookmarks)
{
	USES_CONVERSION;

	//clear all the bookmarks
	clearBookmarks();

	//if there is no new bookmark data, it means that no bookmark file was found
	//and one could not be created 
	//so disable the add bookmark menu option and return
	if (pBookmarks == NULL)
	{
		CMenu* p_menu = NULL;
		p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
		//exit if no menu is present..this could mean we are in basic view mode
		if (p_menu == NULL) return;
		//get to "bookmarks"
		p_menu = p_menu->GetSubMenu(4);
		p_menu->EnableMenuItem(ID_AMIS_ADD_BOOKMARK, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		return;
	}

	//add all the bookmarks for this book
	for (int i=0; i<pBookmarks->getNumberOfItems(); i++)
	{
		amis::dtb::PositionMark* p_mark = NULL;
		p_mark = pBookmarks->getItem(i);
		if (p_mark) addBookmark(p_mark);
	}
}
//clear all the bookmarks from the Bookmarks menu
void MenuManip::clearBookmarks()
{
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	//exit if no menu is present..this could mean we are in basic view mode
	if (p_menu == NULL) return;
	//get to "bookmarks"
	p_menu = p_menu->GetSubMenu(4);
	int num_entries = p_menu->GetMenuItemCount();
	//remove all but the first two ("add bookmark" and the separator)
	for (int i=num_entries-1; i>=2; i--) 
		p_menu->RemoveMenu(i, MF_BYPOSITION);
	
	//enable the "add bookmark" item
	p_menu->EnableMenuItem(ID_AMIS_ADD_BOOKMARK, MF_BYCOMMAND | MF_ENABLED);
}
void MenuManip::setPauseState(bool pauseState)
{
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	if (p_menu == NULL) return;
	
	p_menu = p_menu->GetSubMenu(2);
	assert(p_menu);

	bool b_BookIsPlaying = MainWndParts::Instance()->mpMainFrame->mbWasPlayingWhenLostFocus 
		|| !pauseState;
	
	UiItem* p_uiItem = DataTree::Instance()->findUiItemInContainers(ID_AMIS_PLAYPAUSE, (b_BookIsPlaying ? "canPause" : "canPlay"));
	if (p_uiItem != NULL)
	{
		Label* accelerator = NULL;
		if (p_uiItem->getUiItemType() == ACTION)
		{
			accelerator = ((Action*)p_uiItem)->getKeyboardAccelerator();
		}
		Label* mnemonic = NULL;
		if (p_uiItem->getUiItemType() == ACTION)
		{
			mnemonic = ((Action*)p_uiItem)->getMnemonic();
		}
		Label* p_caption = p_uiItem->getCaption();
		if (p_caption != NULL)
		{
			TextAudioPair* pair = p_caption->getContents();
			if (pair != NULL)
			{
				TextNodeSV* textNode = pair->getText();
				if (textNode != NULL)
				{
					wstring str = textNode->getTextString();
					if (mnemonic != NULL)
					{
						TextAudioPair* pair = mnemonic->getContents();
						if (pair != NULL)
						{
							TextNodeSV* node = pair->getText();
							if (node != NULL)
							{
								wstring strMnemonic = node->getTextString();
								int index = str.find_first_of(strMnemonic, 0);
								if (index != -1)
								{
									str.insert(index, _T("&"));
								}
							}
						}
					}

					if (accelerator != NULL)
					{
						str.append(_T("\t"));
						TextAudioPair* pair = accelerator->getContents();
						if (pair != NULL)
						{
							TextNodeSV* node = pair->getText();
							if (node != NULL)
							{
								str.append(node->getTextString());
							}
						}
					}
					p_menu->ModifyMenu(0, MF_BYPOSITION, ID_AMIS_PLAYPAUSE, str.c_str());
				}
			}
		}
	}
}

void MenuManip::addPageStyles()
{
	USES_CONVERSION;

	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	//exit if no menu is present..this could mean we are in basic view mode
	if (p_menu == NULL) return;
	//get to "view"
	p_menu = p_menu->GetSubMenu(1);
	//get to "page styles"
	p_menu = p_menu->GetSubMenu(3);
	int num_entries = p_menu->GetMenuItemCount();
	int i;
	//remove all but the first one ("None")
	for (i=num_entries-1; i>=1; i--) 
		p_menu->RemoveMenu(i, MF_BYPOSITION);
	
	//add all the page styles in the settings/css/customStyles folder
	amis::UrlList custom_styles = *Preferences::Instance()->getCustomCssFiles();
	for (i=0; i<custom_styles.size(); i++)
	{
		CString menu_label;
		string file_name = amis::util::FilePathTools::getFileName(custom_styles[i].get_file());
		menu_label = A2T(file_name.c_str());
		p_menu->AppendMenu(MF_STRING, AMIS_PAGE_STYLE_BASE_ID + i, menu_label);
	}
}
void MenuManip::setCheckmarkOnForNavigationContainer(UINT idx)
{
	CMenu* p_menu = NULL;
	p_menu = MainWndParts::Instance()->mpMainFrame->GetMenu();
	//exit if no menu is present..this could mean we are in basic view mode
	if (p_menu == NULL) return;
	//get "View"
	p_menu = p_menu->GetSubMenu(1);
	
	//turn off the checkmark for all navigation container items
	//all the navigation containers are below a separator whose index is 7)
	int count = p_menu->GetMenuItemCount() - 7;
	for (int i = 0; i<count; i++)
		setItemCheckmark(AMIS_VIEW, false, AMIS_VIEW_MENU_BASE_ID + i);

	//turn on the checkmark for the given index
	setItemCheckmark(AMIS_VIEW, true, AMIS_VIEW_MENU_BASE_ID + idx);
}