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

#ifndef DTBWITHHOOKS_H
#define DTBWITHHOOKS_H

#include "util/SearchForFilesMFC.h"
#include "dtb/Dtb.h"

namespace amis
{
namespace dtb
{
//the reasons for using this class (instead of just using the base dtb class) are:
//1. it's a singleton
//2. it hooks into the GUI components that we have
class DtbWithHooks : public Dtb
{
protected:
	DtbWithHooks();
public:
	static DtbWithHooks* Instance();
	static void ttsTwoDone();
	static bool isNull();
	void DestroyInstance();
	~DtbWithHooks();
	bool open(const ambulant::net::url*, const ambulant::net::url*, bool saveInHistory=true);
	smil::SmilMediaGroup* startReading(bool);
	smil::SmilMediaGroup* nextPhrase();
	smil::SmilMediaGroup* previousPhrase();
	void nextSection();
	void previousSection();
	void nextPage();
	void previousPage();
	void nextSmilDocument();
	void previousSmilDocument();
	void loadNavNode(nav::NavNode*);
	void updateCustomTestStates(bool playAll = false);
	amis::dtb::Bookmark* addBookmark();
	void loadBookmark(int);
	amis::dtb::smil::SmilMediaGroup* loadSmilFromUrl(const ambulant::net::url*);
	bool canGoToNextPhrase();
	bool canGoToPreviousPhrase();
	bool canGoToNextSection();
	bool canGoToPreviousSection();
	bool canGoToNextPage();
	bool canGoToPreviousPage();

	//part of a workaround to stop highlighting of text nodes
	//this highlighting happens sometimes to nodes that should be skipped
	//the node_started/node_stopped events are over before the highlighting commands get sent
	//i don't know where they come from..
	//it only happens when we try to load the lastmark of a document (url#frag; ambulant not started playing yet)
	string getIdOfLastmarkNode();
	bool getIsWaitingForLastmarkNode();
	void setIsWaitingForLastmarkNode(bool);

	bool hasAudio();
	bool isPlaying();
	void pause();
	void resume();
	void speakTTS(wstring);
	//bool getTTSNextPhraseFlag();
	//void setTTSNextPhraseFlag(bool);
	void stopTTS();

private:
	void pauseTTS();
	void resumeTTS();
	
	void addToHistory();
	void makeAllLabelsHumanReadable();
	void makeLabelHumanReadable(amis::MediaGroup*, std::string);

	//this object is instantiated and destroyed by this class, but otherwise is managed by the base class
	amis::util::SearchForFilesMFC* mpFileSearcherTmp;
	
	//part of a workaround to stop highlighting of text nodes
	//this highlighting happens sometimes to nodes that should be skipped
	//the node_started/node_stopped events are over before the highlighting commands get sent
	//i don't know where they come from..
	//it only happens when we try to load the lastmark of a document (url#frag; ambulant not started playing yet)
	bool mbIsWaitingForLastmarkNode;
	string mIdOfLastmarkNode;
	bool mbTTSNextPhraseFlag;
	wstring mLastString;
private:
	static DtbWithHooks* pinstance;
};
}
}
#endif