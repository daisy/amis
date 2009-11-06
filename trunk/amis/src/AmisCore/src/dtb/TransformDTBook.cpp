#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include "dtb/TransformDTBook.h"
#include "util/Log.h"
#include "util/xercesutils.h"

#include <iostream>

amis::dtb::TransformDTBook::TransformDTBook()
{
	mpDoc = NULL;
}
amis::dtb::TransformDTBook::~TransformDTBook()
{
}
string amis::dtb::TransformDTBook::getResults()
{
	string html = "<html>";
//	erase everything that comes before the <html> start tag
/*	int pos = mResults.find(html);
	mResults.replace(0, pos, "");
	mResults.insert(0, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
*/
	return mResults;
}
//pass the entire file
bool amis::dtb::TransformDTBook::transform(string dtbook, string stylesheet)
{
	mStylesheet = stylesheet;
	
	try
    {
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException &toCatch)
    {
       return false;
    }

	string core = "Core";
	mpImpl = DOMImplementationRegistry::getDOMImplementation(X(core.c_str()));

	//
    //  Create MemBufferInputSource from the buffer containing the XML
    //  statements.
    //
    //  NOTE: We are using strlen() here, since we know that the chars in
    //  our hard coded buffer are single byte chars!!! The parameter wants
    //  the number of BYTES, not chars, so when you create a memory buffer
    //  give it the byte size (which just happens to be the same here.)
    //
   /* MemBufInputSource* memBufIS = new MemBufInputSource
    (
	(const XMLByte*)dtbook.c_str()
		, strlen(dtbook.c_str())
        , "dtbook"
        , false
    );
*/
	/*DOMLSInput* input = ((DOMImplementationLS*)mpImpl)->createLSInput();
	const XMLCh* input_string = XMLString::transcode(dtbook.c_str());
//	input->setEncoding(X("utf-8"));
	input->setStringData(input_string);
	*/

	//XercesDOMParser *parser = new XercesDOMParser;
	DOMLSParser* parser = ((DOMImplementationLS*)mpImpl)->createLSParser(xercesc_3_0::DOMImplementationLS::MODE_SYNCHRONOUS,
		NULL);
	DOMConfiguration  *config = parser->getDomConfig();
	
	if (config->canSetParameter(XMLUni::fgDOMErrorHandler, this))
		config->setParameter(XMLUni::fgDOMErrorHandler, this);
	if (config->canSetParameter(XMLUni::fgDOMValidate, false))
		config->setParameter(XMLUni::fgDOMValidate, false);
	if (config->canSetParameter(XMLUni::fgDOMElementContentWhitespace, false))
		config->setParameter(XMLUni::fgDOMElementContentWhitespace, false);
	if (config->canSetParameter(XMLUni::fgDOMNamespaces, true))
		config->setParameter(XMLUni::fgDOMNamespaces, true);
	if (config->canSetParameter(XMLUni::fgXercesLoadExternalDTD, false))
		config->setParameter(XMLUni::fgXercesLoadExternalDTD, false);
	if (config->canSetParameter(XMLUni::fgXercesSkipDTDValidation, true))
		config->setParameter(XMLUni::fgXercesSkipDTDValidation, true);
	
    /*
	parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationSchemaFullChecking(false);
	parser->setIncludeIgnorableWhitespace(false);
	parser->setLoadExternalDTD(false);
	parser->setSkipDTDValidation(true);
	parser->setErrorHandler(this);
	*/
	bool errorsOccurred = false;
    try
    {
	//	parser->parse(*memBufIS);
		//mpDoc= parser->parse(input);
		mpDoc = parser->parseURI(X(dtbook.c_str()));
    }
    catch (const OutOfMemoryException&)
    {
		amis::util::Log::Instance()->writeError("Parse error: out of memory", "TransformDTBook::transform");
		errorsOccurred = true;
    }
    catch (const XMLException& e)
    {
		char* exception = XMLString::transcode(e.getMessage());
		string msg = "Parse error: ";
		msg += exception;
		amis::util::Log::Instance()->writeError(msg, "TransformDTBook::transform");
		errorsOccurred = true;
		XMLString::release(&exception);
    }

    catch (const DOMException& e)
    {
		amis::util::Log::Instance()->writeError("Parse error: DOMException", "TransformDTBook::transform");
        errorsOccurred = true;
    }
    catch (...)
    {
		amis::util::Log::Instance()->writeError("General xerces error", "TransformDTBook::transform");
        errorsOccurred = true;
    }

	if (!errorsOccurred)
	{
		//mpDoc = parser->getDocument();
		if (mpDoc->getDocumentElement())
		{
			const XMLCh* enc = mpDoc->getXmlEncoding();

			char* name = XMLString::transcode(mpDoc->getDocumentElement()->getNodeName());
			if (!strcmp(name, "dtbook"))
			{
				process();
				domToString();
			}
			else
			{
				errorsOccurred = true;
			}
		}
		else
		{
			errorsOccurred = true;
		}
	}
	if (errorsOccurred)
	{
		delete parser;
//		delete memBufIS;
		XMLPlatformUtils::Terminate();
		return false;
	}
	else
	{
		return true;
	}
}

//make dtbook IE-friendly
void amis::dtb::TransformDTBook::process()
{
	string html_tag = "html";
	string body_tag = "body";
	string head_tag = "head";
	string dtbook_tag = "dtbook";
	string book_tag = "book";
	string frontmatter_tag = "frontmatter";
	string bodymatter_tag = "bodymatter";
	string rearmatter_tag = "rearmatter";

	DOMElement* dtbook = (DOMElement*)mpDoc->getElementsByTagName(X(dtbook_tag.c_str()))->item(0);
	DOMElement* head = (DOMElement*)mpDoc->getElementsByTagName(X(head_tag.c_str()))->item(0);
	DOMElement* book = (DOMElement*)mpDoc->getElementsByTagName(X(book_tag.c_str()))->item(0);

	//replace <dtbook> by <html>
	DOMElement* html = mpDoc->createElement(X(html_tag.c_str()));
	//replace <book> by <body>
	DOMElement* body = mpDoc->createElement(X(body_tag.c_str()));

	DOMNodeList* frontmatters = book->getElementsByTagName(X(frontmatter_tag.c_str()));
	DOMNodeList* bodymatters = book->getElementsByTagName(X(bodymatter_tag.c_str()));
	DOMNodeList* rearmatters = book->getElementsByTagName(X(rearmatter_tag.c_str()));

	if (frontmatters->getLength() > 0)
		moveNode((DOMElement*)frontmatters->item(0), book, body);
	if (bodymatters->getLength() > 0)
		moveNode((DOMElement*)bodymatters->item(0), book, body);
	if (rearmatters->getLength() > 0)
		moveNode((DOMElement*)rearmatters->item(0), book, body);
	
	//remove <book> from <dtbook>
	dtbook->removeChild(book);
	book->release();

	//clone <head> and move to <html>
	moveNode(head, dtbook, html);

	//attach <body> to <html>
	html->appendChild(body);
	//remove <dtbook> from the document
	mpDoc->removeChild(dtbook);
	dtbook->release();
	//add <html> as the new root element
	mpDoc->appendChild(html);

	//add http-equiv
	//<meta http-equiv="content-type" content="text/html;charset=utf-8" />
	string meta_tagname = "meta";
	string http_equiv_attr = "http-equiv";
	string http_equiv_value="content-type";
	string content_attr = "content";
	string content_value = "text/html;charset=";
	char* encoding = XMLString::transcode(mpDoc->getXmlEncoding());
	content_value += "utf-8";//encoding;
	XMLString::release(&encoding);

	DOMElement* meta_http_equiv = mpDoc->createElement(X(meta_tagname.c_str()));
	meta_http_equiv->setAttribute(X(http_equiv_attr.c_str()), X(http_equiv_value.c_str()));
	meta_http_equiv->setAttribute(X(content_attr.c_str()), X(content_value.c_str()));

	//add a stylesheet link
	//<link rel="stylesheet" type="text/css" href="dtbookbasic.css"/>
/*	string link_tagname = "link";
	string rel_attr = "rel";
	string rel_attr_value="stylesheet";
	string type_attr = "type";
	string type_attr_value = "text/css";
	string href_attr = "href";
	string href_attr_value = mStylesheet;
	
	DOMElement* link = mpDoc->createElement(X(link_tagname.c_str()));
	link->setAttribute(X(rel_attr.c_str()), X(rel_attr_value.c_str()));
	link->setAttribute(X(type_attr.c_str()), X(type_attr_value.c_str()));
	link->setAttribute(X(href_attr.c_str()), X(href_attr_value.c_str()));

	DOMElement* head_elm = (DOMElement*)mpDoc->getElementsByTagName(X(head_tag.c_str()))->item(0);
	head_elm->appendChild(meta_http_equiv);
	head_elm->appendChild(link);
*/
	/*string target="xml-stylesheet";
	string data="type=\"text/css\" href=\"";
	data += mStylesheet;
	data += "\"";
	DOMProcessingInstruction* pi = mpDoc->createProcessingInstruction(X(target.c_str()), X(data.c_str()));
	mpDoc->insertBefore(pi, html);
*/
	//get rid of the doctype declaration
	DOMDocumentType* doctype = mpDoc->getDoctype();
	mpDoc->removeChild(doctype);
	doctype->release();
}

void amis::dtb::TransformDTBook::domToString()
{
	DOMLSSerializer* p_writer = ((DOMImplementationLS*)mpImpl)->createLSSerializer();
	
	/*XMLCh* docstring = p_writer->writeToString(mpDoc);
	mResults = XMLString::transcode(docstring);
	int pos = mResults.find("UTF-16");
	*/
	DOMLSOutput* theOutput = ((DOMImplementationLS*)mpImpl)->createLSOutput();
	theOutput->setEncoding(mpDoc->getXmlEncoding());
	MemBufFormatTarget memTarget;
	theOutput->setByteStream(&memTarget);
	p_writer->write(mpDoc, theOutput);

	mResults = (char*)memTarget.getRawBuffer();
	// remember that the result will be deallocated as soon as you destroy memTarget 
	
	/*
	string core = "Core";
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X(core.c_str()));
	DOMLSSerializer* p_writer = ((DOMImplementationLS*)impl)->createLSSerializer();
	DOMLSOutput* p_output = ((DOMImplementationLS*)impl)->createLSOutput(); 
	XMLFormatTarget* p_form_target = new MemBufFormatTarget();
	p_output->setEncoding(X("utf-8"));
	p_output->setSystemId(X("amis")); 
	p_output->setByteStream(p_form_target);
	p_writer->write(mpDoc, p_output);
	
	char* theXMLString_Encoded = (char*)((MemBufFormatTarget*)p_form_target)->getRawBuffer();

	p_output->release();
	p_writer->release();

	mResults = theXMLString_Encoded;
	*/
}

//clone, remove, append, release
void amis::dtb::TransformDTBook::moveNode(DOMElement* node, DOMElement* oldParent, DOMElement* newParent)
{
	DOMElement* clone = (DOMElement*)node->cloneNode(true);
	oldParent->removeChild(node);
	newParent->appendChild(clone);
	node->release();
}

void amis::dtb::TransformDTBook::printSubtree(DOMNode* node)
{
	DOMElement* elm = (DOMElement*) node;
	const char* name = XMLString::transcode(elm->getTagName());
	
	if (name)
		cout<<name<<endl;
	for (int i = 0; i<node->getChildNodes()->getLength(); i++)
		printSubtree(node->getChildNodes()->item(i));
	
	cout<<endl;
}
bool amis::dtb::TransformDTBook::handleError (const DOMError &domError)
{
	char* msg = XMLString::transcode(domError.getMessage());
	DOMLocator* l = domError.getLocation();
	XMLFileLoc loc = l->getLineNumber();

	return true;	
}

/*
void amis::dtb::TransformDTBook::warning(const SAXParseException& e)
{
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse warning: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeWarning(msg, "XercesSaxParseBase::error");
	
	XMLString::release(&xerces_msg);
}

void amis::dtb::TransformDTBook::error(const SAXParseException& e)
{
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse error: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeError(msg, "XercesSaxParseBase::error");
	
	XMLString::release(&xerces_msg);
}

void amis::dtb::TransformDTBook::fatalError(const SAXParseException& e)
{	
	char* xerces_msg = XMLString::transcode(e.getMessage());
	long line = e.getLineNumber();
	char ch_line[10];
	itoa(line, ch_line, 10);
	string msg = "Parse fatal error: \n\t" + mFilepath.get_url() + "\n\tline: " + ch_line +  "\n\t" + xerces_msg;
	amis::util::Log::Instance()->writeError(msg, "XercesSaxParseBase::error");
	mError.setCode(amis::PARSE_ERROR);
	mError.setMessage(msg);
	
	XMLString::release(&xerces_msg);
}
*/