#include "gui/self-voicing/datamodel/Action.h"
#include <fstream>

using namespace amis::gui::spoken;

Action::Action()
{
	this->setUiItemType(ACTION);
	mImages.empty();
	mAccelerator = NULL;
	mMnemonic = NULL;
}
Action::~Action()
{
	ImageNodeSV* p_image = NULL;
	for (int i=mImages.size()-1; i>=0; i--)
	{
		p_image = mImages[i];
		mImages.pop_back();
		if (p_image != NULL)
			delete p_image;

		p_image = NULL;
	}
	mWidgetCategories.clear();
	mWidgetCategories.erase();
	mCondition.clear();
	mCondition.erase();
	if (mAccelerator) delete mAccelerator;
	if (mMnemonic) delete mMnemonic;
}
ImageNodeSV* Action::getImage(ImageType type)
{
	ImageNodeSV* p_node = NULL;

	for (int i=0; i<mImages.size(); i++)
	{
		if (mImages[i]->getImageType() == type)
		{
			p_node = mImages[i];
			break;
		}
	}

	return p_node;
}

std::string Action::getWidgetCategories()
{
	return mWidgetCategories;
}
Label* Action::getKeyboardAccelerator()
{
	return mAccelerator;
}
Label* Action::getMnemonic()
{
	return mMnemonic;
}

void Action::addImage(ImageNodeSV* pNode)
{
	mImages.push_back(pNode);
}

void Action::setWidgetCategories(std::string widgetCategories)
{
	this->mWidgetCategories.assign(widgetCategories);
}
void Action::setKeyboardAccelerator(Label* accelerator)
{
	this->mAccelerator = accelerator;
}
void Action::setMnemonic(Label* mnemonic)
{
	this->mMnemonic = mnemonic;
}

void Action::setCondition(std::string rule)
{
	mCondition.assign(rule);
}

std::string Action::getCondition()
{
	return mCondition;
}

void Action::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;
	int j=0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Action"<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"id = "<<this->getId()<<endl;
		
	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"mfc id = "<<this->getMfcId()<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"widget = "<<this->mWidgetCategories<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"accelerator = "<<this->mAccelerator<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"condition = "<<mCondition<<endl;

	for (i=0; i<numTabs+1; i++){out<<"\t";}
	out<<"Images"<<endl;

	for (i=0; i<mImages.size(); i++)
	{
		for (j=0; j<numTabs+2; j++){out<<"\t";}
		out<<"src = "<<mImages[i]->getSrc()<<endl;

		for (j=0; j<numTabs+2; j++){out<<"\t";}
		out<<"type = ";

		if (mImages[i]->getImageType() == SMALL_ICON)
		{
			out<<"small icon"<<endl;
		}
		else
		{
			out<<"large icon"<<endl;
		}
	}
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

	LabelList* p_list = NULL;
	p_list = this->getLabelList();
	if (p_list != NULL)
		p_list->testPrint(outfile, numTabs + 1);
	
}
