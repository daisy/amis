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
#ifndef SPINE_H
#define SPINE_H

#include <string>
#include <vector>
#include "ambulant/net/url.h"

using namespace std;

namespace amis
{
namespace dtb
{
/*!
	The Spine keeps track of the in-order list of all SMIL files that 
	make up a Daisy book.
 */
class Spine
{
public:
	Spine();
	~Spine();

//note: removed forced value on bool (skipCheck = false) for python linking
	void addFile(const ambulant::net::url*, bool);
	//void addFile(string, bool);

	bool isFilePresent(const ambulant::net::url*);
	bool isEmpty();

	const ambulant::net::url* getNextFile();
	const ambulant::net::url* getPreviousFile();
	const ambulant::net::url* getFirstFile();
	const ambulant::net::url* getLastFile();
	bool goToFile(const ambulant::net::url*);
	void print();
	int getNumberOfSmilFiles();
	const ambulant::net::url* getSmilFilePath(unsigned int);

private:
	vector<ambulant::net::url> mSpineList;
	unsigned int mListIndex;
};
}
}
#endif
