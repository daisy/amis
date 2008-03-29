
#include "gui/self-voicing/Prompt.h"
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
	if (mpContents != NULL) {	
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
	if (!textString.empty()) {
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
	if (!textString.empty()) {
		p_text = new TextNodeSV();
		p_text->setTextString(textString);
		p_pair->setText(p_text);
	}

	amis::AudioNode* p_audio = NULL;
	if (!audioSrc.empty()) {
		p_audio = new amis::AudioNode();
		p_audio->setSrc(audioSrc);
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
		p_audio->setSrc(pMediaGroup->getAudio(0)->getSrc());
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
void PromptItem::testPrint(std::string outfile, int numTabs)
{	
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Prompt Item"<<endl;

	if (mRefId.size() > 0)
	{
		for (i=0; i<numTabs + 1; i++){out<<"\t";}
		out<<"ref id = "<<mRefId<<endl;
	}
	out.close();
	this->getContents()->testPrint(outfile, numTabs + 1);

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
void PromptVar::testPrint(std::string outfile, int numTabs)
{	
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Prompt Variable"<<endl;

	if (mName.size() > 0)
	{
		for (i=0; i<numTabs + 1; i++){out<<"\t";}
		out<<"Name = "<<mName<<endl;
	}

	if (mParam.size() > 0)
	{
		for (i=0; i<numTabs + 1; i++){out<<"\t";}
		out<<"Param = "<<mParam<<endl;
	}

	if (this->getContents() != NULL)
	{
		this->getContents()->testPrint(outfile, numTabs + 1);
	}

	out.close();


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
		if (p_item != NULL) {
			if (p_item->getPromptItemType() == PROMPT_VARIABLE) {
				PromptVar* p = (PromptVar*)p_item;
				delete p;
			} else {
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
void Prompt::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Prompt"<<endl;
	
	for (i=0; i<numTabs + 1; i++){out<<"\t";}
	out<<"id = "<<mId<<endl;

	for (i=0; i<numTabs + 1; i++){out<<"\t";}
	out<<"type = "<<mType<<endl;

	out.close();

	for(int j=0; j<mItems.size(); j++)
	{
		mItems[j]->testPrint(outfile, numTabs + 1);
	}
}
