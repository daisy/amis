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
#include "dtb/smil/SmilTree.h"
#include "dtb/smil/Node.h"
#include "dtb/smil/SeqNode.h"
#include "dtb/smil/ParNode.h"

using namespace std;

//--------------------------------------------------
//Default constructor
//--------------------------------------------------
amis::dtb::smil::ParNode::ParNode()
{
	//empty function
}

//--------------------------------------------------
//Destructor
//--------------------------------------------------
amis::dtb::smil::ParNode::~ParNode()
{
	//empty function
} 

//--------------------------------------------------
/*!
	this node has type PAR
*/
//--------------------------------------------------
NodeType amis::dtb::smil::ParNode::getTypeOfNode()
{
	return PAR;
}

//--------------------------------------------------
/*!
	invoke play on all child nodes and pass them a pointer to the 
	SmilMediaGroup which is being passed around the tree.

  	@param[out] pPlayGroup
		The object pointed to by this parameter will have added to it
		additional playback data

	@param[in] pPlayGroup
		Pointer to an initialized object which represents all the nodes in 
		this playback cluster
*/
//--------------------------------------------------
//this function takes a partially complete MediaGroup as input
//it adds data to this group
void amis::dtb::smil::ParNode::play(SmilMediaGroup* pPlayGroup)
{
	//local variables
	int i;
	int len = NumChildren();
	

	//remember this Id information because it helps us find the last good position
	if (getElementId().compare("") != 0)
	{
		getSmilTreePtr()->setCurrentId(getElementId());
	}

	//check if this node could be escaped, then make note of this
	if (getSkipOption().compare("") != 0)
	{
		getSmilTreePtr()->setCouldEscape(true);
		getSmilTreePtr()->setPotentialEscapeNode(this);
	}//end escape check

	//for-loop through each child node and collect and aggragate playback information for each
	for (i = 0; i<len; i++)
	{
		//make sure we aren't skipping this child node
		if (getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
		{
			//add this child's output string the local string variable
			getChild(i)->play(pPlayGroup);
		}
	}//end of for-loop

}

//--------------------------------------------------
/*!
	this function searches for a seq node child (of this par node) 
	and calls its setNext function
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::ParNode::setNext()
{
	//local variables
	int i;
	amis::Error return_value(amis::AT_END, "");
	int len = NumChildren();

	//just support pars with one seq
	//d202 technically allows more than one seq although this is not seen in practice
	//d3 forbids having more than one seq directly nested under a par node

	//for-loop through this par node's children and look for a 
	//seq node to call setNext() on
	for (i = 0; i<len; i++)
	{
		//if the child is a seq and we are not skipping it
		if (getChild(i)->getTypeOfNode() == SEQ &&
			getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
		{
			//tell this seq node to advance to the next child
			//if everything is okay, the seq node function call will return OK.
			//it will return LIST_END if it cannot advance further, in which case we will
			//also return LIST_END from this function
			return_value = ((SeqNode*)getChild(i))->setNext();
			break;
		}
	}

	//return either OK or LIST_END
	return return_value;
} 

//--------------------------------------------------
/*!
	this function searches for a seq node child (of this par node), 
	and calls its setPrevious function
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::ParNode::setPrevious()
{
	//local variables
	int i;
	amis::Error return_value(amis::AT_BEGINNING, "");
	int len = NumChildren();

	//just support pars with one seq
	//d202 technically allows more than one seq
	//but d3 forbids it

	//for-loop through this par node's children and look for a 
	//seq node to call setPrevious() on
	for (i = 0; i<len; i++)
	{
		//if the child is a seq and we are not skipping it
		if (getChild(i)->getTypeOfNode() == SEQ &&
			getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
		{
			//tell this seq node to advance to the next child
			//if everything is okay, the seq node function call will return OK.
			//it will return LIST_BEGIN if it cannot advance further, in which case we will
			//also return LIST_BEGIN from this function
			return_value = ((SeqNode*)getChild(i))->setPrevious();
			break;
		}
	}

	//return either OK or LIST_BEGIN
	return return_value;
}

//--------------------------------------------------
/*!
	this function searches for a seq node child and calls its setFirst function
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::ParNode::setFirst()
{
	//local variables
	int i;
	int len = NumChildren();
	amis::Error return_value;

	//for-loop through this par node's children to search for a seq node
	for (i = 0; i<len; i++)
	{
		//if the child is a seq and we are not skipping it, then tell it 'setFirst'
		if (getChild(i)->getTypeOfNode() == SEQ && 
			getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
		{
			return_value = ((SeqNode*)getChild(i))->setFirst();
		}

	}//end for-loop

	return return_value;
}
//--------------------------------------------------
/*!
	this function searches for a seq node child and calls its setLast function
*/
//--------------------------------------------------
amis::Error amis::dtb::smil::ParNode::setLast()
{
	//local variables
	int i;
	int len = NumChildren();
	amis::Error return_value;

	//for-loop through this par node's children to search for a seq node
	for (i = 0; i<len; i++)
	{
		//if the child is a seq and we are not skipping it, then tell it 'setLast'
		if (getChild(i)->getTypeOfNode() == SEQ && 
			getSmilTreePtr()->mustSkipOrEscapeNode(getChild(i)) == false)
		{
			return_value = ((SeqNode*)getChild(i))->setLast();
		}
	}//end for-loop

	return return_value;
}
void amis::dtb::smil::ParNode::acceptDepthFirstOnActivePath(SmilVisitor* v)
{
	if (v->preVisit(this))
	{
		//visit all children since this is a par node
		Node* p_node = this->getChild(0);
		while (p_node != NULL)
		{
			p_node->acceptDepthFirstOnActivePath(v);
			p_node = p_node->getFirstSibling();
		}
	}
}
