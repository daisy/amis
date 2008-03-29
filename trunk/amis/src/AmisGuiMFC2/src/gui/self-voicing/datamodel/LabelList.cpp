#include "gui/self-voicing/datamodel/LabelList.h"
#include <fstream>

using namespace amis::gui::spoken;

LabelList::LabelList()
{
	mLabels.empty();
}
LabelList::~LabelList()
{
	int sz = mLabels.size();
	Label* p_label = NULL;

	for (int i=sz-1; i>=0; i--)
	{
		p_label = mLabels[i];
		mLabels.pop_back();

		if (p_label!=NULL)
			delete p_label;

		p_label = NULL;
	}
}

int LabelList::getNumberOfLabels()
{
	return mLabels.size();
}

Label* LabelList::getLabel(int idx)
{
	if (idx >= 0 && idx < mLabels.size())
	{
		return mLabels[idx];
	}
	else
	{
		return NULL;
	}
}

bool LabelList::doesListRepeat()
{
	return mbRepeat;
}

void LabelList::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i = 0;
	for (i = 0; i<numTabs; i++){out<<"\t";}
	out<<"List";
	
	if (mbRepeat == true)
		out<<"\t repeat = yes"<<endl;
	else
		out<<"\t repeat = no"<<endl;

	out.close();

	for (i=0; i<mLabels.size(); i++)
	{
		mLabels[i]->testPrint(outfile, numTabs + 1);
	}

}

void LabelList::setRepeat(bool repeat)
{
	mbRepeat = repeat;
}

void LabelList::addLabel(Label* pLabel)
{
	mLabels.push_back(pLabel);
}
