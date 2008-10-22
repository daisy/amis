/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

#ifndef LABELLIST_H
#define LABELLIST_H

#include "Label.h"


namespace amis
{
namespace gui
{
namespace spoken
{
class LabelList
{
public:
	LabelList();
	~LabelList();

	int getNumberOfLabels();
	Label* getLabel(int);

	bool doesListRepeat();

	void setRepeat(bool);
	void addLabel(Label*);

private:
	vector <Label*> mLabels;
	bool mbRepeat;
	
	friend class DataReader;
};
}}}

#endif