/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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
#include "AmisCore.h"

using namespace std;

amis::dtb::DtbIndex::DtbIndex()
{
}
amis::dtb::DtbIndex::~DtbIndex()
{
}

//parse the file and add its data to the indices	
void amis::dtb::DtbIndex::indexLocation(const ambulant::net::url* pLocation)
{
	string smil_file_name = amis::util::FilePathTools::getFileName(pLocation->get_file());
	if (wasFileIndexedAlready(smil_file_name) == true) return;
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
	mSmilFileToSmilIdMap[smil_file_name] = id_list;

}
//This function DOES NOT WORK!!!
bool amis::dtb::DtbIndex::wasFileIndexedAlready(string file)
{
	if (mSmilFileToSmilIdMap.size() > 0 ) //&& key "file" exists
	{
		return true;
	}
	else
	{
		return false;
	}
}