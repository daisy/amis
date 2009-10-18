/*
 * Copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xalanc/Include/PlatformDefinitions.hpp"


#if defined(XALAN_CLASSIC_IOSTREAMS)
#include <iostream.h>
#else
#include <iostream>
#include <strstream>
#endif
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/XSLT/XSLTInputSource.hpp>

using namespace std;

bool is_dtbook(std::string& url)
{
	//TODO: check if this is actually a dtbook file (they're not all called "dtbook")
    // Get the directory part
	size_t slashpos = url.find_last_of("/");
	if (slashpos <= 0)
		return false;
	std::string filename = url.substr(slashpos+1);
	
	if (!filename.compare("dtbook.xml")) return true;
	else return false;
}

bool dtbookfixer(std::string& whole_file, std::ostream& os)
{
	int	theResult = -1;
	try
	{
		XALAN_USING_XERCES(XMLPlatformUtils)
		XALAN_USING_XALAN(XalanTransformer)

		// Call the static initializer for Xerces.
		XMLPlatformUtils::Initialize();
		// Initialize Xalan.
		XalanTransformer::initialize();
		
		// Create a XalanTransformer.
		XalanTransformer theXalanTransformer;

		istrstream theXMLStream(whole_file.c_str(), strlen(whole_file.c_str()));
		
		//XALAN_USING_XALAN(XSLTInputSource)
		XALAN_USING_XALAN(XSLTResultTarget)

		XSLTResultTarget result(os);

		// Do the transform.
		theResult = theXalanTransformer.transform(&theXMLStream, "c:\\devel\\dtbook2xhtml.xsl", os);

		// Terminate Xalan...
		XalanTransformer::terminate();

		//this crashes.
		// Terminate Xerces...
		//XMLPlatformUtils::Terminate();

		// Clean up the ICU, if it's integrated...
		//XalanTransformer::ICUCleanUp();
		
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;
}