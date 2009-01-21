// RegOcx.cpp : Defines the entry point for the DLL application.
//
/*
#include "windows.h"
#include "malloc.h"
#include <string>

using namespace std;

typedef HRESULT (*DllRegisterServer) (void);
typedef HRESULT (*DllUnregisterServer) (void);
//typedef long (*DllRegisterServer) (void);
*/
/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	//LoadLibrary(
    return TRUE;
}
*/
/*
BOOL RunDllReg(LPTSTR lpFileName) {
	LPTSTR lpSysDir = (LPTSTR) malloc(255);
	GetSystemDirectory(lpSysDir, 255);

	

	//USES_CONVERSION;

	TCHAR szBuffer[256];
	GetModuleFileName(NULL, szBuffer, 256);
	//CString cstr_app_path = szBuffer;
	std::wstring strDir(szBuffer);
	int pos = strDir.find_last_of('\\'); //ReverseFind
	
	if (pos >= 0)
	{
		strDir = strDir.substr(0, pos); //Mid
	}

	std::wstring strFullPath = strDir.append(lpFileName);

	/*for (int i=0; i<255; i++) {
		if ((char) lpSysDir[i] == 0) {
			memcpy((lpSysDir + i), lpFileName, 27);
			lpSysDir[i + 27] = 0;
			break;
		}
	}*/
	
/*	bool bResult = false;
	HMODULE hModule;

	try {
		hModule = LoadLibrary(strFullPath.c_str());
		if (hModule == 0) {	

			std::wstring strMessage(L"ActiveX library is missing, DirectX audio will play only at normal speed.\n\n");
			strMessage.append(strFullPath);
			

		MessageBox(NULL, strMessage.c_str(), L"ActiveX Library not found", MB_OK);

			throw(1);
		}
		DllRegisterServer DllReg = (DllRegisterServer) GetProcAddress(hModule, "DllRegisterServer");
		if (DllReg == 0) throw(2);
		HRESULT hResult = DllReg();
		if (hResult != S_OK) throw(3);

		bResult = true;
	} catch (...) {
	}

	FreeLibrary(hModule);
	free(lpSysDir);

	return bResult;
}

BOOL RunDllUnreg(LPTSTR lpFileName) {
	LPTSTR lpSysDir = (LPTSTR) malloc(255);
	GetSystemDirectory(lpSysDir, 255);

	std::wstring strDir(lpSysDir);
	std::wstring strFullPath = strDir.append(lpFileName);
	/*
	for (int i=0; i<255; i++) {
		if ((char) lpSysDir[i] == 0) {
			memcpy((lpSysDir + i), lpFileName, 27);
			lpSysDir[i + 27] = 0;
			break;
		}
	}
	*/
	
/*	bool bResult = false;
	HMODULE hModule;

	try {
		hModule = LoadLibrary(strFullPath.c_str());
		if (hModule == 0) {	

			//std::wstring strMessage(L"ActiveX library is missing, DirectX audio will play only at normal speed.\n\n");
			//strMessage.append(strFullPath);
			

		//MessageBox(NULL, strMessage.c_str(), L"ActiveX Library not found", MB_OK);

			throw(1);
		}
		DllRegisterServer DllReg = (DllUnregisterServer) GetProcAddress(hModule, "DllUnregisterServer");
		if (DllReg == 0) throw(2);
		HRESULT hResult = DllReg();
		if (hResult != S_OK) throw(3);

		bResult = true;
	} catch (...) {
	}

	FreeLibrary(hModule);
	free(lpSysDir);

	return bResult;
}

BOOL RegisterOCX() {
    //if (!RunDllReg("\\DirectShowAudioHandler.ocx")) return false;
	if (!RunDllReg(L"\\TransformSample.ax")) return false;
	//if (!RunDllReg("\\IppDecryptor.dll")) return false;
	//if (!RunDllReg("\\ComDlg32.ocx")) return false;

	return true;
}

BOOL UnregisterOCX() {
    //if (!RunDllUnreg("\\DirectShowAudioHandler.ocx")) return false;
	if (!RunDllUnreg(L"\\TransformSample.ax")) return false;
	//if (!RunDllUnreg("\\IppDecryptor.dll")) return false;
	//if (!RunDllUnreg("\\ComDlg32.ocx")) return false;

	return true;
}
*/