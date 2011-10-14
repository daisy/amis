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

//SYSTEM INCLUDES
#include <iostream>

//PROJECT INCLUDES
#include "dtb/smil/SmilEngineConstants.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/SmilTree.h"
#include "dtb/smil/TimeContainerNode.h"

using namespace std;

//--------------------------------------------------
//Default constructor
/*!
	initialize member variables.
*/
//--------------------------------------------------
amis::dtb::smil::TimeContainerNode::TimeContainerNode()
{
	mNumChildren = 0;
	mpFirstChild = NULL;
	mSkipOptionName = "";
}

//--------------------------------------------------
//Destructor
/*!
	delete all children recursively
*/
//--------------------------------------------------
amis::dtb::smil::TimeContainerNode::~TimeContainerNode()
{
	Node* p_tmp_node;
	
	//if this node has no children, perform any internal cleanup and return
	if (mpFirstChild == NULL)
	{
		return;
	}

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

//--------------------------------------------------
//add a child to this time container node
//--------------------------------------------------
void amis::dtb::smil::TimeContainerNode::addChild(Node* pNode)
{
	//check if first child does not exist
	if (mNumChildren ==0 || mpFirstChild == NULL)
	{	
		//set new node pointer as first child
		mpFirstChild = pNode;
		
		//set the new child's parent to be this node
		mpFirstChild->setParent(this);

		mNumChildren++;
	}

	//if exists, add new node pointer as a sibling
	else
	{
		mpFirstChild->addSibling(pNode);
		mNumChildren++;
	}
}

//--------------------------------------------------
//set a skippability option for this node. 
/*!
	an empty string "" means that this node has no skippability option
*/
//--------------------------------------------------
void amis::dtb::smil::TimeContainerNode::setSkipOption(string skipOptionName)
{
	mSkipOptionName = skipOptionName;
}

//--------------------------------------------------
//return the skippability option associated with this node
//--------------------------------------------------
string amis::dtb::smil::TimeContainerNode::getSkipOption()
{
	return mSkipOptionName;
}

//--------------------------------------------------
//return a pointer to the child node at the specified index
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::TimeContainerNode::getChild(int index)
{
	//local variables
	Node* p_tmp_node;
	
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
		
		//return a pointer to the requested child
		return p_tmp_node;
	}

	else
	{
		//index out of bounds or this node has no children
		return NULL;
	}
	
}

//--------------------------------------------------
//return the number of children that this node has
//--------------------------------------------------
int amis::dtb::smil::TimeContainerNode::NumChildren()
{
	return mNumChildren;
}

//--------------------------------------------------
/*!
	search for this Id in this node's subtree and setup the subtreetree 
	to play starting at that Id
*/
//--------------------------------------------------
bool amis::dtb::smil::TimeContainerNode::setAtId(string id)
{
	int i;
	int len = NumChildren();
	bool b_found = false;
	SeqNode* p_seq;

	Node* p_node;

	//recursive search for id
	for (i=0; i<len; i++)
	{
		p_node = getChild(i);

		//is this child the one with the id?
		if (p_node->getElementId().compare(id) == 0)
		{
			if (this->getTypeOfNode() == SEQ)
			{
				p_seq = (SeqNode*)this;
				p_seq->setChildIndex(i);
			}

			//if the id was found on a seq, set that seq's child index to 0
			if (p_node->getTypeOfNode() == SEQ)
			{
				((SeqNode*)getChild(i))->setChildIndex(0);
			}

			b_found = true;
		}

		//does this child have children with the id?
		else
		{
			if (p_node->getCategoryOfNode() == TIME_CONTAINER)
			{
				b_found = ((TimeContainerNode*)p_node)->setAtId(id);

				if(b_found == true && this->getTypeOfNode() == SEQ)
				{	
					p_seq = (SeqNode*)this;
					p_seq->setChildIndex(i);
				}
			}
		
		}

		if (b_found == true)
		{
			break;
		}
	}

	return b_found;
}

//--------------------------------------------------
//Get the category of this node
/*!
	category is TIME_CONTAINER
*/
//--------------------------------------------------
NodeType amis::dtb::smil::TimeContainerNode::getCategoryOfNode()
{
	return TIME_CONTAINER;
}

void amis::dtb::smil::TimeContainerNode::acceptDepthFirst(SmilVisitor* v)
{
	if (v->preVisit(this))
	{
		//read the children
		if (mpFirstChild != NULL)
		{
			mpFirstChild->acceptDepthFirst(v);
		}
	}
	if (this->getFirstSibling() != NULL)
	{
		this->getFirstSibling()->acceptDepthFirst(v);
	}
}
