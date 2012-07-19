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

#include "dtb/TransformDTBook.h"
#include "util/Log.h"
#include "util/FilePathTools.h"
#include <fstream>
#include <iostream>

bool has_java();
string createTempFileName();

amis::dtb::TransformDTBook::TransformDTBook()
{
	this->TEMPFILEINDEX.assign("dtbtemps");
}
amis::dtb::TransformDTBook::~TransformDTBook()
{
}

void amis::dtb::TransformDTBook::setTempDir(string tempdir) 
{
	#ifdef AMIS_COMPILER_MSVC
	USES_CONVERSION;

	TCHAR szBuffer[256];
	GetModuleFileName(NULL, szBuffer, 256);
	CString cstr_app_path = szBuffer;
	int pos = cstr_app_path.ReverseFind('\\');
	if (pos >= 0) cstr_app_path = cstr_app_path.Mid(0, pos + 1);
	
	mBin = W2CA(cstr_app_path);
	mTempFiles.clear();
	
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

void amis::dtb::TransformDTBook::removeTempFiles() 
{
	if (mTempFiles.size() == 0)
	{
		loadTempFileReferences();
	}
	// delete any temp files that we've created
	StringMap::iterator it;
	for (it = mTempFiles.begin(); it != mTempFiles.end(); it++)
	{
		string filepath = it->second;
		
#ifdef AMIS_COMPILER_MSVC
		remove(filepath.c_str());
#else
		//todo: something for other platforms
#endif
		
	}

string dtbtempsfile = mTempdir + this->TEMPFILEINDEX;
#ifdef AMIS_COMPILER_MSVC
	remove(dtbtempsfile.c_str());
#else
	//todo: something for other platforms
#endif
	
}
// get results for a filename
string amis::dtb::TransformDTBook::getResults(string filename)
{
	//amis::util::Log::Instance()->writeTrace("*** Retrieving transformation " + filename);
	readResults(filename);
	return mResults;
}
//put the transformed version of a file into mResults
void amis::dtb::TransformDTBook::readResults(string filename)
{
	if (mTempFiles.size() == 0)
	{
		loadTempFileReferences(); // read the temp file index from disk	
	}
	ifstream f;
	// filename is a key to the mTempFiles map
	string filepath = getTempFileName(filename);
	
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

}

// transform the file at @filepath and save it as tempdir/tempfilename.
// the correlation between the input filename and the temp filename is stored in mTempFiles
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
	string tempfilename = mTempdir + createTempFileName();

	mTempFiles[filepath] = tempfilename;

	//the parameters to our java dtbook-transformation program are:
	//-cp jarfile mainFunction path/to/book/dtbook.xml output.xml transform.xsl baseDir=path/to/book/
	string params = " -cp \"" + 
		mBin + "xslt\\org.daisy.util.jar\" org.daisy.util.xml.xslt.Stylesheet \"" + 
		local_dtbook + "\" \"" + 
		tempfilename + "\" \"" + 
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

string amis::dtb::TransformDTBook::getTempFileName(string filenameKey)
{
	return mTempFiles[filenameKey];
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
// this class needs to share data between two different processes
// the 2 processes will not overlap so it's ok to use a temp file to store the data
// the format is:
// line 1: orig. filepath
// line 2: temp file path
// ...etc...
void amis::dtb::TransformDTBook::loadTempFileReferences()
{
	string path = mTempdir + this->TEMPFILEINDEX;
	ifstream f;
	f.open(path.c_str(), ios::in);
	if (f.fail()) return;

	while (!f.eof())
	{
		string s1 = "";
		string s2 = "";
		getline(f, s1);
		getline(f, s2);
		// put the file data into the hash map
		mTempFiles[s1] = s2;
	}

	f.close();
}

// call when done transforming all a book's files
// it will write a temp file containing an index of the temp files generated
void amis::dtb::TransformDTBook::writeTempFileReferences()
{
	string path = mTempdir + this->TEMPFILEINDEX;
	ofstream f;
	f.open(path.c_str(), ios::out);
	if (f.fail()) return;
	
	amis::StringMap::iterator it;
	for (it = mTempFiles.begin(); it != mTempFiles.end(); it++)
	{
		f<<it->first<<endl;
		f<<it->second<<endl;
	}
	f.close();
}
string createTempFileName() 
{
	char buffer[256];
	tmpnam (buffer);
	string name(buffer);
	return name;
}