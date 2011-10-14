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
#ifndef FILEPATHTOOLS_H
#define FILEPATHTOOLS_H

#include <string>
#include "ambulant/net/url.h"
#include "AmisCore.h"

using namespace std;

namespace amis
{
namespace util
{
std::string ambulantUrlToString(const ambulant::net::url*);
string calculateRelativeFilename(const ambulant::net::url*, const ambulant::net::url*);
string ambulantUrlToStringWithRef(const ambulant::net::url*);
string getFileNameWithRef(const ambulant::net::url*);
string getFileNameWithRef(string url);
ambulant::net::url makeUrlFromString(std::string, bool forceToUrl = false, bool forceToLocal = false);

//!File path tools: utility functions for working with file paths
class FilePathTools
{
public:
	//!get the file name (file.ext) from a string
	static string getFileName(string);
	//!get the target from a string (#target)
	static string getTarget(string);
	//!get the file extension
	static string getExtension(string);
	//!get as a local file path
	static string getAsLocalFilePath(string);
	//!clear the target from a string
	static string clearTarget(string);
	//!calculate a new path based on a starting path and a relative path
	static string goRelativePath(string, string);
	//!convert the slashes to go forward
	static string convertSlashesFwd(string);
	//!convert the slashes to go backward
	static string convertSlashesBkw(string);
	//!get the parent directory.  input can be a file or directory path.
	static string getParentDirectory(string);
	static string getAsUrlPath(string);
	static bool urlListContains(string, amis::UrlList*);
};
}
}
#endif
