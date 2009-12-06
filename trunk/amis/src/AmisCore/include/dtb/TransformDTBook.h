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
#ifndef TRANSFORMDTBOOK_H
#define TRANSFORMDTBOOK_H


#include <string>
#if 0
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include  <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SaxParseException.hpp>

XERCES_CPP_NAMESPACE_USE
#endif

using namespace std;

#define REQUIRED_JAVA_VERSION				1.6

namespace amis
{
namespace dtb
{
	class TransformDTBook //add for xerces parsing: public DOMErrorHandler 
	{
#if 0
	protected:
		TransformDTBook();	
	public:
		static TransformDTBook* Instance();
		static void DestroyInstance();
#else
	public:
		TransformDTBook();
#endif
	public:
		~TransformDTBook();
		bool transform(string);
		string getResults();
#if 0
		bool handleError (const DOMError &domError); 
	private:
		void process();
		void printSubtree(DOMNode*);
		void moveNode(DOMElement* node, DOMElement* oldParent, DOMElement* newParent);
		void domToString();
		
		string mStylesheet;
		xercesc_3_0::DOMDocument* mpDoc;
		DOMImplementation* mpImpl;
#endif
	private:
		void readResults();
		string mResults;
		string mBin;
#if 0
		static TransformDTBook* pinstance;
#endif
	};
}
}
#endif
