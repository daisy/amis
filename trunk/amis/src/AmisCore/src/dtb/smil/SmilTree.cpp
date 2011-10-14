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
#include <string>
#include <iostream>
#include <vector>

#include "dtb/smil/Node.h"
#include "dtb/smil/TimeContainerNode.h"
#include "dtb/smil/ContentNode.h"
#include "dtb/smil/ParNode.h"
#include "dtb/smil/SeqNode.h"

#include "dtb/smil/SmilTree.h"

using namespace std;

amis::dtb::smil::SmilTree::SmilTree()
{
	//initialize variables and clear the skip options list
	mpRoot = NULL;
	mpSkipOptions = NULL;
	mpEscapeNode = NULL;
	mbEscapeRequested = false;
	mbCouldEscape = false;
	mError.clear();
}

amis::dtb::smil::SmilTree::~SmilTree()
{
	//delete the root node
	if (mpRoot != NULL) delete mpRoot;
}
void amis::dtb::smil::SmilTree::setRoot(SeqNode* pNewRoot)
{
	mpRoot = pNewRoot;
}

amis::dtb::smil::Node* amis::dtb::smil::SmilTree::getRoot()
{
	return mpRoot;
}

amis::Error amis::dtb::smil::SmilTree::getError()
{
	return mError;
}
amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::firstPhrase()
{
	SmilMediaGroup* p_smil_media = NULL;
	mbCouldEscape = false;
	mError.clear();

	//call setFirst on the root
	if (mustSkipOrEscapeNode(mpRoot) == false)
	{
		mError = mpRoot->setFirst();
	
		//ensure it didn't raise an error
		if (mError.getCode() == amis::OK)
		{
			mCurrentId = "";
			//now that the tree's path is set up, execute "play"
			p_smil_media = new SmilMediaGroup();
			mpRoot->play(p_smil_media);
			p_smil_media->setId(this->mCurrentId);
			p_smil_media->setEscape(mbCouldEscape);
		}
	}

	return p_smil_media;
}
//--------------------------------------------------
//go to the last set of par nodes in the tree
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::lastPhrase()
{
	SmilMediaGroup* p_smil_media = NULL;
	mbCouldEscape = false;
	mError.clear();

	if (mustSkipOrEscapeNode(mpRoot) == false)
	{
		//call setLast on the root
		mError = mpRoot->setLast();
	
		if (mError.getCode() == amis::OK)
		{
			this->mCurrentId = "";
			//gather the playback data and send it to the Smil Engine
			p_smil_media = new SmilMediaGroup();
			mpRoot->play(p_smil_media);
			p_smil_media->setEscape(mbCouldEscape);
			p_smil_media->setId(this->mCurrentId);
		}
	}

	return p_smil_media;
}

//--------------------------------------------------
//go to the next set of par nodes in the smil tree
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::nextPhrase()
{
	mbCouldEscape = false;
	SmilMediaGroup* p_smil_media = NULL;
	mError.clear();

	if (isAtEnd(mpRoot))
	{
		mError.setCode(amis::AT_END);
		mError.setMessage("Request for next phrase failed because we are at the end of the smil tree");
	}
	//else, the tree is somewhere in the middle
	else
	{
		if (mustSkipOrEscapeNode(mpRoot) == false)
		{
			//call setNext on the root
			mError = mpRoot->setNext();
			if (mError.getCode() == amis::OK)
			{
				this->mCurrentId = "";
				p_smil_media = new SmilMediaGroup();
				mpRoot->play(p_smil_media);
				p_smil_media->setEscape(mbCouldEscape);
				p_smil_media->setId(mCurrentId);
			}
		}
	}
	return p_smil_media;
}

//--------------------------------------------------
//Go to the previous set of par nodes in the Smil Tree
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::previousPhrase()
{
	mbCouldEscape = false;
	SmilMediaGroup* p_smil_media = NULL;
	mError.clear();

	if (isAtBeginning(mpRoot))
	{
		mError.setCode(amis::AT_BEGINNING);
		mError.setMessage("Request for previous phrase failed because we are at the beginning of the smil tree");
	}
	//else, the tree is somewhere in the middle
	else
	{
		if (mustSkipOrEscapeNode(mpRoot) == false)
		{
			//call setPrevious() on the root
			mError = mpRoot->setPrevious();
			if (mError.getCode() == amis::OK)
			{
				this->mCurrentId = "";
				p_smil_media = new SmilMediaGroup();
				mpRoot->play(p_smil_media);
				p_smil_media->setId(this->mCurrentId);
				p_smil_media->setEscape(mbCouldEscape);
			}
		}
	}

	return p_smil_media;
}

amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::escapeCurrentStructure()
{
	SmilMediaGroup* p_smil_media = NULL;

	if (mbCouldEscape == true)
	{
		mbCouldEscape = false;
		mbEscapeRequested = true;
		p_smil_media = new SmilMediaGroup();
		p_smil_media = nextPhrase();
		p_smil_media->setEscape(mbCouldEscape);
		mbEscapeRequested = false;
	}
	else
	{
		mbCouldEscape = false;
		p_smil_media = nextPhrase();
		p_smil_media->setEscape(mbCouldEscape);
	}

	return p_smil_media;
}

bool amis::dtb::smil::SmilTree::isEmpty()
{
	//test the root for NULL
	if (mpRoot == NULL) return true;
	else return false;
}

//--------------------------------------------------
//print the tree
//--------------------------------------------------
void amis::dtb::smil::SmilTree::print()
{
	cout<<endl<<"TREE for *"<<this->mFilepath<<"*"<<endl;
	printNode(mpRoot, 0);
}

//--------------------------------------------------
/*!
	recursive function to print a node at a certain indentation level
*/
//--------------------------------------------------
void amis::dtb::smil::SmilTree::printNode(Node* pNode, int level)
{
	TimeContainerNode* p_time_container;
	int cnt;

	//print this node
	pNode->print(level);

	//if can have children
	if (pNode->getCategoryOfNode() == TIME_CONTAINER)
	{
		p_time_container = (TimeContainerNode*)pNode;
		
		//print the children
		for (cnt = 0; cnt < p_time_container->NumChildren(); cnt++)
			printNode(p_time_container->getChild(cnt), level + 1);
	}
}


//--------------------------------------------------
//assign skip options to the list
/*!
	this option list points to the list in the smil engine.  updates made there
	are automatically reflected here.
*/
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setCustomTestSet(amis::dtb::CustomTestSet* pSkipOptions)
{
	mpSkipOptions = pSkipOptions;
}


//--------------------------------------------------
/*!
	Based on the skip options and settings in the tree to which this node belongs, 
	should this node be played or not?
*/
//--------------------------------------------------
bool amis::dtb::smil::SmilTree::mustSkipOrEscapeNode(Node* pNode)
{
	int i;
	int len;
	bool return_value = false;
	string sys_skip_option;
	string node_skip_option;

	len = mpSkipOptions->getLength();

	//if this is not a time container (seq or par) then we cannot escape or skip it
	if (pNode->getCategoryOfNode() != TIME_CONTAINER)
	{
		return_value = false;
	}

	//if this is our escapable node, and we have been asked to escape the current structure
	else if(pNode == mpEscapeNode && mbEscapeRequested == true)
	{
		return_value = true;
	}
	else
	{
		node_skip_option = ((TimeContainerNode*)pNode)->getSkipOption();

		for (i = 0; i<len; i++)
		{	
			sys_skip_option = mpSkipOptions->getCustomTest(i)->getId();


			//if the id's match and willSkip is true, then this node must be skipped
			if (sys_skip_option.compare(node_skip_option) == 0 &&
				mpSkipOptions->getCustomTest(i)->getCurrentState() == false)
			{
				return_value = true;
				break;
			}
		} //end of for-loop through skip options
	}
	//test for a special case where this node isn't skippable, but its children are,
	//making it, effectively, skippable itself
	if (return_value == false && pNode->getCategoryOfNode() == TIME_CONTAINER)
	{
		return_value = true;
		for (int i = 0; i<((TimeContainerNode*)pNode)->NumChildren(); i++)
		{
			//make sure each child is not to be skipped or escaped
			if (mustSkipOrEscapeNode(((TimeContainerNode*)pNode)->getChild(i)) == false)
			{
				return_value = false;
				break;
			}
		}
	}

	return return_value;
}

//--------------------------------------------------
/*!
	Remember that this node is elegible for being escaped
*/
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setPotentialEscapeNode(Node* pNode)
{
	if (pNode == NULL)
	{
		mpEscapeNode = NULL;
	}
	else
	{
		//we can only set up a node for potential escapability if it is a seq or par
		if (pNode->getCategoryOfNode() == TIME_CONTAINER)
		{
			mpEscapeNode = (TimeContainerNode*)pNode;
		}
	}

}


//--------------------------------------------------
//Go to a specific Id and start playback there
//--------------------------------------------------
amis::dtb::smil::SmilMediaGroup* amis::dtb::smil::SmilTree::goToId(string id)
{
	SmilMediaGroup* p_smil_media = NULL;
	mError.clear();

	if (mpRoot->getElementId().compare(id) == 0)
	{
		p_smil_media = new SmilMediaGroup();
		p_smil_media = firstPhrase();
		p_smil_media->setEscape(mbCouldEscape);
	}
	else
	{
		if (mpRoot->setAtId(id) == true)
		{
			this->mCurrentId = "";
			p_smil_media = new SmilMediaGroup();
			mpRoot->play(p_smil_media);
			p_smil_media->setId(this->mCurrentId);
			p_smil_media->setEscape(mbCouldEscape);
		}
		else
		{
			mError.setCode(amis::NOT_FOUND);
			mError.setMessage("The id *" + id + "* was not found in " + getSmilFilePath());
		}
	}

	return p_smil_media;
}


//--------------------------------------------------
//Set the smil file path that is the source for this tree
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setSmilFilePath(string filepath)
{
	mFilepath = filepath;
}


//--------------------------------------------------
//Get the smil file path that is the source for this tree
//--------------------------------------------------
string amis::dtb::smil::SmilTree::getSmilFilePath()
{
	return mFilepath;
}

//--------------------------------------------------
/*!
	Get the current Id (the innermost nested id in the playback chain)
*/
//--------------------------------------------------
string amis::dtb::smil::SmilTree::getCurrentId()
{
	return mCurrentId;
}

//--------------------------------------------------
//Set the current Id
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setCurrentId(string id)
{
	mCurrentId = id;
}

//--------------------------------------------------
//Get the most recent text Id 
//--------------------------------------------------
string amis::dtb::smil::SmilTree::getRecentTextId()
{
	return mRecentTextId;
}

//--------------------------------------------------
//Set the most recent text Id
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setRecentTextId(string id)
{
	mRecentTextId = id;
}


//--------------------------------------------------
//return whether the current media data could be escaped
//--------------------------------------------------
bool amis::dtb::smil::SmilTree::getCouldEscape()
{
	return mbCouldEscape;
}

//--------------------------------------------------
//set whether the current media data could be escaped
//--------------------------------------------------
void amis::dtb::smil::SmilTree::setCouldEscape(bool escape)
{
	mbCouldEscape = escape;
}

bool amis::dtb::smil::SmilTree::isAtBeginning(amis::dtb::smil::Node* pNode)
{
	amis::dtb::smil::PositionVisitor v;
	return v.isAtBeginning(this);
}

bool amis::dtb::smil::SmilTree::isAtEnd(amis::dtb::smil::Node* pNode)
{
	amis::dtb::smil::PositionVisitor v;
	return v.isAtEnd(this);
}

void amis::dtb::smil::SmilTree::acceptDepthFirst(SmilVisitor* v)
{
	mpRoot->acceptDepthFirst(v);
}

void amis::dtb::smil::SmilTree::acceptDepthFirstOnActivePath(SmilVisitor* v)
{
	mpRoot->acceptDepthFirstOnActivePath(v);
}
