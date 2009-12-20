// DmpPpg.cpp : Implementation of the CDmpPropPage property page class.

#include "StdAfxFIX.h"

#include "DmpPpg.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

IMPLEMENT_DYNCREATE(CDmpPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDmpPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CDmpPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDmpPropPage, "DMP.DmpPropPage.1",
	0x8119c479, 0x4aab, 0x4d86, 0xbb, 0xed, 0x46, 0x24, 0x67, 0x6e, 0x8c, 0x22)


/////////////////////////////////////////////////////////////////////////////
// CDmpPropPage::CDmpPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CDmpPropPage

BOOL CDmpPropPage::CDmpPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DMP_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CDmpPropPage::CDmpPropPage - Constructor

CDmpPropPage::CDmpPropPage() :
	COlePropertyPage(IDD, IDS_DMP_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CDmpPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CDmpPropPage::DoDataExchange - Moves data between page and properties

void CDmpPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CDmpPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CDmpPropPage message handlers
