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

#ifndef UIITEM_H
#define UIITEM_H

#include "gui/self-voicing/UiMediaTypes.h"
#include "Label.h"
#include "LabelList.h"


namespace amis
{
namespace gui
{
namespace spoken
{
enum UiItemType {NONE, CONTAINER, ACTION, DIALOG, CONTROL};

class Switch;

class UiItem
{
public:
	UiItem();
	~UiItem();

	UiItem* findUiItemInSwitch(string switchCondition); //to look in the Switch children

	Label* getDescription();
	Label* getCaption();

	int getMfcId(); //std::string
	std::string getId();
	UiItemType getUiItemType();

	Switch* getSwitch();
	LabelList* getLabelList();

	void setLabelList(LabelList*);
protected:
	void setDescription(Label*);
	void setCaption(Label*);
	void setMfcId(int); //std::string
	void setId(std::string);
	void setUiItemType(UiItemType);
	void setSwitch(Switch*);
	

	

private:
	Label* mpDescription;
	Label* mpCaption;
	//std::string mMfcId;
	int mMfcId;
	std::string mId;
	UiItemType mUiItemType;
	Switch* mpSwitch;

	//some ui items contain a list of items, populated at runtime
	//eg. list of all bookmarks or list of recently read books
	//or list of skippable structures
	LabelList* mpLabelList;

	
friend class DataReader;
};
}}}
#endif
