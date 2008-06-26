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

#include "dtb/nav/NavModel.h"
#include <iostream>
using namespace std;

amis::dtb::nav::NavModel::NavModel()
{
	//make a new nav map.
	//all models will have a nav map and page list
	mpNavMap = new NavMap();
	mpPageList = new PageList();
	mpPageList->setNavModel(this);
	mpNavMap->setNavModel(this);
	NavPoint* p_root = new NavPoint();
	//give the map a 'fake' root
	p_root->setClass("ROOT");
	p_root->setLevel(0);
	p_root->setPlayOrder(-1);
	p_root->setId("ROOT");
	p_root->setParent(NULL);
	p_root->setNavModel(this);
	p_root->setLabel(NULL);
	mpNavMap->setRoot(p_root);
	mpSmilIdNodeMap = NULL;
}

amis::dtb::nav::NavModel::~NavModel()
{
	int i;
	int sz = mNavLists.size();
	NavList* p_tmp_navlist;

	if (mpNavMap != NULL) delete mpNavMap;

	//delete nav lists and page list
	for (i=sz-1; i>=0; i--)
	{
		p_tmp_navlist = NULL;
		p_tmp_navlist = mNavLists[i];
		mNavLists.pop_back();
		if (p_tmp_navlist != NULL) delete p_tmp_navlist;
	}

	if (mpPageList != NULL) delete mpPageList;

	//delete the node map
	if (mpSmilIdNodeMap != NULL)
	{
		while (mpSmilIdNodeMap->size() > 0)
		{
			NodeRefMap::iterator it = mpSmilIdNodeMap->begin();
			NavNodeList* p_list = it->second;
			if (p_list != NULL) delete p_list;
			mpSmilIdNodeMap->erase(it);
			it = mpSmilIdNodeMap->begin();
		}
		delete mpSmilIdNodeMap;
	}

}

unsigned int amis::dtb::nav::NavModel::getNumberOfNavLists()
{
	return mNavLists.size();
}

amis::dtb::nav::NavList* amis::dtb::nav::NavModel::getNavList(unsigned int index)
{
	if (index < mNavLists.size())
		return mNavLists[index];
	else
		return NULL;
}
void amis::dtb::nav::NavModel::addToPlayOrderList(amis::dtb::nav::NavNode* pNode)
{
	if (pNode == NULL) return;
	//1 is the first playOrder value; mpBigOrderedList is ordered by play order
	int node_play_order = pNode->getPlayOrder();
	if (node_play_order >= mPlayOrderList.size() + 1)
	{
		mPlayOrderList.push_back(pNode);
	}
	//we are adding nodes out of order (not unusual)
	else
	{
		for (int i = 0; i<mPlayOrderList.size(); i++)
		{
			int play_order = mPlayOrderList[i]->getPlayOrder();
			if (play_order == node_play_order - 1)
			{
				//insert after the current node
				vector<amis::dtb::nav::NavNode*>::iterator it = mPlayOrderList.begin();
				it = it + i + 1;
				mPlayOrderList.insert(it, pNode);
				break;
			}
			else if (play_order == node_play_order + 1)
			{
				//insert before the current node
				vector<amis::dtb::nav::NavNode*>::iterator it = mPlayOrderList.begin();
				it = it + i;
				mPlayOrderList.insert(it, pNode);
				break;
			}
			else if (play_order > node_play_order + 1)
			{
				//insert before the current node
				vector<amis::dtb::nav::NavNode*>::iterator it = mPlayOrderList.begin();
				it = it + i;
				mPlayOrderList.insert(it, pNode);
				break;
			}
		}
	}
}

amis::dtb::nav::NavListList* amis::dtb::nav::NavModel::getNavLists()
{
	return &mNavLists;
}

amis::dtb::nav::NavList* amis::dtb::nav::NavModel::getNavList(string listname)
{
	bool b_found = false;
	unsigned int i;

	for (i=0; i<mNavLists.size(); i++)
	{
		if (mNavLists[i]->getId().compare(listname) == 0)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == true)
	{
		return mNavLists[i];
	}
	else
	{
		return NULL;
	}
}

int amis::dtb::nav::NavModel::addNavList(string listname)
{
	NavList* p_temp;
	bool b_found = false;

	for (unsigned int i=0; i<mNavLists.size(); i++)
	{
		p_temp = mNavLists[i];
		if (listname.compare(p_temp->getId()) == 0)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == false)
	{
		NavList* p_list = new NavList();
		p_list->setId(listname);
		p_list->setNavModel(this);
		mNavLists.push_back(p_list);

		return mNavLists.size() - 1;
	}
	else
	{
		return -1;
	}
}


amis::dtb::nav::PageList* amis::dtb::nav::NavModel::getPageList()
{
	return mpPageList;
}


bool amis::dtb::nav::NavModel::hasPages()
{
	if (mpPageList->getLength() > 0) return true;
	else return false;
}


amis::dtb::nav::NavMap* amis::dtb::nav::NavModel::getNavMap()
{
	return mpNavMap;
}

void amis::dtb::nav::NavModel::acceptDepthFirst(NavVisitor* v)
{
	if (mpNavMap != NULL && mpNavMap->isEmpty() == false)
		mpNavMap->acceptDepthFirst(v);

	if (mpPageList != NULL && mpPageList->getLength() > 0)
		mpPageList->acceptDepthFirst(v);

	for (int i = 0; i<mNavLists.size(); i++)
	{
		if (mNavLists[i] != NULL && mNavLists[i]->getLength() > 0)
			mNavLists[i]->acceptDepthFirst(v);
	}
	
}
void amis::dtb::nav::NavModel::setSmilIdNodeMap(NodeRefMap* pMap)
{
	mpSmilIdNodeMap = pMap;
}
void amis::dtb::nav::NavModel::testMap()
{
	if (mpSmilIdNodeMap == NULL)
		cout<<"map is null!"<<endl;
	else
	{
		string key = "bagw0001.smil#bagw_0001";
		NavNodeList* node_list = NULL;
		node_list = (*mpSmilIdNodeMap)[key];
		if (node_list == NULL)
			cout<<"data not available"<<endl;
		else
			cout<<"data OK!  this id has "<<node_list->size()<<" associated nodes"<<endl;
	}
}

amis::dtb::nav::NavNode* amis::dtb::nav::NavModel::getNodeForSmilId(string id, NavContainer* pContainer)
{
	NavNode* p_node = NULL;
	if (mpSmilIdNodeMap != NULL)
	{
		NavNodeList* node_list = NULL;
		node_list = (*mpSmilIdNodeMap)[id];
		if (node_list != NULL)
		{
			for (int i = 0; i<node_list->size(); i++)
			{
				NavNode* p_n = (*node_list)[i];
				if (p_n->getNavContainer() == pContainer)
				{
					p_node = p_n;
					break;
				}
			}
		}
	}
	
	return p_node;
}

amis::dtb::nav::NavPoint* amis::dtb::nav::NavModel::previousSection(int currentPlayOrder)
{
	//minus 2 because: play order is 1-based while our list is 0-based
	//so a node with a play order of 10 will be in position 9
	//and we want to start looking backwards from position 8
	int idx = currentPlayOrder - 2;
	if (idx >= mPlayOrderList.size() || idx < 0) return NULL;

	for (int i = idx; i >=0; i--)
	{
		if (mPlayOrderList[i]->getTypeOfNode() == NavNode::NAV_POINT)
		{
			NavPoint* p_node = (NavPoint*)mPlayOrderList[i];
			return p_node;
		}
	}
	return NULL;
}
amis::dtb::nav::NavPoint* amis::dtb::nav::NavModel::nextSection(int currentPlayOrder)
{
	int idx = currentPlayOrder;
	if (idx >= mPlayOrderList.size() || idx < 0) return NULL;

	for (int i = idx; i < mPlayOrderList.size(); i++)
	{
		if (mPlayOrderList[i]->getTypeOfNode() == NavNode::NAV_POINT)
		{
			NavPoint* p_node = (NavPoint*)mPlayOrderList[i];
			return p_node;
		}
	}
	return NULL;
}
//get the previous section at the given level
amis::dtb::nav::NavPoint* amis::dtb::nav::NavModel::previousSection(int currentPlayOrder, int level)
{
	NavPoint* p_nav = previousSection(currentPlayOrder);
	while (p_nav != NULL)
	{
		if (p_nav->getLevel() <= level)
			return p_nav;
		else
			p_nav = previousSection(p_nav->getPlayOrder());
	}
	return NULL;

}
//get the next section at the given level
amis::dtb::nav::NavPoint* amis::dtb::nav::NavModel::nextSection(int currentPlayOrder, int level)
{
	NavPoint* p_nav = nextSection(currentPlayOrder);
	while (p_nav != NULL)
	{
		if (p_nav->getLevel() <= level)
			return p_nav;
		else
			p_nav = nextSection(p_nav->getPlayOrder());
	}
	return NULL;
}
amis::dtb::nav::PageTarget* amis::dtb::nav::NavModel::previousPage(int currentPlayOrder)
{
	int idx = currentPlayOrder - 2;
	if (idx >= mPlayOrderList.size() || idx <= 0) return NULL;

	for (int i = idx; i >=0; i--)
	{
		if (mPlayOrderList[i]->getTypeOfNode() == NavNode::PAGE_TARGET)
		{
			PageTarget* p_node = (PageTarget*)mPlayOrderList[i];
			return p_node;
		}
	}
	return NULL;
}
amis::dtb::nav::PageTarget* amis::dtb::nav::NavModel::nextPage(int currentPlayOrder)
{
	//play order is 1-based; the array is 0-based
	int idx = currentPlayOrder;
	if (idx >= mPlayOrderList.size() || idx <= 0) return NULL;

	for (int i = idx; i < mPlayOrderList.size(); i++)
	{
		if (mPlayOrderList[i]->getTypeOfNode() == NavNode::PAGE_TARGET)
		{
			PageTarget* p_node = (PageTarget*)mPlayOrderList[i];
			return p_node;
		}
	}
	return NULL;
}
amis::dtb::nav::NavNodeList* amis::dtb::nav::NavModel::getPlayOrderList()
{
	return &mPlayOrderList;
}