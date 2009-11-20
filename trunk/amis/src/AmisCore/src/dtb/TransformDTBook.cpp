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


amis::dtb::TransformDTBook::TransformDTBook()
{
#if 0
	mpDoc = NULL;
#endif
}
amis::dtb::TransformDTBook::~TransformDTBook()
{
}
string amis::dtb::TransformDTBook::getResults()
{
	ifstream f;
	string filepath = mBin + "tmpdtbook.xml";
	f.open(filepath.c_str());
	if (f.fail()) return "";

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
	return mResults;
}

bool amis::dtb::TransformDTBook::transform(string filepath)
{
	bool retval = false;
#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;

	TCHAR szBuffer[256];
	GetModuleFileName(NULL, szBuffer, 256);
	CString cstr_app_path = szBuffer;
	int pos = cstr_app_path.ReverseFind('\\');
	if (pos >= 0) cstr_app_path = cstr_app_path.Mid(0, pos + 1);
	mBin = W2CA(cstr_app_path);
	string bookdir = amis::util::FilePathTools::getParentDirectory(filepath);
	bookdir += "/";
	string local_dtbook = amis::util::FilePathTools::getAsLocalFilePath(filepath);

	//the parameters to our java dtbook-transformation program are:
	//-cp jarfile mainFunction path/to/book/dtbook.xml output.xml transform.xsl baseDir=path/to/book/
	string params = " -cp \"" + 
		mBin + "xslt\\org.daisy.util.jar\" org.daisy.util.xml.xslt.Stylesheet \"" + 
		local_dtbook + "\" \"" + 
		mBin + "tmpdtbook.xml\" \"" + 
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