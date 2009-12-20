//////////////////////////////////////////////////////////////////////////
//PathDialog.h file
//
//Written by Nguyen Tan Hung <tanhung@yahoo.com>
//////////////////////////////////////////////////////////////////////////

//taken from:
//http://www.codeguru.com/Cpp/W-D/dislog/dialogforselectingfolders/article.php/c2019/
//and modified a bit

#ifndef PATHDIALOG_H
#define PATHDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include "shlobj.h"

namespace amis
{
namespace gui
{
namespace dialogs
{

class CPathDialog;

// CPathDialogSub - intercepts messages from child controls
class CPathDialogSub : public CWnd
{
public:
	CPathDialog* m_pPathDialog;
protected:
    afx_msg void OnOK();      
	afx_msg void OnChangeEditPath();
    DECLARE_MESSAGE_MAP()

	friend CPathDialog;
};

// CPathDialog dialog
class CPathDialog
{
public:
	CPathDialog(LPCTSTR lpszCaption=NULL, LPCTSTR lpszTitle=NULL, LPCTSTR lpszInitialPath=NULL, CWnd* pParent = NULL);
	CString GetPathName();
	virtual int DoModal();
	static int Touch(LPCTSTR lpPath, BOOL bValidate=TRUE);
	static int MakeSurePathExists(LPCTSTR lpPath);
	static BOOL IsFileNameValid(LPCTSTR lpFileName);
	static int ConcatPath(LPTSTR lpRoot, LPCTSTR lpMorePath);

private:
	static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam, LPARAM pData);
	LPCTSTR m_lpszCaption;
	LPCTSTR m_lpszInitialPath;
	TCHAR m_szPathName[MAX_PATH];
	BROWSEINFO m_bi;
	HWND m_hWnd;
	CWnd*	m_pParentWnd;
	BOOL m_bParentDisabled;
	BOOL m_bGetSuccess;
	CPathDialogSub m_PathDialogSub;

	friend CPathDialogSub;
};
}
}
}
#endif 