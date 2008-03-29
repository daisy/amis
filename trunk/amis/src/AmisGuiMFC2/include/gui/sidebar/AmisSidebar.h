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
#ifndef AMISSIDEBAR_H
#define AMISSIDEBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "../../../resource.h"
#include "gui/sidebar/cdxCDynamicBar.h"
#include "gui/sidebar/NavListControl.h"
#include "dtb/nav/NavTarget.h"
#include "util/Color.h"

#include <vector>
#include <string>

#define AMIS_SIDEBAR_SMALLEST_FONT_SIZE	100
#define AMIS_SIDEBAR_BIGGEST_FONT_SIZE	400
namespace amis
{
namespace gui
{
namespace sidebar
{
class CAmisSidebar: public cdxCDynamicBarDlg
{
public:
	CAmisSidebar(CWnd* pParent = NULL);
	~CAmisSidebar();
	CNavListControl* addNavListWidget();
	void addTab(std::wstring);
	void expandAllSections();
	void expandSections(int level);
	int getExposedDepth();
	void clearAll();
	void setHighlightBGColor(amis::util::Color);
	void setHighlightFGColor(amis::util::Color);
	void setPageBGColor(amis::util::Color);
	void setPageFGColor(amis::util::Color);
	void setFontName(std::string);
	void setFontSize(double);
	void setSelectedNode(amis::dtb::nav::NavPoint*);
	void setSelectedNode(amis::dtb::nav::PageTarget*);
	enum { IDD = IDD_SIDEBAR };
	CTabCtrl mTabStrip;
	CTreeCtrl mTree;
	CListCtrl mPageList;
	int mTabSel;

	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNavListSelect(NMHDR* pNMHDR, LRESULT* pResult, amis::dtb::nav::NavTarget* pData);
	afx_msg void OnPageListClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPageListKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedListPage(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	
private:
	void showPageList();
	void showNavList(unsigned int);
	void showNavMap();
	void treeBranchCollapse(HTREEITEM);
	void treeBranchExpand(HTREEITEM, int);

	CRect mInitialRect;
	CRect mInitialParentRect;
	std::vector<CNavListControl*> mNavLists;
	CImageList mImagesForTab;
	int mNumTabs;
	int mExposedDepth;
	bool mIsShiftDown;
	bool mIsControlDown;
	CFont mDefaultFont;
	CFont* mpAlteredFont;
	int mFontSizeInPoints;
	bool mbIgnoreTreeSelect;
	bool mbIgnorePageListSelect;
	DECLARE_MESSAGE_MAP();

friend class AmisSidebarLoader;
};
}
}
}
#endif 