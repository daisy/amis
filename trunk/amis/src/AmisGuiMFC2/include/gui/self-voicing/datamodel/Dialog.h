#ifndef DIALOG_H
#define DIALOG_H

#include "UiItem.h"
#include "gui/self-voicing/Prompt.h"
#include "DialogControl.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class Dialog : public UiItem
{
public:
	Dialog();
	~Dialog();

	int getNumChildControls();
	DialogControl* getChildControl(int);

	int getNumPrompts();
	Prompt* getPrompt(int);
	Prompt* getPrompt(std::string);
	DialogControl* findControl(int);
	
	void testPrint(std::string, int);

protected:
	void addChildControl(DialogControl*);
	void addPrompt(Prompt*);

private:
	vector <DialogControl*> mChildControls;
	vector <Prompt*> mPrompts;

friend class DataReader;
};
}}}
#endif