#include "pdtb.h"
#include "stdafx.h"
//for string transformations
#include <algorithm>
#include "util/Log.h"

#include "ambulant/common/plugin_engine.h"
#ifdef WITH_PROTECTED_BOOK_SUPPORT
#include "../../ambulant-private/pdtbplugin/pdtbplugin.h"
#endif
#include "util/FilePathTools.h"

//get the private key from the registry and load it in the ambulant plugin engine
void addUserKeysToAmbulantPluginEngine()
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT

	// XXXJack this currently gets only a single UAK. That is silly...
	CWinApp *app = AfxGetApp();
	assert(app);
	LPBYTE keydataptr;
	UINT keydatalen;
	BOOL ok = app->GetProfileBinary(_T("UAKs"), _T("Key#1"), &keydataptr, &keydatalen);
	if (!ok) return;
	std::string keydata((char *)keydataptr, keydatalen);
	delete keydataptr;
	ambulant::common::plugin_engine *pe = ambulant::common::plugin_engine::get_plugin_engine();
	assert(pe);
	pdtb_plugin_interface *pdata = (pdtb_plugin_interface *)pe->get_extra_data("pdtb_plugin");
	if (!pdata)
	{
		amis::util::Log::Instance()->writeError("Error loading UAK", 
			"pdtb.cpp, addUserKeysToAmbulantPluginEngine()");
		return; 
	}
	pdata->add_uak_data(keydata);
	amis::util::Log::Instance()->writeMessage("Loaded UAK", 
		"pdtb.cpp, addUserKeysToAmbulantPluginEngine()");
#endif
}

//import a new private key into the registry
bool importUserKeysIntoRegistry(std::string filename)
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	// XXXJack this currently supports storage of only one UAK. That is silly...
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == NULL) return false; //TODO give error message

	char buffer[4000];
	size_t len = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	if (len <= 0 || len >= sizeof(buffer)) return false; //TODO give error message

	CWinApp *app = AfxGetApp();
	assert(app);
	BOOL ok = app->WriteProfileBinary(_T("UAKs"), _T("Key#1"), (LPBYTE)buffer, len);
	if (!ok)
	{
		amis::util::Log::Instance()->writeError("Could not import UAK into registry",
			"pdtb.cpp, importUserKeysIntoRegistry");
		return false;
	}
	else
		amis::util::Log::Instance()->writeMessage("Imported UAK into registry",
			"pdtb.cpp, importUserKeysIntoRegistry");

	// Also add the new key to the pdtb plugin.
	addUserKeysToAmbulantPluginEngine();
	return true;
#else
	return false;
#endif
}

//amis::dtb::Dtb::processNcc calls this function via callback
//set the book key in the ambulant plugin engine
bool registerBookKeyFile(const ambulant::net::url* navfile, const ambulant::net::url* keyfile)
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	// Check whether the plugin is available.
	ambulant::common::plugin_engine *pe = ambulant::common::plugin_engine::get_plugin_engine();
	assert(pe);
	pdtb_plugin_interface *pdata = (pdtb_plugin_interface*)pe->get_extra_data("pdtb_plugin");
	if (!pdata)
	{
		amis::util::Log::Instance()->writeError("Ambulant PDTB plugin not found", 
			"pdtb.cpp, registerBookKeyFile");
		return false;
	}
	return pdata->set_key(*navfile, *keyfile);
	return false;
#else
	return false;
#endif
}

bool isUserKeyFile(string filename)
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT

	string file_ext = amis::util::FilePathTools::getExtension(filename);
	//convert the string to lower case before doing a comparison
	std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), (int(*)(int))tolower);
	if (file_ext == "uak") return true;
	else return false;
#else
	return false;
#endif
}
