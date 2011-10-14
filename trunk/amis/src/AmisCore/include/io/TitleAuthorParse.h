/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef TITLEAUTHORPARSE_H
#define TITLEAUTHORPARSE_H

#include "io/XercesSaxParseBase.h"

#include <string>
#include <vector>
#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

namespace amis
{
namespace io
{
class TitleAuthorParse : public XercesSaxParseBase
{
public:
	TitleAuthorParse();
	~TitleAuthorParse();

	bool readFromFile(const ambulant::net::url*);
	amis::MediaGroup* getAuthorInfo();
	amis::MediaGroup* getTitleInfo();

	//SAX METHODS
	void startElement(const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   Attributes&	attributes);
	 void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const XMLSize_t);

private:
	amis::MediaGroup* mpAuthorInfo;
	amis::MediaGroup* mpTitleInfo;

	bool mbIsDataExtracted;
	
	//flags for use while parsing
	bool mbFlagGetChars;
	bool mbFlagGetSmilHref;
	bool mbFlagProcessingAuthor;
	bool mbFlagProcessingTitle;
	
	wstring mTempChars;
	string mSmilHref;
	//name of element to get character data for
	string mChardataElm;

	//if true, the corresponding media groups will be destroyed outside this class
	bool mbTransferredTitleData;
	bool mbTransferredAuthorData;

	enum Filetype {NCC, NCX} mFileType;
};
}
}

#endif

