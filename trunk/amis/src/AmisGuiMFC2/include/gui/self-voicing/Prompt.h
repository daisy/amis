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

#ifndef PROMPT_H
#define PROMPT_H


#include "UiMediaTypes.h"

namespace amis
{
	namespace gui
	{
		namespace spoken
		{
			enum PromptItemType {PROMPT_VARIABLE, PROMPT_ITEM};

			class PromptItemBase 
			{
			public:
				PromptItemBase();
				~PromptItemBase();

				PromptItemType getPromptItemType();
				TextAudioPair* getContents();

				void setContents(std::wstring, std::string);
				void setContents(std::wstring textString, amis::AudioNode* p_audio);

				void setPromptItemType(PromptItemType);
				void setContents(TextAudioPair*);
				void setContents(amis::MediaGroup*);

			private:
				PromptItemType mType;
				TextAudioPair* mpContents;

			};


			class PromptItem : public PromptItemBase
			{
			public:
				PromptItem();
				~PromptItem();

				std::string getRefId();
				std::string getId();

				PromptItem* clone();
				void setReferenceContents(PromptItem*);

			protected:
				void setRefId(std::string);
				void setId(std::string);

			private:
				std::string mRefId;
				std::string mId;

				friend class DataReader;
			};

			class PromptVar : public PromptItemBase
			{
			public:
				PromptVar();
				~PromptVar();

				//the variable parameter
				std::string getParam();

				//the variable name
				std::string getName();

				PromptVar* clone();

			protected:
				void setParam(std::string);
				void setName(std::string);

			private:
				std::string mParam;
				std::string mName;

				friend class DataReader;
			};



			class Prompt
			{
			public:
				Prompt();
				~Prompt();

				int getNumberOfItems();
				PromptItemBase* getItem(int);

				std::string getType();
				std::string getId();

				std::wstring concatAllText();
				Prompt* clone();


			protected:
				void addItem(PromptItemBase*);
				void setType(std::string);
				void setId(std::string);

			private:
				vector <PromptItemBase*> mItems;
				std::string mType;
				std::string mId;

				friend class DataReader;
			};
		}
	}
}
#endif