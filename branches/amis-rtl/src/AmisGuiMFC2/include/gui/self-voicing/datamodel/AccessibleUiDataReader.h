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


#ifndef ACCESSIBLEUIDATAREADER_H
#define ACCESSIBLEUIDATAREADER_H

//SYSTEM INCLUDES
#include <string>
#include <vector>

#include "io/XercesSaxParseBase.h"

#include "DataTree.h"
#include "UiItem.h"


#include "Error.h"

//NAMESPACES
XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END


using namespace std;

using namespace amis::io;


namespace amis
{
namespace gui
{
namespace spoken
{
	class DataReader  : public XercesSaxParseBase
{
public:
	//!constructor
	DataReader();

	//!destructor
	~DataReader();

	//MEMBER FUNCTIONS
	//!set the file to be processed
	amis::ErrorCode readFile(string, DataTree*);

	//SAX METHODS
	void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes);
	void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const XMLSize_t);
	/*end of sax methods*/

	string getIniFilePath();
	void setAppPath(string appPath);
	
private:
	UiItem* findMostRecentUiItem(UiItemType);
	
	//MEMBER VARIABLES
	DataTree* mpData;
	
	vector <UiItem*> mUiItems;

	string mFilepath;

	string mAppPath;
	string mIniFilePath;

	Label* mpRecentLabel;
	Switch* mpRecentSwitch;
	TextNodeSV* mpRecentTextNode;
	Prompt* mpRecentPrompt;

	bool mbFlag_GetChars;
	wstring mCollectedText;

	LabelList* mpRecentLabelList;

	bool mbFlag_ProcessingGeneralPrompts;
	bool mbFlag_ProcessingPromptItemRefs;
	
};

}}}

#endif