#include "gui/self-voicing/datamodel/DataTree.h"
#include <fstream>

using namespace amis::gui::spoken;

DataTree* DataTree::pinstance = 0;

DataTree* DataTree::Instance()
{
	if (pinstance == 0)  // is it the first call?
    {  
      pinstance = new DataTree; // create sole instance
    }
    return pinstance; // address of sole instance
}


void DataTree::DestroyInstance()
{
	if (pinstance != NULL) delete pinstance;
}

DataTree::DataTree()
{
	mContainersList.empty();
	mDialogsList.empty();
	mGeneralPromptsList.empty();
	mPromptItemsList.empty();
	mNeedsResolution.empty();
}
DataTree::~DataTree()
{
	int sz = 0;
	int i = 0;

	Container* p_container = NULL;
	Dialog* p_dialog = NULL;
	Prompt* p_prompt = NULL;
	PromptItem* p_prompt_item = NULL;

	sz = mContainersList.size();

	for (i=sz -1; i>=0; i--)
	{
		p_container = mContainersList[i];
		mContainersList.pop_back();
		if (p_container != NULL)
			delete p_container;

		p_container = NULL;
	}

	sz = mDialogsList.size();

	for (i=sz -1; i>=0; i--)
	{
		p_dialog = mDialogsList[i];
		mDialogsList.pop_back();
		if (p_dialog != NULL)
			delete p_dialog;

		p_dialog = NULL;
	}

	sz = mGeneralPromptsList.size();

	for (i=sz -1; i>=0; i--)
	{
		p_prompt = mGeneralPromptsList[i];
		mGeneralPromptsList.pop_back();
		if (p_prompt != NULL)
			delete p_prompt;

		p_prompt = NULL;
	}

	sz = mPromptItemsList.size();

	for (i=sz -1; i>=0; i--)
	{
		p_prompt_item = mPromptItemsList[i];
		mPromptItemsList.pop_back();
		if (p_prompt_item != NULL)
			delete p_prompt_item;

		p_prompt_item = NULL;
	}

	//don't need to delete stuff from this list, it only held pointers
	mNeedsResolution.empty();
}
	
Container* DataTree::getContainer(int idx)
{
	if (idx >=0 && idx < mContainersList.size())
	{
		return mContainersList[idx];
	}
	else
	{
		return NULL;
	}
}

int DataTree::getNumContainers()
{
	return mContainersList.size();
}

void DataTree::addContainer(Container* pContainer)
{
	mContainersList.push_back(pContainer);
}
Dialog* DataTree::getDialog(int idx)
{
	if (idx >=0 && idx < mDialogsList.size())
	{
		return mDialogsList[idx];
	}
	else
	{
		return NULL;
	}
}

int DataTree::getNumDialogs()
{
	return mDialogsList.size();
}

void DataTree::addDialog(Dialog* pDialog)
{
	mDialogsList.push_back(pDialog);
}
Dialog* DataTree::findDialog(int mfcid)
{
	Dialog* p_dlg = NULL;

	for (int i=0; i<mDialogsList.size(); i++)
	{
		if (mDialogsList[i]->getMfcId() == mfcid)
		{
			p_dlg = mDialogsList[i];
			break;
		}
	}

	return p_dlg;
}

UiItem* DataTree::findUiItemInContainers(int mfcID, string switchCondition) {

	for (int i=0; i<mContainersList.size(); i++)
	{
		UiItem* uiItem = mContainersList[i]->findUiItem(mfcID, switchCondition);

		if (uiItem != NULL)
		{
			return uiItem;
		}
	}

	return NULL;
}

Container* DataTree::findContainer(std::string id)
{
	Container* p_container = NULL;

	for (int i=0; i<mContainersList.size(); i++)
	{
		Container* container = mContainersList[i];
		string strID = container->getId();
		if (strID.compare(id) == 0)
		{
			p_container = mContainersList[i];
			break;
		} else {
			p_container = container->findContainer(id);
			if (p_container != NULL)
			{
				break;
			}
		}
	}

	return p_container;
}

Prompt* DataTree::findPrompt(std::string id)
{
	Prompt* p_obj = NULL;

	for (int i=0; i<mGeneralPromptsList.size(); i++)
	{
		if (mGeneralPromptsList[i]->getId() == id)
		{
			p_obj = mGeneralPromptsList[i];
			break;
		}
	}

	return p_obj;
}
PromptItem* DataTree::findPromptItem(std::string id)
{
	PromptItem* p_obj = NULL;

	for (int i=0; i<mPromptItemsList.size(); i++)
	{
		if (mPromptItemsList[i]->getId() == id)
		{
			p_obj = mPromptItemsList[i];
			break;
		}
	}

	return p_obj;
}

int DataTree::getNumPrompts()
{
	return mGeneralPromptsList.size();
}

int DataTree::getNumPromptItems()
{
	return mPromptItemsList.size();
}

Prompt* DataTree::getPrompt(int idx)
{
	if (idx >=0 && idx < mGeneralPromptsList.size())
	{
		return mGeneralPromptsList[idx];
	}
	else
	{
		return NULL;
	}
}

PromptItem* DataTree::getPromptItem(int idx)
{
	if (idx >=0 && idx < mPromptItemsList.size())
	{
		return mPromptItemsList[idx];
	}
	else
	{
		return NULL;
	}
}

void DataTree::addPrompt(Prompt* pPrompt)
{
	mGeneralPromptsList.push_back(pPrompt);
}

void DataTree::addPromptItem(PromptItem* pItem)
{
	mPromptItemsList.push_back(pItem);
}

void DataTree::addForResolution(PromptItem* pItem)
{
	this->mNeedsResolution.push_back(pItem);
}

void DataTree::resolvePromptItemsWithRefIds()
{
	for (int i = 0; i<this->mNeedsResolution.size(); i++)
	{
		PromptItem* p_ref = NULL;

		string str = mNeedsResolution[i]->getRefId();
		
		p_ref = this->findPromptItem(str);

		if (p_ref != NULL)
		{
			mNeedsResolution[i]->setReferenceContents(p_ref);
		} else {
			int debug = 0;
		}
	}
}

void DataTree::testPrint(std::string outfile)
{
	fstream out;
	out.open(outfile.c_str(), ios::trunc | ios::out);

	out<<"Data Tree\n=========================="<<endl;
	out<<"Containers\n-----"<<endl;

	out.close();

	for (int i=0; i<mContainersList.size(); i++)
	{
		mContainersList[i]->testPrint(outfile, 1);
	}

	out.open(outfile.c_str(), ios::app | ios::out);

	out<<"Dialogs\n-----"<<endl;

	out.close();

	for (int i=0; i<mDialogsList.size(); i++)
	{
		mDialogsList[i]->testPrint(outfile, 1);
	}

	out.open(outfile.c_str(), ios::app | ios::out);

	out<<"General Prompts\n-----"<<endl;

	out.close();

	for (int i=0; i<mGeneralPromptsList.size(); i++)
	{
		mGeneralPromptsList[i]->testPrint(outfile, 1);
	}

	out.open(outfile.c_str(), ios::app | ios::out);

	out<<"Prompt Item Refs\n-----"<<endl;

	out.close();

	for (int i=0; i<mPromptItemsList.size(); i++)
	{
		mPromptItemsList[i]->testPrint(outfile, 1);
	}

}
