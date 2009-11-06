#ifndef TRANSFORMDTBOOK_H
#define TRANSFORMDTBOOK_H


#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include  <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SaxParseException.hpp>
using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace amis
{
namespace dtb
{
	class TransformDTBook : public DOMErrorHandler //public ErrorHandler
	{
	public:
		TransformDTBook();
		~TransformDTBook();
		bool transform(string, string);
		string getResults();
		bool handleError (const DOMError &domError); 
		/*void error(const SAXParseException&);
		void fatalError(const SAXParseException&);
		void warning(const SAXParseException&);
		void resetErrors() {}*/
	private:
		void process();
		void printSubtree(DOMNode*);
		void moveNode(DOMElement* node, DOMElement* oldParent, DOMElement* newParent);
		void domToString();
		
		string mStylesheet;
		string mResults;
		xercesc_3_0::DOMDocument* mpDoc;
		DOMImplementation* mpImpl;
	};
}
}
#endif
