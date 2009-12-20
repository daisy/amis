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
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "stdafx.h"
#include "ambulant/net/url.h"
#include "util/Color.h"
#include <string>
#include "AmisCore.h"
#include "ModuleDescData.h"
#include "util/Log.h"

namespace amis
{
class Preferences
{
protected:
	Preferences();
	
public:
	static Preferences* Instance();
	void DestroyInstance();
	~Preferences();
	//the preferences IO class should call this function after reading in the XML file
	void scanAll();
	void logPreferences(bool logOnlyUserControllable = false);
	void resetColors();

	void setUiLangId(string);
	string getUiLangId();

	void setStartInBasicView(bool);
	bool getStartInBasicView();

	void setLoadLastBook(bool);
	bool getLoadLastBook();

	void setPauseOnLostFocus(bool);
	bool getPauseOnLostFocus();

	void setIsSelfVoicing(bool);
	bool getIsSelfVoicing();

	void setTTSVoiceIndex(int);
	int getTTSVoiceIndex();

	void setIsSelfVoicingTTSOnly(bool);
	bool getIsSelfVoicingTTSOnly();

	void setWasExitClean(bool);
	bool getWasExitClean();

	void setHighlightText(bool);
	bool getHighlightText();
	
	void setDisableScreensaver(bool);
	bool getDisableScreensaver();

	void setAmbulantPrefersFFMpeg(bool);
	bool getAmbulantPrefersFFMpeg();

	void setUserBmkDir(const ambulant::net::url*);
	const ambulant::net::url* getUserBmkDir();
	
	void setLangpacksDir(const ambulant::net::url*);
	const ambulant::net::url* getLangpacksDir();

	void setFontsizeCssDir(const ambulant::net::url*);
	const ambulant::net::url* getFontsizeCssDir();

	void setCustomCssDir(const ambulant::net::url*);
	const ambulant::net::url* getCustomCssDir();

	void setAmisCssFile(const ambulant::net::url*);
	const ambulant::net::url* getAmisCssFile();
	
	void setSourceUrl(const ambulant::net::url*);
	const ambulant::net::url* getSourceUrl();

	void setHighlightBGColor(amis::util::Color);
	amis::util::Color getHighlightBGColor();
	void setHighlightFGColor(amis::util::Color);
	amis::util::Color getHighlightFGColor();
	void setAppFontName(std::string);
	std::string getAppFontName();

	amis::UrlList* getFontsizeCssFiles();
	amis::UrlList* getCustomCssFiles();
	amis::StringModuleMap* getInstalledLanguages();

	amis::ModuleDescData* getLanguageData(std::string id);
	amis::ModuleDescData* getCurrentLanguageData();

	bool getIsFirstTime();
	void setIsFirstTime(bool);

	int getTTSVolumePct();
	void setTTSVolumePct(int);
	int getAudioVolumePct();
	void setAudioVolumePct(int);

	amis::util::LogLevel getLogLevel();
	void setLogLevel(amis::util::LogLevel);
	bool getIsLoggingEnabled();
	void setIsLoggingEnabled(bool);

	bool getCacheIndex();
	void setCacheIndex(bool);

	bool getMustAvoidDirectX();
	void setMustAvoidDirectX(bool);

	bool getMustAvoidTTS();
	void setMustAvoidTTS(bool);

private:
	void scanDirectoriesForCssFiles();
	void scanDirectoriesForLanguagePackFiles();
	void processLanguagePackModuleDescData(amis::ModuleDescData*);
	
	
	//the following gets calculated at runtime
	//1. map the id of the language to the module description (path, media label, etc) 
	amis::StringModuleMap mInstalledLanguages;
	//2. the fontsize css files
	amis::UrlList mFontsizeCssFiles;
	//3. the custom css files
	amis::UrlList mCustomCssFiles;
	
	//the source preferences XML file
	ambulant::net::url mSourceUrl;
	
	ambulant::net::url mUserBmkDir;
	ambulant::net::url mLangpacksDir;
	ambulant::net::url mFontsizeCssDir;
	ambulant::net::url mCustomCssDir;
	ambulant::net::url mAmisCssFile;
	
	bool mbWasExitClean;
	
	string mUiLangId;
	bool mbStartInBasicView;
	bool mbLoadLastBook;
	bool mbPauseOnLostFocus;
	bool mbIsSelfVoicing;
	int mTTSVoiceIndex;
	bool mbIsSelfVoicingTTSOnly;
	bool mbHighlightText;
	bool mbDisableScreensaver;
	bool mbIsFirstTime;
	bool mbAmbulantPrefersFFMpeg;
	int mTTSVolumePct;
	int mAudioVolumePct;
	util::Color mHighlightFG;
	util::Color mHighlightBG;
	std::string mAppFontName;
	
	//the logging preferences
	amis::util::LogLevel mLogLevel;
	bool mbLoggingEnabled;
	bool mbCacheIndex;
	bool mbMustAvoidDirectX;
	bool mbMustAvoidTTS;
private:
	static Preferences* pinstance;

};
}

#endif