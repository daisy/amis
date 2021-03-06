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
#ifndef NCCFILEREADER_H
#define NCCFILEREADER_H


#include <string>

#include <xercesc/sax2/DefaultHandler.hpp>

#include "Error.h"
#include "io/NavFileReader.h"
#include "dtb/Spine.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

using namespace std;

namespace amis
{
namespace io
{
class NccFileReader : public NavFileReader
{
public:
	NccFileReader();
	~NccFileReader();

	bool readFromFile(const ambulant::net::url*);
	amis::dtb::Metadata* getMetadata();
	amis::dtb::nav::NavPoint* getTitle();

	//!xerces start element event
	void startElement(const   XMLCh* const    uri,
			const   XMLCh* const    localname,
			const   XMLCh* const    qname,
			const   Attributes&	attributes);
	//!xerces end element event
    void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void characters(const XMLCh *const, const XMLSize_t);

private:
	int isHeading(string);
	void processHeading(int, const Attributes*);
	void processSpan(const Attributes*);
	void processDiv(const Attributes*);
	void processMetadata(string, const Attributes*);
	
	bool mbFlag_ProcessMetadataChars;
	amis::dtb::Metadata* mpMetadata;
	amis::dtb::MetaItem* mpCurrentMetaItem;
	amis::dtb::nav::NavPoint* mpTitle;


};
}
}

#endif
