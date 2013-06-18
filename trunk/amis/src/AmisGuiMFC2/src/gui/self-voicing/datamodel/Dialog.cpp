/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Copyright (c) 2004 DAISY Consortium

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

void Dialog::addChildControl(DialogControl* pControl)
{
	mChildControls.push_back(pControl);
}

void Dialog::addPrompt(Prompt* pPrompt)
{
	mPrompts.push_back(pPrompt);
}
