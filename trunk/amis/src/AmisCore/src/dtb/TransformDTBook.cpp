#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "dtb/TransformDTBook.h"
#include "util/Log.h"
#include "util/xercesutils.h"

#include "io/XercesDomWriter.h"
#include <iostream>

void print_subtree(DOMNode*);

//pass the entire file
bool amis::dtb::TransformDTBook::transform(string dtbook)
{
	try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException &toCatch)
    {
       return false;
    }

	//
    //  Create MemBufferInputSource from the buffer containing the XML
    //  statements.
    //
    //  NOTE: We are using strlen() here, since we know that the chars in
    //  our hard coded buffer are single byte chars!!! The parameter wants
    //  the number of BYTES, not chars, so when you create a memory buffer
    //  give it the byte size (which just happens to be the same here.)
    //
    MemBufInputSource* memBufIS = new MemBufInputSource
    (
	(const XMLByte*)dtbook.c_str()
		, strlen(dtbook.c_str())
        , "dtbook"
        , false
    );

	XercesDOMParser *parser = new XercesDOMParser;
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationSchemaFullChecking(false);
	parser->setIncludeIgnorableWhitespace(false);
	parser->setLoadExternalDTD(false);
	parser->setSkipDTDValidation(true);
//    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
//    parser->setErrorHandler(errReporter);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    bool errorsOccurred = false;

    try
    {
        parser->parse(*memBufIS);
    }
    catch (const OutOfMemoryException&)
    {
//        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorsOccurred = true;
    }
    catch (const XMLException& e)
    {
//        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
//             << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorsOccurred = true;
    }

    catch (const DOMException& e)
    {
 //       const unsigned int maxChars = 2047;
 //       XMLCh errText[maxChars + 1];

 //       XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << gXmlFile << "'\n"
 //            << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

   //     if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
   //          XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

        errorsOccurred = true;
    }

    catch (...)
    {
     //   XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        errorsOccurred = true;
    }

	if (errorsOccurred)
	{
		delete parser;
		delete memBufIS;
		// And call the termination method
		XMLPlatformUtils::Terminate();
		return false;
	}
	else
	{
		process(parser->getDocument());
	}
}

//make dtbook IE-friendly
void amis::dtb::TransformDTBook::process(xercesc_3_0::DOMDocument* doc)
{
	DOMElement* dtbook = (DOMElement*)doc->getElementsByTagName(X("dtbook"))->item(0);
	DOMElement* head = (DOMElement*)doc->getElementsByTagName(X("head"))->item(0);
	DOMElement* book = (DOMElement*)doc->getElementsByTagName(X("book"))->item(0);

	string html_tag = "html";
	string body_tag = "body";

	//replace <dtbook> by <html>
	DOMElement* html = doc->createElement(X(html_tag.c_str()));
	//replace <book> by <body>
	DOMElement* body = doc->createElement(X(body_tag.c_str()));

	//move the <book> child nodes to <body>
	DOMNodeList* book_children = book->getChildNodes();
	for (int i = 0; i<book_children->getLength(); i++)
	{
		DOMNode* child = book_children->item(i);
		DOMElement* clone = (DOMElement*)child->cloneNode(true);
		
		print_subtree(child);

		book->removeChild(child);
		body->appendChild(clone);
		child->release();
	}
	
	
	//remove <book> from <dtbook>
	dtbook->removeChild(book);
	book->release();

	//clone <head> and move to <html>
	DOMElement* head_clone = (DOMElement*)head->cloneNode(true);
	dtbook->removeChild(head);
	html->appendChild((DOMNode*)head_clone);
	head->release();

	//attach <body> to <html>
	html->appendChild(body);
	//remove <dtbook> from the document
	doc->removeChild(dtbook);
	dtbook->release();
	//add <html> as the new root element
	doc->appendChild(html);

	//get rid of the doctype declaration
	DOMDocumentType* doctype = doc->getDoctype();
	doc->removeChild(doctype);
	doctype->release();
	
	//TODO: add css reference

	amis::io::XercesDomWriter w;
	w.writeToFile(doc, "c:\\devel\\dtbook_transform.html");

	
}

void print_subtree(DOMNode* node)
{
	DOMElement* elm = (DOMElement*) node;
	const char* name = XMLString::transcode(elm->getTagName());
	
	if (name)
		cout<<name<<endl;
	for (int i = 0; i<node->getChildNodes()->getLength(); i++)
		print_subtree(node->getChildNodes()->item(i));
	
	cout<<endl;
}