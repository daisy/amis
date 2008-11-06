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

//count the number of UAKs in the registry
int getKeyCount()
{
	HKEY hKey;
	//"ERROR_SUCCESS" means it worked
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Amis\\AMIS\\UAKs", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		//this text is arbitrary
		TCHAR acValueName[10] = TEXT("KEY##");
		DWORD dwValueNameLen = sizeof(acValueName);
		DWORD dwIdx = 0;

		while (RegEnumValue(hKey, dwIdx, acValueName, &dwValueNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			dwIdx++;
			//reset this value; otherwise the next call to RegEnumValue fails
			dwValueNameLen = sizeof(acValueName);
		}
		RegCloseKey(hKey);
		return dwIdx;

	}
	return 0;
}


//get the private key from the registry and load it in the ambulant plugin engine
void addUserKeysToAmbulantPluginEngine()
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	CWinApp *app = AfxGetApp();
	assert(app);
	ambulant::common::plugin_engine *pe = ambulant::common::plugin_engine::get_plugin_engine();
	assert(pe);	
	int num_keys = getKeyCount();
	for (int i = 0; i<num_keys; i++)
	{
		LPBYTE keydataptr;
		UINT keydatalen;
		CString key_name;
		key_name.Format(_T("Key#%d"), i+1);
		BOOL ok = app->GetProfileBinary(_T("UAKs"), key_name, &keydataptr, &keydatalen);
		if (!ok) continue;
		std::string keydata((char *)keydataptr, keydatalen);
		delete keydataptr;
		pdtb_plugin_interface *pdata = (pdtb_plugin_interface *)pe->get_extra_data("pdtb_plugin");
		if (!pdata)
		{
			amis::util::Log::Instance()->writeError("Error loading UAK", 
				"pdtb.cpp, addUserKeysToAmbulantPluginEngine()");
			continue; 
		}
		pdata->add_uak_data(keydata);
		amis::util::Log::Instance()->writeMessage("Loaded UAK", 
			"pdtb.cpp, addUserKeysToAmbulantPluginEngine()");
	}
#endif
}

//import a new private key into the registry
bool importUserKeysIntoRegistry(std::string filename)
{
#ifdef WITH_PROTECTED_BOOK_SUPPORT
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
	{
		amis::util::Log::Instance()->writeError("Error reading UAK", "ptdb.cpp, importUserKeysIntoRegistry");
		return false; 
	}

	char buffer[4000];
	size_t len = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	if (len <= 0 || len >= sizeof(buffer))
	{
		amis::util::Log::Instance()->writeError("Error reading UAK", "ptdb.cpp, importUserKeysIntoRegistry");
		return false; 
	}

	CWinApp *app = AfxGetApp();
	assert(app);
	int key_idx = getKeyCount() + 1;
	CString key_name;
	key_name.Format(_T("Key#%d"), key_idx); 
	BOOL ok = app->WriteProfileBinary(_T("UAKs"), key_name, (LPBYTE)buffer, len);
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

