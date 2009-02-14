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


#include "gui/self-voicing/UiMediaTypes.h"
#include <fstream>

using namespace amis::gui::spoken;

//***********
//TextNode
TextNodeSV::TextNodeSV()
{
	mMnemonic = "";
}

TextNodeSV::~TextNodeSV()
{
	mMnemonic.clear();
	mMnemonic.erase();
}

std::string TextNodeSV::getMnemonic()
{
	return mMnemonic;
}
void TextNodeSV::setMnemonic(std::string mnemonic)
{
	mMnemonic.assign(mnemonic);
}

//just copy over the text string and the mnemonic
//not complete but will do for now
TextNodeSV* TextNodeSV::clone()
{
	TextNodeSV* p_new = new TextNodeSV();

	p_new->setMnemonic(this->getMnemonic());

	amis::TextNode::cloneContents(p_new);

	return p_new;
}

//***********
//ImageNode
ImageNodeSV::ImageNodeSV()
{
	mImageType = SMALL_ICON;
}
ImageNodeSV::~ImageNodeSV()
{
}
ImageType ImageNodeSV::getImageType()
{
	return mImageType;
}
void ImageNodeSV::setImageType(ImageType imgType)
{
	mImageType = imgType;
}
ImageNodeSV* ImageNodeSV::clone()
{
	ImageNodeSV* p_new = new ImageNodeSV();

	p_new->setImageType(this->getImageType());

	amis::ImageNode::cloneContents(p_new);

	return p_new;
}


//******************
//TextAudioPair

TextAudioPair::TextAudioPair()
{
	mpText = NULL;
	mpAudio = NULL;
	mpMediaGroupVersion = NULL;
}
TextAudioPair::~TextAudioPair()
{
	if (mpMediaGroupVersion != NULL)
		delete mpMediaGroupVersion;
	if (mpText != NULL)
		delete mpText;
	if (mpAudio != NULL)
		delete mpAudio;

}

TextNodeSV* TextAudioPair::getText()
{
	return mpText;
}

amis::AudioNode* TextAudioPair::getAudio()
{
	return mpAudio;
}

void TextAudioPair::setText(TextNodeSV* pNode)
{
	this->mpText = pNode;
}
void TextAudioPair::setAudio(amis::AudioNode* pNode)
{
	this->mpAudio = pNode;
}
TextAudioPair* TextAudioPair::clone()
{
	TextAudioPair* p_new = new TextAudioPair();
	
	amis::AudioNode* p_audio = (amis::AudioNode*)this->getAudio()->clone();
	TextNodeSV* p_text = this->getText()->clone();

	p_new->setAudio(p_audio);
	p_new->setText(p_text);

	return p_new;
}
amis::MediaGroup* TextAudioPair::getAsAmisMediaGroup()
{
	if (mpMediaGroupVersion != NULL)
		delete mpMediaGroupVersion;

	mpMediaGroupVersion = new amis::MediaGroup();

	mpMediaGroupVersion->setText(this->mpText);
	mpMediaGroupVersion->addAudioClip(this->mpAudio);

	return mpMediaGroupVersion;
}
