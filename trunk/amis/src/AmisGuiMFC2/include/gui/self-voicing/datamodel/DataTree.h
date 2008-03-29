#ifndef DATATREE_H
#define DATATREE_H

#include "Container.h"
#include "Dialog.h"
#include "Switch.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class DataTree
{
protected:
	DataTree();
	
public:
	static DataTree* Instance();
	void DestroyInstance();
	~DataTree();

	Container* getContainer(int);
	int getNumContainers();
	Container* findContainer(std::string);
	UiItem* findUiItemInContainers(int mfcID, string switchCondition = "");
	
	Dialog* getDialog(int);
	int getNumDialogs();
	Dialog* findDialog(int);

	Prompt* findPrompt(std::string);
	PromptItem* findPromptItem(std::string);
	int getNumPrompts();
	int getNumPromptItems();
	Prompt* getPrompt(int);
	PromptItem* getPromptItem(int);

	void testPrint(std::string);

protected:
	void addContainer(Container*);
	void addDialog(Dialog*);

	void addPrompt(Prompt*);
	void addPromptItem(PromptItem*);

	void addForResolution(PromptItem*);
	void resolvePromptItemsWithRefIds();

private:
	vector <Container*> mContainersList;
	vector <Dialog*> mDialogsList;
	//these are "freelance" items
	vector <PromptItem*> mPromptItemsList;
	//these are error messages and other items with no
	//hardcoded GUI link
	vector <Prompt*> mGeneralPromptsList;

	//this is a temporary list
	vector <PromptItem*> mNeedsResolution;


private:
	static DataTree* pinstance;

	friend class DataReader;
};
}}}
#endif