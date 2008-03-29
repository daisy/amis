// testAmisCoreWithGui.h : main header file for the testAmisCoreWithGui application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "dtb/dtb.h"


// CtestAmisCoreWithGuiApp:
// See testAmisCoreWithGui.cpp for the implementation of this class
//

class CtestAmisCoreWithGuiApp : public CWinApp
{
public:
	CtestAmisCoreWithGuiApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnDoSomething();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppExit();
private:
	amis::dtb::Dtb* mpDtb;
public:
	virtual int ExitInstance();
};

extern CtestAmisCoreWithGuiApp theApp;