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


//input:
// wholeFile: a string containing the entire file
// xsltpath: the path to the xslt file
// outputStream: output stream that will receive the transformed output
bool dtbookfixer(std::string& wholeFile, std::string xsltpath, std::ostream& outputStream)
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
		XalanTransformer xalan;

		istrstream input_stream(wholeFile.c_str(), strlen(wholeFile.c_str()));
		XALAN_USING_XALAN(XSLTResultTarget)

		XSLTResultTarget result(outputStream);

		// Do the transform.
		int success = xalan.transform(&input_stream, xsltpath.c_str(), outputStream);
		
		// Terminate Xalan...
		XalanTransformer::terminate();

		//this crashes.
		// Terminate Xerces...
		//XMLPlatformUtils::Terminate();
		
		return true;
	}
	catch(...)
	{
		return false;
	}

	return false;
}