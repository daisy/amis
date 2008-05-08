/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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


#ifndef LABEL_H
#define LABEL_H

#include "gui/self-voicing/Prompt.h"


namespace amis
{
namespace gui
{
namespace spoken
{
enum LabelType {CAPTION, DESCRIPTION, ACCELERATOR, MNEMONIC};

class Label
{
public:
	Label();
	~Label();

	TextAudioPair* getContents();
	
	void setContents(TextAudioPair*);
	
	int getMfcId(); //std::string
	Prompt* getPrompt();
	std::string getCondition();

	LabelType getType();

	Label* clone();

	
protected:
	void setMfcId(int); //std::string
	void setPrompt(Prompt*);
	void setType(LabelType);
	void setCondition(std::string);

private:
	TextAudioPair* mpContents;
	//std::string mMfcId;
	int mMfcId;
	std::string mCondition;

	Prompt* mpPrompt;
	
	LabelType mLabelType;

	friend class DataReader;
};
}}}
#endif