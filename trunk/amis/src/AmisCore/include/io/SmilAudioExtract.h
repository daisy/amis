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
#ifndef SMILAUDIOEXTRACT_H
#define SMILAUDIOEXTRACT_H

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
class SmilAudioExtract: public XercesSaxParseBase
{
public:
	
	SmilAudioExtract();
	~SmilAudioExtract();
	
	//extract a single audio element from a par or text id
	amis::AudioNode* getAudioAtId(const ambulant::net::url*);
	
	//SAX METHODS
	void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes);
    void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	//we don't need this method but we need to implement it
	void characters(const XMLCh *const chars, const XMLSize_t len){}
	/*end of sax methods*/

private:

	amis::AudioNode* mpAudioInfo;
	string mId;
	
	//flags for parsing
	bool mbFlagInPar;
	bool mbFlagFoundId;
	bool mbIsDataExtracted;
};
}
}
#endif

