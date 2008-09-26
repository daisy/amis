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
#ifndef SMILAUDIOEXTRACT_H
#define SMILAUDIOEXTRACT_H

#include "io/XercesSaxParseBase.h"
#include "Media.h"
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
{//! The SmilAudioExtract parses a SMIL file to get audio data for a single element
/*!
	this class is based on the SmilAudioRetrieve Nav utility class
	but differs because that class builds a list of all audio references, 
	under the assumption that it may have to pull several references from the same 
	file.  in that scenario, this saves parsing time.  but here we just want one
	reference so building that list is unneccesary*/
class SmilAudioExtract: public DefaultHandler
{
	
public:
	
	//LIFECYCLE
	SmilAudioExtract();
	~SmilAudioExtract();
	
	//!extract a single audio element from a par or text id
	amis::AudioNode* getAudioAtId(string filepath);
	
	//SAX METHODS
	void startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes);
    void endElement(const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname);
	void error(const SAXParseException&);
	void fatalError(const SAXParseException&);
	void warning(const SAXParseException&);
	/*end of sax methods*/

private:

	//!get an attribute value from the member variable mpAttributes
	string getAttributeValue(string attributeName);

	//!smil source path
	string mFilePath;
	//!audio information
	amis::AudioNode* mpAudioInfo;
	//!target id
	string mId;
	//!pointer to attributes collection for node being currently processed
	const Attributes* mpAttributes;

	//!flag if we are in a par element
	bool mb_flagInPar;
	//!if we found the id (on a text or par)
	bool mb_flagFoundId;
	//!if the search is done and the data has been collected
	bool mb_flagFinished;
};
}

#endif

