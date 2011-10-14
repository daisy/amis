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

#include <iostream> 

#include "dtb/nav/NavNode.h"
#include "dtb/nav/NavModel.h"
#include "util/FilePathTools.h"
#include "util/Log.h"
//for string transformations
#include <algorithm>

using namespace std;

amis::dtb::nav::NavNode::NavNode()
{
	mContent = "";
	mId = "";
	mPlayOrder = -1;
	mClass = "";
	mRel = "";
	mpLabel = NULL;
	mpNavModel = NULL;
}

amis::dtb::nav::NavNode::~NavNode()
{
//	cout<<"Deleting: "<<mId<<endl;
	if (mpLabel != NULL)
	{
		mpLabel->destroyContents();
		delete mpLabel;
	}
}

void amis::dtb::nav::NavNode::setNavModel(NavModel* pModel)
{
	mpNavModel = pModel;
	if (mId != "")
		mpNavModel->addIndex(this, mId);
	else
		amis::util::Log::Instance()->writeTrace("Warning: added nav model before adding ID to NavNode", "NavNode::setNavModel");
}

void amis::dtb::nav::NavNode::setLabel(amis::MediaGroup* pLabel)
{
	mpLabel = pLabel;
}

void amis::dtb::nav::NavNode::setContent(string content)
{
	if (getNavContainer()->getNavModel()->getAreFilenamesLowercase() == true)
	{
		//make the filename lowercase
		std::string filename = amis::util::FilePathTools::clearTarget(content);
		std::string target = amis::util::FilePathTools::getTarget(content);
		std::transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))tolower);
		mContent = filename + "#" + target;
	}
	else
	{
		mContent = content;
	}
}

void amis::dtb::nav::NavNode::setId(string id)
{
	 mId = id;
}

void amis::dtb::nav::NavNode::setPlayOrder(int playOrder)
{
	mPlayOrder = playOrder;
}

void amis::dtb::nav::NavNode::setClass(string classname)
{
	mClass = classname;
}


amis::MediaGroup* amis::dtb::nav::NavNode::getLabel()
{
	return mpLabel;
}

//this function returns a content URL and denotes the node as 'selected'
//the play order in the nav model is changed.
string amis::dtb::nav::NavNode::getContent()
{
	return mContent;
}

string amis::dtb::nav::NavNode::getId()
{
	return mId;
}

int amis::dtb::nav::NavNode::getPlayOrder()
{
	return mPlayOrder;
}

string amis::dtb::nav::NavNode::getClass()
{
	return mClass;
}

amis::dtb::nav::NavNode::TypeOfNode amis::dtb::nav::NavNode::getTypeOfNode()
{
	return mTypeOfNode;
}
amis::dtb::nav::NavContainer* amis::dtb::nav::NavNode::getNavContainer()
{
	return mpNavContainer;
}
void amis::dtb::nav::NavNode::setNavContainer(NavContainer* pContainer)
{
	mpNavContainer = pContainer;
}

//rel is used in Daisy 2 to give the volume number of a section
void amis::dtb::nav::NavNode::setRel(string rel)
{
	mRel = rel;
}
string amis::dtb::nav::NavNode::getRel()
{
	return mRel;
}
//--------------------------------------------------
/*! 
	Print information about this node to the screen.
	@param[in] level
		the number of tabs to indent each line
*/
//--------------------------------------------------
void amis::dtb::nav::NavNode::print(int level)
{
	// loop counter
	int i, j;
	
	// print out the correct number of tabs
	for (i = 0; i<level; i++) cout<<"\t";
	cout<<"ID: "<<mId<<endl;

	for (i = 0; i<level; i++) cout<<"\t";
	cout<<"CONTENT: "<<mContent<<endl;

	for (i=0; i<level; i++) cout<<"\t";
	cout<<"TEXT: "<<this->mpLabel->getText()->getTextString().c_str()<<endl;

	for (j=0; j<(int)this->mpLabel->getNumberOfAudioClips(); j++)
	{
		for (i=0; i<level; i++) cout<<"\t";
		cout<<"AUDIO: "<<this->mpLabel->getAudio(j)->getPath()<<" from "<<
			this->mpLabel->getAudio(j)->getClipBegin()<<" to "<<
			this->mpLabel->getAudio(j)->getClipEnd()<<endl;
	}
	

	for (i=0; i<level; i++) cout<<"\t";
	cout<<"ORDER: "<<this->getPlayOrder()<<endl;

	for (i=0; i<level; i++) cout<<"\t";
	cout<<"CLASS: "<<this->getClass()<<endl;

	cout<<endl;
}
