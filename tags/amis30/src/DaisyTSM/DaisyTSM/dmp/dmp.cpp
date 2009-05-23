// dmp.cpp : Implementation of CDmpApp and DLL registration.


#include "StdAfxFIX.h"

//#include "stdafx.h"

//#include <dshow.h>

#include "dmp.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

CDmpApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xd825129b, 0xb483, 0x46bc, { 0xb9, 0x4f, 0x6, 0xc1, 0xfd, 0x5a, 0xeb, 0xb1 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CDmpApp::InitInstance - DLL initialization

BOOL CDmpApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
		// TODO: Add your own module initialization code here.
	{

	}
	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CDmpApp::ExitInstance - DLL termination

int CDmpApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
/*
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

*/

// DANOUL
// #include "dmpppg.cpp"
// #include "dmpctl.cpp"

