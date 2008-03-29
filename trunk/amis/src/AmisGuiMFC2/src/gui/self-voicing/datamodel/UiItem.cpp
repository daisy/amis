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

UiItem* UiItem::findUiItemInSwitch(string switchCondition) {
	
	//todo: the switch should be part of the children of a Container, it should not be here ! (the data model is wrong)
	Switch* zwitch;
	if ((zwitch = this->getSwitch()) != NULL) {

		UiItem* uiItem = NULL;
		if ((uiItem = zwitch->getAction(switchCondition)) != NULL) {
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

