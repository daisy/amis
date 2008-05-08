/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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


#include "gui/self-voicing/datamodel/UiItem.h"
#include "gui/self-voicing/datamodel/Switch.h"

using namespace amis::gui::spoken;

UiItem::UiItem()
{
	mpDescription = NULL;
	mpCaption = NULL;
	mpSwitch = NULL;
	mpLabelList = NULL;
}
UiItem::~UiItem()
{
	if (mpCaption != NULL)
		delete mpCaption;
	if (mpDescription != NULL)
		delete mpDescription;
	if (mpSwitch != NULL)
		delete mpSwitch;
	if (mpLabelList != NULL)
		delete mpLabelList;

}

UiItem* UiItem::findUiItemInSwitch(string switchCondition) 
{
	
	//todo: data model improvement possible: the switch should be part of the children of a Container, it should not be here.
	Switch* zwitch;
	if ((zwitch = this->getSwitch()) != NULL)
	{

		UiItem* uiItem = NULL;
		if ((uiItem = zwitch->getAction(switchCondition)) != NULL)
		{
			return uiItem;
		}
	}
	return NULL;
}
Label* UiItem::getDescription()
{
	return mpDescription;
}

Label* UiItem::getCaption()
{
	return mpCaption;
}

int UiItem::getMfcId() //std::string
{
	return mMfcId;
}

std::string UiItem::getId()
{
	return mId;
}
UiItemType UiItem::getUiItemType()
{
	return mUiItemType;
}
Switch* UiItem::getSwitch()
{
	return mpSwitch;
}
LabelList* UiItem::getLabelList()
{
	return mpLabelList;
}
void UiItem::setSwitch(Switch* pSwitch)
{
	mpSwitch = pSwitch;
}
void UiItem::setDescription(Label* pDesc)
{
	mpDescription = pDesc;
}
void UiItem::setCaption(Label* pCaption)
{
	mpCaption = pCaption;
}

void UiItem::setMfcId(int mfcId)
{
	mMfcId = mfcId;
}

void UiItem::setId(std::string id)
{
	mId.assign(id);
}

void UiItem::setUiItemType(UiItemType itemType)
{
	mUiItemType = itemType;
}
void UiItem::setLabelList(LabelList* pList)
{
	mpLabelList = pList;
}

