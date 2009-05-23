#if !defined(AFX_DMPPPG_H__8EEF5A55_005D_4D39_A1B9_6539C5CAF85C__INCLUDED_)
#define AFX_DMPPPG_H__8EEF5A55_005D_4D39_A1B9_6539C5CAF85C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "stdafx.h"
#include "resource.h"       // main symbols


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

//#include <afxext.h>         // MFC extensions
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
//#include <afxdb.h>			// MFC database classes
//#include <afxdao.h>			// MFC DAO database classes


// DmpPpg.h : Declaration of the CDmpPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CDmpPropPage : See DmpPpg.cpp.cpp for implementation.

class CDmpPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDmpPropPage)
	DECLARE_OLECREATE_EX(CDmpPropPage)

// Constructor
public:
	CDmpPropPage();

// Dialog Data
	//{{AFX_DATA(CDmpPropPage)
	enum { IDD = IDD_PROPPAGE_DMP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CDmpPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMPPPG_H__8EEF5A55_005D_4D39_A1B9_6539C5CAF85C__INCLUDED)
