#if !defined(AFX_DMP_H__E4FEBB0F_294F_4CC2_9DEE_1609DF3534F5__INCLUDED_)
#define AFX_DMP_H__E4FEBB0F_294F_4CC2_9DEE_1609DF3534F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// dmp.h : main header file for DMP.DLL

#include <afxctl.h>

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDmpApp : See dmp.cpp for implementation.

// My functions

class CDmpApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMP_H__E4FEBB0F_294F_4CC2_9DEE_1609DF3534F5__INCLUDED)
