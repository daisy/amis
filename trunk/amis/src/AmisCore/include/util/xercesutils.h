/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

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
#ifndef XERCESUTILS_H
#define XERCESUTILS_H

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>

// Note: This class is something I borrowed from the Xerces example
// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode); 
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

//end of borrowed Xerces Code.

//make getting attribute values easier!
class SimpleAttrs
{
public:
	static string get(string attribute_name, const Attributes* pAttrs)
	{
		XMLCh* attr_name = XMLString::transcode(attribute_name.c_str());
		char* attr_val = NULL;
		attr_val = XMLString::transcode(pAttrs->getValue(attr_name));
		string return_value = "";
		if (attr_val != NULL)
		{
			return_value.assign(attr_val);
			XMLString::release(&attr_val);
		}
		XMLString::release(&attr_name);
		return return_value;
	}

	static wstring getw(string attribute_name, const Attributes* pAttrs)
	{
		XMLCh* attr_name = XMLString::transcode(attribute_name.c_str());

#if defined (AMIS_COMPILER_GCC)
//note!! we assumed that mbstowcs uses UTF8 for the narrow string
		char* temp = XMLString::transcode(pAttrs->getValue(attr_name));
		int len = strlen(temp);
		wchar_t* wtemp = new wchar_t[len+1];
		mbstowcs(wtemp, temp, len);
		wstring return_value;
		return_value.assign(wtemp);
		delete wtemp;
		XMLString::release(&temp);
#elif defined (AMIS_COMPILER_MSVC)
		wstring return_value = pAttrs->getValue(attr_name);
#endif
		XMLString::release(&attr_name);

		return return_value;
	}
};

#endif
