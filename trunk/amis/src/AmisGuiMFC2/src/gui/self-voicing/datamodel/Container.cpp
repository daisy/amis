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


#include "gui/self-voicing/datamodel/Container.h"
#include "gui/self-voicing/datamodel/Switch.h"
#include <fstream>

using namespace amis::gui::spoken;

Container::Container()
{
	mChildItems.empty();
	
	this->setUiItemType(CONTAINER);

	mAccelerator = NULL;
	
	mMnemonic = NULL;
}
Container::~Container()
{
	if (mAccelerator) delete  mAccelerator;
	if (mMnemonic) delete  mMnemonic;

	Action* p_action = NULL;
	Container* p_container = NULL;
	UiItem* p_item = NULL;

	for (int i=mChildItems.size()-1; i>=0; i--)
	{
		p_item = mChildItems[i];
		mChildItems.pop_back();
		if (p_item != NULL)
		{
			if (p_item->getUiItemType() == ACTION)
			{
				p_action = (Action*) p_item;

				if (p_action != NULL)
					delete p_action;

				p_action = NULL;
			}
			else if (p_item->getUiItemType() == CONTAINER)
			{
				p_container = (Container*) p_item;
				if (p_container != NULL)
					delete p_container;

				p_container = NULL;
			} else
			{				
				UiItemType debug = p_item->getUiItemType();
				int i = 0;
			}
		}
		p_item = NULL;
	}
}
Container* Container::findContainer(string id)
{
	for (int i=mChildItems.size()-1; i>=0; i--)
	{
		UiItem* uiItemZz = (UiItem*) mChildItems[i];


		switch(uiItemZz->getUiItemType()) {
			case CONTAINER: {
						
				if (uiItemZz->getId().compare(id) == 0)
				{
					return (Container*)uiItemZz;
				}
				Container* container = ((Container*)uiItemZz)->findContainer(id);
				if (container != NULL)
				{
					return container;
				}
				break;
							
							}
			case NONE: // todo: What is this type anyway ??
			case ACTION:
			case DIALOG:
			case CONTROL:
				break;
		}
	}
	return NULL;
}
UiItem* Container::findUiItem(int mfcID, string switchCondition)
{
	UiItem* uiItem = NULL;
	if (!switchCondition.empty())
	{
		uiItem = UiItem::findUiItemInSwitch(switchCondition); // Super class method (hack for SWITCH)
		if (uiItem != NULL)
		{
			return uiItem;
		}
	}
	for (int i=mChildItems.size()-1; i>=0; i--)
	{
		UiItem* uiItemZz = (UiItem*) mChildItems[i];

		if (uiItemZz->getMfcId() == mfcID)
		{
			return uiItemZz;
		}

		switch(uiItemZz->getUiItemType())
		{
			case CONTAINER:
				uiItem = ((Container*)uiItemZz)->findUiItem(mfcID, switchCondition);
				if (uiItem != NULL)
				{
					return uiItem;
				}
				break;
			case NONE: // todo: What is this type anyway ??
			case ACTION:
			case DIALOG:
			case CONTROL:
				if (!switchCondition.empty())
				{
					uiItem = uiItemZz->findUiItemInSwitch(switchCondition);
					if (uiItem != NULL) 
					{
						return uiItem;
					}
				}
				break;
		}
	}
	return NULL;
}

void Container::setKeyboardAccelerator(Label* accelerator)
{
	this->mAccelerator = accelerator;
}

void Container::setMnemonic(Label* mnemonic)
{
	this->mMnemonic = mnemonic;
}
Label* Container::getKeyboardAccelerator()
{
	return mAccelerator;
}

Label* Container::getMnemonic()
{
	return mMnemonic;
}

int Container::getNumChildItems()
{
	return mChildItems.size();
}
UiItem* Container::getChildItem(int idx)
{
	if (idx >=0 && idx<mChildItems.size())
	{
		return mChildItems[idx];
	}
	else
	{
		return NULL;
	}
}

void Container::addChildItem(UiItem* pItem)
{
	mChildItems.push_back(pItem);
}
