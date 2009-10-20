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
#ifndef NCXFILEREADER_H
#define NCXFILEREADER_H

#include <string>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "io/NavFileReader.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

using namespace std;

namespace amis
{
namespace io
{
class NcxFileReader : public NavFileReader
{
public:
	NcxFileReader();
	~NcxFileReader();

	bool readFromFile(const ambulant::net::url*);
	amis::MediaGroup* getTitle();
	amis::MediaGroup* getAuthor();

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
	void processHeading(const Attributes* pAttrs);
	void processNavLabel(const Attributes* pAttrs);
	void processText(const Attributes* pAttrs);
	void processAudio(const Attributes* pAttrs);
	void processNavTarget(const Attributes* pAttrs);
	void processPageTarget(const Attributes* pAttrs);
	void processCustomTest(const Attributes* pAttrs);

private:
	amis::MediaGroup* mpCurrentMediaGroup;
	amis::MediaGroup* mpTitle;
	amis::MediaGroup* mpAuthor;
};
}
}

#endif
