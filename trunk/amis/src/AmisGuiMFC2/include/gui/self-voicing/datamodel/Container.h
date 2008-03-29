#ifndef CONTAINER_H
#define CONTAINER_H


#include "UiItem.h"
#include "Action.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class Container : public UiItem
{
public:
	Container();
	~Container();

	int getNumChildItems();
	UiItem* getChildItem(int);

	UiItem* findUiItem(int mfcID, string switchCondition = ""); //overrides UiItem to look in children as well.
	Container* findContainer(string id);

	void testPrint(std::string, int);
	Label* getKeyboardAccelerator();
	Label* getMnemonic();

protected:
	void addChildItem(UiItem*);

	void setKeyboardAccelerator(Label*);
	void setMnemonic(Label*);

private:
	
	Label* mMnemonic;
	Label* mAccelerator;
	vector <UiItem*> mChildItems;

friend class DataReader;
};
}}}
#endif