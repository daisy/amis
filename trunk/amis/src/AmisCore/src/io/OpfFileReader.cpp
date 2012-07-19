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

#include <iostream>

#include "util/FilePathTools.h"
#include <xercesc/sax2/Attributes.hpp>
#include "io/OpfFileReader.h"
#include "util/xercesutils.h"

amis::io::OpfFileReader::OpfFileReader()
{
}
amis::io::OpfFileReader::~OpfFileReader()
{
}

bool amis::io::OpfFileReader::readFromFile(const ambulant::net::url* filename)
{
	mTempChars.erase();
	mbFlag_GetChars = false;
	mbFlag_InSpine = false;
	mbFlag_InManifest = false;
	mpMetadata = new amis::dtb::Metadata();
	mpSpine = new amis::dtb::Spine();
	mUnsortedSmilFiles.clear();
	mItemRefs.clear();
	mTextFilenames.clear();
	mpFilename = filename;
	bool b_return_value = amis::io::XercesSaxParseBase::parseFile(filename);
	sortSpine();

	return b_return_value;
}

amis::dtb::Spine* amis::io::OpfFileReader::getSpine()
{
	return mpSpine;
}

amis::dtb::Metadata* amis::io::OpfFileReader::getMetadata()
{
	return mpMetadata;
}

const ambulant::net::url* amis::io::OpfFileReader::getNavFilename()
{
	return &mNavFilename;
}

const ambulant::net::url* amis::io::OpfFileReader::getResourceFilename()
{
	return &mResourceFilename;
}

const amis::UrlList* amis::io::OpfFileReader::getTextFilenames()
{
	return &mTextFilenames;
}

//!xerces start element event
void amis::io::OpfFileReader::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{
	char *node_name_ = XMLString::transcode(qname);
	string node_name;
	node_name.assign(node_name_);
	XMLString::release(&node_name_);

	if (node_name.compare("manifest") == 0)
	{
		mbFlag_InManifest = true;
	}
	else if (node_name.compare("spine") == 0)
	{
		mbFlag_InSpine = true;
	}
	//all the filenames gathered here are stored as full paths
	else if (node_name.compare("item") == 0 && mbFlag_InManifest == true)
	{
		bool is_local = mpFilename->is_local_file();
		string id = SimpleAttrs::get("id", &attributes);
		string media_type = SimpleAttrs::get("media-type", &attributes);

		if (id == "ncx")
		{
			string filename = SimpleAttrs::get("href", &attributes);
			ambulant::net::url temp = amis::util::makeUrlFromString(filename, !is_local, is_local);
			mNavFilename = temp.join_to_base(*mpFilename);
		}
		else if (id == "resource")
		{
			string filename = SimpleAttrs::get("href", &attributes);
			ambulant::net::url temp = amis::util::makeUrlFromString(filename, !is_local, is_local);
			mResourceFilename = temp.join_to_base(*mpFilename);
		}

		if (media_type == "application/x-dtbook+xml")
		{
			string filename = SimpleAttrs::get("href", &attributes);
			ambulant::net::url temp = amis::util::makeUrlFromString(filename, !is_local, is_local);
			mTextFilenames.push_back(temp.join_to_base(*mpFilename));
		}
		//add the smil files to the manifest list
		//the media type "text/sml" is used in bookshare books
		//and "application/smil+xml" is used by a Pipeline script (at least in one example, though that
		// may have been fixed)
		else if ((media_type == "application/smil" || media_type == "text/sml" 
					|| media_type == "application/smil+xml") && id.size() > 0)
		{
			string filename = SimpleAttrs::get("href", &attributes);
			ambulant::net::url temp = amis::util::makeUrlFromString(filename, !is_local, is_local);
			mUnsortedSmilFiles[id] = temp.join_to_base(*mpFilename);
		}
	}
	else if (node_name.compare("itemref") == 0 && mbFlag_InSpine == true)
	{
		string item_id;
		item_id.assign(SimpleAttrs::get("idref", &attributes));
		mItemRefs.push_back(item_id);
	}
	else if (node_name.compare("meta") == 0 ||
		node_name.substr(0, 3).compare("dc:") == 0)
	{
		amis::dtb::MetaItem* p_meta_item = NULL;
		//just a plain "meta" tag with all data in attributes
		if (node_name.compare("meta") == 0)
		{
			mbFlag_GetChars = false;
			p_meta_item = new amis::dtb::MetaItem();
			string meta_name = SimpleAttrs::get("name", &attributes);
			wstring meta_content = SimpleAttrs::getw("content", &attributes);
			p_meta_item->mName.assign(meta_name);
			p_meta_item->mContent = meta_content;
		}
		//it's a dublin core (with character data)
		else
		{
			mbFlag_GetChars = true;
			if (node_name == "dc:Creator" && mpMetadata->getMetadata("dc:creator") != NULL)
			{
				p_meta_item = mpMetadata->getMetadata("dc:creator");
				if (p_meta_item == NULL) p_meta_item = new amis::dtb::MetaItem();
			}
			else
			{
				p_meta_item = new amis::dtb::MetaItem();
				p_meta_item->mName.assign(node_name);
			}
		}
		mpCurrentMetaItem = p_meta_item;
		mpMetadata->addMetadata(p_meta_item);
		mTempChars.erase();
	}
}

//!xerces end element event
void amis::io::OpfFileReader::endElement(const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname)
{
	char* element_name = XMLString::transcode(qname); 
	string node_name;
	node_name.assign(element_name);
	XMLString::release(&element_name); 

	if (node_name.substr(0, 3).compare("dc:") == 0 ||
		node_name.compare("meta") == 0 && mpCurrentMetaItem != NULL)
	{
		if (mpCurrentMetaItem->mContent.size() > 0)
		{
			mpCurrentMetaItem->mContent.append(L", ");
			mpCurrentMetaItem->mContent.append(mTempChars);
		}
		else
		{
			mpCurrentMetaItem->mContent.assign(mTempChars);
		}
	}
	else if (node_name.compare("manifest") == 0)
	{
		mbFlag_InManifest = false;
	}
	else if (node_name.compare("spine") == 0)
	{
		mbFlag_InSpine = false;
	}
}

void amis::io::OpfFileReader::characters(const XMLCh* const chars, const XMLSize_t length)
{
	if (mbFlag_GetChars == true) mTempChars.append((wchar_t*)chars);
}

void amis::io::OpfFileReader::sortSpine()
{
	unsigned int i;
	string search_id;
	
	for (i = 0; i<mItemRefs.size(); i++)
	{
		search_id = mItemRefs[i];
		if (mUnsortedSmilFiles[search_id].is_empty_path() == false)
		{
			//we can always skip the check to see if the file already exists
			//in the spine; this check is redundant for an OPF file (but not for an NCC)
			mpSpine->addFile(&mUnsortedSmilFiles[search_id], true);
		}
	}
}
