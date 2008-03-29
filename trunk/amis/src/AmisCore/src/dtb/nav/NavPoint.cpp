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

#include "dtb/nav/NavPoint.h"
#include "dtb/nav/NavMap.h"
#include "dtb/nav/NavModel.h"

amis::dtb::nav::NavPoint::NavPoint()
{
	mNumChildren = 0;
	mpSibling = NULL;
	mpParent = NULL;
	mpFirstChild = NULL;
	resetChildCount();
	this->mTypeOfNode = NavNode::NAV_POINT;
}

//Destructor
amis::dtb::nav::NavPoint::~NavPoint()
{
	NavPoint* p_tmp_node;
	
	//if this node has no children, perform any internal cleanup and return
	if (mpFirstChild == NULL) return;

	//else, this node has one or more children
	else
	{
		//loop through the children and delete one by one
		while(mNumChildren > 0)
		{
			p_tmp_node = getChild(mNumChildren-1);
			delete p_tmp_node;
			//each time a child is deleted, decrement the number of children
			mNumChildren--;
		}
	}
}

void amis::dtb::nav::NavPoint::resetChildCount()
{
	mChildCount = -1;
}

amis::dtb::nav::NavPoint* amis::dtb::nav::NavPoint::next()
{
	if (mChildCount < mNumChildren-1)
	{
		mChildCount++;
		NavPoint* ptmp = getChild(mChildCount);
		ptmp->resetChildCount();
		return ptmp;
	}
	else 
	{
		//we are leaving this branch so reset the child count
		this->resetChildCount();
		if (mpParent != NULL)
			return mpParent->next();
		else
			return NULL;
	}
	
}


//@todo: complete this function
amis::dtb::nav::NavPoint* amis::dtb::nav::NavPoint::previous()
{
	return NULL;
}

//return the immediate sibling
amis::dtb::nav::NavPoint* amis::dtb::nav::NavPoint::getFirstSibling()
{
	return mpSibling;
}

amis::dtb::nav::NavPoint* amis::dtb::nav::NavPoint::getChild(int index)
{
	NavPoint* p_tmp_node;
	
	//check the bounds of the requested index
	if (index < mNumChildren && index >= 0)
	{
		//initialize the temp variable to the first child pointer
		//we can assume it exists because of the if-statement conditions
		p_tmp_node = mpFirstChild;
		
		//for-loop to iterate through the sibling list until we get to the node at index-1
		for (int i=0; i<index; i++)
		{
			p_tmp_node = p_tmp_node->getFirstSibling();
		}

		mChildCount = index;
		
		//return a pointer to the requested child
		return p_tmp_node;
	}

	else
	{
		//index out of bounds or this node has no children
		return NULL;
	}
	
}


//return the number of children that this node has
int amis::dtb::nav::NavPoint::getNumChildren()
{
	return mNumChildren;
}

//return the level in the hierarchy (0 is outermost, 1 is indented once, etc)
int amis::dtb::nav::NavPoint::getLevel()
{
	return mLevel;
}

//--------------------------------------------------
// Add a sibling to this node
/*!
	This is a recursive function that will add the new node at the 
	end of the sibling chain
*/
//--------------------------------------------------
void amis::dtb::nav::NavPoint::addSibling(NavPoint* pNode)
{
	if (mpSibling == NULL)
	{ 
		//add the new node pointer as its sibling
		mpSibling = pNode;	
		//set the new node's parent to be the same as this node's parent
		mpSibling->setParent(this->getParent());
		//register this node with the play order list
		mpNavModel->addToPlayOrderList(pNode);
	} 
	else
	{
		//recursive call to try and add the new node to the sibling of this node
		mpSibling->addSibling(pNode);
	} 
}


void amis::dtb::nav::NavPoint::addChild(NavPoint* pNode)
{
	pNode->setNavModel(mpNavModel);
	NavMap* p_map = mpNavModel->getNavMap();

	if (p_map != NULL)
	{
		pNode->setNavContainer(p_map);
		p_map->recordNewDepth(pNode->getLevel());
	}

	//check if first child does not exist
	if (mNumChildren == 0 || mpFirstChild == NULL)
	{	
		//set new node pointer as first child
		mpFirstChild = pNode;
		//set the new child's parent to be this node
		mpFirstChild->setParent(this);
		//register this node with the play order list
		mpNavModel->addToPlayOrderList(pNode);
	}

	//if exists, add new node pointer as a sibling
	else
	{
		mpFirstChild->addSibling(pNode);
	}

	mNumChildren++;
}


void amis::dtb::nav::NavPoint::setLevel(int level)
{
	mLevel = level;
}

//--------------------------------------------------
// Set a pointer to this node's parent
//--------------------------------------------------
void amis::dtb::nav::NavPoint::setParent(NavPoint* pNode)
{
	mpParent = pNode;
}

//--------------------------------------------------
// Get a pointer to this node's parent
//--------------------------------------------------
amis::dtb::nav::NavPoint* amis::dtb::nav::NavPoint::getParent()
{
	return mpParent;
}

void amis::dtb::nav::NavPoint::acceptDepthFirst(NavVisitor* v)
{
	if (v->preVisit(this))
	{
		//read the children first
		if (mpFirstChild != NULL)
			mpFirstChild->acceptDepthFirst(v);
		//then look to the siblings
		if (mpSibling != NULL)
			mpSibling->acceptDepthFirst(v);
	}
}
