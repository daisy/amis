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
#include "stdafx.h"
#include "gui/sidebar/NavListControl.h"
#include "gui/sidebar/AmisSidebar.h"
#include "dtb/nav/NavTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace amis::gui::sidebar;

CNavListControl::CNavListControl()
{
}

CNavListControl::~CNavListControl()
{
}

void CNavListControl::setId(std::string id)
{
	mId = id;
}

std::string CNavListControl::getId()
{
	return mId;
}

BEGIN_MESSAGE_MAP(CNavListControl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, doItemSelect)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeyDown)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CNavListControl::doItemSelect(NMHDR* pNMHDR, LRESULT* pResult)
{
	CAmisSidebar* parent = NULL;
	parent = (CAmisSidebar*)this->GetParent();

	amis::dtb::nav::NavTarget* p_target = NULL;
	int curr_sel = this->GetNextItem(-1, LVNI_SELECTED);
	;

	if (curr_sel > -1)
	{
		p_target = (amis::dtb::nav::NavTarget*)this->GetItemData(curr_sel);
		parent->OnNavListSelect(p_target);
		*pResult = 0;
	}
}


int CNavListControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	CWnd* p_parent = this->GetParent();
	CRect rect;
	p_parent->GetClientRect(&rect);
	int w = rect.Width();
	this->SetColumnWidth(0, w);
	return 0;
}

void amis::gui::sidebar::CNavListControl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	CWnd* p_parent = this->GetParent();
	CRect rect;
	p_parent->GetClientRect(&rect);
	int w = rect.Width();
	this->SetColumnWidth(0, w);
}

amis::dtb::nav::NavTarget* CNavListControl::previousItem()
{
	amis::dtb::nav::NavTarget* p_target = NULL;
	int curr_sel = this->GetNextItem(-1, LVNI_SELECTED);
	
	if (curr_sel - 1 > -1 && curr_sel - 1 < this->GetItemCount())
		curr_sel--;
	else
		curr_sel = 0;
		
	p_target = (amis::dtb::nav::NavTarget*)this->GetItemData(curr_sel);
	return p_target;
}

amis::dtb::nav::NavTarget* CNavListControl::nextItem()
{
	amis::dtb::nav::NavTarget* p_target = NULL;
	int curr_sel = this->GetNextItem(-1, LVNI_SELECTED);
	
	if (curr_sel + 1 > -1 && curr_sel + 1 < this->GetItemCount())
		curr_sel++;
	else
		curr_sel = this->GetItemCount() - 1;

	p_target = (amis::dtb::nav::NavTarget*)this->GetItemData(curr_sel);
	return p_target;
}

void CNavListControl::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVKEYDOWN* pKeyDown = (NMLVKEYDOWN*)pNMHDR;

	CAmisSidebar* parent = NULL;
	parent = (CAmisSidebar*)this->GetParent();

	amis::dtb::nav::NavTarget* p_target = NULL;
	
	if (pKeyDown->wVKey == VK_UP)
	{
		p_target = this->previousItem();
		parent->OnNavListSelect(p_target);
		*pResult = 0;
	}
	else if (pKeyDown->wVKey == VK_DOWN)
	{
		p_target = this->nextItem();
		parent->OnNavListSelect(p_target);
		*pResult = 0;
	}
}
