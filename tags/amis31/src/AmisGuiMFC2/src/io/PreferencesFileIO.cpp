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

#include "io/PreferencesFileIO.h"
#include "io/XercesDomWriter.h"
#include "util/FilePathTools.h"
#include "util/xercesutils.h"
#include <iostream>

#if defined(AMIS_PLATFORM_WINDOWS)
//for creating directories on windows
#include "io.h"
#include "direct.h"
#endif

//XERCES INCLUDES
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax2/Attributes.hpp>

using namespace amis::io;

PreferencesFileIO::PreferencesFileIO()
{
}
PreferencesFileIO::~PreferencesFileIO()
{
}
//this file is local! and the address does not look like a URL.
bool PreferencesFileIO::readFromFile(string filepath)
{
	mpPrefs = Preferences::Instance();
	ambulant::net::url tmp_url = ambulant::net::url::from_filename(filepath);
	mpPrefs->setSourceUrl(&tmp_url);
	bool ret = parseFile(&tmp_url);
	if (ret) 
		mpPrefs->scanAll();
	return ret;
}

//--------------
//xerces' start element function
void PreferencesFileIO::startElement(const   XMLCh* const    uri,
		const   XMLCh* const    localname,
		const   XMLCh* const    qname,
		const   Attributes&	attributes)
{
	char* node_name = XMLString::transcode(qname); 
	
	if (strcmp(node_name, "entry") == 0)
	{
		string id;
		id.assign(SimpleAttrs::get("id", &attributes));
		string value;
		value.assign(SimpleAttrs::get("value", &attributes));

		addEntry(id, value);
	}

	XMLString::release(&node_name);
}

void PreferencesFileIO::addEntry(string id, string value)
{
	ambulant::net::url source_url = *(mpPrefs->getSourceUrl());
	
	if (id.compare("ui-lang-id") == 0)
	{
		mpPrefs->setUiLangId(value);
	}
	else if (id.compare("start-in-basic-view") == 0)
	{
		if (value.compare("yes") == 0) 
			mpPrefs->setStartInBasicView(true);
		else 
			mpPrefs->setStartInBasicView(false);
	}
	else if (id.compare("load-last-book") == 0)
	{
		if (value.compare("yes") == 0) 
			mpPrefs->setLoadLastBook(true);
		else 
			mpPrefs->setLoadLastBook(false);
	}

	else if (id.compare("is-self-voicing") == 0)
	{
		if (value.compare("yes") == 0) 
			mpPrefs->setIsSelfVoicing(true);
		else 
			mpPrefs->setIsSelfVoicing(false);
	}
	else if (id.compare("highlight-text") == 0)
	{
		if (value.compare("yes") == 0) 
			mpPrefs->setHighlightText(true);
		else 
			mpPrefs->setHighlightText(false);
	}
	else if (id.compare("is-self-voicing-tts-only") == 0)
	{
		if (value.compare("yes") == 0) 
			mpPrefs->setIsSelfVoicingTTSOnly(true);
		else 
			mpPrefs->setIsSelfVoicingTTSOnly(false);
	}
	else if (id.compare("tts-voice-index") == 0)
	{
		int voice;
		voice = atoi(value.c_str());
		mpPrefs->setTTSVoiceIndex(voice);
	}

	else if (id.compare("pause-on-lost-focus") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setPauseOnLostFocus(true);
		else
			mpPrefs->setPauseOnLostFocus(false);
	}

	else if (id.compare("was-exit-clean") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setWasExitClean(true);
		else
			mpPrefs->setWasExitClean(false);
	}
	else if (id.compare("disable-screensaver") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setDisableScreensaver(true);
		else
			mpPrefs->setDisableScreensaver(false);
	}
	else if (id.compare("amis-css-file") == 0)
	{
		ambulant::net::url file = ambulant::net::url::from_filename(value);
		file = file.join_to_base(source_url);
		mpPrefs->setAmisCssFile(&file);
	}
	else if (id.compare("langpacks-dir") == 0)
	{
		ambulant::net::url file = ambulant::net::url::from_filename(value);
		file = file.join_to_base(source_url);
		mpPrefs->setLangpacksDir(&file);
	}
	else if (id.compare("fontsize-css-dir") == 0)
	{
		ambulant::net::url file = ambulant::net::url::from_filename(value);
		file = file.join_to_base(source_url);
		mpPrefs->setFontsizeCssDir(&file);
	}
	else if (id.compare("custom-css-dir") == 0)
	{
		ambulant::net::url file = ambulant::net::url::from_filename(value);
		file = file.join_to_base(source_url);
		mpPrefs->setCustomCssDir(&file);
	}
	else if (id.compare("user-bmk-dir") == 0)
	{
		ambulant::net::url file = ambulant::net::url::from_filename(value);
		file = file.join_to_base(source_url);
		mpPrefs->setUserBmkDir(&file);
	}
	else if (id.compare("highlight-fg-color") == 0)
	{
		amis::util::Color c(value);
		mpPrefs->setHighlightFGColor(c);
	}
	else if (id.compare("highlight-bg-color") == 0)
	{
		amis::util::Color c(value);
		mpPrefs->setHighlightBGColor(c);
	}
	else if (id.compare("app-font-name") == 0)
	{
		mpPrefs->setAppFontName(value);
	}
	else if (id.compare("is-first-time") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setIsFirstTime(true);
		else
			mpPrefs->setIsFirstTime(false);
	}
	else if (id.compare("is-logging-enabled") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setIsLoggingEnabled(true);
		else
			mpPrefs->setIsLoggingEnabled(false);
	}
	else if (id.compare("logging-level") == 0)
	{
		if (value.compare("FULL_LOGGING") == 0)
			mpPrefs->setLogLevel(amis::util::FULL_LOGGING);
		else if (value.compare("MEDIUM_LOGGING") == 0)
			mpPrefs->setLogLevel(amis::util::MEDIUM_LOGGING);
		else
			mpPrefs->setLogLevel(amis::util::LOW_LOGGING);
	}
	else if (id.compare("ambulant-prefers-ffmpeg") == 0)
	{
		if (value.compare("yes") == 0)
			mpPrefs->setAmbulantPrefersFFMpeg(true);
		else
			mpPrefs->setAmbulantPrefersFFMpeg(false);
	}
	else if (id.compare("tts-volume-pct") == 0)
	{
		int val = atoi(value.c_str());
		mpPrefs->setTTSVolumePct(val);
	}
	else if (id.compare("audio-volume-pct") == 0)
	{
		int val = atoi(value.c_str());
		mpPrefs->setAudioVolumePct(val);
	}
	else if (id.compare("cache-index") == 0)
	{
		if (value == "yes")
			mpPrefs->setCacheIndex(true);
		else
			mpPrefs->setCacheIndex(false);
	}
	else if (id.compare("must-avoid-directx") == 0)
	{
		if (value == "yes")
			mpPrefs->setMustAvoidDirectX(true);
		else
			mpPrefs->setMustAvoidDirectX(false);
	}
	else if (id.compare("must-avoid-tts") == 0)
	{
		if (value == "yes")
			mpPrefs->setMustAvoidTTS(true);
		else
			mpPrefs->setMustAvoidTTS(false);
	}
}


bool PreferencesFileIO::writeToFile(string filepath, amis::Preferences* pPrefs)
{
	if (pPrefs == NULL)
		return false;

	mpPrefs = pPrefs;
	
	XercesDomWriter writer;
	writer.initialize("preferences");
	mpDoc = writer.getDocument();
	writeData();
	
	
	//make sure the path to the file exists
	string dir = amis::util::FilePathTools::getParentDirectory(filepath);
	if (access(dir.c_str(), 0) == -1)
	{
		_mkdir(dir.c_str());
	}

	writer.writeToFile(filepath);
	return true;
}

void PreferencesFileIO::writeData()
{
	DOMElement* pGeneralSection = NULL;
	DOMElement* pEntry = NULL;
	string tmp_str;

	pGeneralSection = mpDoc->createElement(X("section"));
	pGeneralSection->setAttribute(X("name"), X("general"));

	pEntry = createEntry("ui-lang-id", mpPrefs->getUiLangId());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("start-in-basic-view", mpPrefs->getStartInBasicView());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("load-last-book", mpPrefs->getLoadLastBook());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("is-self-voicing", mpPrefs->getIsSelfVoicing());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("highlight-text", mpPrefs->getHighlightText());
	pGeneralSection->appendChild((DOMNode*)pEntry);
	
	pEntry = createEntry("disable-screensaver", mpPrefs->getDisableScreensaver());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	std::string voice;
	char buff[5] = "";
	itoa(mpPrefs->getTTSVoiceIndex(), buff, 10);
	voice.assign(buff);
	pEntry = createEntry("tts-voice-index", voice);
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("pause-on-lost-focus", mpPrefs->getPauseOnLostFocus());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("is-self-voicing-tts-only", mpPrefs->getIsSelfVoicingTTSOnly());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("was-exit-clean", mpPrefs->getWasExitClean());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("amis-css-file", mpPrefs->getAmisCssFile()->get_file());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("ambulant-prefers-ffmpeg", mpPrefs->getAmbulantPrefersFFMpeg());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	//Directories
	pEntry = createEntry("langpacks-dir", mpPrefs->getLangpacksDir()->get_file());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("user-bmk-dir", mpPrefs->getUserBmkDir()->get_file());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("fontsize-css-dir", mpPrefs->getFontsizeCssDir()->get_file());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("custom-css-dir", mpPrefs->getCustomCssDir()->get_file());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("highlight-fg-color", mpPrefs->getHighlightFGColor().getAsHtmlHexColor());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("highlight-bg-color", mpPrefs->getHighlightBGColor().getAsHtmlHexColor());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("app-font-name", mpPrefs->getAppFontName());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("is-first-time", mpPrefs->getIsFirstTime());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("is-logging-enabled", mpPrefs->getIsLoggingEnabled());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	std::string str_loglevel;
	if (mpPrefs->getLogLevel() == amis::util::FULL_LOGGING)
		str_loglevel = "FULL_LOGGING";
	else if (mpPrefs->getLogLevel() == amis::util::MEDIUM_LOGGING)
		str_loglevel = "MEDIUM_LOGGING";
	else
		str_loglevel = "LOW_LOGGING";

	pEntry = createEntry("logging-level", str_loglevel);
	pGeneralSection->appendChild((DOMNode*)pEntry);

	std::string tts_vol;
	char tts_vol_buff[5] = "";
	itoa(mpPrefs->getTTSVolumePct(), tts_vol_buff, 10);
	tts_vol.assign(tts_vol_buff);
	pEntry = createEntry("tts-volume-pct", tts_vol);
	pGeneralSection->appendChild((DOMNode*)pEntry);
	
	std::string audio_vol;
	char audio_vol_buff[5] = "";
	itoa(mpPrefs->getAudioVolumePct(), audio_vol_buff, 10);
	audio_vol.assign(audio_vol_buff);
	pEntry = createEntry("audio-volume-pct", audio_vol);
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("cache-index", mpPrefs->getCacheIndex());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("must-avoid-directx", mpPrefs->getMustAvoidDirectX());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	pEntry = createEntry("must-avoid-tts", mpPrefs->getMustAvoidTTS());
	pGeneralSection->appendChild((DOMNode*)pEntry);

	//get a pointer to the root element
    DOMElement* pRootElem = mpDoc->getDocumentElement();
	//append the general section element
	pRootElem->appendChild((DOMNode*)pGeneralSection);
}

DOMElement* PreferencesFileIO::createEntry(std::string id, std::string value)
{
	DOMElement* pEntry = mpDoc->createElement(X("entry"));
	pEntry->setAttribute(X("id"), X(id.c_str()));
	pEntry->setAttribute(X("value"), X(value.c_str()));
	return pEntry;
}

DOMElement* PreferencesFileIO::createEntry(std::string id, bool value)
{
	std::string boolstr;
	if (value == true) boolstr = "yes";
	else boolstr = "no";
	return createEntry(id, boolstr);
}