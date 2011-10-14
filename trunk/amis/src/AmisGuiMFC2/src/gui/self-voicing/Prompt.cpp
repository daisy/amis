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


#include "gui/self-voicing/Prompt.h"
#include "Preferences.h"
#include <fstream>

using namespace amis::gui::spoken;

//**
//PromptItemBase
PromptItemBase::PromptItemBase()
{
	mpContents = NULL;
}
PromptItemBase::~PromptItemBase()
{
	if (mpContents != NULL)
	{	
		delete mpContents;
	}
}
TextAudioPair* PromptItemBase::getContents()
{
	return mpContents;
}

PromptItemType PromptItemBase::getPromptItemType()
{
	return mType;
}
void PromptItemBase::setPromptItemType(PromptItemType type)
{
	mType = type;
}

void PromptItemBase::setContents(TextAudioPair* pContents)
{
	if (mpContents != NULL)
		delete mpContents;

	mpContents = pContents;
}

void PromptItemBase::setContents(std::wstring textString, amis::AudioNode* p_audio)
{
	if (mpContents != NULL)
		delete mpContents;

	TextAudioPair* p_pair = new TextAudioPair();

	TextNodeSV* p_text = NULL;
	if (!textString.empty())
	{
		p_text = new TextNodeSV();
		p_text->setTextString(textString);
		p_pair->setText(p_text);
	}

	p_pair->setAudio(p_audio);

	this->mpContents = p_pair;

}
void PromptItemBase::setContents(std::wstring textString, std::string audioSrc)
{
	if (mpContents != NULL)
		delete mpContents;

	TextAudioPair* p_pair = new TextAudioPair();

	TextNodeSV* p_text = NULL;
	if (!textString.empty())
	{
		p_text = new TextNodeSV();
		p_text->setTextString(textString);
		p_pair->setText(p_text);
	}

	amis::AudioNode* p_audio = NULL;
	if (!audioSrc.empty())
	{
		p_audio = new amis::AudioNode();

		ambulant::net::url audio_src = ambulant::net::url::from_filename(audioSrc);
		amis::ModuleDescData* p_langpack_data = amis::Preferences::Instance()->getCurrentLanguageData();
		audio_src = audio_src.join_to_base(*p_langpack_data->getXmlFileName());

		p_audio->setPath(audio_src.get_file());
		p_pair->setAudio(p_audio);
	}

	this->mpContents = p_pair;

}

void PromptItemBase::setContents(amis::MediaGroup* pMediaGroup)
{
	if (mpContents != NULL)
		delete mpContents;

	mpContents = new TextAudioPair();
	
	if (pMediaGroup->hasText() == true)
	{
		TextNodeSV* p_text = new TextNodeSV();
		p_text->setTextString(pMediaGroup->getText()->getTextString());
		mpContents->setText(p_text);
	}

	if (pMediaGroup->hasAudio() == true)
	{
		amis::AudioNode* p_audio = new amis::AudioNode();
		p_audio->setPath(pMediaGroup->getAudio(0)->getPath());
		p_audio->setClipBegin(pMediaGroup->getAudio(0)->getClipBegin());
		p_audio->setClipEnd(pMediaGroup->getAudio(0)->getClipEnd());

		mpContents->setAudio(p_audio);
	}
}

//**
//PromptItem
PromptItem::PromptItem()
{
	mRefId="";
	mId = "";
	setPromptItemType(PROMPT_ITEM);
}
PromptItem::~PromptItem()
{
	mRefId.clear();
	mRefId.erase();
	mId.clear();
	mId.erase();
}

std::string PromptItem::getRefId()
{
	return mRefId;
}
std::string PromptItem::getId()
{
	return mId;
}
void PromptItem::setRefId(std::string refId)
{
	mRefId.assign(refId);
}
void PromptItem::setId(std::string id)
{
	mId.assign(id);
}

//make a hardcopy of the contents pointed to by refid
void PromptItem::setReferenceContents(PromptItem* pItem)
{
	if (pItem != NULL)
	{
		TextAudioPair* p_pair = NULL;
		p_pair = pItem->getContents();
		if (p_pair != NULL)
		{
			TextAudioPair* p_these_contents = new TextAudioPair();

			if (p_pair->getText() != NULL)
			{
				p_these_contents->setText(p_pair->getText()->clone());
			}
			if (p_pair->getAudio() != NULL)
			{
				p_these_contents->setAudio((amis::AudioNode*)p_pair->getAudio()->clone());
			}

			this->setContents(p_these_contents);
		}
	}

}
PromptItem* PromptItem::clone()
{
	PromptItem* p_new = new PromptItem();
	p_new->setPromptItemType(this->getPromptItemType());
	p_new->setRefId(this->mRefId);
	
	TextAudioPair* p_pair = this->getContents()->clone();

	p_new->setContents(p_pair);

	return p_new;

}


//**
//PromptVar
PromptVar::PromptVar()
{
	mParam = "";
	mName = "";

	setPromptItemType(PROMPT_VARIABLE);
}
PromptVar::~PromptVar()
{
	mParam.clear();
	mParam.erase();
	mName.clear();
	mName.erase();
	clearSubItems();
}

std::string PromptVar::getParam()
{
	return mParam;
}

std::string PromptVar::getName()
{
	return mName;
}
void PromptVar::setParam(std::string param)
{
	mParam.assign(param);
}
void PromptVar::setName(std::string name)
{
	mName.assign(name);
}

void PromptVar::appendSubItem(PromptItem* pi)
{
	mSubItems.push_back(pi);
}

int PromptVar::subItemsCount()
{
	int sz = mSubItems.size();
	return sz;
}

PromptItem * PromptVar::getSubItem(int idx)
{
	return mSubItems[idx];
}

void PromptVar::clearSubItems()
{
	int sz = mSubItems.size();
	for (int i = 0; i < sz; i++)
	{
		PromptItem * pi = mSubItems[i];
		delete pi;
	}
	mSubItems.clear();
	mSubItems.empty();
}

bool PromptVar::hasSubItems()
{
	return mSubItems.size() != 0;
}

//prompt variables don't copy their contents
//since that changes so often
PromptVar* PromptVar::clone()
{
	PromptVar* p_new = new PromptVar();
	p_new->setName (this->mName);
	p_new->setParam (this->mParam);
	p_new->setPromptItemType(this->getPromptItemType());

	return p_new;

}
//**
//Prompt
Prompt::Prompt()
{
	mItems.empty();
	mType.clear();
	mId.clear();
}
Prompt::~Prompt()
{
	mType.clear();
	mId.clear();
	
	mType.erase();
	mId.erase();

	PromptItemBase* p_item = NULL;

	for (int i=mItems.size()-1; i>=0; i--)
	{
		p_item = mItems[i];
		mItems.pop_back();
		if (p_item != NULL)
		{
			if (p_item->getPromptItemType() == PROMPT_VARIABLE)
			{
				PromptVar* p = (PromptVar*)p_item;
				delete p;
			}
			else
			{
				PromptItem* p = (PromptItem*)p_item;
				delete p;
			}
		}
		p_item = NULL;
	}
}
int Prompt::getNumberOfItems()
{
	return mItems.size();
}
PromptItemBase* Prompt::getItem(int idx)
{
	if (idx >=0 && idx < mItems.size())
	{
		return mItems[idx];
	}
	else
	{
		return NULL;
	}
}

/*
void Prompt::swapItem(int idx, PromptItemBase* pItem)
{
    std::vector<PromptItemBase*>::iterator it = mItems.begin() + idx;
    it = mItems.insert(it, pItem);
	it = mItems.begin() + idx + 1;
    mItems.erase(it);
}
void Prompt::insertItem(int idx, PromptItemBase* pItem)
{
    std::vector<PromptItemBase*>::iterator it = mItems.begin() + idx;
    it = mItems.insert(it, pItem);
}
*/

void Prompt::addItem(PromptItemBase* pItem)
{
	mItems.push_back(pItem);
}
std::string Prompt::getType()
{
	return mType;
}
void Prompt::setType(std::string type)
{
	mType.assign(type);
}

std::string Prompt::getId()
{
	return mId;
}
void Prompt::setId(std::string id)
{
	mId.assign(id);
}

//return a string containing all the promptItem texts
//if the prompt has unresolved variables, insert them as the variable name 
//(good for testing)
std::wstring Prompt::concatAllText()
{
	std::wstring ret_string;
	ret_string.erase();

	PromptVar* p_var = NULL;
	TextAudioPair* p_contents = NULL;
	PromptItemBase* p_base = NULL;

	int sz = mItems.size();
	for (int i=0; i<sz; i++)
	{
		p_base = mItems[i];

		if (p_base->getPromptItemType() == PROMPT_VARIABLE &&
			p_base->getContents() == NULL)
		{
			p_var = (PromptVar*)p_base;
			//@todo
			//this variable was not resolved, so append its name to the list
			//convert std::string to std::wstring: can we do this here with no MFC dependencies?
			//ret_string.append(p_var->getName());
		}
		else
		{
			p_contents = p_base->getContents();
			if (p_contents != NULL)
			{
				if (p_contents->getText() != NULL)
				{
					ret_string.append(p_contents->getText()->getTextString());
				}
			}
			else
			{
				std::string sz_err = " #ERROR# ";
				//@todo
				//convert std::string to std::wstring: can we do this here with no MFC dependencies?
				//ret_string.append(sz_err.c_str());
			}
		}
	}

	return ret_string;
}

Prompt* Prompt::clone()
{
	Prompt* p_new = new Prompt();
	p_new->setId(this->mId);
	p_new->setType(this->mType);

	PromptItem* p_item;
	PromptVar* p_var;
	
	for (int i=0; i<this->mItems.size(); i++)
	{
		if (mItems[i]->getPromptItemType() == PROMPT_ITEM)
		{
			p_item = (PromptItem*)mItems[i];
			p_new->addItem(p_item->clone());
		}
		else if (mItems[i]->getPromptItemType() == PROMPT_VARIABLE)
		{
			p_var = (PromptVar*)mItems[i];
			p_new->addItem(p_var->clone());
		}
	}

	return p_new;
}
