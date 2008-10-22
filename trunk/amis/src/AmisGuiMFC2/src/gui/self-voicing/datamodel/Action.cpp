/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

