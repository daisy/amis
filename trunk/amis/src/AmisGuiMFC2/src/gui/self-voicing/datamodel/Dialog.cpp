
#include "gui/self-voicing/datamodel/Dialog.h"
#include <fstream>

using namespace amis::gui::spoken;

Dialog::Dialog()
{
	mChildControls.empty();
	mPrompts.empty();

	this->setUiItemType(DIALOG);
}

Dialog::~Dialog()
{
	//delete mChildControls
	//delete mPrompts

	int i = 0;

	DialogControl* p_control = NULL;
	Prompt* p_prompt = NULL;

	for (i = mChildControls.size() - 1; i>=0; i--)
	{
		p_control = mChildControls[i];
		mChildControls.pop_back();

		if (p_control != NULL)
			delete p_control;

		p_control = NULL;
	}

	for (i = mPrompts.size() - 1; i>=0; i--)
	{
		p_prompt = mPrompts[i];
		mPrompts.pop_back();

		if (p_prompt != NULL)
			delete p_prompt;

		p_prompt = NULL;
	}
}

int Dialog::getNumChildControls()
{
	return mChildControls.size();
}

DialogControl* Dialog::getChildControl(int idx)
{
	if (idx >= 0 && idx < mChildControls.size())
	{
		return mChildControls[idx];
	}
	else
	{
		return NULL;
	}
}
DialogControl* Dialog::findControl(int mfcid)
{
	DialogControl* p_obj = NULL;

	for (int i=0; i<mChildControls.size(); i++)
	{
		int id = mChildControls[i]->getMfcId();
		if (id == mfcid)
		{
			p_obj = mChildControls[i];
			break;
		}
	}

	return p_obj; 
}
int Dialog::getNumPrompts()
{
	return mPrompts.size();
}

Prompt* Dialog::getPrompt(int idx)
{
	if (idx >= 0 && idx < mPrompts.size())
	{
		return mPrompts[idx];
	}
	else
	{
		return NULL;
	}
}

Prompt* Dialog::getPrompt(std::string type)
{
	Prompt* p_prompt = NULL;

	for (int i=0; i<mPrompts.size(); i++)
	{
		if (mPrompts[i]->getType() == type)
		{
			p_prompt = mPrompts[i];
			break;
		}
	}

	return p_prompt;
}

void Dialog::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Dialog"<<endl;

	out.close();

	Label* p_label = NULL;
	p_label = this->getCaption();
	if (p_label!=NULL)
	{
		p_label->testPrint(outfile, numTabs+1);
	}

	p_label = NULL;
	p_label = this->getDescription();
	if (p_label != NULL)
	{
		p_label->testPrint(outfile, numTabs+1);
	}

	out.close();

	for (i=0; i<mChildControls.size(); i++)
	{
		mChildControls[i]->testPrint(outfile, numTabs + 1);
	}

	for (i=0; i<mPrompts.size(); i++)
	{
		mPrompts[i]->testPrint(outfile, numTabs + 1);
	}

}

void Dialog::addChildControl(DialogControl* pControl)
{
	mChildControls.push_back(pControl);
}

void Dialog::addPrompt(Prompt* pPrompt)
{
	mPrompts.push_back(pPrompt);
}
