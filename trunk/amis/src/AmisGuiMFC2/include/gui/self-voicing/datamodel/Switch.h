#ifndef SWITCH_H
#define SWITCH_H

#include "Action.h"
#include "Label.h"


namespace amis
{
namespace gui
{
namespace spoken
{
enum SwitchType {ACTIONSWITCH, CAPTIONSWITCH, DESCRIPTIONSWITCH};

class Switch
{
public:
	Switch();
	~Switch();

	SwitchType getSwitchType();

	//UiItem* findUiItem(string switchCondition);

	Action* getAction(std::string);
	//Action* getAction(std::string, int);

	Label* getLabel(std::string);

	void testPrint(std::string, int);

protected:
	void setSwitchType(SwitchType);
	void addAction(Action*);
	void addLabel(Label*);

private:
	vector <Action*> mActions;
	vector <Label*> mLabels;

	SwitchType mSwitchType;
	
	friend class DataReader;
};
}}}

#endif