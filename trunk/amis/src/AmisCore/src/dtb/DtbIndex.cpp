/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

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
#include "dtb/DtbIndex.h"
#include "io/QuickDataSmilFileReader.h"
#include "util/FilePathTools.h"
#include "dtb/nav/NavNode.h"
#include "AmisCore.h"

using namespace std;

amis::dtb::DtbIndex::DtbIndex()
{
}
amis::dtb::DtbIndex::~DtbIndex()
{
}
void amis::dtb::DtbIndex::initialize(amis::dtb::Dtb* pDtb)
{
	mpDtb = pDtb;
	//initalize the map with the names of each SMIL file
	amis::dtb::Spine* p_spine = mpDtb->getSpine();
	amis::StdStringList empty_list;
	empty_list.empty();
	for (int i = 0; i<p_spine->getNumberOfSmilFiles(); i++)
	{
		mSmilFileToSmilIdMap[p_spine->getSmilFilePath(i)]= empty_list;
	}
}
//parse the file and add its data to the indices	
void amis::dtb::DtbIndex::indexLocation(const ambulant::net::url* pLocation)
{
	if (wasFileIndexedAlready(pLocation) == true) return;
	amis::io::QuickDataSmilFileReader quick_reader;
	//get all the ids and text src's from this smil file.  also fill in the audio data for the nav nodes.
	//big id list is a list of all SMIL element IDs
	//text map is a map of the smilfile name and the text src in it
	//p_nav_nodes (NULL) could be a list of pointers to nav nodes that refer to this smil file
	//if so, then the nav node audio bits would get filled in
	amis::StdStringList id_list;
	if (!quick_reader.readFromFile(pLocation, &id_list, &mTextIdToSmilAddressMap, NULL)) 
		return;
	//store the file name and all its IDs
	mSmilFileToSmilIdMap[pLocation] = id_list;
	calculateNodeRangeData(pLocation);
}
bool amis::dtb::DtbIndex::wasFileIndexedAlready(const ambulant::net::url* file)
{
	if (mSmilFileToSmilIdMap[file].size() > 0) return true;
	else return false;
}
//this function assumes the given file has been indexed already and that data is available
void amis::dtb::DtbIndex::calculateNodeRangeData(const ambulant::net::url* file)
{
	amis::dtb::nav::WhoRefersToThisSmilFile who_visitor;
	amis::dtb::nav::NavNodeList* p_nodes = who_visitor.findOut(mpDtb->getNavModel(), file);
	
	//now we have all the nav nodes that refer to the given smil file

	//get a range for each one -- get its start ID from Node.content
	//get its end ID:
	// - end of the file
	// - right before the start of the node of the same type (e.g. 2 NavPoints)
	//This assumes that NavPoint ranges (at the deepest section depth) do not cross SMIL file boundaries 
	//this is a safe assumption for NavPoints (in practice).
	//However, this may not give an accurate range for pages.  but do we need those now that we have the new
	//play order list?
	//the ranges are being calculated as the indexing is occuring, so using the end of the file is
	//convenient, since the next or previous file might not be indexed when we would need it if we
	//wanted to cross SMIL file boundaries.

	//add each ID in the range to mSmilIdToNavNodeMap
}
