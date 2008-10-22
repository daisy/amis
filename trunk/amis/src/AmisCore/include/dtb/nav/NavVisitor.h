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
#ifndef NAVVISITOR_H
#define NAVVISITOR_H

#include "dtb/Spine.h"
#include <iostream>
#include <string>
#include <vector>
#include "AmisCore.h"

#include "util/ThreadYielder.h"

using namespace std;

namespace amis
{
namespace dtb
{
namespace nav
{
class NavModel;
class NavNode;
class NavPoint;
class PageTarget;

class NavVisitor
{
public:
	NavVisitor(){}
	virtual ~NavVisitor() {}
	virtual bool preVisit(amis::dtb::nav::NavNode*){return false;}
	
	inline void setThreadYielder(ThreadYielder * ty) {mThreadYielder = ty;}

protected:
	ThreadYielder * mThreadYielder;
};

class BuildSpineVisitor : public NavVisitor
{
public:
	BuildSpineVisitor() {mpSpine = NULL;}
	~BuildSpineVisitor() {}
	amis::dtb::Spine* getSpine(NavModel*, const ambulant::net::url*);
	bool preVisit(NavNode*);

private:
	amis::dtb::Spine* mpSpine;
	const ambulant::net::url* mpBookDirectory;
};


class ResolveSmilDataVisitor : public NavVisitor
{
public:
	ResolveSmilDataVisitor(){mpMap = NULL; mpTextMap = NULL;}
	~ResolveSmilDataVisitor(){}
	void resolve(NavModel*, Spine*, /*const ambulant::net::url*,*/ bool);
	bool preVisit(NavNode*);
	//call this after calling resolve(..) to get a map of text IDs and their smil src refs
	amis::StringMap* getSmilTextMap();
private:
	amis::StdStringList getRange(string, string);
	void printMap(bool = false);
	void addToMap(string, NavNode*);

private:
	amis::dtb::nav::NodeRefMap* mpMap;
	amis::StringMap* mpTextMap;
	amis::StdStringList mBigIdList;
	int mPhase;

	amis::dtb::nav::NavNode* mpPreviousNavNode;
};

class WhoRefersToThisSmilFile : public NavVisitor
{
public:
	WhoRefersToThisSmilFile(){mpNodes = NULL;}
	~WhoRefersToThisSmilFile(){}

	NavNodeList* findOut(NavModel*, const ambulant::net::url*);
	bool preVisit(NavNode*);
private:
	NavNodeList* mpNodes;
	ambulant::net::url mFilename;
};

class VisitTheTitle : public NavVisitor
{
public:
	VisitTheTitle(){mpTitle = NULL;};
	~VisitTheTitle(){};

	amis::MediaGroup* getTheTitle(amis::dtb::nav::NavModel*);
	bool preVisit(NavNode*);
private:
	amis::MediaGroup* mpTitle;
};
}
}
}
#endif
