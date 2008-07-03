// RegOcx.cpp : Defines the entry point for the DLL application.
//

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

BOOL RunDllReg(LPTSTR lpFileName);
BOOL RunDllUnreg(LPTSTR lpFileName);
BOOL RegisterOCX();
BOOL UnregisterOCX();