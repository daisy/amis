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
#ifndef	XERCESSAXPARSEBASE_H
#define XERCESSAXPARSEBASE_H

#include <xercesc/sax2/DefaultHandler.hpp>
#include <string>
#include "ambulant/net/url.h"

#include "Error.h"

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class Attributes;
XERCES_CPP_NAMESPACE_END

namespace amis
{
namespace io
{
class XercesSaxParseBase : public DefaultHandler
{
public:
	XercesSaxParseBase();
	virtual ~XercesSaxParseBase() = 0;
	bool parseFile(const ambulant::net::url*);
	amis::Error getError();
	const ambulant::net::url* getFilepath();

	//SAX METHODS
	virtual void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes) = 0;
	virtual void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname) = 0;
	virtual void characters(const XMLCh *const, const XMLSize_t) = 0;
	void error(const SAXParseException&);
	void fatalError(const SAXParseException&);
	void warning(const SAXParseException&);
	/*end of sax methods*/

protected:
	ambulant::net::url mFilepath;
	amis::Error mError;
};
}
}

#endif
