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
#ifndef MODULEDESCREADER_H
#define MODULEDESCREADER_H

#include "ModuleDescData.h"
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
class ModuleDescReader : public XercesSaxParseBase
{
public:
	ModuleDescReader();
	~ModuleDescReader();

	bool readFromFile(const ambulant::net::url*);
	amis::ModuleDescData* getModuleDescData();

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
	wstring mTempWChars;
	bool mb_flagGetChars;
	amis::ModuleDescData* mpData;
	amis::MediaGroup* mpTempLabel;
};
}
}

#endif
