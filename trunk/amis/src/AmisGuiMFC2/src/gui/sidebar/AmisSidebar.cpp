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
#include "stdafx.h"
#include "gui/sidebar/AmisSidebar.h"
#include "util/FilePathTools.h"
#include "DtbWithHooks.h"
#include "dtb/nav/NavModel.h"
#include "dtb/nav/NavMap.h"
#include "dtb/nav/NavNode.h"
#include "gui/AmisApp.h"
#include "../../../resource.h"
#include "util/Log.h"
#include "Preferences.h"
#include "gui/MenuManip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace amis::gui::sidebar;

BEGIN_MESSAGE_MAP(CAmisSidebar, cdxCDynamicBarDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangeTree)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PAGE, OnPageListClick)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PAGE, OnPageListKeyDown)
END_MESSAGE_MAP()

CAmisSidebar::CAmisSidebar(CWnd* pParent /*=NULL*/)
	: cdxCDynamicBarDlg(CAmisSidebar::IDD, pParent)
{
	mTabSel = 0;
	mFontSizeInPoints = AMIS_SIDEBAR_SMALLEST_FONT_SIZE;
}

CAmisSidebar::~CAmisSidebar()
{
	clearAll();
	mImagesForTab.DeleteImageList();
	mDefaultFont.DeleteObject();
}

void CAmisSidebar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, mTabStrip);
	DDX_Control(pDX, IDC_TREE, mTree);
	DDX_Control(pDX, IDC_LIST_PAGE, mPageList);
}

void CAmisSidebar::clearAll()
{
	amis::util::Log::Instance()->writeMessage("Clearing all data from the sidebar", "CAmisSidebar::clearAll", "AmisGuiMFC2");
	CNavListControl* p_navl = NULL;
	unsigned int sz = mNavLists.size();

	for (int i=sz-1; i>=0; i--)
	{
		p_navl = mNavLists[i];
		if (p_navl) 
		{
			p_navl->DestroyWindow();
		}
		mNavLists.pop_back();
		if (p_navl) delete p_navl;
	}

	if (mTree) mTree.DeleteAllItems();
	if (mPageList) mPageList.DeleteAllItems();

	if (mTabStrip)
	{
		mTabStrip.DeleteAllItems();
		this->RedrawWindow();
	}
	mNumTabs = 0;
}
void CAmisSidebar::setHighlightBGColor(amis::util::Color clr)
{
	//TODO: implement sidebar highlight control
}
void CAmisSidebar::setHighlightFGColor(amis::util::Color clr)
{
	//TODO: implement sidebar highlight control
	//this foreground color feature is easy but meaningless until the BG color can also be changed
}
void CAmisSidebar::setPageBGColor(amis::util::Color clr)
{
	mTree.SetBkColor(clr.getAsColorRef());
	mPageList.SetBkColor(clr.getAsColorRef());
	for (int i = 0; i<mNavLists.size(); i++)
	{
		mNavLists[i]->SetBkColor(clr.getAsColorRef());
	}
}
void CAmisSidebar::setPageFGColor(amis::util::Color clr)
{
	mTree.SetTextColor(clr.getAsColorRef());
	mPageList.SetTextColor(clr.getAsColorRef());
	for (int i = 0; i<mNavLists.size(); i++)
	{
		mNavLists[i]->SetTextColor(clr.getAsColorRef());
	}
}
void CAmisSidebar::setFontName(std::string fontName)
{
	USES_CONVERSION;
	CFont* p_font = NULL;
	p_font=mTree.GetFont();
	LOGFONT lf;
	p_font->GetLogFont(&lf);
	CString font_name;
	font_name = A2T(amis::Preferences::Instance()->getSidebarFontName().c_str());
	lstrcpy(lf.lfFaceName, font_name);
	//font default size and boldness values
	lf.lfHeight = mFontSizeInPoints;
	lf.lfWeight = 600;
	mDefaultFont.DeleteObject();
	mDefaultFont.CreatePointFontIndirect(&lf);
	mTree.SetFont(&mDefaultFont);
	mPageList.SetFont(&mDefaultFont);
	for (int i = 0; i < mNavLists.size(); i++)
	{
		mNavLists[i]->SetFont(&mDefaultFont);
	}
}
//pct should give some idea of the total possible gradients
void CAmisSidebar::setFontSize(double pct)
{
	LOGFONT lf;
	mDefaultFont.GetLogFont(&lf);
	int diff = AMIS_SIDEBAR_BIGGEST_FONT_SIZE - AMIS_SIDEBAR_SMALLEST_FONT_SIZE;
	if (pct <= 0.0) 
		mFontSizeInPoints = AMIS_SIDEBAR_SMALLEST_FONT_SIZE;
	else if (pct >= 1.0) 
		mFontSizeInPoints = AMIS_SIDEBAR_BIGGEST_FONT_SIZE;
	else
		mFontSizeInPoints = AMIS_SIDEBAR_SMALLEST_FONT_SIZE + (pct * diff);

	lf.lfHeight = mFontSizeInPoints;
	mDefaultFont.DeleteObject();
	mDefaultFont.CreatePointFontIndirect(&lf);
	mTree.SetFont(&mDefaultFont);
	mPageList.SetFont(&mDefaultFont);
	for (int i = 0; i < mNavLists.size(); i++)
	{
		mNavLists[i]->SetFont(&mDefaultFont);
	}
}
void CAmisSidebar::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	mTabSel = mTabStrip.GetCurSel();
	changeView(mTabSel);
	*pResult = 0;
}


void CAmisSidebar::OnSelchangeTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (theApp.isBookOpen() == false) return;
	if (mbIgnoreTreeSelect == true) return;
	amis::dtb::nav::NavPoint* p_nav = NULL;
	HTREEITEM item;
	item = mTree.GetSelectedItem();
	p_nav = (amis::dtb::nav::NavPoint*)mTree.GetItemData(item);
	if (p_nav != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(p_nav);
}

//this function is triggered by CNavListControl objects
void CAmisSidebar::OnNavListSelect(NMHDR* pNMHDR, LRESULT* pResult, amis::dtb::nav::NavTarget* pData)
{
	if (theApp.isBookOpen() == false) return;
	if (pData != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(pData);
}

void CAmisSidebar::OnPageListClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (theApp.isBookOpen() == false) return;
	if (mbIgnorePageListSelect == true) return;
	amis::dtb::nav::PageTarget* p_page = NULL;
	int curr_sel;
	curr_sel = mPageList.GetSelectionMark();

	if (curr_sel > -1)
	{
		p_page = (amis::dtb::nav::PageTarget*)mPageList.GetItemData(curr_sel);
		if (p_page != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(p_page);
	}
}

void CAmisSidebar::OnPageListKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	//right now, the current selection mark has not been updated
	//by the list control because the message hasn't gone through yet
	//so we need to go either forward or back one, depending on what key the user pressed

	if (mbIgnorePageListSelect == true)
	{
		*pResult = 0;
		return;
	}

	NMLVKEYDOWN* pKeyDown = (NMLVKEYDOWN*)pNMHDR;
	amis::dtb::nav::PageTarget* p_page = NULL;
	int curr_sel;
	string content;
	curr_sel = mPageList.GetSelectionMark();
	if (pKeyDown->wVKey == VK_UP && (!mIsControlDown || !mIsShiftDown))
	{
		if (curr_sel - 1 > -1 && curr_sel - 1 < mPageList.GetItemCount())
		{
			curr_sel--;
			p_page = (amis::dtb::nav::PageTarget*)mPageList.GetItemData(curr_sel);
		}
	}
	else if (pKeyDown->wVKey == VK_DOWN && (!mIsControlDown || !mIsShiftDown))
	{
		if (curr_sel + 1 > -1 && curr_sel + 1 < mPageList.GetItemCount())
		{
			curr_sel++;
			p_page = (amis::dtb::nav::PageTarget*)mPageList.GetItemData(curr_sel);
		}
	}

	if (p_page != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(p_page);
	*pResult = 0;
}

BOOL CAmisSidebar::OnInitDialog() 
{
	USES_CONVERSION;

	cdxCDynamicBarDlg::OnInitDialog();

	mTree.GetWindowRect(mInitialRect);
	this->GetWindowRect(mInitialParentRect);

	
	mNumTabs = 0;
	mIsShiftDown = false;
	mIsControlDown = false;

	int iconsz_x, iconsz_y;
	iconsz_x = 16;
	iconsz_y = 16;

	mImagesForTab.Create(iconsz_x, iconsz_y,ILC_COLOR24 | ILC_MASK,4,0);

	//load the icons from file
	HICON hIcon[3];
	CString icon_tree, icon_pages, icon_special;
	string app_path = theApp.getAppPath();
	string str_icon_tree, str_icon_pages, str_icon_special;

 	str_icon_tree = app_path + "settings\\img\\hierarchy.ico";
	str_icon_pages = app_path + "settings\\img\\pages.ico";
	str_icon_special = app_path + "settings\\img\\figures.ico";
	icon_tree = A2CW(str_icon_tree.c_str());
	icon_pages = A2CW(str_icon_pages.c_str());
	icon_special = A2CW(str_icon_special.c_str());
	
	hIcon[0] = (HICON)LoadImage(NULL, (LPCTSTR)icon_tree, IMAGE_ICON,
				iconsz_x, iconsz_y, LR_LOADFROMFILE);
	hIcon[1] = (HICON)LoadImage(NULL, (LPCTSTR)icon_pages, IMAGE_ICON,
				iconsz_x, iconsz_y, LR_LOADFROMFILE);
	hIcon[2] = (HICON)LoadImage(NULL, (LPCTSTR)icon_special, IMAGE_ICON,
				iconsz_x, iconsz_y, LR_LOADFROMFILE);

	mImagesForTab.Add(hIcon[0]);
	mImagesForTab.Add(hIcon[1]);
	mImagesForTab.Add(hIcon[2]);

	mTabStrip.SetImageList(&mImagesForTab);

	AddSzControl(mTree,mdResize,mdResize);
	AddSzControl(mTabStrip,mdResize,mdRepos);
	AddSzControl(mPageList, mdResize, mdResize);

	mPageList.ShowWindow(SW_HIDE);
	mTabStrip.ShowWindow(SW_HIDE);
	mTree.ShowWindow(SW_HIDE);
	mbIgnoreTreeSelect = false;
	mbIgnorePageListSelect = false;
	return TRUE;  
}

//This handles remembering shift and control keymasks for anything in this dialog
//including the sections tree, page list, and other lists
BOOL CAmisSidebar::PreTranslateMessage(MSG* pMsg)
{
	/*the key presses go something like this:
	1. keydown goes to this function as a message
	2. we filter it so the tree/list view only gets keys that are meaningful to it
	3. all other keyspresses are passed on to the parent window
	4. if the list widget got the keypress, its own keydown function gets invoked
	5. if the keydown was shift or control, make a note of it and pass it to the parent window
	6. send keyups to the same place as the keydown
	*/

	if (pMsg->message == WM_KEYDOWN) 
	{
		//let the arrow keys operate the tree view
		if ((pMsg->wParam == VK_UP ||
			pMsg->wParam == VK_DOWN ||
			pMsg->wParam == VK_RETURN)&& 
			mIsControlDown == false && 
			mIsShiftDown == false)
		{
			TRACE(_T("\n dialog key down, giving to dialog \n"));
			return cdxCDynamicBarDlg::PreTranslateMessage(pMsg);
		}
		
		//pass other keys through
		else
		{
			if (pMsg->wParam == VK_SHIFT)
			{
				mIsShiftDown = true;
				TRACE(_T("\n Shift \n"));
			}
			else if (pMsg->wParam == VK_CONTROL)
			{
				mIsControlDown = true;
				TRACE(_T("\n Control \n"));
			}
			
			TRACE(_T("\n dialog key down, pass through \n"));
			//the parent is the main MDI window
			CWnd* p_parent = this->GetTopLevelParent();
			p_parent->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return FALSE;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		if ((pMsg->wParam == VK_LEFT ||
				pMsg->wParam == VK_RIGHT ||
				pMsg->wParam == VK_UP ||
				pMsg->wParam == VK_DOWN ||
				pMsg->wParam == VK_RETURN)&& 
				mIsControlDown == false && 
				mIsShiftDown == false)
		{
			TRACE(_T("\nkeyup, sending to dialog\n"));
			return cdxCDynamicBarDlg::PreTranslateMessage(pMsg);
		}
		else
		{
			TRACE(_T("\n dialog key up, pass through \n"));
			if (pMsg->wParam == VK_SHIFT) mIsShiftDown = false;
			else if (pMsg->wParam == VK_CONTROL) mIsControlDown = false;

			//the parent is the main MDI window
			CWnd* p_parent = this->GetTopLevelParent();
			p_parent->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return FALSE;
		}
	}
	else
	{
		//pass other messages (not keydown/keyup) through
		return cdxCDynamicBarDlg::PreTranslateMessage(pMsg);
	}

}
void CAmisSidebar::showPageList()
{
	mPageList.ShowWindow(SW_SHOW);
	mTree.ShowWindow(SW_HIDE);

	for (unsigned int i=0; i<mNavLists.size(); i++)
		mNavLists[i]->ShowWindow(SW_HIDE);

	mPageList.SetFocus();
	this->RedrawWindow();
	//TODO: then amis should play an audio prompt "pages"
}

void CAmisSidebar::showNavList(unsigned int idx)
{
	if (idx >= mNavLists.size()) return;

	//something crashed here, hence the try/catch
	try 
	{
		for (unsigned int i=0; i<mNavLists.size(); i++)
			mNavLists[i]->ShowWindow(SW_HIDE);

		mNavLists[idx]->ShowWindow(SW_SHOW);
	
		mPageList.ShowWindow(SW_HIDE);
		mTree.ShowWindow(SW_HIDE);
		mNavLists[idx]->SetFocus();

		this->RedrawWindow();
	}
	catch(...)
	{
	}

	//TODO: amis should play an audio prompt for the chosen list
}

void CAmisSidebar::showNavMap()
{
	mTree.ShowWindow(SW_SHOW);
	for (unsigned int i=0; i<mNavLists.size(); i++)
		mNavLists[i]->ShowWindow(SW_HIDE);

	mPageList.ShowWindow(SW_HIDE);
	mTree.SetFocus();
	this->RedrawWindow();
	//TODO: amis should play an audio prompt "sections"
}

CNavListControl* CAmisSidebar::addNavListWidget()
{
	CRect rect, parent_rect;
	int top, bottom, left, right;
	CNavListControl* p_nav_list = NULL;
	p_nav_list = new CNavListControl();
	mNavLists.push_back(p_nav_list);

	GetWindowRect(parent_rect);
	//the pageview's rectangle
	mPageList.GetWindowRect(rect);
	
	//use init rects when sizing a runtime control
	//the sizing control manager requires this
	left = mInitialRect.left - mInitialParentRect.left;
	top = mInitialRect.top - mInitialParentRect.top;
	bottom = (mInitialRect.bottom - mInitialRect.top) + top;
	right = (mInitialRect.right - mInitialRect.left)+ left;

	CRect new_rect(left, top, right, bottom);

	p_nav_list->Create(LVS_LIST | LVS_SINGLESEL | LVA_ALIGNLEFT, new_rect, this, 
		AMIS_NAVLIST_BASE_ID + mNumTabs);
	p_nav_list->SetExtendedStyle(LVS_SHAREIMAGELISTS);

	AddSzControl(*p_nav_list, mdResize, mdResize);

	return p_nav_list;
}

void CAmisSidebar::addTab(std::wstring label)
{
	CString tab_label = label.c_str();
	int pos = mTabStrip.GetItemCount() + 1;
	mTabStrip.InsertItem(pos, tab_label, pos-1);

	mNumTabs++;
}

//*******************
//tree view expand and collapse
//*******************
int CAmisSidebar::getExposedDepth()
{
	return mExposedDepth;
}

void CAmisSidebar::expandAllSections()
{
	int max_depth = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
	expandSections(max_depth);
}
//expand the tree view to a certain depth
//completely collapse all nodes not shown
void CAmisSidebar::expandSections(int level)
{
	int max_allowed_depth = amis::dtb::DtbWithHooks::Instance()->getNavModel()->getNavMap()->getMaxDepth();
	if (max_allowed_depth == 0) max_allowed_depth = 1;

	if (level > 0 && level <= max_allowed_depth) mExposedDepth = level;
	else if (level == 0) level = 1;
	else level = max_allowed_depth;

	mExposedDepth = level;

	//save the current selection
	HTREEITEM h_selected = NULL;
	h_selected = mTree.GetSelectedItem();

	HTREEITEM h_curr = mTree.GetRootItem();

	//first: collapse all branches
	while (h_curr != NULL)
	{
		treeBranchCollapse(h_curr);
		h_curr = mTree.GetNextSiblingItem(h_curr);
	}
	h_curr = mTree.GetRootItem();
	
	//then expand to the right depth
	if (level > 1)
	{
		while (h_curr != NULL)
		{
			treeBranchExpand(h_curr, level-1);
			h_curr = mTree.GetNextSiblingItem(h_curr);
		}
	}
	
	//make sure the selection is visible
	if (h_selected == NULL) h_selected = mTree.GetRootItem();
	mTree.EnsureVisible(h_selected);
}

void CAmisSidebar::treeBranchCollapse(HTREEITEM hItem)
{
	HTREEITEM h_curr = hItem;

	if(h_curr != NULL && mTree.ItemHasChildren(h_curr) == TRUE)
	{
		mTree.Expand(h_curr, TVE_COLLAPSE );
        h_curr = mTree.GetChildItem(h_curr);
        
		while (h_curr != NULL)
		{
			treeBranchCollapse(h_curr);
			h_curr = mTree.GetNextSiblingItem(h_curr);
        }
	}
}

void CAmisSidebar::treeBranchExpand(HTREEITEM hItem, int level)
{
	HTREEITEM h_curr = hItem;

	if (h_curr != NULL && level > 0 && mTree.ItemHasChildren(h_curr) == TRUE)
	{
		mTree.Expand(h_curr, TVE_EXPAND);
		h_curr = mTree.GetChildItem(h_curr);

		while (h_curr != NULL)
		{
			treeBranchExpand(h_curr, level - 1);
			h_curr = mTree.GetNextSiblingItem(h_curr);
		}
	}		
}

void CAmisSidebar::setSelectedNode(amis::dtb::nav::NavPoint* pNode)
{
	if (pNode == NULL) return;
	mbIgnoreTreeSelect = true;
	
	HTREEITEM h_curr;
	HTREEITEM h_temp;
	h_curr = mTree.GetFirstVisibleItem();
	bool b_found = false;
	amis::dtb::nav::NavPoint* p_item_data = NULL;
	while (h_curr != NULL && b_found == false)
	{
		p_item_data = (amis::dtb::nav::NavPoint*)mTree.GetItemData(h_curr);
		if (p_item_data == pNode)
		{
			b_found = true;
		}
		else
		{
			//it will not autotraverse in-order, so give it a parameter..next sibling, child, whatever
			//the idea is to go to the first child
			h_temp = mTree.GetNextItem(h_curr, TVGN_CHILD);
			//otherwise go to the sibling item
			if (h_temp == NULL)	h_temp = mTree.GetNextItem(h_curr, TVGN_NEXT);
			//otherwise find a parent who has siblings
			if (h_temp == NULL)
			{
				h_temp = h_curr;
				HTREEITEM h_parent;
				do
				{
					h_parent = mTree.GetParentItem(h_temp);
					if (h_parent != NULL)
					{
						h_temp = mTree.GetNextItem(h_parent, TVGN_NEXT);
						if (h_temp == NULL) h_temp = h_parent;
						else break;
					}
					else
					{
						//we are at the root so just return NULL
						//the root has no info
						h_temp = NULL;
						break;
					}
				}
				while (1);
			}
			h_curr = h_temp;
		}
	}

	if (b_found == true && h_curr != NULL) 
		mTree.SelectItem(h_curr) == TRUE;

	mbIgnoreTreeSelect = false;
}
void CAmisSidebar::setSelectedNode(amis::dtb::nav::PageTarget* pNode)
{
	if (pNode == NULL) return;
	mbIgnorePageListSelect = true;
	
	LVITEM item;
	mPageList.SetSelectionMark(pNode->getIndex());
	mPageList.GetItem(&item);
	mPageList.SetItemState(pNode->getIndex(), 0, LVIS_SELECTED);

	mbIgnorePageListSelect = false;
}
void CAmisSidebar::selectTab(int sel)
{
	if (sel > mTabStrip.GetItemCount() - 1) return;
	changeView(sel);
}
void CAmisSidebar::changeView(int sel)
{
	mTabStrip.SetCurSel(sel);
	MenuManip::Instance()->setCheckmarkOnForNavigationContainer(sel);
	if (sel == 0) 
	{
		showNavMap();
	}
	else if(sel == 1)
	{
		if (amis::dtb::DtbWithHooks::Instance()->getNavModel()->hasPages() == true) showPageList();
		else showNavList(sel-1);
	}
	else
	{
		if (amis::dtb::DtbWithHooks::Instance()->getNavModel()->hasPages() == true) showNavList(sel-2);
		else showNavList(sel-1);
	}
}
void CAmisSidebar::setFocusToActiveList()
{
	int idx = mTabStrip.GetCurSel();
	changeView(idx);
}