#ifndef ACTION_H
#define ACTION_H

#include "gui/self-voicing/UiMediaTypes.h"
#include "UiItem.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class Action : public UiItem
{
public:
	Action();
	~Action();
	ImageNodeSV* getImage(ImageType);
	
	std::string getWidgetCategories();

	Label* getMnemonic();
	Label* getKeyboardAccelerator();

	std::string getCondition();

	void testPrint(std::string, int);

protected:
	void addImage(ImageNodeSV*);
	void setWidgetCategories(std::string);

	void setKeyboardAccelerator(Label*);
	void setMnemonic(Label*);

	void setCondition(std::string);
	
private:
	std::vector<ImageNodeSV*> mImages;
	std::string mWidgetCategories;

	Label* mAccelerator;
	Label* mMnemonic;

	std::string mCondition;

	friend class DataReader;
};

}}}
#endif