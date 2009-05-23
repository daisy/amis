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
// AmisApp.h : main header file for the AmisGuiMFC2 application
//
#ifndef AMISGUIMFC2_H
#define AMISGUIMFC2_H

#pragma once
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///This gets rid of some warnings
#pragma warning(disable : 4049)

#include "../../resource.h"       // main symbols
#include <string>

#include "ambulant/net/url.h"
#include "gui/MainFrm.h"
#include "BookList.h"

#include "gui/self-voicing/AudioSequencePlayer.h"
#include "gui/self-voicing/datamodel/AccessibleUIDataReader.h"

#define WITH_DAISY_PDTB
#define AMIS_MAX_ANYTHING					49
#define AMIS_MAX_AUDIO_RATE					8.0
#define AMIS_MAX_RECENT_BOOKS				5

//#ifndef AVOID_SELF_VOICING_COM_STUFF
//#define AVOID_SELF_VOICING_COM_STUFF
//#endif

namespace amis
{
namespace gui
{
class CAmisApp : public CWinApp
{
public:
	CAmisApp();

	static std::string getPromptIDFromSideBarName(std::string item_id);
	
	static std::wstring pauseBookAndEmitMessage(std::string msg);
	static std::wstring pauseBookAndEmitMessage(std::string msg1, std::string msg2);

	static std::wstring emitMessage(std::string msg, bool repeat = false);

	static bool beforeModalBox();
	static void afterModalBox(bool b_was_playing);
	static void generalBookErrorMsgBox(CString str);

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	std::string getAppPath();
	std::string getAppSettingsPath();
	amis::BookList* getHistory();
	bool isBookOpen();
	bool shouldIgnoreOpenDocEvent();
	bool canIncreasePlaybackSpeed();
	bool canDecreasePlaybackSpeed();
	bool getIsWaiting();
	void setIsWaiting(bool);
	bool getShouldNotRenderAudio();

	//Menu handlers
	afx_msg void OnFileOpen();
	afx_msg void OnPlayPause();
	afx_msg void OnNavPrevPhrase();
	afx_msg void OnNavNextPhrase();
	afx_msg void OnFileExit();
	afx_msg void OnFileRecentBook(UINT);
	afx_msg void OnNavNextSection();
	afx_msg void OnNavPreviousSection();
	afx_msg void OnNavShowSectionDepth(UINT);
	afx_msg void OnNavNextPage();
	afx_msg void OnNavPrevPage();
	afx_msg void OnFileClose();
	afx_msg void OnAbout();
	afx_msg void OnViewSidebarOnoff();
	afx_msg void OnPlayEscape();
	afx_msg void OnFileOpenFromUrl();
	afx_msg void OnSkipInfo();
	afx_msg void OnAddBookmark();
	afx_msg void OnSelectBookmark(UINT);
	afx_msg void OnGoToPage();
	afx_msg void OnIncreaseFontSize();
	afx_msg void OnDecreaseFontSize();
	afx_msg void OnToggleContrast();
	afx_msg void OnSearchForBooks();
	afx_msg void OnLoadCd();
	afx_msg void OnVolumeUp();
	afx_msg void OnVolumeDown();
	afx_msg void OnVolumeUpBOOK();
	afx_msg void OnVolumeDownBOOK();
	afx_msg void OnVolumeUpUI();
	afx_msg void OnVolumeDownUI();
	afx_msg void OnVolumeDownTTS();
	afx_msg void OnVolumeUpTTS();
	afx_msg void OnSpeedUp();
	afx_msg void OnSpeedDown();
	afx_msg void OnSpeedNormal();
	afx_msg void OnPreferences();
	afx_msg void OnPublicationSummary();
	afx_msg void OnToggleView();
	afx_msg void OnClearPageStyle();
	afx_msg void OnApplyPageStyle(UINT);
	afx_msg void OnNextPageStyle();
	afx_msg void OnShowTextStyle();
	afx_msg void OnChangeTab(UINT);
	afx_msg void OnShowFindInText();
	afx_msg void OnFindNextInText();
	afx_msg void OnFindPreviousInText();
	afx_msg void OnFocusOnSidebar();
	afx_msg void OnToggleSelfVoicingAudio();
	afx_msg void OnFocusOnText();
	afx_msg void OnResetHighlightColors();
	afx_msg void OnToggleContentAudio();
	afx_msg void OnShowHelpContents();
	afx_msg void OnShowKeyboardShortcuts();
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

private:
	void initializePathsAndFiles(string preferencesFile="");
	void initializeSelfVoicing();
	bool openBook(const ambulant::net::url* url, bool saveInHistory = true);
	void openLastReadBook();
	void updateFontSizeButtons();
	void updateSpeedButtons();
	void initializeAmbulantPreferences();
	ambulant::net::url findBookInLangpackSubdir(std::string);

	std::string mAppPath;
	std::string mLocalizationDll;
	amis::BookList* mpHistory;
	bool mbBookIsOpen;
	bool mbShouldIgnoreOpenDocEvent;
	bool mbWasLastExitClean;
	bool mbIsPlayingHelpBook;
	bool mbIsPlayingShortcutsBook;
	std::string mLanguagePreference;
	bool mbIsWaitingToLoad;
	bool mbShouldNotRenderAudio;
	bool mbOverrideReopen;
public:
	DECLARE_MESSAGE_MAP()

};

}
}

extern amis::gui::CAmisApp theApp;
#endif