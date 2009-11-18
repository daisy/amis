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

namespace amis
{
namespace dtb
{
	class TransformDTBook //add for xerces parsing: public DOMErrorHandler 
	{
	public:
		TransformDTBook();
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
		string mResults;
		string mBin;
	};
}
}
#endif
