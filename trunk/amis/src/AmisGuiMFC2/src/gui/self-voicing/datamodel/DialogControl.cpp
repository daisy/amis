#include "gui/self-voicing/datamodel/DialogControl.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include <fstream>

using namespace amis::gui::spoken;

DialogControl::DialogControl()
{
	setUiItemType(CONTROL);
	
	mMnemonic = NULL;
}

DialogControl::~DialogControl()
{
	if (mMnemonic) delete  mMnemonic;
}

void DialogControl::setMnemonic(Label* mnemonic)
{
	this->mMnemonic = mnemonic;
}
Label* DialogControl::getMnemonic()
{
	return mMnemonic;
}
std::string DialogControl::getWidgetCategories()
{
	return mWidgetCategories;
}


void DialogControl::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i = 0;

	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Control"<<endl;

	for (i=0; i<numTabs + 1; i++){out<<"\t";}
	out<<"mfc id = "<<this->getMfcId()<<endl;

	for (i=0; i<numTabs + 1; i++){out<<"\t";}
	out<<"Widget type = "<<mWidgetCategories<<endl;

	out.close();

	if (this->getSwitch() != NULL)
	{
		this->getSwitch()->testPrint(outfile, numTabs + 1);
	}

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

void DialogControl::setWidgetCategories(std::string widgetCategories)
{
	mWidgetCategories.assign(widgetCategories);
}

