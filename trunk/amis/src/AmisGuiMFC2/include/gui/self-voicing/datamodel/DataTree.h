/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2004-2009  DAISY Consortium

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