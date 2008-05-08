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

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "gui/self-voicing/datamodel/IniFile.h"

#include <vector>
#include <string>

#include "gui/self-voicing/datamodel/AccessibleUiDataReader.h"
#include "gui/self-voicing/datamodel/Switch.h"

#include "Preferences.h"
#include "util/FilePathTools.h"

#include "util/xercesutils.h"

using namespace amis::gui::spoken;

DataReader::DataReader()
{
	mUiItems.empty();
	mpRecentLabel = NULL;
	mpRecentTextNode = NULL;
	mpData = NULL;
	mpRecentSwitch = NULL;
	mCollectedText.empty();
}

DataReader::~DataReader()
{
	
}

void DataReader::setAppPath(string appPath)
{
	mAppPath = appPath;
}

string DataReader::getIniFilePath()
{
	if (! mIniFilePath.empty()) return mIniFilePath;

	//mIniFilePath = amis::Preferences::Instance()->mUiLangDir;
	mIniFilePath = "./settings/";
	mIniFilePath = amis::util::FilePathTools::goRelativePath(mAppPath, mIniFilePath);
	mIniFilePath = amis::util::FilePathTools::goRelativePath(mIniFilePath, "./resource.h.ini");	
	mIniFilePath = amis::util::FilePathTools::getAsLocalFilePath(mIniFilePath);

	return mIniFilePath;
}

//--------------
//start parsing the file and filling in the data model (passed in as pData)
amis::ErrorCode DataReader::readFile(string inputFilepath, DataTree* pData)
{
	mUiItems.empty();
	mpRecentLabel = NULL;
	mpRecentTextNode = NULL;
	mpData = NULL;
	mpRecentPrompt = NULL;
	mbFlag_GetChars = false;
	mpRecentLabelList = NULL;
	mbFlag_ProcessingGeneralPrompts = false;
	mbFlag_ProcessingPromptItemRefs = false;

	mpData = pData;

	//mFilepath = amis::util::FilePathTools::getAsLocalFilePath(inputFilepath);
	//const char* cp_file = NULL; cp_file = mFilepath.c_str();

	ambulant::net::url file = ambulant::net::url::from_filename(inputFilepath);
	bool ok = this->parseFile(&file);


	//resolve prompt items
    mpData->resolvePromptItemsWithRefIds();

	return (ok ? amis::OK : amis::PARSE_ERROR);
}

//--------------
//xerces' start element function
void DataReader::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{

	char* node_name = XMLString::transcode(qname);

	//container element
	if (strcmp(node_name, "container") == 0)
	{
		Container* p_container = new Container();

		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (!id.empty()) p_container->setId(id);
		
		string accelerator;
		accelerator.assign(SimpleAttrs::get("accelerator", &attributes));
		//if (!accelerator.empty()) p_container->setKeyboardAccelerator(accelerator);

		Container* p_parent_container = NULL;

		p_parent_container = (Container*)findMostRecentUiItem(CONTAINER);

		if (p_parent_container != NULL)
		{
			p_parent_container->addChildItem(p_container);
		}
		else
		{
			mpData->addContainer(p_container);
		}

		mUiItems.push_back(p_container);
	}

	

	//action element
	else if (strcmp(node_name, "action") == 0)
	{
		Container* p_container = NULL;
		p_container = (Container*)findMostRecentUiItem(CONTAINER);
		
		if (p_container != NULL)
		{
			Action* p_action = new Action();

		string mfcid;
		mfcid.assign(SimpleAttrs::get("mfcid", &attributes));
		if (!mfcid.empty())
		{
				int valueInt;
				if (mfcid.compare("IDOK") == 0)
				{
					valueInt = 1;
				}
				else if (mfcid.compare("IDCANCEL") == 0)
				{
					valueInt = 2;
				}
				else
				{
					string valueStr = CIniFile::GetValue(mfcid, "resources", getIniFilePath());
					CIniFile::string2int(valueStr.c_str(), valueInt);
				}
				p_action->setMfcId(valueInt);
		}
		
		string widget;
		widget.assign(SimpleAttrs::get("widget", &attributes));
		if (!widget.empty())
		{
				p_action->setWidgetCategories(widget);
		}

		string accelerator;
		accelerator.assign(SimpleAttrs::get("accelerator", &attributes));
		if (!accelerator.empty())
		{
				//p_action->setKeyboardAccelerator(accelerator);
		}

		string condition;
		condition.assign(SimpleAttrs::get("condition", &attributes));
		if (!condition.empty())
		{
				p_action->setCondition(condition);
		}

			if (mpRecentSwitch != NULL)
			{
				mpRecentSwitch->addAction(p_action);
			}
			else
			{
				p_container->addChildItem(p_action);
			}
			
			mUiItems.push_back(p_action);
			
		}
		
	}

	//caption or description element
	else if (strcmp(node_name, "caption") == 0 || 
		strcmp(node_name, "description") == 0 || 
		strcmp(node_name, "accelerator") == 0 || 
		strcmp(node_name, "mnemonic") == 0)
	{
		UiItem* p_item = NULL;
		p_item = mUiItems.back();

		if (p_item != NULL)
		{
			Label* p_label = new Label();
			
		string mfcid;
		mfcid.assign(SimpleAttrs::get("mfcid", &attributes));
		if (!mfcid.empty())
		{
			
				int valueInt;
				if (mfcid.compare("IDOK") == 0)
				{
					valueInt = 1;
				}
				else if (mfcid.compare("IDCANCEL") == 0) 
				{
					valueInt = 2;
				}
				else
				{
					string valueStr = CIniFile::GetValue(mfcid, "resources", getIniFilePath());
					CIniFile::string2int(valueStr.c_str(), valueInt);
				}
				p_label->setMfcId(valueInt);
		}
		string condition;
		condition.assign(SimpleAttrs::get("condition", &attributes));
		if (!condition.empty())
		{
				p_label->setCondition(condition);
				
			}

			if (strcmp(node_name, "caption") == 0)
			{
				p_label->setType(CAPTION);

				if (mpRecentSwitch != NULL && 
					mpRecentSwitch->getSwitchType() == CAPTIONSWITCH)
				{
					mpRecentSwitch->addLabel(p_label);
				}
				else
				{
					if (mpRecentLabelList != NULL)
					{
						mpRecentLabelList->addLabel(p_label);
					}
					else
					{
						p_item->setCaption(p_label);
					}
				}
			}
			else if (strcmp(node_name, "mnemonic") == 0 && p_item->getUiItemType() == ACTION)
			{
				p_label->setType(MNEMONIC);
				if (mpRecentLabelList != NULL)
				{
					mpRecentLabelList->addLabel(p_label);
				}
				else
				{
					((Action*)p_item)->setMnemonic(p_label);
				}
			}
			else if (strcmp(node_name, "mnemonic") == 0 && p_item->getUiItemType() == CONTAINER)
			{
				p_label->setType(MNEMONIC);
				if (mpRecentLabelList != NULL)
				{
					mpRecentLabelList->addLabel(p_label);
				}
				else
				{
					((Container*)p_item)->setMnemonic(p_label);
				}
			}
			else if (strcmp(node_name, "mnemonic") == 0 && p_item->getUiItemType() == CONTROL)
			{
				p_label->setType(MNEMONIC);
				if (mpRecentLabelList != NULL)
				{
					mpRecentLabelList->addLabel(p_label);
				}
				else
				{
					((DialogControl*)p_item)->setMnemonic(p_label);
				}
			}
			else if (strcmp(node_name, "accelerator") == 0 && p_item->getUiItemType() == ACTION)
			{
				p_label->setType(ACCELERATOR);
				if (mpRecentLabelList != NULL)
				{
					mpRecentLabelList->addLabel(p_label);
				}
				else
				{
					((Action*)p_item)->setKeyboardAccelerator(p_label);
				}
			}
			else if (strcmp(node_name, "accelerator") == 0 && p_item->getUiItemType() == CONTAINER)
			{
				p_label->setType(ACCELERATOR);
				if (mpRecentLabelList != NULL)
				{
					mpRecentLabelList->addLabel(p_label);
				}
				else
				{
					((Container*)p_item)->setKeyboardAccelerator(p_label);
				}
			}
			else if (strcmp(node_name, "description") == 0)
			{
				p_label->setType(DESCRIPTION);
				
				if (mpRecentSwitch != NULL && 
					mpRecentSwitch->getSwitchType() == DESCRIPTIONSWITCH)
				{
					mpRecentSwitch->addLabel(p_label);
				}
				else
				{
					if (mpRecentLabelList != NULL)
					{
						mpRecentLabelList->addLabel(p_label);
					}
					else
					{
						p_item->setDescription(p_label);
					}
				}
			}
			else
			{
				int debug = 0;
			}

			mpRecentLabel = p_label;
		}
	}
	
	//text element
	else if (strcmp(node_name, "text") == 0)
	{
		if (this->mbFlag_ProcessingPromptItemRefs == true)
		{
			int last_prompt_item = mpData->getNumPromptItems();
			if (last_prompt_item > 0)
			{
				PromptItem* p_item = NULL;
				p_item = mpData->getPromptItem(last_prompt_item-1);

				if (p_item != NULL)
				{
					TextNodeSV* p_text = new TextNodeSV();

					string mnemonic;
					mnemonic.assign(SimpleAttrs::get("mnemonic", &attributes));
					if (!mnemonic.empty())
					{

						p_text->setMnemonic(mnemonic);
					}

					mpRecentTextNode = p_text;
					
					mbFlag_GetChars = true;
					p_item->getContents()->setText(p_text);
				}
			}
		} else
		if (mpRecentLabel != NULL && mpRecentPrompt == NULL)
		{
			TextNodeSV* p_text = new TextNodeSV();

			string mnemonic;
			mnemonic.assign(SimpleAttrs::get("mnemonic", &attributes));
			if (!mnemonic.empty())
			{
				p_text->setMnemonic(mnemonic);

			}

			mpRecentTextNode = p_text;

			
			TextAudioPair* p_pair = NULL;
			p_pair = mpRecentLabel->getContents();

			//if a text-audio pair hasn't been started for the current label
			//then make one
			if (p_pair == NULL)
			{
				p_pair = new TextAudioPair();
				mpRecentLabel->setContents(p_pair);
			}

			p_pair->setText(p_text);

			mbFlag_GetChars = true;
		}
		//if there is no label open but there is a prompt open, then the
		//text node should go on its last item, if exists
		else if (mpRecentPrompt != NULL)
		{
			int sz = mpRecentPrompt->getNumberOfItems();
			if (sz > 0)
			{
				PromptItemBase* p_item = mpRecentPrompt->getItem(sz-1);

				if (p_item->getPromptItemType() == PROMPT_ITEM)
				{
					TextNodeSV* p_text = new TextNodeSV();

					string mnemonic;
					mnemonic.assign(SimpleAttrs::get("mnemonic", &attributes));
					if (!mnemonic.empty())
					{

						p_text->setMnemonic(mnemonic);

					}

					mpRecentTextNode = p_text;
					
					mbFlag_GetChars = true;
					p_item->getContents()->setText(p_text);
				}
			}
		}
		
	}

	//audio element
	else if (strcmp(node_name, "audio") == 0)
	{
		if (this->mbFlag_ProcessingPromptItemRefs == true)
		{
			int last_prompt_item = mpData->getNumPromptItems();
			if (last_prompt_item > 0)
			{
				PromptItem* p_item = NULL;
				p_item = mpData->getPromptItem(last_prompt_item-1);

				if (p_item != NULL)
				{
					amis::AudioNode* p_audio = new amis::AudioNode();

					string src;
					src.assign(SimpleAttrs::get("src", &attributes));
					if (!src.empty())
					{
						p_audio->setSrc(src);
					}
	
					p_item->getContents()->setAudio(p_audio);
				}
			}
		} else
		if (mpRecentLabel != NULL && mpRecentPrompt == NULL)
		{
			amis::AudioNode* p_audio = new amis::AudioNode();
			

			string src;
			src.assign(SimpleAttrs::get("src", &attributes));
			if (!src.empty())
			{

				p_audio->setSrc(src);
			}

			TextAudioPair* p_pair = NULL;
			p_pair = mpRecentLabel->getContents();

			//if a text-audio pair hasn't been started for the current label
			//then make one
			if (p_pair == NULL)
			{
				p_pair = new TextAudioPair();
				mpRecentLabel->setContents(p_pair);
			}

			p_pair->setAudio(p_audio);
		}

		//if there is no label open but there is a prompt open, then the
		//audio node should go on its last item, if exists
		else if (mpRecentPrompt != NULL)
		{
			int sz = mpRecentPrompt->getNumberOfItems();
			if (sz > 0)
			{
				PromptItemBase* p_item = mpRecentPrompt->getItem(sz-1);

				if (p_item->getPromptItemType() == PROMPT_ITEM)
				{
					amis::AudioNode* p_audio = new amis::AudioNode();

					string src;
					src.assign(SimpleAttrs::get("src", &attributes));
					if (!src.empty())
					{
						p_audio->setSrc(src);

					}


					p_item->getContents()->setAudio(p_audio);			
				}
			}
		}
	}

	//icon element
	else if (strcmp(node_name, "icon") == 0)
	{
		Action* p_action = NULL;
		p_action = (Action*)findMostRecentUiItem(ACTION);

		if (p_action != NULL)
		{
			ImageNodeSV* p_image = new ImageNodeSV();

		string type;
		type.assign(SimpleAttrs::get("type", &attributes));
		if (!type.empty())
		{
				if (type.compare("smallIcon") == 0)
				{
					p_image->setImageType(SMALL_ICON);
				}
				else if(type.compare("largeIcon") == 0)
				{
					p_image->setImageType(LARGE_ICON);
				}
			}

		string src;
		src.assign(SimpleAttrs::get("src", &attributes));
		if (!src.empty())
		{
				p_image->setSrc(src);
			}


			p_action->addImage(p_image);
		}
	}

	//dialog element
	else if (strcmp(node_name, "dialog") == 0)
	{
		Dialog* p_dialog = new Dialog();

		mUiItems.push_back(p_dialog);
		
		mpData->addDialog(p_dialog);

		string mfcid;
		mfcid.assign(SimpleAttrs::get("mfcid", &attributes));
		if (!mfcid.empty())
		{

				int valueInt;
				if (mfcid.compare("IDOK") == 0)
				{
					valueInt = 1;
				}
				else if (mfcid.compare("IDCANCEL") == 0)
				{
					valueInt = 2;
				}
				else 
				{
					string valueStr = CIniFile::GetValue(mfcid, "resources", getIniFilePath());
					CIniFile::string2int(valueStr.c_str(), valueInt);
				}
				p_dialog->setMfcId(valueInt);

		}

		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (!id.empty())
		{
			p_dialog->setId(id);
		}


	}
	else if (strcmp(node_name, "control") == 0)
	{
		
		Dialog* p_dialog = NULL;
		p_dialog = (Dialog*)findMostRecentUiItem(DIALOG);

		if (p_dialog != NULL)
		{
			DialogControl* p_dlg_control = new DialogControl();

		string mfcid;
		mfcid.assign(SimpleAttrs::get("mfcid", &attributes));
		if (!mfcid.empty())
		{
				
				int valueInt;
				if (mfcid.compare("IDOK") == 0)
				{
					valueInt = 1;
				}
				else if (mfcid.compare("IDCANCEL") == 0)
				{
					valueInt = 2;
				}
				else
				{
					string valueStr = CIniFile::GetValue(mfcid, "resources", getIniFilePath());
					CIniFile::string2int(valueStr.c_str(), valueInt);
				}
				p_dlg_control->setMfcId(valueInt);

			}

		string widget;
		widget.assign(SimpleAttrs::get("widget", &attributes));
		if (!widget.empty())
		{
				p_dlg_control->setWidgetCategories(widget);
			}

		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (!id.empty())
		{
				p_dlg_control->setId(id);
			}

			p_dialog->addChildControl(p_dlg_control);

			mUiItems.push_back(p_dlg_control);
		}

		
	}
	else if (strcmp(node_name, "list") == 0)
	{
		LabelList* p_list = new LabelList();

		string repeat;
		repeat.assign(SimpleAttrs::get("repeat", &attributes));
		if (!repeat.empty())
		{
			if (repeat.compare("yes") == 0)
				p_list->setRepeat(true);
			else
				p_list->setRepeat(false);

		}

		mpRecentLabelList = p_list;

		UiItem* p_item = NULL;
		p_item = mUiItems.back();

		if (p_item != NULL)
			p_item->setLabelList(p_list);
	}
	else if (strcmp(node_name, "switch") == 0)
	{
		if (mUiItems.size() > 0)
		{
			Switch* p_switch = new Switch();
			mUiItems.back()->setSwitch(p_switch);
	
		string element;
		element.assign(SimpleAttrs::get("element", &attributes));
		if (!element.empty())
		{
				if (element.compare("action") == 0)
					p_switch->setSwitchType(ACTIONSWITCH);
				else if (element.compare("caption") == 0)
					p_switch->setSwitchType(CAPTIONSWITCH);
				else if (element.compare("description") == 0)
					p_switch->setSwitchType(DESCRIPTIONSWITCH);

				
			}

			mpRecentSwitch = p_switch;
		}
	}
	else if (strcmp(node_name, "prompt") == 0)
	{
		Prompt* p_prompt = new Prompt();

		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (!id.empty())
		{
			p_prompt->setId(id);
		}

		string type;
		type.assign(SimpleAttrs::get("type", &attributes));
		if (!type.empty())
		{
			p_prompt->setType(type);
		}

		this->mpRecentPrompt = p_prompt;
		
		if (this->mbFlag_ProcessingGeneralPrompts == true)
		{
			mpData->addPrompt(mpRecentPrompt);
		}
		else 
		//if we are processing a label, then the prompt belongs to it
		if (mpRecentLabel != NULL)
		{
			mpRecentLabel->setPrompt(mpRecentPrompt);
		}

		//else it belongs to a dialog
		else
		{
			Dialog* p_dlg = NULL;
			p_dlg = (Dialog*)findMostRecentUiItem(DIALOG);
			if (p_dlg != NULL)
			{
				p_dlg->addPrompt(mpRecentPrompt);
			}
		}
	}
	else if (strcmp(node_name, "promptVar") == 0)
	{
		if (mpRecentPrompt == NULL)
		{
			mpRecentPrompt = new Prompt();

			//if we are processing a label, then the prompt belongs to it
			if (mpRecentLabel != NULL)
			{
				mpRecentLabel->setPrompt(mpRecentPrompt);
			}
			else
			{
				int debug  = 0 ;
			}
		}

		PromptVar* p_var = new PromptVar();

		string name;
		name.assign(SimpleAttrs::get("name", &attributes));
		if (!name.empty())
		{
			p_var->setName(name);
		}

		string param;
		param.assign(SimpleAttrs::get("param", &attributes));
		if (!param.empty())
		{
			p_var->setParam(param);
		}


		mpRecentPrompt->addItem(p_var);
	}
	else if (strcmp(node_name, "promptItem") == 0)
	{
		if (mpRecentPrompt == NULL && this->mbFlag_ProcessingPromptItemRefs == false)
		{
			mpRecentPrompt = new Prompt();
			//if we are processing a label, then the prompt belongs to it
			if (mpRecentLabel != NULL)
			{
				mpRecentLabel->setPrompt(mpRecentPrompt);
			}
		}

		PromptItem* p_item = new PromptItem();

		string refid;
		refid.assign(SimpleAttrs::get("refid", &attributes));
		if (!refid.empty())
		{
			p_item->setRefId(refid);

			//add this to the temporary list of things that need to be resolved
			mpData->addForResolution(p_item);
		}
		
		//if there was no refid, then this promptItem will have child contents
		else
		{
			TextAudioPair* p_pair = new TextAudioPair();
			p_item->setContents(p_pair);
		}

		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		if (!id.empty())
		{
			p_item->setId(id);
		}

		//if we are not processing prompt item refs
		//then mpRecentPrompt will exist (either from a prior iteration or
		//created above in this else-if block
		if (this->mbFlag_ProcessingPromptItemRefs == false)
		{
			mpRecentPrompt->addItem(p_item);	
		}
		//otherwise, if we are processing the promptItemRefs section
		//add it to the data tree
		else
		{
			mpData->addPromptItem(p_item);
		}
		
	}
	else if (strcmp(node_name, "generalPrompts") == 0)
	{
		this->mbFlag_ProcessingGeneralPrompts = true;
	}
	else if (strcmp(node_name, "promptItemRefs") == 0)
	{
		this->mbFlag_ProcessingPromptItemRefs = true;
	}
	else
	{
		//not recognized
	}



	XMLString::release(&node_name);
}

//--------------------------------------------------
//! (SAX Event) close this element 
//--------------------------------------------------
void DataReader::endElement( const XMLCh* const uri,
					const XMLCh* const localname,
					 const XMLCh* const qname)
{
	//local variable
	char* element_name = XMLString::transcode(qname);

	//if it's a ui item
	if (strcmp (element_name, "container") == 0 ||
		strcmp(element_name, "action") == 0 ||
		strcmp(element_name, "dialog") == 0 ||
		strcmp(element_name, "control") == 0)
	{
		//pop it off the stack
		mUiItems.pop_back();
	}
	//if it was a TEXT or NOTE element, get the accumulated text string and put it on the node
	else if (strcmp(element_name, "text") == 0 ||
			strcmp(element_name, "note") == 0)
	{
		if (mpRecentTextNode != NULL)
			mpRecentTextNode->setTextString(mCollectedText);
	
		mCollectedText.erase();
		mbFlag_GetChars = false;
		mpRecentTextNode = NULL;
	}


	else if(strcmp(element_name, "caption") == 0 ||
		strcmp(element_name, "description") == 0)
	{
		mpRecentLabel = NULL;
		mpRecentPrompt = NULL;
	}

	else if (strcmp(element_name, "switch") == 0)
	{
		mpRecentSwitch = NULL;
	}
	
	else if(strcmp(element_name, "prompt") == 0)
	{
		mpRecentPrompt = NULL;

	}
	else if (strcmp(element_name, "list") == 0)
	{
		mpRecentLabelList = NULL;
	}
	else if (strcmp(element_name, "generalPrompts") == 0)
	{
		this->mbFlag_ProcessingGeneralPrompts = false;
	}
	else if (strcmp(element_name, "promptItemRefs") == 0)
	{
		this->mbFlag_ProcessingPromptItemRefs = false;
	}


	XMLString::release(&element_name);

}

void DataReader::characters(const XMLCh *const chars, const unsigned int length)
{
	if (mbFlag_GetChars == true)
	{
		mCollectedText.append((wchar_t*)chars);	
	}
	
}		

UiItem* DataReader::findMostRecentUiItem(UiItemType itemType)
{
	UiItem* p_item = NULL;

	for (int i=mUiItems.size()-1; i>=0; i--)
	{
		if (mUiItems[i]->getUiItemType() == itemType)
		{
			p_item = mUiItems[i];
			break;
		}
	}

	return p_item;
}

