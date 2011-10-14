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
#include "dtb/smil/ContentNode.h"
#include "dtb/smil/SmilTree.h"

using namespace std;

amis::dtb::smil::ContentNode::ContentNode()
{
	mpMediaData = NULL;
	mNodeType = NONE;
}

amis::dtb::smil::ContentNode::~ContentNode()
{
//the calling appliation is expected to delete the media data when they're done with it
//	if (mNodeType != NONE && mpMediaData != NULL) delete mpMediaData;
}


NodeType amis::dtb::smil::ContentNode::getCategoryOfNode()
{
	return CONTENT;
}

NodeType amis::dtb::smil::ContentNode::getTypeOfNode()
{
	return mNodeType;
}

void amis::dtb::smil::ContentNode::createNewAudio()
{
	mpMediaData = new amis::AudioNode();
	mNodeType = AUD;	
}
void amis::dtb::smil::ContentNode::createNewImage()
{
	mpMediaData = new amis::ImageNode();
	mNodeType = IMG;	
}
void amis::dtb::smil::ContentNode::createNewText()
{
	mpMediaData = new amis::TextNode();
	mNodeType = TXT;	
}
amis::MediaNode* amis::dtb::smil::ContentNode::getMediaNode()
{
	return mpMediaData;
}
void amis::dtb::smil::ContentNode::play(amis::dtb::smil::SmilMediaGroup* pPlayGroup)
{
	if (mNodeType == AUD)
	{
		pPlayGroup->addAudioClip((amis::AudioNode*)this->getMediaNode());

	}

	else if (mNodeType == IMG)
	{
		pPlayGroup->setImage((amis::ImageNode*)this->getMediaNode());
	}
	else if (mNodeType == TXT)
	{
		pPlayGroup->setText((amis::TextNode*)this->getMediaNode());
	}

	//if the current id is blank, set it to the current media node
	if (this->getSmilTreePtr()->getCurrentId().compare("") == 0)
	{
		this->getSmilTreePtr()->setCurrentId(this->getElementId());
	}
}

void amis::dtb::smil::ContentNode::acceptDepthFirst(SmilVisitor* v)
{
	//content nodes don't have children, so just visit this node and then its siblings
	v->preVisit(this);

	if (this->getFirstSibling() != NULL)
		this->getFirstSibling()->acceptDepthFirst(v);
}
void amis::dtb::smil::ContentNode::acceptDepthFirstOnActivePath(SmilVisitor* v)
{
	v->preVisit(this);

	if (this->getFirstSibling() != NULL)
		this->getFirstSibling()->acceptDepthFirst(v);
}
