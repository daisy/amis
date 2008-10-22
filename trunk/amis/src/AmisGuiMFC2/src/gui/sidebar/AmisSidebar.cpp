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
#include "gui/sidebar/AmisSidebar.h"
#include "util/FilePathTools.h"
#include "DtbWithHooks.h"
#include "dtb/nav/NavModel.h"
#include "dtb/nav/NavMap.h"
#include "dtb/nav/NavNode.h"
#include "gui/AmisApp.h"
#include "gui/MainWndParts.h"
#include "../../../resource.h"
#include "util/Log.h"
#include "Preferences.h"
#include "gui/MenuManip.h"
#include "Preferences.h"
#include "gui/self-voicing/audiosequenceplayer.h"

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
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_PAGE, OnPageListSetFocus)
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
	amis::util::Log::Instance()->writeMessage("Clearing all data from the sidebar", "CAmisSidebar::clearAll");
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
void CAmisSidebar::OnNavListSelect(amis::dtb::nav::NavTarget* pData)
{
	USES_CONVERSION;
	if (theApp.isBookOpen() == false) return;
	CString msg;
	msg.Format(_T("ON NAV LIST SELECT: %s\n"), A2T(pData->getContent().c_str()));
	TRACE(msg);
	if (pData != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(pData);
}

void CAmisSidebar::OnPageListClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (theApp.isBookOpen() == false) return;
	if (mbIgnorePageListSelect == true) return;
	amis::dtb::nav::PageTarget* p_page = NULL;
	int curr_sel;
	curr_sel = mPageList.GetNextItem(-1, LVNI_SELECTED);
	

	if (curr_sel > -1)
	{
		p_page = (amis::dtb::nav::PageTarget*)mPageList.GetItemData(curr_sel);
		if (p_page != NULL) amis::dtb::DtbWithHooks::Instance()->loadNavNode(p_page);
	}
}

void CAmisSidebar::OnPageListSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	//find the current page
	/*const ambulant::net::url* p_url = amis::gui::MainWndParts::Instance()->mpMmView->getCurrentUrl();
	string id = p_url->get_ref();
	string file = amis::util::FilePathTools::getFileName(p_url->get_url());
	file = file + "#" + id;
	amis::dtb::nav::PageTarget* p_page = NULL;
	amis::dtb::nav::NavModel* p_nav = amis::dtb::DtbWithHooks::Instance()->getNavModel();
	p_page = (amis::dtb::nav::PageTarget*)p_nav->getNodeForSmilId(file, p_nav->getPageList());
	setSelectedNode(p_page);*/
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
	string app_path = theApp.getAppSettingsPath();
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

BOOL CAmisSidebar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) 
	{
		//don't process the up/down arrows for list controls
		if ((pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN) && mTabSel != 0)
		{
			return cdxCDynamicBarDlg::PreTranslateMessage(pMsg);
		}
		else
		{
			//if the keystroke was interesting to page/nav lists, then we've already exited at this point
			//pass other keys through to the main application
			if (pMsg->wParam == VK_SHIFT)
			{
				if (pMsg->message == WM_KEYDOWN)
					mIsShiftDown = true;
				else
					mIsShiftDown = false;
			}
			else if (pMsg->wParam == VK_CONTROL) 
			{
				if (pMsg->message == WM_KEYDOWN)
					mIsControlDown = true;
				else
					mIsControlDown = false;
			}
			
			TRACE(_T("\n dialog key down, pass through \n"));
			//the parent is the main MDI window
			CWnd* p_parent = this->GetTopLevelParent();
			p_parent->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return FALSE;
		}
	}
	else // not keyup/down
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
}

void CAmisSidebar::showNavMap()
{
	mTree.ShowWindow(SW_SHOW);
	for (unsigned int i=0; i<mNavLists.size(); i++)
		mNavLists[i]->ShowWindow(SW_HIDE);

	mPageList.ShowWindow(SW_HIDE);
	mTree.SetFocus();
	this->RedrawWindow();
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
	int max_image_index = mImagesForTab.GetImageCount() - 1;
	int image_index = pos - 1;
	//if we have more tabs than images, then re-use the last image in the image list
	if (image_index > max_image_index) image_index = max_image_index;
	
	mTabStrip.InsertItem(pos, tab_label, image_index);

	mNumTabs++;
}

//*******************
//tree view expand and collapse
//*******************
//This gets the depth set by the 
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
	
	//update the selection
	h_selected = findParentAtLevel(h_selected, level);
	if (h_selected == NULL) h_selected = mTree.GetRootItem();
	this->mbIgnoreTreeSelect = true;
	mTree.EnsureVisible(h_selected);
	mTree.SelectItem(h_selected);
	this->mbIgnoreTreeSelect = false;
}

void CAmisSidebar::treeBranchCollapse(HTREEITEM hItem)
{
	HTREEITEM h_curr = hItem;

	mTree.Expand(h_curr, TVE_COLLAPSE );
	if(h_curr != NULL && mTree.ItemHasChildren(h_curr) == TRUE)
	{
		
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

	//don't select an item if it is deeper than our exposed depth
	if (b_found == true && h_curr != NULL && getLevel(h_curr) <= this->mExposedDepth) 
		mTree.SelectItem(h_curr) == TRUE;

	mbIgnoreTreeSelect = false;
}
void CAmisSidebar::setSelectedNode(amis::dtb::nav::PageTarget* pNode)
{
	if (pNode == NULL) return;
	mbIgnorePageListSelect = true;
	
	int idx = pNode->getIndex();
	mPageList.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	mPageList.SetSelectionMark(idx);
	idx = mPageList.GetSelectionMark();
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
	mTabSel = sel;
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
	
	amis::gui::CAmisApp::pauseBookAndEmitMessage("sidebarHasFocus");
}
void CAmisSidebar::setFocusToActiveList()
{
	int idx = mTabStrip.GetCurSel();
	changeView(idx);
}
void CAmisSidebar::updateSelection()
{
	const ambulant::net::url* p_url = MainWndParts::Instance()->mpMmView->getCurrentUrl();
	string id = p_url->get_ref();
	string file = amis::util::FilePathTools::getFileName(p_url->get_url());
	file = file + "#" + id;
	
	amis::dtb::nav::NavModel* p_nav = amis::dtb::DtbWithHooks::Instance()->getNavModel();
	amis::dtb::nav::PageTarget* p_page = NULL;
	amis::dtb::nav::NavPoint* p_section = NULL;
	if (p_nav->getPageList() != NULL)
		p_page = (amis::dtb::nav::PageTarget*)p_nav->getNodeForSmilId(file, p_nav->getPageList());
	else
		p_page = NULL;
	p_section = (amis::dtb::nav::NavPoint*)p_nav->getNodeForSmilId(file, p_nav->getNavMap());

	setSelectedNode(p_page);
	setSelectedNode(p_section);
}

HTREEITEM CAmisSidebar::findParentAtLevel(HTREEITEM hItem, int level)
{
	vector<HTREEITEM> parent_stack;
	parent_stack.clear();
	HTREEITEM parent = mTree.GetParentItem(hItem);
	while (parent != NULL)
	{
		CString text = mTree.GetItemText(parent);
		parent_stack.push_back(parent);
		parent = mTree.GetParentItem(parent);
	}

	//level is 1-based
	level = level - 1;
	//count from the end because our stack has the innermost items first
	level = parent_stack.size() - 1 - level;
		
	if (parent_stack.size() > 0 && level < parent_stack.size() && level >= 0) 
		return parent_stack[level];
	else
		return hItem;
}

int CAmisSidebar::getLevel(HTREEITEM hItem)
{
	HTREEITEM parent = mTree.GetParentItem(hItem);
	int level = 1;
	while (parent != NULL)
	{
		parent = mTree.GetParentItem(parent);
		level++;
	}
	
	return level;
}

void CAmisSidebar::OnPageListKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (mbIgnorePageListSelect == true)
	{
		*pResult = 0;
		return;
	}

	NMLVKEYDOWN* pKeyDown = (NMLVKEYDOWN*)pNMHDR;
	amis::dtb::nav::PageTarget* p_page = NULL;
	int curr_sel;
	string content;
	curr_sel = mPageList.GetNextItem(-1, LVNI_SELECTED);

	
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
