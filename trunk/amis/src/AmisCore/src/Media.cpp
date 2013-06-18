/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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
//Amis media objects implementation

#include "Media.h"
#include <iostream>
using namespace std;

/*
MediaNode class
*/
amis::MediaNode::MediaNode()
{
	mId = "";
	mSrc = "";
	mHref = "";
	mLangCode = "";
}

amis::MediaNode::~MediaNode()
{
	mId.clear();
	mId.erase();
	mSrc.clear();
	mSrc.erase();
	mHref.clear();
	mHref.erase();
	mLangCode.clear();
	mLangCode.erase();
}

void amis::MediaNode::cloneContents(MediaNode* p_new)
{
	p_new->setHref(this->getHref());
	p_new->setId(this->getId());
	p_new->setLangCode(this->getLangCode());
	p_new->setMediaNodeType(this->getMediaNodeType());
	p_new->setPath(this->getPath());
}
void amis::MediaNode::setId(string id)
{
	mId.assign(id);
}

void amis::MediaNode::setPath(string src)
{
	mSrc.assign(src);
}

void amis::MediaNode::setHref(string href)
{
	mHref.assign(href);
}

void amis::MediaNode::setLangCode(string langcode)
{
	mLangCode.assign(langcode);
}

string amis::MediaNode::getId()
{
	return mId;
}

string amis::MediaNode::getPath()
{
	return mSrc;
}

string amis::MediaNode::getHref()
{
	return mHref;
}

string amis::MediaNode::getLangCode()
{
	return mLangCode;
}

amis::MediaNodeType amis::MediaNode::getMediaNodeType()
{
	return mMediaNodeType;
}

void amis::MediaNode::setMediaNodeType(MediaNodeType nodeType)
{
	mMediaNodeType = nodeType;
}

void amis::MediaNode::print(bool verbose)
{
	if (verbose == true)
	{
		if (mMediaNodeType == AUDIO)
			cout<<"audio node:";
		else if (mMediaNodeType == IMAGE)
			cout<<"image node:";
		else if (mMediaNodeType == TEXT)
			cout<<"text node:";
		
		cout<<endl;
	}
	cout<<"\tID="<<mId<<endl;
	cout<<"\tSRC="<<mSrc<<endl;
	if (mMediaNodeType == TEXT)
	{
		amis::TextNode* t = (amis::TextNode*)this;
		if (t->getTextString().length() > 0)
		{
 			cout<<"\tSTRING=";
			wcout<<t->getTextString()<<endl;
		}
	}
	cout<<endl;
}
/*
TextNode class
*/
amis::TextNode::TextNode()
{
	setMediaNodeType(amis::TEXT);
	mLangDir = amis::LTR;
	mText.erase();
}

amis::TextNode::~TextNode()
{
//	mText.clear();
//	mText.erase();
}

void amis::TextNode::cloneContents(TextNode* p_new)
{
	MediaNode::cloneContents(p_new);
	p_new->setTextString(this->getTextString());
	p_new->setLangDir(this->getLangDir());
}
void amis::TextNode::setTextString(wstring text)
{
	mText.assign(text);
}

void amis::TextNode::setLangDir(TextDirection direction)
{
	mLangDir = direction;
}

wstring amis::TextNode::getTextString()
{
	return mText;
}

amis::TextDirection amis::TextNode::getLangDir()
{
	return mLangDir;
}
amis::TextNode* amis::TextNode::clone()
{
	amis::TextNode* p_new = new amis::TextNode();

	this->cloneContents(p_new);

	return p_new;
}


/*
AudioNode class
*/

amis::AudioNode::AudioNode()
{
	setMediaNodeType(amis::AUDIO);
	mClipBegin = "";
	mClipEnd = "";
}

amis::AudioNode::~AudioNode()
{
	mClipBegin.clear();
	mClipBegin.erase();
	
	mClipEnd.clear();
	mClipEnd.erase();
}

void amis::AudioNode::setClipBegin(const string clipBegin)
{
	mClipBegin.assign(clipBegin);
}

void amis::AudioNode::setClipEnd(const string clipEnd)
{
	mClipEnd.assign(clipEnd);
}

string amis::AudioNode::getClipBegin()
{
	return mClipBegin;
}

string amis::AudioNode::getClipEnd()
{
	return mClipEnd;
}
amis::AudioNode* amis::AudioNode::clone()
{
	amis::AudioNode* p_new = new amis::AudioNode();

	p_new->setClipBegin(this->getClipBegin());
	p_new->setClipEnd(this->getClipEnd());

	MediaNode::cloneContents(p_new);

	return p_new;
}
void amis::AudioNode::print()
{
	cout<<"audio node"<<endl;
	cout<<"\tfrom"<<getClipBegin()<<" to "<<getClipEnd()<<endl;

	amis::MediaNode::print(false);
}

/*
ImageNode class
*/


amis::ImageNode::ImageNode()
{
	setMediaNodeType(amis::IMAGE);
}



amis::ImageNode::~ImageNode()
{
}

void amis::ImageNode::cloneContents(ImageNode* p_new)
{
	MediaNode::cloneContents(p_new);
}
amis::ImageNode* amis::ImageNode::clone()
{
	amis::ImageNode* p_new = new amis::ImageNode();
	MediaNode::cloneContents(p_new);
	return p_new;
}

/*
MediaGroup class
*/


amis::MediaGroup::MediaGroup()
{
	mpTextNode = NULL;
	mpImageNode = NULL;
	mpAudioNodes.clear();
}


/*!
	destructor does not delete the pointers as they may be referenced elsewhere
*/

amis::MediaGroup::~MediaGroup()
{
	mpAudioNodes.clear();
}


/*!
	important to call this function if MediaGroup is being deleted and you want 
	the nodes it references deleted as well

	@warning
		this deletes the pointers, so use carefully!
*/

void amis::MediaGroup::destroyContents()
{
	if (mpTextNode != NULL)
		delete mpTextNode;

	if (mpImageNode != NULL)
		delete mpImageNode;

	AudioNode* tmp_ptr = NULL;
	unsigned int sz = mpAudioNodes.size();

	for (int i=sz-1; i>=0; i--)
	{
		tmp_ptr = NULL;
		tmp_ptr = mpAudioNodes[i];
		mpAudioNodes.pop_back();
		if (tmp_ptr != NULL)
		{
			delete tmp_ptr;
		}
	}

}

void amis::MediaGroup::setText(TextNode* pTextNode)
{
	mpTextNode = pTextNode;
}

void amis::MediaGroup::setImage(ImageNode* pImageNode)
{
	mpImageNode = pImageNode;
}

void amis::MediaGroup::addAudioClip(AudioNode* pAudioNode)
{
	if (pAudioNode != NULL)
		mpAudioNodes.push_back(pAudioNode);
}

bool amis::MediaGroup::hasText()
{
	if (mpTextNode != NULL)
		return true;
	else
		return false;
}

bool amis::MediaGroup::hasAudio()
{
	if (mpAudioNodes.size() != 0)
		return true;
	else
		return false;
}

bool amis::MediaGroup::hasImage()
{
	if (mpImageNode != NULL)
		return true;
	else
		return false;
}

amis::TextNode* amis::MediaGroup::getText()
{
	return mpTextNode;
}

amis::ImageNode* amis::MediaGroup::getImage()
{
	return mpImageNode;
}

amis::AudioNode* amis::MediaGroup::getAudio(unsigned int index)
{
	if (index < mpAudioNodes.size())
		return mpAudioNodes[index];
	else
		return NULL;
}

unsigned int amis::MediaGroup::getNumberOfAudioClips()
{
	return mpAudioNodes.size();
}

void amis::MediaGroup::setId(string id)
{
	mId.assign(id);
}

string amis::MediaGroup::getId()
{
	return mId;
}

void amis::MediaGroup::print()
{
	cout<<"Media group ["<<mId<<"]"<<endl;

	if (this->hasText() == true)
		this->mpTextNode->print();
	
	if (this->hasImage() == true)
		this->mpImageNode->print();
	
	for (int i = 0; i<mpAudioNodes.size(); i++)
		mpAudioNodes[i]->print();

	cout<<endl;
}
