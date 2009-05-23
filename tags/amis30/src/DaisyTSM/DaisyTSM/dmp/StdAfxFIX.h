#if !defined(AFX_STDAFX_H__5E3570A3_4D82_4FC2_9FB5_2302F1E04AB6__INCLUDED_)
#define AFX_STDAFX_H__5E3570A3_4D82_4FC2_9FB5_2302F1E04AB6__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

// Insert your headers here

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#include <windows.h>


//#include <afxctl.h>         // MFC support for ActiveX Controls
//#include <afxext.h>         // MFC extensions
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
//#include <afxdb.h>			// MFC database classes
//#include <afxdao.h>			// MFC DAO database classes

//#error DANOUL

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


/////////////////////////////////////////////////////////////////////////////
// Win32 libraries

#ifndef _AFXDLL
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "nafxcwd.lib")
		#else
			#pragma comment(lib, "nafxcw.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "uafxcwd.lib")
		#else
			#pragma comment(lib, "uafxcw.lib")
		#endif
	#endif
#else
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "mfc71d.lib")
			#pragma comment(lib, "mfcs71d.lib")
		#else
			#pragma comment(lib, "mfc71.lib")
			#pragma comment(lib, "mfcs71.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "mfc71ud.lib")
			#pragma comment(lib, "mfcs71ud.lib")
		#else
			#pragma comment(lib, "mfc71u.lib")
			#pragma comment(lib, "mfcs71u.lib")
		#endif
	#endif
#endif

#ifdef _DLL
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "msvcrtd.lib")
	#else
		#pragma comment(lib, "msvcrt.lib")
	#endif
#else
#ifdef _MT
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "libcmtd.lib")
	#else
		#pragma comment(lib, "libcmt.lib")
	#endif
#else
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "libcd.lib")
	#else
		#pragma comment(lib, "libc.lib")
	#endif
#endif
#endif

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

// force inclusion of NOLIB.OBJ for /disallowlib directives
#pragma comment(linker, "/include:__afxForceEXCLUDE")

// force inclusion of DLLMODUL.OBJ for _USRDLL
#ifdef _USRDLL
#pragma comment(linker, "/include:__afxForceUSRDLL")
#endif

// force inclusion of STDAFX.OBJ for precompiled types
#ifdef _AFXDLL
#pragma comment(linker, "/include:__afxForceSTDAFX")
#endif


#endif // !defined(AFX_STDAFX_H__5E3570A3_4D82_4FC2_9FB5_2302F1E04AB6__INCLUDED_)
