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
#ifndef MODULEDESCDATA_H
#define MODULEDESCDATA_H

#include "Media.h"
#include <string>
#include "ambulant/net/url.h"

using namespace std;

namespace amis
{
class ModuleDescData
{

public:
	ModuleDescData();
	~ModuleDescData();

	enum ModuleType
	{
		NONE,
		LANGPACK,
		LANGPACK_RTL,
		PLUGIN
	};

	amis::MediaGroup* getLabel();
	string getId();
	ModuleType getModuleType();
	string getDllFileName();
	const ambulant::net::url* getXmlFileName();
	bool isEnabled();

	void setLabel(amis::MediaGroup*);
	void setId(string);
	void setModuleType(amis::ModuleDescData::ModuleType);
	void setDllFileName(string);
	void setXmlFileName(ambulant::net::url);
	void setIsEnabled(bool);

private:
	amis::MediaGroup* mpLabel;
	string mId;
	amis::ModuleDescData::ModuleType mType;
	string mDllFileName;
	ambulant::net::url mXmlFileName;
	bool mEnabled;

};
}
#endif
