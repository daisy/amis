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

#include "dtb/nav/NavList.h"
#include "dtb/nav/NavModel.h"
#include "util/FilePathTools.h"
#include <iostream>

using namespace std;

//--------------------------------------------------
//constructor
//--------------------------------------------------
amis::dtb::nav::NavList::NavList()
{
}

//--------------------------------------------------
//destructor
//--------------------------------------------------
amis::dtb::nav::NavList::~NavList()
{
	int sz = mNodes.size();
	NavTarget* tmp_node;

	for (int i=sz-1; i>=0; i--)
	{
		tmp_node = mNodes[i];
		mNodes.pop_back();
		delete tmp_node;
	}
}

//--------------------------------------------------
//add node to list
//--------------------------------------------------
void amis::dtb::nav::NavList::addNode(NavTarget* pNode)
{
	pNode->setNavModel(mpNavModel);
	pNode->setIndex(mNodes.size());
	pNode->setNavContainer(this);
	mNodes.push_back(pNode);
	mpNavModel->addToPlayOrderList(pNode);
}

//--------------------------------------------------
//get the list length
//--------------------------------------------------
int amis::dtb::nav::NavList::getLength()
{
	return mNodes.size();
}

//--------------------------------------------------
//go to a certain node, based on play order
//--------------------------------------------------
amis::dtb::nav::NavNode* amis::dtb::nav::NavList::goToPlayOrder(int playOrder)
{
	bool b_found = false;
	unsigned int i = 0;
	for (i=0; i<mNodes.size(); i++)
	{
		if (mNodes[i]->getPlayOrder() == playOrder)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == true)
		return mNodes[i];
	else
		return NULL;
}

amis::dtb::nav::NavNode* amis::dtb::nav::NavList::goToContentRef(string contentHref)
{
	bool b_found = false;
	string content_href;

	unsigned int i = 0;

	for (i=0; i<mNodes.size(); i++)
	{
		content_href = mNodes[i]->getContent();
		content_href = amis::util::FilePathTools::getFileName(content_href);
		if (content_href.compare(contentHref) == 0)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == true)
		return mNodes[i];
	else
		return NULL;
}

amis::dtb::nav::NavNode* amis::dtb::nav::NavList::goToId(string id)
{
	//this is a bit redundant but easier than refactoring to remove this function entirely
	return this->getNavModel()->getNavNode(id);

	/*
	bool b_found = false;
	unsigned int i = 0;

	for (i=0; i<mNodes.size(); i++)
	{
		if (mNodes[i]->getId().compare(id) == 0)
		{
			b_found = true;
			break;
		}
	}

	if (b_found == true)
		return mNodes[i];
	else
		return NULL;*/
}

void amis::dtb::nav::NavList::print(bool printheader)
{
	if (printheader) cout<<"Nav List: "<<this->getId()<<endl;
	
	for (unsigned int i=0; i<mNodes.size(); i++)
	{
		mNodes[i]->print(0);
	
	}
}

void amis::dtb::nav::NavList::acceptDepthFirst(NavVisitor* v)
{
	if (mNodes.size() > 0)
		mNodes.front()->acceptDepthFirst(v);
}

amis::dtb::nav::NavTarget* amis::dtb::nav::NavList::getAtIndex(int idx)
{
	if (idx >= 0 && idx < mNodes.size())
		return mNodes[idx];
	else
		return NULL;
}
