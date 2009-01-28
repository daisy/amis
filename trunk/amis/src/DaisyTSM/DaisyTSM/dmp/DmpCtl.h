#if !defined(AFX_DMPCTL_H__62F1A308_A87A_472C_970B_65BC44BDA998__INCLUDED_)
#define AFX_DMPCTL_H__62F1A308_A87A_472C_970B_65BC44BDA998__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LIBs to link against can be added here instead of the project/module settings.
//#zpragma comment(lib,"Strmiids.lib") 

//ke20030625
#include "malloc.h"
#include <exception>
//#include <windows.h>
#include <afxpriv.h>



#include "dmp.h"



#include "TransformSample.h"


//#include "TransformSample.h";

//#include "..\Vupp\vuppinterface.h"
//#include "vuppinterface.h"

//#include "..\directx\streams.h"
#include <dshow.h>
#include "itpbts.h"

#include <afxctl.h>

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif




BOOL isSameCLSID(CLSID *compare1, const struct _GUID *compare2);

// DmpCtl.h : Declaration of the CDmpCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CDmpCtrl : See DmpCtl.cpp for implementation.

class CDmpCtrl//NO_ACTIVEX :  public COleControl
{
	//NO_ACTIVEX DECLARE_DYNCREATE(CDmpCtrl)

// Constructor
public:
	CDmpCtrl();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{zAFX_VIRTUAL(CDmpCtrl)
	public:
	//}}zAFX_VIRTUAL

	bool bCatchEvents;
	void handleEvent(long lEventCode);
	IGraphBuilder *pIGB;

	BOOL bProcessEvents;
// Implementation
public:

	double mRate;

	IVuppInterface *pIVuppInterface;
//	ITPBTS *pITPBTs;
	HANDLE hEventHandler;
	HANDLE hThread;

	long currentPosition;
	long stopPosition;
	long startPosition;

	BOOL bInitialized;
	BOOL bFileLoaded;
	//BOOL bEndOfStream;
	BOOL bUseVupp;

	BOOL setSync();
	BOOL insertSource(WCHAR *wFileName);
	BOOL getInterfaceFromFilter(CLSID interfaceCLSID, IID interfaceIID,
									  void **interfacePointer);
	BOOL FindAudioFilter(CLSID *pAudioFilterID);
	BOOL insertAfter(CLSID pBefore, CLSID pInsert);
	BOOL createFilterGraph();
	BOOL destroyFilterGraph();

	int thisInstanceNumber;

	 //BOOL GetEndOfStream();
	 //void SetEndOfStream(BOOL bNewValue);
	 BOOL fileName(LPCSTR iszFileName);
	 BOOL play();
	 BOOL stop();
	 long getPosition();
	 BOOL setPosition(long ilPosition);
	 BOOL setRate(double idRate);
	 double getRate();
	 long getDuration();
	 BOOL setStopPosition(long istopPosition);
	 BOOL insertVupp();
	 long getState();
	 BOOL pause();
	 BOOL setVuppProperties(double idCrossFadeSpeed, short isWindowLength, short isCycleSpeed, short isThreshold, short isRemainder, short isSilence);
	 BOOL setVolume(long ilVolume);
	 long getVolume();
	 //void stopDmpThread();

	 HWND parentWnd; 
	void setCallbackHwnd(HWND);

	 typedef void (*sendMessageCallbackFn)(void *);
	 sendMessageCallbackFn sendMessageCallback;
	void setCallback(sendMessageCallbackFn pFunction);

	static DWORD WINAPI eventHandler(LPVOID lpParam) {
		long lEventCode, lParam1, lParam2;
		HRESULT hResult = E_FAIL;
		bool bSelfBreak = false;
		
		//CoInitialize(NULL);
		//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		CDmpCtrl *pDMP = (CDmpCtrl*)lpParam;
		IMediaEvent *pIME;
		if (pDMP->pIGB->QueryInterface(IID_IMediaEvent, (void**) &pIME)) {
		
		CoUninitialize();
		pDMP = NULL;
		return 0;
		};
		
		while((pDMP->bCatchEvents) && (!bSelfBreak)) {

			hResult = E_FAIL;
			//__try {
				hResult = pIME->GetEvent(&lEventCode, &lParam1, &lParam2, INFINITE);
				pIME->FreeEventParams(lEventCode, lParam1, lParam2);
				if ((hResult == S_OK) && (pDMP->bProcessEvents)) {
					switch(lEventCode) {
/*						case EC_USER + 1:
							pDMP->handleEvent(lEventCode);
							break;*/
						case EC_COMPLETE:
							pDMP->handleEvent(lEventCode);
							break;
						case EC_USER + 4:
							bSelfBreak = true;
							break;
					}
				}
			/*} __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) {
				lEventCode = 2;
				//MessageBox(NULL, "Access violation!", MB_OK);
			}*/
		}
		pIME->Release();

		CoUninitialize();
		pDMP = NULL;
		return 0;
	}

	long logSession(const char *string) {
/*		FILE *file = fopen("d:\\dmp.log", "a");
		if (file == NULL) return 0;

		char *buffer = (char*) malloc(sizeof(char) * 300);
		sprintf(buffer, "instance %d, position %8g: %s\n", thisInstanceNumber, 
			((double) getPosition() / 1000), string);

		//if (fprintf(file, string) < 0) return 0;
		if (fprintf(file, buffer) < 0) return 0;
		free (buffer);

		if (!fclose(file)) return 0;
*/
		return 1;
	}

	long logSession(long number) {
/*		FILE *file = fopen("d:\\dmp.log", "a");
		if (file == NULL) return 0;

		char *buffer = (char*) malloc(sizeof(char) * 300);
		sprintf(buffer, "instance %d, position %8g: %d\n", thisInstanceNumber,
			((double) getPosition() / 1000), number);

		//if (fprintf(file, string) < 0) return 0;
		if (fprintf(file, buffer) < 0) return 0;
		free (buffer);

		//if (fprintf(file, "%d", number) < 0) return 0;

		if (!fclose(file)) return 0;
*/
		return 1;
	}

	~CDmpCtrl();

	//NO_ACTIVEX
	/*
	DECLARE_OLECREATE_EX(CDmpCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CDmpCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDmpCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CDmpCtrl)		// Type name and misc status
	*/

// Message maps
	//{{zAFX_MSG(CDmpCtrl)
	//}}zAFX_MSG
	//NO_ACTIVEX DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CDmpCtrl)
	//NO_ACTIVEX
	/*
	afx_msg BOOL GetEndOfStream();
	afx_msg void SetEndOfStream(BOOL bNewValue);
	afx_msg BOOL fileName(LPCSTR iszFileName);
	afx_msg BOOL play();
	afx_msg BOOL stop();
	afx_msg long getPosition();
	afx_msg BOOL setPosition(long ilPosition);
	afx_msg BOOL setRate(double idRate);
	afx_msg double getRate();
	afx_msg long getDuration();
	afx_msg BOOL setStopPosition(long istopPosition);
	afx_msg BOOL insertVupp();
	afx_msg long getState();
	afx_msg BOOL pause();
	afx_msg BOOL setVuppProperties(double idCrossFadeSpeed, short isWindowLength, short isCycleSpeed, short isThreshold, short isRemainder, short isSilence);
	afx_msg BOOL setVolume(long ilVolume);
	afx_msg long getVolume();
	afx_msg void stopDmpThread();
	*/
	//}}AFX_DISPATCH
	//NO_ACTIVEX DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CDmpCtrl)
	//NO_ACTIVEX
	/*
	void FireNewTime(long ilTime)
		{FireEvent(eventidNewTime,EVENT_PARAM(VTS_I4), ilTime);}
	void FireEndOfStream()
		{FireEvent(eventidEndOfStream,EVENT_PARAM(VTS_NONE));}
	void FireError(long errorNumber)
		{FireEvent(DISPID_ERROREVENT,EVENT_PARAM(VTS_I4), errorNumber);}
		*/
	//}}AFX_EVENT
	//NO_ACTIVEX DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	//NO_ACTIVEX
	/*
	enum {
	//{{AFX_DISP_ID(CDmpCtrl)
	dispidEndOfStream = 1L,
	dispidFileName = 2L,
	dispidPlay = 3L,
	dispidStop = 4L,
	dispidGetPosition = 5L,
	dispidSetPosition = 6L,
	dispidSetRate = 7L,
	dispidGetRate = 8L,
	dispidGetDuration = 9L,
	dispidSetStopPosition = 10L,
	dispidInsertVupp = 11L,
	dispidGetState = 12L,
	dispidPause = 13L,
	dispidSetVuppProperties = 14L,
	dispidSetVolume = 15L,
	dispidGetVolume = 16L,
	dispidStopDmpThread = 17L,
	eventidEndOfStream = 2L,
	eventidNewTime = 1L,
	//}}AFX_DISP_ID
	};
	*/
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*
extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
*/

#endif // !defined(AFX_DMPCTL_H__62F1A308_A87A_472C_970B_65BC44BDA998__INCLUDED)
