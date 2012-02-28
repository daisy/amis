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

#if defined(AMIS_COMPILER_MSVC)
//windows header containing charset conversion macros
#include <afxpriv.h>
#endif

#if 0
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include "util/xercesutils.h"
#endif

#include "dtb/TransformDTBook.h"
#include "util/Log.h"
#include "util/FilePathTools.h"
#include <fstream>
#include <iostream>


bool has_java();

#if 0
amis::dtb::TransformDTBook* amis::dtb::TransformDTBook::pinstance = 0;

amis::dtb::TransformDTBook* amis::dtb::TransformDTBook::Instance()
{
	if (pinstance == 0) pinstance = new amis::dtb::TransformDTBook(); 
    return pinstance;
}

void amis::dtb::TransformDTBook::DestroyInstance()
{
	delete pinstance;
}
#endif
amis::dtb::TransformDTBook::TransformDTBook(string tempdir)
{
#if 0
	mpDoc = NULL;
#endif
#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;

	TCHAR szBuffer[256];
	GetModuleFileName(NULL, szBuffer, 256);
	CString cstr_app_path = szBuffer;
	int pos = cstr_app_path.ReverseFind('\\');
	if (pos >= 0) cstr_app_path = cstr_app_path.Mid(0, pos + 1);
	
	mBin = W2CA(cstr_app_path);

	
#if _DEBUG
	// for development purposes: replace the networked VMWare path with a local path
	string search = "\\\\vmware-host\\shared folders";
	string replacement = "z:";
	int pos2 = mBin.find(search);
	if (pos2 >= 0) mBin.replace(pos2, strlen(search.c_str()), replacement);
 
#endif

#else
	mBin = "";
#endif

	mTempdir = tempdir;
}
amis::dtb::TransformDTBook::~TransformDTBook()
{
}
string amis::dtb::TransformDTBook::getResults()
{
	readResults();
	return mResults;
}
//put the results into mResults
void amis::dtb::TransformDTBook::readResults()
{
	ifstream f;
	string filepath = mTempdir + "tmpdtbook.xml";
	f.open(filepath.c_str());
	if (f.fail())
	{
		mResults = "";
		return;
	}

	mResults = "";
	while (!f.eof())
	{
		string tmp;
		getline(f, tmp);
		mResults += tmp;
	}
	f.close();
#ifdef AMIS_COMPILER_MSVC
	remove(filepath.c_str());
#else
	//todo: something for other platforms
#endif

}
bool amis::dtb::TransformDTBook::transform(string filepath)
{	
	if (filepath == "") {
		return false;
	}

	if (!has_java())
	{
		amis::util::Log::Instance()->writeWarning(
			"Could not transform DTBook because Java was not found", 
			"amis::dtb::Dtb::TransformDTBook()");
		return false;
	}

	bool retval = false;
#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;
	string bookdir = amis::util::FilePathTools::getParentDirectory(filepath);
	bookdir += "/";
	string local_dtbook = amis::util::FilePathTools::getAsLocalFilePath(filepath);

	//the parameters to our java dtbook-transformation program are:
	//-cp jarfile mainFunction path/to/book/dtbook.xml output.xml transform.xsl baseDir=path/to/book/
	string params = " -cp \"" + 
		mBin + "xslt\\org.daisy.util.jar\" org.daisy.util.xml.xslt.Stylesheet \"" + 
		local_dtbook + "\" \"" + 
		mTempdir + "tmpdtbook.xml\" \"" + 
		mBin + "xslt\\dtbook\\dtbook2xhtml.xsl\" baseDir=\"" + 
		bookdir + "\"";

	string exe =  "java";

	SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_HIDE;
    sei.lpVerb = _T("open");
    sei.lpFile = A2T(exe.c_str());
	sei.lpParameters = A2T(params.c_str());
	ShellExecuteEx(&sei);
	::WaitForSingleObject(sei.hProcess, INFINITE);
	::CloseHandle(sei.hProcess);

	if (ShellExecuteEx(&sei) == FALSE)
	{
		amis::util::Log::Instance()->writeTrace("error converting dtbook ");
		mResults = "";
		retval = false;
	}
	else
	{
		amis::util::Log::Instance()->writeTrace("converted dtbook ");
		retval = true;
	}
#else
	//TODO: something for other platforms
#endif
	return retval;
}


bool has_java()
{
#ifdef AMIS_PLATFORM_WINDOWS
	USES_CONVERSION;
	HKEY hKey;
	//"ERROR_SUCCESS" means it worked
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		L"SOFTWARE\\JavaSoft\\Java Runtime Environment", 
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char buf[255] = {0};
		DWORD type = REG_SZ;
		DWORD size = 255;
		if (RegQueryValueEx(hKey, L"CurrentVersion", 
			NULL, &type, (LPBYTE)buf, &size) == ERROR_SUCCESS)
		{
			string v = "";
			for (int i = 0; i<255; i++)
				if (buf[i]) v += buf[i];
			
			RegCloseKey(hKey);
			float version = atof(v.c_str());
			if (version >= REQUIRED_JAVA_VERSION)
				return true;
		}
		else
		{	
			RegCloseKey(hKey);
		}
	}
	return false;
#endif
	return false;
}
