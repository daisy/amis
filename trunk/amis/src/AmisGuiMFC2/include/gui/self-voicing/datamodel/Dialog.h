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