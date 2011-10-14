/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#include "gui/sidebar/AmisSidebarLoader.h"
#include "util/Log.h"
#include "Preferences.h"

using namespace amis::gui::sidebar;

AmisSidebarLoader* AmisSidebarLoader::pinstance = 0;

AmisSidebarLoader::AmisSidebarLoader()
{
	mpSidebar = NULL;
}

AmisSidebarLoader* AmisSidebarLoader::Instance()
{
	if (pinstance == 0) pinstance = new AmisSidebarLoader(); 
    return pinstance;
}

void AmisSidebarLoader::DestroyInstance()
{
	delete pinstance;
}

AmisSidebarLoader::~AmisSidebarLoader()
{
}

void AmisSidebarLoader::loadNavigationData(amis::dtb::nav::NavModel* pNavModel, amis::gui::sidebar::CAmisSidebar* pSidebar)
{
	amis::util::Log::Instance()->writeMessage("Loading navigation data into the sidebar", "AmisSidebarLoader::loadNavigationData");
	mpSidebar = pSidebar;
	mpSidebar->clearAll();
	
	mpSidebar->mTabStrip.ShowWindow(SW_SHOW);

	//1. load nav map
	if (pNavModel != NULL && pNavModel->getNavMap() != NULL)
	{
		//add the nav map to the tab
		mpSidebar->addTab(getTextLabel(pNavModel->getNavMap()));

		//load its data
		pNavModel->getNavMap()->acceptDepthFirst(this);
		
		mpSidebar->mTree.Expand(mpSidebar->mTree.GetRootItem(), 3);
		mpSidebar->mTree.RedrawWindow();
	}
	
	//2. load the pagelist
	if (pNavModel->hasPages()) 
	{
		mpSidebar->addTab(getTextLabel(pNavModel->getPageList()));
		loadNavList(pNavModel->getPageList(), &mpSidebar->mPageList);
		mpSidebar->mPageList.RedrawItems(0, mpSidebar->mPageList.GetItemCount()-1);
		mpSidebar->mPageList.RedrawWindow();
	}
	
	//3. load the navlists (if exist)
	amis::dtb::nav::NavListList* p_lists = NULL;
	p_lists = pNavModel->getNavLists();
	if (p_lists != NULL || p_lists->size() > 0)
	{
		for (int i=0; i<p_lists->size();i++)
		{
			mpSidebar->addTab(getTextLabel((*p_lists)[i]));
			CNavListControl* p_list_widget = mpSidebar->addNavListWidget();
			p_list_widget->setId((*p_lists)[i]->getId());
			loadNavList((*p_lists)[i], p_list_widget);
			p_list_widget->RedrawItems(0, p_list_widget->GetItemCount() - 1);
			p_list_widget->RedrawWindow();
		}
	}

	//expand all sections
	mpSidebar->expandAllSections();
	mpSidebar->setFontName(Preferences::Instance()->getAppFontName());
	amis::util::Log::Instance()->writeMessage("Data loaded into sidebar", "AmisSidebarLoader::loadNavigationData");
	mpSidebar->selectTab(0);
}

//this gets triggered while loading the nav map (navigation tree structure)
//we are assuming that NavNode is actually a NavPoint
bool AmisSidebarLoader::preVisit(amis::dtb::nav::NavNode* pNode)
{
	if (pNode == NULL) return false;
	
	HTREEITEM hItem;
	HTREEITEM hParent;
	CString tmpstr = _T("-");
	
	if (pNode->getLabel() != NULL && pNode->getLabel()->hasText() == true)
	{
		tmpstr = pNode->getLabel()->getText()->getTextString().c_str();
	}
	
	//nodes will represent open nodes, one at each level
	//the vector is only as long as the current nested level
	int level = ((amis::dtb::nav::NavPoint*)pNode)->getLevel();
	if (level == 1)
	{
		hItem = mpSidebar->mTree.InsertItem(tmpstr);
		hParent = hItem;
		mWidgetNodes.clear();
	}
	else if (level > 1)
	{
		//find the right parent node
		//subtract one to adjust for 0-based array
		//subtract one more to adjust for what the parent node index should be
		int adjusted_level = level - 2;
		int sz = mWidgetNodes.size();

		//loop backwards, removing a node for each level that cannot be a parent
		//to the new node
		for (int i=sz-1; i > adjusted_level; i--)
		{
			mWidgetNodes.pop_back();
		}

		hItem = mpSidebar->mTree.InsertItem(tmpstr, mWidgetNodes.back());
	}
	mWidgetNodes.push_back(hItem);
	mpSidebar->mTree.SetItemData(hItem, (DWORD)pNode);

	return true;
}

//this could load pages or a nav list (pagelists are navlists; pagetargets are navtargets)
void AmisSidebarLoader::loadNavList(amis::dtb::nav::NavList* pList, CListCtrl* pListWidget)
{
	amis::dtb::nav::NavTarget* p_node = NULL;
	
	for (int i = 0; i<pList->getLength(); i++)
	{
		p_node = NULL;
		p_node = pList->getAtIndex(i);
		if (p_node != NULL)
		{
			std::wstring label = getTextLabel(p_node);
			pListWidget->InsertItem(i, label.c_str());
			pListWidget->SetItemData(i, (DWORD)p_node);
		}
	}
}

std::wstring AmisSidebarLoader::getTextLabel(amis::dtb::nav::NavContainer* pData)
{
	amis::MediaGroup* p_label = NULL;
	p_label = pData->getLabel();
	
	if (p_label != NULL)
	{
		if (p_label->hasText() == true)
			return p_label->getText()->getTextString().c_str();
	}
	return _T("");
}

std::wstring AmisSidebarLoader::getTextLabel(amis::dtb::nav::NavNode* pNode)
{
	amis::MediaGroup* p_label = NULL;
	p_label = pNode->getLabel();
	
	if (p_label != NULL)
	{
		if (p_label->hasText() == true)
			return p_label->getText()->getTextString().c_str();
	}
	return _T("");
}

