/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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
#ifndef DTBINDEX_H
#define DTBINDEX_H

#include "AmisCore.h"
#include "ambulant/net/url.h"
#include "dtb/Dtb.h"

namespace amis
{
namespace dtb
{
//this class handles the job of indexing the book, piece by piece
//indexing speeds up processing and searching
class DtbIndex
{
public:
	DtbIndex();
	~DtbIndex();
	void initialize(amis::dtb::Dtb*);
	void indexLocation(const ambulant::net::url*);
	bool wasFileIndexedAlready(const ambulant::net::url*);
	void calculateNodeRangeData(const ambulant::net::url*);
private:
	amis::StringMap mTextIdToSmilAddressMap;
	amis::UrlPtrToStringListMap mSmilFileToSmilIdMap;
	amis::dtb::nav::NodeRefMap mSmilIdToNavNodeMap;
	amis::dtb::Dtb* mpDtb;
};
}
}
#endif