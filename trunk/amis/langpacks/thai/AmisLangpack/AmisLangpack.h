// AmisLangpack.h : main header file for the AMISLANGPACK DLL
//

#if !defined(AFX_AMISLANGPACK_H__1B4EB7C9_366C_443E_9014_DEC8869223D2__INCLUDED_)
#define AFX_AMISLANGPACK_H__1B4EB7C9_366C_443E_9014_DEC8869223D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAmisLangpackApp
// See AmisLangpack.cpp for the implementation of this class
//

class CAmisLangpackApp : public CWinApp
{
public:
	CAmisLangpackApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmisLangpackApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAmisLangpackApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMISLANGPACK_H__1B4EB7C9_366C_443E_9014_DEC8869223D2__INCLUDED_)
