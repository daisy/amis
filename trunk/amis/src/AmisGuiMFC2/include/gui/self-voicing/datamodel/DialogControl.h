#ifndef DIALOGCONTROL_H
#define DIALOGCONTROL_H

#include "gui/self-voicing/UiMediaTypes.h"
#include "gui/self-voicing/Prompt.h"
#include "UiItem.h"
#include "LabelList.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class DialogControl : public UiItem
{
public:
	DialogControl();
	~DialogControl();
	
	std::string getWidgetCategories();
	
	Label* getMnemonic();

	void testPrint(std::string, int);

protected:
	void setWidgetCategories(std::string);
		void setMnemonic(Label*);

private:
	std::string mWidgetCategories;
	Label* mMnemonic;

	friend class DataReader;

};
}}}
#endif