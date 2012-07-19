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
#ifndef TRANSFORMDTBOOK_H
#define TRANSFORMDTBOOK_H


#include <string>
#include <vector>
#include "AmisCore.h"

using namespace std;

#define REQUIRED_JAVA_VERSION				1.6

namespace amis
{
namespace dtb
{
	class TransformDTBook
	{
	public:
		TransformDTBook();	
		~TransformDTBook();

		void setTempDir(string);
		bool transform(string);
		string getResults(string);
		void removeTempFiles();
		void writeTempFileReferences();
	private:
		void readResults(string);
		string getTempFileName(string);
		void loadTempFileReferences();
		
		string mResults;
		string mBin;
		string mTempdir;
		// mTempFiles is a hash map of transformed files. the original file location is the key and the mangled temporary file is the value.
		amis::StringMap mTempFiles;

		string TEMPFILEINDEX;

	};
}
}
#endif
