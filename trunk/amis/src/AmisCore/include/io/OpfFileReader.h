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
#ifndef OPFFILEREADER_H
#define OPFFILEREADER_H

#include "io/XercesSaxParseBase.h"
#include "dtb/Spine.h"
#include "dtb/Metadata.h"

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
class OpfFileReader : public XercesSaxParseBase
{
public:
	OpfFileReader();
	~OpfFileReader();

	bool readFromFile(const ambulant::net::url*);
	
	amis::dtb::Spine* getSpine();
	amis::dtb::Metadata* getMetadata();
	const ambulant::net::url* getNavFilename();
	const ambulant::net::url* getResourceFilename();
	const amis::UrlList* getTextFilenames();

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

private:
	void sortSpine();

	wstring mTempChars;
	bool mbFlag_GetChars;
	bool mbFlag_InSpine;
	bool mbFlag_InManifest;

	amis::dtb::Metadata* mpMetadata;
	amis::dtb::Spine* mpSpine;
	ambulant::net::url mResourceFilename;
	ambulant::net::url mNavFilename;
	amis::UrlList mTextFilenames;

	//the smil files from the manifest
	amis::UrlMap mUnsortedSmilFiles;
	//temporary list
	StdStringList mItemRefs;
	//the input filename
	const ambulant::net::url* mpFilename;
	//temporary metadata item
	amis::dtb::MetaItem* mpCurrentMetaItem;
};
}
}

#endif
