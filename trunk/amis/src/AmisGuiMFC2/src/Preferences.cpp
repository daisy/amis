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

#include "stdafx.h"
#include "Preferences.h"
#include "gui/AmisApp.h"
#include "util/SearchForFilesMFC.h"
#include "util/FilePathTools.h"
#include "io/ModuleDescReader.h"
#include "util/Log.h"


#include "gui/self-voicing/TTSPlayer.h"

using namespace amis;

Preferences* Preferences::pinstance = 0;

Preferences* Preferences::Instance()
{
	if (pinstance == 0) pinstance = new Preferences;
    return pinstance;
}

void Preferences::DestroyInstance()
{
	delete pinstance;
}

Preferences::Preferences()
{
	
	mUiLangId = "eng-US";
	mbPauseOnLostFocus = true;
	mbIsSelfVoicing = false;
	mbIsSelfVoicingTTSOnly = false;
	mbLoadLastBook = false;
	mbStartInBasicView = false;
	mTTSVoiceIndex = 0;
	mbDisableScreensaver = true;
	mAudioVolumePct = 100;
	mTTSVolumePct = 70;
	mbHighlightText = true;
	mHighlightFG.set("#000000");
	mHighlightBG.set("#FFFF00");
	mAppFontName = "Arial";
	mbIsFirstTime = false;
	mbAmbulantPrefersFFMpeg = false;

#ifdef _DEBUG
	mbLoggingEnabled = true;
	mLogLevel = amis::util::FULL_LOGGING;
#else
	mbLoggingEnabled = false;
	mLogLevel = amis::util::LOW_LOGGING;
#endif
	mbCacheIndex = true;
	mbMustAvoidDirectX = false;
	mbMustAvoidTTS = false;

	mFontsizeCssFiles.clear();
	mCustomCssFiles.clear();
	//note that file paths here are hardcoded relative to the application directory
	//whereas in the prefs XML file, they are relative to the prefs XML file
	//they get overridden by reading in the preferences file; these defaults are just here as a safety measure
	mFontsizeCssDir = ambulant::net::url::from_filename("./css/font/");
	mCustomCssDir = ambulant::net::url::from_filename("./css/customStyles/");
	mLangpacksDir = ambulant::net::url::from_filename("./lang/");
	mUserBmkDir = ambulant::net::url::from_filename("./bmk/");
	mAmisCssFile = ambulant::net::url::from_filename("./css/amis.css");	
	
	ambulant::net::url app_path = ambulant::net::url::from_filename(theApp.getAppSettingsPath());
	mFontsizeCssDir = mFontsizeCssDir.join_to_base(app_path);
	mCustomCssDir = mCustomCssDir.join_to_base(app_path);
	mLangpacksDir = mLangpacksDir.join_to_base(app_path);
	mUserBmkDir = mUserBmkDir.join_to_base(app_path);
	mAmisCssFile = mAmisCssFile.join_to_base(app_path);
	
}

//this function should be called after the preferences XML file has been parsed
void Preferences::scanAll()
{
	scanDirectoriesForCssFiles();
	scanDirectoriesForLanguagePackFiles();
	
}
void Preferences::scanDirectoriesForCssFiles()
{
	amis::util::SearchForFilesMFC searcher;
	searcher.addSearchCriteria(".css");
	//these files will only bother developers
	searcher.addSearchExclusionCriteria(".svn");
	searcher.setRecursiveSearch(false);
	
	ambulant::net::url url;
	//if the version is IE8, reset the directory to be the IE8 subfolder
	int ieversion = theApp.getIeVersion();
	if (ieversion >=8)
	{
		url = ambulant::net::url::from_filename("./ie8/");
		url = url.join_to_base(mFontsizeCssDir);
	}
	else
	{
		url = mFontsizeCssDir;
	}
	
	if (searcher.startSearch(url.get_file()) > 0)
	{
		mFontsizeCssFiles = *searcher.getSearchResults();
	}
	else 
	{
		TRACE(_T("No fontsize css files found\n"));
		amis::util::Log::Instance()->writeWarning("No fontsize css files found in directory",
			&url, "Preferences::scanDirectoriesForCssFiles");
	}
	
	searcher.clearSearchResults();
	if (searcher.startSearch(mCustomCssDir.get_file()) > 0)
	{
		mCustomCssFiles = *searcher.getSearchResults();
	}
	else 
	{
		TRACE(_T("No contrast css files found\n"));
		string log_msg = "No contrast css files found in " + mCustomCssDir.get_url();
	}
}

//search for "moduleDesc.xml" files that inside the root language pack directory
//save the ones that have type="langpack" enabled="yes" on the root moduleDesc element
//store the language id and media group of the language label
void Preferences::scanDirectoriesForLanguagePackFiles()
{
	amis::util::SearchForFilesMFC searcher;
	searcher.addSearchCriteria("moduleDesc.xml");
	//these files will only bother developers
	searcher.addSearchExclusionCriteria(".svn");
	searcher.setRecursiveSearch(true);
	if (searcher.startSearch(mLangpacksDir.get_file()) > 0)
	{
		amis::UrlList list = *searcher.getSearchResults();
		for (int i = 0; i<list.size(); i++)
		{
			amis::io::ModuleDescReader reader;
			if (reader.readFromFile(&list[i]))
			{
				amis::ModuleDescData* p_data = reader.getModuleDescData();
				if (p_data == NULL)
				{
					TRACE(_T("No data available for language pack\n"));
					string log_msg = "No data available for language pack " + list[i].get_url();
					amis::util::Log::Instance()->writeError("No data available for language pack", 
						&list[i], "Preferences::scanDirectoriesForLanguagePackFiles");
				}
				else
				{
					if (p_data->isEnabled() && p_data->getModuleType() == amis::ModuleDescData::LANGPACK)
					{
						string id = p_data->getId();
						mInstalledLanguages[id] = p_data;
						string log_msg = "Added language pack for " + id;
						amis::util::Log::Instance()->writeMessage(log_msg, 
							"Preferences::scanDirectoriesForLanguagePackFile");
					}
				}
			}
			else
			{
				TRACE(_T("Could not read language pack file\n"));
				amis::util::Log::Instance()->writeError("Could not read language pack file: ", &list[i], 
					"Preferences::scanDirectoriesForLanguagePackFiles");
			}
		}
	}
	else
	{
		TRACE(_T("No language pack files found\n"));
		amis::util::Log::Instance()->writeError("No language pack files found", 
			"Preferences::scanDirectoriesForLanguagePackFiles");
	}
}

Preferences::~Preferences()
{
	amis::StringModuleMap::iterator it = mInstalledLanguages.begin();
	while (mInstalledLanguages.size() > 0)
	{
		amis::ModuleDescData* p_module = it->second;
		if (p_module != NULL) delete p_module;
		mInstalledLanguages.erase(it);
		it = mInstalledLanguages.begin();
	}
}

void Preferences::setUiLangId(string value)
{
	mUiLangId = value;
}
string Preferences::getUiLangId()
{
	return mUiLangId;
}

void Preferences::setStartInBasicView(bool value)
{
	mbStartInBasicView = value;
}

bool Preferences::getStartInBasicView()
{
	return mbStartInBasicView;
}

void Preferences::setLoadLastBook(bool value)
{
	mbLoadLastBook = value;
}

bool Preferences::getLoadLastBook()
{
	return mbLoadLastBook;
}

void Preferences::setPauseOnLostFocus(bool value)
{
	mbPauseOnLostFocus = value;
}

bool Preferences::getPauseOnLostFocus()
{
	return mbPauseOnLostFocus;
}

void Preferences::setIsSelfVoicing(bool value)
{
	mbIsSelfVoicing = value;
}

bool Preferences::getIsSelfVoicing()
{
	return mbIsSelfVoicing;
}

void Preferences::setTTSVoiceIndex(int value)
{
	mTTSVoiceIndex = value;
}

int Preferences::getTTSVoiceIndex()
{
	return mTTSVoiceIndex;
}

void Preferences::setIsSelfVoicingTTSOnly(bool value)
{
	mbIsSelfVoicingTTSOnly = value;
}

bool Preferences::getIsSelfVoicingTTSOnly()
{
	return mbIsSelfVoicingTTSOnly;
}

void Preferences::setWasExitClean(bool value)
{
	mbWasExitClean = value;
}

bool Preferences::getWasExitClean()
{
	return mbWasExitClean;
}
void Preferences::setHighlightText(bool value)
{
	mbHighlightText = value;
}
bool Preferences::getHighlightText()
{
	return mbHighlightText;
}
void Preferences::setDisableScreensaver(bool value)
{
	mbDisableScreensaver = value;
}
bool Preferences::getDisableScreensaver()
{
	return mbDisableScreensaver;
}
void Preferences::setUserBmkDir(const ambulant::net::url* value)
{
	mUserBmkDir = *value;
}

const ambulant::net::url* Preferences::getUserBmkDir()
{
	return &mUserBmkDir; 
}

void Preferences::setLangpacksDir(const ambulant::net::url* value)
{
	mLangpacksDir = *value;
}

const ambulant::net::url* Preferences::getLangpacksDir()
{
	return &mLangpacksDir;
}

//set the base font css dir
//there may be subdirs, such as the one for ie8 css files
void Preferences::setFontsizeCssDir(const ambulant::net::url* value)
{
	mFontsizeCssDir = *value;
}

const ambulant::net::url* Preferences::getFontsizeCssDir()
{
	return &mFontsizeCssDir;
}
void Preferences::setCustomCssDir(const ambulant::net::url* value)
{
	mCustomCssDir = *value;
}

const ambulant::net::url* Preferences::getCustomCssDir()
{
	return &mCustomCssDir;
}

void Preferences::setAmisCssFile(const ambulant::net::url* value)
{
	mAmisCssFile = *value;
}

const ambulant::net::url* Preferences::getAmisCssFile()
{
	return &mAmisCssFile;
}


void Preferences::setSourceUrl(const ambulant::net::url* value)
{
	mSourceUrl = *value;
}

const ambulant::net::url* Preferences::getSourceUrl()
{
	return &mSourceUrl;
}

amis::UrlList* Preferences::getFontsizeCssFiles()
{
	return &mFontsizeCssFiles;
}

amis::UrlList* Preferences::getCustomCssFiles()
{
	return &mCustomCssFiles;
}
amis::StringModuleMap* Preferences::getInstalledLanguages()
{
	return &mInstalledLanguages;
}

amis::ModuleDescData* Preferences::getLanguageData(std::string id)
{
	amis::ModuleDescData* p_data = mInstalledLanguages[id];
	return p_data;
}

amis::ModuleDescData* Preferences::getCurrentLanguageData()
{
	amis::ModuleDescData* p_data = mInstalledLanguages[getUiLangId()];
	return p_data;
}
void Preferences::setHighlightBGColor(amis::util::Color value)
{
	mHighlightBG = value;
}
amis::util::Color Preferences::getHighlightBGColor()
{
	return mHighlightBG;
}
void Preferences::setHighlightFGColor(amis::util::Color value)
{
	mHighlightFG = value;
}
amis::util::Color Preferences::getHighlightFGColor()
{
	return mHighlightFG;
}
void Preferences::setAppFontName(std::string value)
{
	mAppFontName = value;
}
std::string Preferences::getAppFontName()
{
	return mAppFontName;
}
void Preferences::setAmbulantPrefersFFMpeg(bool value)
{
	mbAmbulantPrefersFFMpeg = value;
}
bool Preferences::getAmbulantPrefersFFMpeg()
{
	return mbAmbulantPrefersFFMpeg;
}

void Preferences::logPreferences(bool logOnlyUserControllable)
{
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeMessage("Preferences", "Preferences::logPreferences");
	
	if (logOnlyUserControllable)
		p_log->writeMessage("Only user-controllable preferences", "");
	else
		p_log->writeMessage("All preferences", "");

	p_log->writeMessage("\tPreferences XML File: ", getSourceUrl(), "");
	
	string log_msg = "\tLanguage pack = " + getUiLangId();
	p_log->writeMessage(log_msg, "");

	log_msg = "\tStartup view = ";
	if (getStartInBasicView()) log_msg += "Basic";
	else log_msg += "Default";
	p_log->writeMessage(log_msg, "");

	log_msg = "\tWill load last book on startup? ";
	if (getLoadLastBook()) log_msg += "Yes";
	else log_msg += "No";
	p_log->writeMessage(log_msg, "");
	
	log_msg = "\tWill pause when AMIS loses application focus? ";
	if (getPauseOnLostFocus()) log_msg += "Yes";
	else log_msg += "No";
	p_log->writeMessage(log_msg, "");

	log_msg = "\tIs self-voicing? ";
	if (getIsSelfVoicing()) log_msg += "Yes";
	else log_msg += "No";
	p_log->writeMessage(log_msg, "");
	
	if (!getMustAvoidTTS())
	{
		int indexTTSVoice = getTTSVoiceIndex();
		log_msg = "\tTTS voice index ";
		char strTTSVoice[3];
		sprintf(strTTSVoice, "%d", indexTTSVoice);
		log_msg += strTTSVoice;
		//note that this is just getting the current voice
		//no need to query both TTSPlayer instances
		std::string strVoice = amis::tts::TTSPlayer::InstanceOne()->GetVoiceName();
		log_msg += " // ";
		log_msg += strVoice;
		p_log->writeMessage(log_msg, "");
	}

	log_msg = "\tHighlight text? ";
	if (getHighlightText()) log_msg += "Yes";
	else log_msg += "No";
	p_log->writeMessage(log_msg, "");

	if (getHighlightText())
	{
		log_msg = "\tHighlight FG: ";
		log_msg += getHighlightFGColor().getAsHtmlHexColor();
		p_log->writeMessage(log_msg, "");

		log_msg = "\tHighlight BG: ";
		log_msg += getHighlightBGColor().getAsHtmlHexColor();
		p_log->writeMessage(log_msg, "");
	}

	log_msg = "\tDisable screensaver? ";
	if (getDisableScreensaver()) log_msg += "Yes";
	else log_msg += "No";
	p_log->writeMessage(log_msg, "");

	if (!logOnlyUserControllable)
	{
		if (getIsSelfVoicing())
		{
			log_msg = "\tSelf-voicing uses TTS or pre-recorded audio? ";
			if (getIsSelfVoicingTTSOnly()) log_msg += "TTS";
			else log_msg += "Audio";
			p_log->writeMessage(log_msg, "");
		}

		log_msg = "\tFirst time running AMIS? ";
		if (getIsFirstTime()) log_msg += "Yes";
		else log_msg += "No";
		p_log->writeMessage(log_msg, "");

		log_msg = "\tLogging enabled: ";
		if (getIsLoggingEnabled()) log_msg += "Yes";
		else log_msg += "No";
		p_log->writeMessage(log_msg, "");

		log_msg = "\tLogging level: ";
		if (getLogLevel() == amis::util::LOW_LOGGING) log_msg += "Low";
		else if (getLogLevel() == amis::util::MEDIUM_LOGGING) log_msg += "Medium";
		else log_msg += "Full";
		p_log->writeMessage(log_msg, "");

		if (!getMustAvoidTTS())
		{
			log_msg = "\tTTS volume percent: ";
			char tts_vol[5];
			sprintf(tts_vol, "%d", getTTSVolumePct());
			log_msg += tts_vol;
			p_log->writeMessage(log_msg, "");
		}

		log_msg = "\tAudio volume percent: ";
		char audio_vol[5];
		sprintf(audio_vol, "%d", getAudioVolumePct());
		log_msg += audio_vol;
		p_log->writeMessage(log_msg, "");

		log_msg = "\tAmbulant prefers FFMpeg: ";
		if (getAmbulantPrefersFFMpeg()) log_msg += "yes";
		else log_msg += "no";
		p_log->writeMessage(log_msg, "");

		log_msg = "\tApplication font: ";
		log_msg += getAppFontName();
		p_log->writeMessage(log_msg, "");

		log_msg = "\tCache index: ";
		if (getCacheIndex()) log_msg += "yes";
		else log_msg += "no";
		p_log->writeMessage(log_msg, "");
		
		log_msg = "\tAvoid DirectX: ";
		if (getMustAvoidDirectX()) log_msg += "yes";
		else log_msg += "no";
		p_log->writeMessage(log_msg, "");
		
		log_msg = "\tAvoid TTS: ";
		if (getMustAvoidTTS()) log_msg += "yes";
		else log_msg += "no";
		p_log->writeMessage(log_msg, "");
		
		log_msg = "\tDid AMIS exit cleanly last time? ";
		if (getWasExitClean()) log_msg += "Yes";
		else log_msg += "No";
		p_log->writeMessage(log_msg, "Preferences::logAllPreferences");

		p_log->writeMessage("\tInstalled language packs:", "");
		amis::StringModuleMap::iterator it;
		it = getInstalledLanguages()->begin();
		while (it != getInstalledLanguages()->end())
		{
			string lang_id = it->first;
			log_msg = "\t\t" + lang_id;
			p_log->writeMessage(log_msg, "");
			it++;
		}

		p_log->writeMessage("\tBookmark dir = ", getUserBmkDir(), "");
		p_log->writeMessage("\tLangpacks dir = ", getLangpacksDir(), "");
		p_log->writeMessage("\tFontsize css dir = ", getFontsizeCssDir(), "");
		p_log->writeMessage("\tContrast css dir = ", getCustomCssDir(), "");
		p_log->writeMessage("\tAmis css file = ", getAmisCssFile(), "");
		
		p_log->writeMessage("\tFontsize CSS files:", "");
		for (int i = 0; i<getCustomCssFiles()->size(); i++)
			p_log->writeMessage("\t\t", &(*getCustomCssFiles())[i], "");

		p_log->writeMessage("\tCustom CSS files:", "");
		for (int i = 0; i<getCustomCssFiles()->size(); i++)
			p_log->writeMessage("\t\t", &(*getCustomCssFiles())[i], "");
		
		p_log->writeMessage("End Preferences\n\n", "");
	}
}
void Preferences::resetColors()
{
	mHighlightFG.set("#000000");
	mHighlightBG.set("#FFFF00");
}
void Preferences::setIsFirstTime(bool val)
{
	mbIsFirstTime = val;
}
bool Preferences::getIsFirstTime()
{
	return mbIsFirstTime;
}
amis::util::LogLevel Preferences::getLogLevel()
{
	return mLogLevel;
}
void Preferences::setLogLevel(amis::util::LogLevel level)
{
	mLogLevel = level;
}
bool Preferences::getIsLoggingEnabled()
{
	return mbLoggingEnabled;
}
void Preferences::setIsLoggingEnabled(bool value)
{
	mbLoggingEnabled = value;
}

int Preferences::getTTSVolumePct()
{
	return mTTSVolumePct;
}

void Preferences::setTTSVolumePct(int value)
{
	mTTSVolumePct = value;
}
int Preferences::getAudioVolumePct()
{
	return mAudioVolumePct;
}
void Preferences::setAudioVolumePct(int value)
{
	mAudioVolumePct = value;
}
bool Preferences::getCacheIndex()
{
	return mbCacheIndex;
}
void Preferences::setCacheIndex(bool value)
{
	mbCacheIndex = value;
}

bool Preferences::getMustAvoidDirectX()
{
	return mbMustAvoidDirectX;
}
void Preferences::setMustAvoidDirectX(bool value)
{
	mbMustAvoidDirectX = value;
	if (mbMustAvoidDirectX)
	{
		mbAmbulantPrefersFFMpeg = true;
		mbIsSelfVoicingTTSOnly = true;
	}
}

bool Preferences::getMustAvoidTTS()
{
	return mbMustAvoidTTS;
}
void Preferences::setMustAvoidTTS(bool value)
{
	mbMustAvoidTTS = value;
}
