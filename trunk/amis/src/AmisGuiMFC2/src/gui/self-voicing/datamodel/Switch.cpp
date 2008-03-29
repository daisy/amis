#include "gui/self-voicing/datamodel/Switch.h"

#include "../../AmisGuiMFC2/resource.h"

#include <fstream>

using namespace amis::gui::spoken;

Switch::Switch()
{
	mActions.empty();
	mLabels.empty();
}
Switch::~Switch()
{
	int sz; 
	int i = 0;

	Action* p_action = NULL;
	Label* p_label = NULL;

	for (i=mActions.size() - 1; i>=0; i--)
	{
		p_action = mActions[i];
		mActions.pop_back();

		if (p_action != NULL)
			delete p_action;

		p_action = NULL;
	}

	for (i=mLabels.size() - 1; i>=0; i--)
	{
		p_label = mLabels[i];
		mLabels.pop_back();

		if (p_label != NULL)
			delete p_label;

		p_label = NULL;
	}
}

SwitchType Switch::getSwitchType()
{
	return mSwitchType;
}

//switch actions
Action* Switch::getAction(std::string condition)
{
	Action* p_action = NULL;

	for (int i=0; i<mActions.size(); i++)
	{
		string str = mActions[i]->getCondition();
		if (str == condition)
		{
			p_action = mActions[i];
			break;
		}
	}

	return p_action;
}

					/*
UiItem* Switch::findUiItem(string switchCondition) {
	

			switch(getSwitchType()) {
				case ACTIONSWITCH: //SwitchType::

					//todo: this is UGLYYYY !! Should not be here !
					if (mfcID == ID_MENU_PLAYPAUSE) {
						
			bool b_BookIsPlaying = AmisAudio::InstanceOne()->isPlaying(); // BOOK Audio Player
			
			//bool b_GuiIsPlaying = AmisAudio::InstanceTwo()->isPlaying(); // GUI Audio Player
			Action* action = getAction((b_BookIsPlaying ? "canPause" : "canPlay"), mfcID);
Action* action = getAction(switchCondition);
						if (action != NULL) {
							return action;
						}
						
					}

					Action* action = getAction(switchCondition);
					if (action != NULL) {
						return action;
					}

					break;
				case CAPTIONSWITCH:
				case DESCRIPTIONSWITCH:
					Label* label = getLabel(switchCondition);
					if (label != NULL) {
						return label;
					}
					break;
			}
}

					*/
/*
Action* Switch::getAction(std::string condition, int mfcId)
{
	Action* p_action = NULL;

	for (int i=0; i<mActions.size(); i++)
	{
		if (mActions[i]->getCondition() == condition && mActions[i]->getMfcId() == mfcId)
		{
			p_action = mActions[i];
			break;
		}
	}

	return p_action;
}*/

//switch captions/descriptions
Label* Switch::getLabel(std::string condition)
{
	Label* p_label = NULL;

	for (int i=0; i<mLabels.size(); i++)
	{
		if (mLabels[i]->getCondition() == condition)
		{
			p_label = mLabels[i];
			break;
		}
	}

	return p_label;
}

void Switch::setSwitchType(SwitchType type)
{
	mSwitchType = type;
}

void Switch::addAction(Action* pAction)
{
	mActions.push_back(pAction);
}

void Switch::addLabel(Label* pLabel)
{
	mLabels.push_back(pLabel);
}

void Switch::testPrint(std::string outfile, int numTabs)
{
	fstream out;
	out.open(outfile.c_str(), ios::app | ios::out);

	int i=0;
	
	for (i=0; i<numTabs; i++){out<<"\t";}
	out<<"Switch"<<endl;

	out.close();

	if (mSwitchType == ACTIONSWITCH)
	{
		for (i=0; i<mActions.size(); i++)
		{
			mActions[i]->testPrint(outfile, numTabs + 1);
		}
	}
	else
	{
		for (i=0; i<mLabels.size(); i++)
		{
			mLabels[i]->testPrint(outfile, numTabs + 1);
		}
	}
}
