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

#ifndef SMILFILEREADER_H
#define SMILFILEREADER_H

#include <vector>
#include <string>
#include <xercesc/sax2/DefaultHandler.hpp>

#include "Error.h"
#include "io/XercesSaxParseBase.h"
#include "dtb/smil/NodeFactory.h"
#include "AmisCore.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

using namespace std;

namespace amis
{
namespace io
{
class QuickDataSmilFileReader : public XercesSaxParseBase 
{
public:
	QuickDataSmilFileReader();
	~QuickDataSmilFileReader();

	//main method to create a smil tree from a filepath
	bool readFromFile(const ambulant::net::url*, StdStringList*, StringMap*, amis::dtb::nav::NavNodeList*);
	
	void startElement(const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   Attributes&	attributes);
	void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const unsigned int);

private:
	bool isSupported(string);
	amis::dtb::nav::NavNode* matchesNodeInList(string content);

	StdStringList* mpList;
	StringMap* mpStringMap;
	amis::dtb::nav::NavNodeList* mpNodes;
	string mFilename;
	bool mbFlag_GetNextAudio;

};
}
}

#endif
