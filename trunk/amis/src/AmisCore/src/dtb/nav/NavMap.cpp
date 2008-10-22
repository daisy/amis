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

#include <iostream>
#include "util/FilePathTools.h"
#include "dtb/nav/NavMap.h"

using namespace std;

amis::dtb::nav::NavMap::NavMap()
{
	mpRoot = NULL;
	mMaxDepth = -1;
	//the default id for this container
	setId("sections");
}

amis::dtb::nav::NavMap::~NavMap()
{	
	delete mpRoot;
}



//--------------------------------------------------
//print the tree
//--------------------------------------------------
void amis::dtb::nav::NavMap::print()
{
	cout<<endl<<"***TREE***"<<endl;
	printNode(mpRoot, 0);
}


//--------------------------------------------------
/*!
	recursive function to print a node at a certain indentation level
*/
//--------------------------------------------------
void amis::dtb::nav::NavMap::printNode(NavPoint* pNode, int level)
{
	
	int cnt;
	int i;

	// print out the correct number of tabs
	for (i = 0; i<level; i++)
	{
		cout<<"\t";
	}
	cout<<"LEVEL = "<<pNode->getLevel()<<endl;
	for (i = 0; i<level; i++)
	{
		cout<<"\t";
	}
	cout<<"ORDER = "<<pNode->getPlayOrder()<<endl;

	NavPoint* p_tmp = pNode->getParent();
	if (p_tmp != NULL)
	{
		// print out the correct number of tabs
		for (i = 0; i<level; i++)
		{
			cout<<"\t";
		}
		cout<<"PARENT = "<<p_tmp->getId()<<endl;
	}
	else
	{
		cout<<"I AM ROOT"<<endl;
	}

	//print this node
	if (level > 0)
	{
		pNode->print(level);
	}


	//print the children
	for (cnt = 0; cnt < pNode->getNumChildren(); cnt++)
	{
		printNode(pNode->getChild(cnt), level + 1);
	}
}

amis::dtb::nav::NavPoint* amis::dtb::nav::NavMap::getRoot()
{
	return mpRoot;
}

void amis::dtb::nav::NavMap::setRoot(NavPoint* pNode)
{
	mpRoot = pNode;
	recordNewDepth(pNode->getLevel());
}

void amis::dtb::nav::NavMap::recordNewDepth(int new_depth)
{
	if (new_depth > mMaxDepth) mMaxDepth = new_depth;
}

int amis::dtb::nav::NavMap::getMaxDepth()
{
	return mMaxDepth;
}

//find the node with this play order
amis::dtb::nav::NavNode* amis::dtb::nav::NavMap::goToPlayOrder(int playOrder)
{
	NavPoint* p_node = mpRoot;
	bool b_found = false;

	while (b_found == false && p_node != NULL)
	{
		if (p_node->getPlayOrder() == playOrder)
			b_found = true;
		else
			p_node = p_node->next();
	}

	if (b_found == true) return p_node;
	else return NULL;
}

//make the current nav point the one with this smil ref if exists
amis::dtb::nav::NavNode* amis::dtb::nav::NavMap::goToContentRef(string contentHref)
{
	NavPoint* p_node = mpRoot;

	bool b_found = false;

	string content_href;
	string content_target;

	while (b_found == false && p_node != NULL)
	{
		content_href = p_node->getContent();
		content_target = amis::util::FilePathTools::getTarget(content_href);
		content_href = amis::util::FilePathTools::getFileName(content_href);
		content_href += "#";
		content_href += content_target;

		if (content_href.compare(contentHref) == 0) 
			b_found = true;
		else 
			p_node = p_node->next();
	}

	if (b_found == true) return p_node;
	else return NULL;
}

//--------------------------------------------------
//returns whether this tree is empty or not
//--------------------------------------------------
bool amis::dtb::nav::NavMap::isEmpty()
{
	//test the root for NULL
	if (mpRoot == NULL) return true;
	else return false;
}

int amis::dtb::nav::NavMap::getNumberOfNavPoints()
{
	NavPoint* p_node = mpRoot;
	int n = 0;
	while (p_node != NULL)
	{
		n++;
		p_node = p_node->next();
	}
	return n-1;
}


amis::dtb::nav::NavNode* amis::dtb::nav::NavMap::goToId(string id)
{
	NavPoint* p_node = mpRoot;
	bool b_found = false;

	while (b_found == false && p_node != NULL)
	{
		if (p_node->getId().compare(id) == 0)
			b_found = true;
		else
			p_node = p_node->next();
	}

	if (b_found == true) return p_node;
	else return NULL;
}

void amis::dtb::nav::NavMap::acceptDepthFirst(NavVisitor* v)
{
	mpRoot->acceptDepthFirst(v);
}
