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


	virtual void testPrint(std::string, int) = 0;

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
