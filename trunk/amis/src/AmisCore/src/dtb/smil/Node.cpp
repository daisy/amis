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


//SYSTEM INCLUDES
#include <iostream>

//PROJECT INCLUDES
#include "dtb/smil/SmilEngineConstants.h"
#include "dtb/smil/TimeContainerNode.h"
#include "dtb/smil/Node.h"

using namespace std;

//--------------------------------------------------
/*!
	the constructor initializes member variables
*/
//--------------------------------------------------
amis::dtb::smil::Node::Node()
{
	mElementId="";
	mpSibling = NULL;
	mpParent = NULL;
	mpSmilTree = NULL;
}

//--------------------------------------------------
/*!
	the destructor does nothing special
*/
//--------------------------------------------------
amis::dtb::smil::Node::~Node()
{
	//empty function
}

//--------------------------------------------------
//	Save a pointer to a SmilTree object.
/*!
	@pre
		This node object is part of that SmilTree
*/
//--------------------------------------------------
void amis::dtb::smil::Node::setSmilTreePtr(amis::dtb::smil::SmilTree* pTree)
{
	mpSmilTree = pTree;
}

//--------------------------------------------------
/*
	return a pointer to the smil tree
*/
//--------------------------------------------------
amis::dtb::smil::SmilTree* amis::dtb::smil::Node::getSmilTreePtr()
{
	return mpSmilTree;
}

//--------------------------------------------------
// Add a sibling to this node
/*!
	This is a recursive function that will add the new node at the 
	end of the sibling chain
*/
//--------------------------------------------------
void amis::dtb::smil::Node::addSibling(Node* pNode)
{
	//if-else block to see if this node's sibling pointer is null
	if (mpSibling == NULL)
	{ 
		//add the new node pointer as its sibling
		mpSibling = pNode;
		
		//set the new node's parent to be the same as this node's parent
		mpSibling->setParent(this->getParent());
	} 
	else
	{
		//recursive call to try and add the new node to the sibling of this node
		mpSibling->addSibling(pNode);
	} 
	//end if-else block to see if this node's sibling pointer is null

}


//--------------------------------------------------
// Set a pointer to this node's parent
//--------------------------------------------------
void amis::dtb::smil::Node::setParent(amis::dtb::smil::TimeContainerNode* pParent)
{
	mpParent = pParent;
}

//--------------------------------------------------
// Get a pointer to this node's parent
//--------------------------------------------------
amis::dtb::smil::TimeContainerNode* amis::dtb::smil::Node::getParent()
{
	return mpParent;
}

//--------------------------------------------------
//Geturn a pointer to this node's sibling
/*!
	each node has one sibling
*/
//--------------------------------------------------
amis::dtb::smil::Node* amis::dtb::smil::Node::getFirstSibling()
{
	return mpSibling;
}


//--------------------------------------------------
//Get this node's ID, as given in the Smil File
//--------------------------------------------------
string amis::dtb::smil::Node::getElementId()
{
	return mElementId;
}

//--------------------------------------------------
//	Set this node's Id.
/*!
	@pre
		Id exists for this node in the Smil File
*/
//--------------------------------------------------
void amis::dtb::smil::Node::setElementId(string newElementId)
{
	mElementId = newElementId;
}

//--------------------------------------------------
/*! 
	Print information about this node to the screen.
	@param[in] level
		the number of tabs to indent each line
*/
//--------------------------------------------------
void amis::dtb::smil::Node::print(int level)
{
	// loop counter
	int i;

	//Node type
	NodeType node_type;

	node_type = this->getTypeOfNode();

	// print out the correct number of tabs
	for (i = 0; i<level; i++)
	{
		cout<<"\t";
	}

	// print out this node's Id
	cout<<"ID: "<<mElementId<<endl;

	// print out the correct number of tabs
	for (i = 0; i<level; i++)
	{
		cout<<"\t";
	}

	// long "if, else if, else" block to check node_type and print the right statement
	if (node_type == A)
	{
		cout<<"Type: A"<<endl;
	}
	else if(node_type == SEQ)
	{
		cout<<"Type: SEQ"<<endl;
	}

	else if(node_type == PAR)
	{
		cout<<"Type: PAR"<<endl;
	}
	else if (node_type == TXT)
	{
		cout<<"Type: TXT"<<endl;
	}

	else if (node_type == AUD)
	{
		cout<<"Type: AUD"<<endl;
	}

	else if(node_type == IMG)
	{
		cout<<"Type: IMG"<<endl;
	}

	else
	{
		cout<<"Type: NONE"<<endl;
	}
	//end of long "if, else if, else" block to check node_type and print accordingly

	//print out an empty line
	cout<<endl;
}
