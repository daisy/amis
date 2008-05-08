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

#include "gui/self-voicing/datamodel/Label.h"
#include <fstream>

using namespace amis::gui::spoken;

Label::Label()
{
	mpContents = NULL;
	mpPrompt = NULL;
}
Label::~Label()
{
	if (mpContents != NULL)
		delete mpContents;
	
	if (mpPrompt != NULL)
		delete mpPrompt;
}

TextAudioPair* Label::getContents()
{
	return mpContents;
}

int Label::getMfcId()
{
	return mMfcId;
}

Prompt* Label::getPrompt()
{
	return mpPrompt;
}

LabelType Label::getType()
{
	return mLabelType;
}

void Label::setType(LabelType type)
{
	mLabelType = type;
}

void Label::setCondition(std::string rule)
{
	mCondition.assign(rule);
}

std::string Label::getCondition()
{
	return mCondition;
}


void Label::setContents(TextAudioPair* pContents)
{
	if (mpContents != NULL)
		delete mpContents;

	mpContents = pContents;
}

//create the contents from minimal information

void Label::setMfcId(int mfcId) //std::string
{
	//mMfcId.assign(mfcId);
	mMfcId = mfcId;
}

void Label::setPrompt(Prompt* pPrompt)
{
	mpPrompt = pPrompt;
}


//return a copy of this
Label* Label::clone()
{
	Label* p_new = new Label();

	p_new->setType(this->mLabelType);
	p_new->setMfcId(this->mMfcId);
	p_new->setCondition(this->mCondition);
	
	if (this->getPrompt() != NULL)
	{
		Prompt* p_prompt = this->getPrompt()->clone();
		p_new->setPrompt(p_prompt);
	}

	if (this->getContents() != NULL)
	{
		TextAudioPair* p_pair = this->getContents()->clone();
		p_new->setContents(p_pair);
	}

	return p_new;

}
	