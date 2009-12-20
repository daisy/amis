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
