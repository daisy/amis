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
#ifndef XERCESDOMWRITER_H
#define XERCESDOMWRITER_H

#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
using namespace std;

XERCES_CPP_NAMESPACE_USE


namespace amis
{
namespace io
{
class XercesDomWriter
{
public:
	XercesDomWriter();
	~XercesDomWriter();

	xercesc_3_0::DOMDocument* getDocument();
	//create a document with the given root element name
	bool initialize(std::string);
	//write the document to file
	void writeToFile(string);
	//write a different document to file
	void writeToFile(xercesc_3_0::DOMDocument*, string);
	
private:
	xercesc_3_0::DOMDocument* mpDoc;
	xercesc_3_0::DOMImplementation* mpImpl;
};
}
}
#endif