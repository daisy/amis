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

void Label::testPrint(std::string outfile, int numTabs, std::string prefix)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}

	if (prefix.size() > 0)
		out<<prefix;

	if (mLabelType == CAPTION)
		out<<"Caption"<<endl;
	else if(mLabelType == DESCRIPTION)
		out<<"Description"<<endl;

	for (i=0; i<numTabs + 1; i++){out<<"\t";}
	out<<"mfc id = "<<mMfcId<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"condition = "<<mCondition<<endl;

	out.close();

	if (mpContents != NULL)
		mpContents->testPrint(outfile, numTabs + 1);

	if (mpPrompt != NULL)
		mpPrompt->testPrint(outfile, numTabs + 1);
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
	