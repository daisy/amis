// DmpCtl.cpp : Implementation of the CDmpCtrl ActiveX Control class.


#include "StdAfxFIX.h"

#include "DmpCtl.h"

#include "DmpPpg.h"

#include <string>
using namespace std;

#include "resource.h"

#include <assert.h>

//#include <dshow.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//med #include "../RegOcx.h"

/*
const GUID CDECL BASED_CODE _tlid =
		{ 0xd825129b, 0xb483, 0x46bc, { 0xb9, 0x4f, 0x6, 0xc1, 0xfd, 0x5a, 0xeb, 0xb1 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
*/

//NO_ACTIVEX IMPLEMENT_DYNCREATE(CDmpCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

//NO_ACTIVEX
/*
zBEGIN_MESSAGE_MAP(CDmpCtrl, COleControl)
	//{{zAFX_MSG_MAP(CDmpCtrl)
	//}}zAFX_MSG_MAP
	zON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
zEND_MESSAGE_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

//NO_ACTIVEX
/*
zBEGIN_DISPATCH_MAP(CDmpCtrl, COleControl)
	//{{zAFX_DISPATCH_MAP(CDmpCtrl)
	DISP_PROPERTY_EX(CDmpCtrl, "EndOfStream", GetEndOfStream, SetEndOfStream, VT_BOOL)
	DISP_FUNCTION(CDmpCtrl, "fileName", fileName, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CDmpCtrl, "play", play, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "stop", stop, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "getPosition", getPosition, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "setPosition", setPosition, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CDmpCtrl, "setRate", setRate, VT_BOOL, VTS_R8)
	DISP_FUNCTION(CDmpCtrl, "getRate", getRate, VT_R8, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "getDuration", getDuration, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "setStopPosition", setStopPosition, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CDmpCtrl, "insertVupp", insertVupp, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "getState", getState, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "pause", pause, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "setVuppProperties", setVuppProperties, VT_BOOL, VTS_R8 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CDmpCtrl, "setVolume", setVolume, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CDmpCtrl, "getVolume", getVolume, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDmpCtrl, "stopDmpThread", stopDmpThread, VT_EMPTY, VTS_NONE)
	//}}zAFX_DISPATCH_MAP
zEND_DISPATCH_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// Event map

//NO_ACTIVEX
/*
zBEGIN_EVENT_MAP(CDmpCtrl, COleControl)
	//{{zAFX_EVENT_MAP(CDmpCtrl)
	zEVENT_CUSTOM("newTime", FireNewTime, VTS_I4)
	zEVENT_CUSTOM("endOfStream", FireEndOfStream, VTS_NONE)
	zEVENT_CUSTOM_ID("error", DISPID_ERROREVENT, FireError, VTS_I4)
	//}}zAFX_EVENT_MAP
zEND_EVENT_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
//NO_ACTIVEX
/*
BEGIN_PROPPAGEIDS(CDmpCtrl, 1)
	PROPPAGEID(CDmpPropPage::guid)
END_PROPPAGEIDS(CDmpCtrl)
*/

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

//NO_ACTIVEX
/*
IMPLEMENT_OLECREATE_EX(CDmpCtrl, "DMP.DmpCtrl.1",
	0xef821ae, 0x89ee, 0x41c5, 0x87, 0xd0, 0xff, 0xb, 0x33, 0xba, 0x4b, 0xa6)
*/

/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

//NO_ACTIVEX
/*
IMPLEMENT_OLETYPELIB(CDmpCtrl, _tlid, _wVerMajor, _wVerMinor)
*/

/////////////////////////////////////////////////////////////////////////////

// Interface IDs

//NO_ACTIVEX
/*
const IID BASED_CODE IID_DDmp =
		{ 0x12d87155, 0xaf68, 0x4b5d, { 0xac, 0x74, 0x72, 0xa1, 0x69, 0x21, 0xc8, 0xbd } };

const IID BASED_CODE IID_DDmpEvents =
		{ 0x2257ed6e, 0xced8, 0x4f21, { 0x95, 0x1c, 0x76, 0x22, 0xec, 0x21, 0x3, 0x16 } };
*/

/////////////////////////////////////////////////////////////////////////////
// Control type information

//NO_ACTIVEX
/*
static const DWORD BASED_CODE _dwDmpOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDmpCtrl, IDS_DMP, _dwDmpOleMisc)
*/

/////////////////////////////////////////////////////////////////////////////
// CDmpCtrl::CDmpCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDmpCtrl

//NO_ACTIVEX
/*
BOOL CDmpCtrl::CDmpCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister) {}
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_DMP,
			IDB_DMP,
			afxRegApartmentThreading,
			_dwDmpOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}
*/

/*
	Code starts here.
*/

EXTERN_GUID(WMFORMAT_WaveFormatEx, 
0x05589f81, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a); 

// Interface and class id declarations, these are IDs of interfaces and classes 
// that we use.

/*
const IID CLSID_MPEG3Decoder =
		{ 951988224, 56308, 4560, { 134,14,0,160,36,207,239,109 } };

const IID IID_IVuppInterface =
		{ 0xa33e626e, 0xd6c4, 0x4559, { 0xa1, 0xd6, 0x9f, 0x1d, 0x95, 0xf0, 0xd8, 0xe2 } };

const IID CLSID_TPBVupp10 =
		{ 0x66172967, 0x56c5, 0x4b89,{ 0xaa, 0x92, 0xc9, 0xef, 0xec, 0x56, 0x46, 0x7b } };

const IID IID_ITPBTS = {
	0x5dd2d89b, 0x6925, 0x4ceb, 0x9c, 0x20, 0xad, 0x79, 0xf, 0x76, 0xe6, 0x38};

const IID CLSID_TPBTSFilter = {
	0xb9b0860, 0xf562, 0x43da, 0x8e, 0xd4, 0xfd, 0x3f, 0xdb, 0xcd, 0x99, 0x6};
*/


/////////////////////////////////////////////////////////////////////////////
// CDmpCtrl::CDmpCtrl - Constructor

CDmpCtrl::CDmpCtrl()
{
	// Initialize COM engine for use by this thread
	//CoInitialize(NULL);
	//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//med RegisterOCX();

	//NO_ACTIVEX InitializeIIDs(&IID_DDmp, &IID_DDmpEvents);

	static int instanceCount = 0;
	thisInstanceNumber = instanceCount;
	instanceCount++;

	parentWnd=NULL;

	bInitialized = false;
	bFileLoaded = false;
	bUseVupp = false;
	//bEndOfStream = false;
	bCatchEvents = false;

	hEventHandler = NULL;
//	pITPBTs = NULL;
	pIVuppInterface = NULL;
	pIGB = NULL;

	startPosition = 0;
	stopPosition = 0;

	bProcessEvents = true;

	hThread = GetCurrentThread();

	logSession("Construct");
	mRate = 1.0;
}

/////////////////////////////////////////////////////////////////////////////
// CDmpCtrl::~CDmpCtrl - Destructor

CDmpCtrl::~CDmpCtrl()
{
	destroyFilterGraph();
	bFileLoaded = false;
	bInitialized = false;

	logSession("Destruct");

	CoUninitialize();

	//med UnregisterOCX();
}

BOOL CDmpCtrl::createFilterGraph() {
	
	logSession("Create filtergraph");

	if (pIGB != NULL) {
		destroyFilterGraph();
	}
	
	bInitialized = FALSE;
	bFileLoaded = FALSE;

	// Here we start creating a FilterGraph object and derivate several
	// interfaces from the object.	

    	if (CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
	    	IID_IGraphBuilder, (void **) &pIGB) != S_OK) {

		//NO_ACTIVEX FireError(lError);
		destroyFilterGraph();
		return false;
	}

	bCatchEvents = true;
	//LPDWORD lpdwThreadID;
	unsigned long lpdwThreadID;
	bProcessEvents = true;
	hEventHandler = CreateThread(NULL, 0, eventHandler, this, 0, &lpdwThreadID);

	

	startPosition = 0;
	bInitialized = TRUE;

	return true;
}

BOOL CDmpCtrl::destroyFilterGraph() {

	logSession("Destroy filtergraph");
	stop();

	if (hEventHandler) {
		
	bCatchEvents = true;
		bProcessEvents = true;

		IMediaEventSink *pIMES = NULL;
		pIGB->QueryInterface(IID_IMediaEventSink, (void**) &pIMES);
		pIMES->Notify(EC_USER + 4, 0, 0);
		pIMES->Release();
		pIMES = NULL;

		
//	bCatchEvents = false;
//		threadSuspended = true;

		//SuspendThread(hEventHandler);
		if (WaitForSingleObject(hEventHandler, INFINITE) != WAIT_OBJECT_0) {
			return false;
		}
		
	}

/*	if (pITPBTs != NULL) {
		pITPBTs->StopEventEngine();
		pITPBTs->Release();
		pITPBTs = NULL;
	}
*/	

	if (pIGB != NULL) {
		//pIGB->Release();
		pIGB = NULL;
	}

	if (pIVuppInterface != NULL) {
		int f =pIVuppInterface->Release();
		pIVuppInterface = NULL;
	}
	return true;
}


// This function inserts a AsyncReader source filter into the FilterGraph filter 
// chain and sets the filename property on the source filter.
BOOL CDmpCtrl::insertSource(WCHAR *wFileName)
{
	IBaseFilter *pAsyncFilter = NULL;
	IPin *pPin = NULL;
	IEnumPins *pEPins = NULL;
	

		// Create a AsyncReader object and derivate the BaseFilter interface
		if (CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void **) &pAsyncFilter) != S_OK) {
				if (pAsyncFilter != NULL) pAsyncFilter->Release();
				return false;
			}
		
		// Add the AsyncReader filter to the filtergraph
			if (pIGB->AddSourceFilter(wFileName, wFileName, &pAsyncFilter) != S_OK) {
				if (pAsyncFilter != NULL) pAsyncFilter->Release();
			return false;
			}
		
		// Enumerate the pins on the AsyncReader object, find the first pin and let
		// the FilterGraph render the whole filter chain from the pin.
		if (pAsyncFilter->EnumPins(&pEPins) != S_OK)  {
				if (pAsyncFilter != NULL) pAsyncFilter->Release();
				
	if (pEPins != NULL) pEPins->Release();
			return false;
			}
		if (pEPins->Next(1, &pPin, NULL) != S_OK)  {
				
	if (pPin != NULL) pPin->Release();
	if (pEPins != NULL) pEPins->Release();
	
				if (pAsyncFilter != NULL) pAsyncFilter->Release();
			return false;
			}
		if (pIGB->Render(pPin) != S_OK)  {
			
	if (pPin != NULL) pPin->Release();
	if (pEPins != NULL) pEPins->Release();
				if (pAsyncFilter != NULL) pAsyncFilter->Release();
				
			return false;
			}

	if (pPin != NULL) pPin->Release();
	if (pEPins != NULL) pEPins->Release();
 	if (pAsyncFilter != NULL) pAsyncFilter->Release();
	return true;
}

BOOL CDmpCtrl::FindAudioFilter(CLSID *pAudioFilterID) {

	IBaseFilter *ipAsyncReaderFilter;
	IEnumFilters *pEnumFilters = NULL;

	// Tell the FilterGraph to enumerate all filters in the filter chain
	pIGB->EnumFilters(&pEnumFilters);
	CLSID *pCLSID = (CLSID*) malloc(sizeof(CLSID));

	BOOL found = 0;
	
	// Go trough the enumberation of all filters and put the current filter in pFilter
	while (pEnumFilters->Next(1, &ipAsyncReaderFilter, NULL) == S_OK) {
		
		// Ask the current filter for it's CLASS ID
		ipAsyncReaderFilter->GetClassID(pCLSID);
		// If the ID is the same as the one that we put in pAfter do a lot of stuff...
		if (isSameCLSID(pCLSID, &CLSID_AsyncReader)) {
			found = TRUE;
			break;
		}
		ipAsyncReaderFilter->Release();
	}
	pEnumFilters->Release();

	if (found && !isSameCLSID(pCLSID, &CLSID_AsyncReader)) {
		free (pCLSID);
		ipAsyncReaderFilter->Release();
		return false;
	}
	if (!found) {
		//free (pCLSID);
		//ipAsyncReaderFilter->Release();
		//return false;
		int i = 0 ;
	}
	
	if (found) {
		//free (pCLSID);
		//ipAsyncReaderFilter->Release();
		//return false;
		int i = 0 ;
	}

	free (pCLSID);
	
	IEnumPins *pPinEnum = NULL;
	IPin *pPin = NULL;
	PIN_DIRECTION pPinDir;
	PIN_INFO pPin_Info;
	
	IBaseFilter *ptempFilter = NULL;
	
	BOOL bolFound = false;
	
	//Gets the Interface that is connected to the sourcefilter
	ptempFilter = ipAsyncReaderFilter;
	ptempFilter->AddRef(); 

	

	//iterate the pins and the FilterInterfaces, find first OUTPUT-pin with mediatype = Audio
	//and wFormatTag == 1 save that filters ID in pAudioFilterID
	while ((ptempFilter != NULL) && (bolFound == false)){  
		
		if (ptempFilter->EnumPins(&pPinEnum) != S_OK) {
			return FALSE;
		}
			// Find the output pin
		while ((pPinEnum->Next(1, &pPin, NULL) == S_OK) && (!bolFound)) {
			// Ask the current pin for it's direction
			pPin->QueryDirection (&pPinDir);
			// If an output pin is found, check the mediatype
			if (pPinDir == PINDIR_OUTPUT) {
				AM_MEDIA_TYPE pmt;
				if (pPin->ConnectionMediaType(&pmt) == S_OK){									
					
					ptempFilter->GetClassID(pAudioFilterID);
					WAVEFORMATEX *waveFormat;
					waveFormat = (WAVEFORMATEX*) pmt.pbFormat;
					//If majortype = Audio, formattype == WMFORMAT_WaveFormatEx 
					//and wFormatTag == 1 (means rawdata) then get the ID and exit	
					if ((pmt.majortype == MEDIATYPE_Audio) && 
						(pmt.formattype == WMFORMAT_WaveFormatEx) && 
						((*waveFormat).wFormatTag == 1)){
//						MessageBox(NULL, "Audiomediatype", MB_OK);
						ptempFilter->GetClassID(pAudioFilterID);
						bolFound = true;						
					} 
					else{
						// move to the next filter in the chain (via the output-, input-pin)
						//and release the interface to the current filter
						IPin *tempIPin;
						
						if (pPin->ConnectedTo(&tempIPin) != S_OK) 
						{
							return false; 
						}
						//pPin_Info Contains a IBaseFilter (pFilter)
						tempIPin->QueryPinInfo(&pPin_Info);
						//The tempIPin is no longer neded
						tempIPin->Release();	
						//The ptempFilter will be soon be assigned a interface to 
						//the next filter in the chain. Therefore must be released.  
						ptempFilter->Release();
						//Gets an interface to the interface via it's INPUT-pin
						ptempFilter = (pPin_Info).pFilter;
				
					}
				}
			}
			// Release the current pin
			ULONG pPincount = pPin->Release();
			
		}
		
	ULONG pPinEnumcount =	pPinEnum->Release();
		
	}

	if (ptempFilter != NULL){	
		ULONG ptempFiltercount = ptempFilter->Release();
	}
	
	ipAsyncReaderFilter->Release();
	
	return true;
}

// This function creates and inserts a filter with a given ID (pInsert) in the 
// filterchain. This filter is to be placed AFTER a filter that allready exist 
// in the chain that has it's ID contained in pAfter
BOOL CDmpCtrl::insertAfter(CLSID pAfter, CLSID pInsert) {

	IBaseFilter *pIBF = NULL;

	//bool bIsVupp = isSameCLSID(&pInsert, &CLSID_TPBVupp10);
	//bIsVupp = false;

	if (false) {
		//HRESULT hr =CoInitialize(0);
		//pIBF = (IBaseFilter*) CTransformSample::CreateInstance((LPUNKNOWN)0, &hr);

		// --- USE THIS !! pIBF = (IBaseFilter*) new CTransformSample(NAME("TPB Vupp 1.0"), (LPUNKNOWN)0, CLSID_TPBVupp10);
	} else {
		
	// Create the new filter by using the supplied ID (pInsert) and get the 
	// BaseFilter interface from the object created.
	HRESULT res;
	res = CoCreateInstance(pInsert, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter,
		(void**) &pIBF);

			if (res != S_OK) {
				if (res == REGDB_E_CLASSNOTREG) {
					int i = 1;

				}
				if (res == CLASS_E_NOAGGREGATION) {
					int i = 1;

				}
				if (res == E_NOINTERFACE) {
					int i = 1;

				}
				return FALSE;

			}
	}


			
		

	// Add the newly created filter to the filter chain
	if (pIGB->AddFilter(pIBF, NULL) != S_OK) {
		pIBF->Release();
		/* if (bIsVupp) {
			delete pIBF;
		} */
		return FALSE;
	}

	IEnumPins *pPinEnum = NULL;
	IPin *pPin = NULL;
	PIN_DIRECTION pPinDir;
	
	IPin *pPinInsertIn = NULL;
	IPin *pPinInsertOut = NULL;
	
	// Enumerate the pins on the newly created filter
	if (pIBF->EnumPins(&pPinEnum) != S_OK) return FALSE;

	// Go trough the enumberation of pins and put the current pin in pPin
	while (pPinEnum->Next(1, &pPin, NULL) == S_OK) {
		
		// Ask the current pin if it's an INPUT or OUTPUT pin
		pPin->QueryDirection(&pPinDir);

		// If a input pin is found, put pPinInsertIn to point at the pin and tell
		// the pin that we have added a reference to it.
		if (pPinDir == PINDIR_INPUT) {
			pPinInsertIn = pPin;
			pPin->AddRef();
		}
		// If a output pin is found, put pPinInsertOut to point at the pin and tell
		// the pin that we have added a reference to it.
		else {
			pPinInsertOut = pPin;
			pPin->AddRef();
		}
		
		// Release the pin that we retrieved trough the enumberation
		pPin->Release();
	}
	// Release the enumberation
	pPinEnum->Release();
	

	IEnumFilters *pEnumFilters = NULL;

	// Tell the FilterGraph to enumerate all filters in the filter chain
	pIGB->EnumFilters(&pEnumFilters);
	
	CLSID *pCLSID = (CLSID*) malloc(sizeof(CLSID));

	IPin *pPinBeforeOut = NULL;
	IPin *pPinAfterIn = NULL;

	IBaseFilter *pFilter = NULL;

	// Go trough the enumberation of all filters and put the current filter in pFilter
	
		while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK) {

			// Ask the current filter for it's CLASS ID
			pFilter->GetClassID(pCLSID);

			// If the ID is the same as the one that we put in pAfter do a lot of stuff...
			if (isSameCLSID(pCLSID, &pAfter)) {

				// Enumerate all pins in the current filter
				if (pFilter->EnumPins(&pPinEnum) != S_OK) return FALSE;

				// Go trough the enumberation of pins and put the current pin in pPin
				while (pPinEnum->Next(1, &pPin, NULL) == S_OK) {

					// Ask the current pin for it's direction
					pPin->QueryDirection (&pPinDir);

					// If an output pin is found, put a reference to pPinBeforeOut and
					// tell the pin that we've got a reference
					if (pPinDir == PINDIR_OUTPUT) {
						pPinBeforeOut = pPin;
						pPin->AddRef();
					}

					// Release the current pin
					pPin->Release();
				}

				// Release the pin enumberation
				pPinEnum->Release();

				
				// Ask the OUTPUT pin that we found for it's current connection,
				// that is the INPUT pin of the filter that it's currently connected to
				// and put a reference to it in pPinAfterIn
				pPinBeforeOut->ConnectedTo(&pPinAfterIn);

				// If the OUTPUT pin of the current filter has a connection,
				// disconnect it and also disconnect the INPUT pin which the OUTPUT pin
				// is connected to. That is disconnect both pins from each other.
				// It's like pulling the plug from the wall and at the same time pushing
				// the wall backwards.
				if (pPinAfterIn != NULL) {
					if (pPinBeforeOut->Disconnect() != S_OK) return FALSE;
					if (pPinAfterIn->Disconnect() != S_OK) return FALSE;
				};
				
				// Connect the OUTPUT pin of the current filter to the INPUT pin of
				// the filter that we created in the beginning (the one to be inserted)
				// of this subroutine.
				if (pIGB->Connect(pPinBeforeOut, pPinInsertIn) != S_OK) return FALSE;

				// Connect the OUTPUT pin of the filter that we created to the INPUT
				// pin of the filter that we a few lines above disconnected from the
				// OUTPUT pin of the current filter.
				if (pIGB->Connect(pPinInsertOut, pPinAfterIn) != S_OK) return FALSE;
				
				// Release all references that we've made
				if (pPinAfterIn != NULL) pPinAfterIn->Release();
				pPinInsertOut->Release();
				pPinInsertIn->Release();
				pPinBeforeOut->Release();
				pFilter->Release();
				break;
			}

			// Release the current filter
			pFilter->Release();
		}
	// Release and free up memory that we've used
	free (pCLSID);
	pEnumFilters->Release();
	pIBF->Release();
	/* if (bIsVupp) {
		delete pIBF;
	}*/

	return TRUE;
}

// This function inserts the VUPP (TSM) filter in the filter chain
BOOL CDmpCtrl::insertVupp() {
	// Insert the VUPP (TSM) filter and get it's controller interface
	
//	if (!insertAfter(CLSID_TPBTSFilter, CLSID_TPBVupp10)) return false;
	CLSID pAudioFilterID;
	if (FindAudioFilter(&pAudioFilterID)) {
		if (!insertAfter(pAudioFilterID, CLSID_TPBVupp10)) {
			return false;
		}
	}

	//if (!insertAfter(CLSID_MPEG3Decoder, CLSID_TPBVupp10)) return false;
	if (!getInterfaceFromFilter(CLSID_TPBVupp10, IID_IVuppInterface, 
		(void**) &pIVuppInterface)) {
		return false;
	}
	
	setRate(mRate);

	bUseVupp = false;

	return true;
}

// This routine retrieves a interface with a given ID (interfaceIID) from a filter
// that exist in the filter chain that has the CLASS ID of filterCLSID
BOOL CDmpCtrl::getInterfaceFromFilter(CLSID filterCLSID, IID interfaceIID,
									  void **interfacePointer) {


	IEnumFilters *pEnumFilters = NULL;
	IBaseFilter *pFilter = NULL;
	CLSID pCLSID;
	
	// Enuemrate all filters in the filter chain
	if (pIGB->EnumFilters(&pEnumFilters) != S_OK) return FALSE;

	// Go trough the enumberation and put the current filter in pFilter
	while (pEnumFilters->Next(1, &pFilter, NULL) == S_OK) {

		// Retrieve the CLASS ID from the current filter
		pFilter->GetClassID(&pCLSID);
		
		// If the CLASS ID on the current filter is the same as the supplied CLASS
		// ID (filterCLSID), do a lot of stuff...
		if (isSameCLSID(&pCLSID, &filterCLSID)) {

			// Ask the current filter for the interface with ID interfaceIID
			// and put a reference to it in interfacePointer
			if (pFilter->QueryInterface(interfaceIID, (void**) interfacePointer) != 
				S_OK) 
				return FALSE;

			// Release the current filter
			pFilter->Release();
			break;
		}

		// Release the current filter
		pFilter->Release();
	}
	
	if (!interfacePointer) return FALSE;

	pEnumFilters->Release();
	return TRUE;
}


// This function renders the whole filter chain from the given filename (iszFileName)
BOOL CDmpCtrl::fileName(LPCSTR iszFileName) 
{
	USES_CONVERSION;

	// Convert iszFileName to UNICODE (wFileName)
	//med -- why?? iszFileName is directly used later on.
	/*USES_CONVERSION;

	WCHAR wFileName[MAX_PATH];
    #ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, iszFileName, -1, wFileName, MAX_PATH);
    #else
        lstrcpy(wFileName, (const unsigned short *) iszFileName);
    #endif
	*/
		
		std::string strFileName;
		strFileName.assign(iszFileName);

//	if (pIGB) destroyFilterGraph();
	
	createFilterGraph();


	bFileLoaded = FALSE;

//	med LPCWSTR str = A2CW(strFileName.c_str());

	//med if (pIGB->RenderFile(str, NULL) != 0) {
	if (pIGB->RenderFile(A2T(iszFileName), NULL) != 0) {
	bFileLoaded = false;
		return false;
	}
/*	ke20030715

	// See insertSource
	if (!insertSource(wFileName)) return false;

	// Insert the TPBTS (time seeker) filter and get it's controller interface
	if (!insertAfter(CLSID_MPEG3Decoder, CLSID_TPBTSFilter)) return false;
	if (!getInterfaceFromFilter(CLSID_TPBTSFilter, IID_ITPBTS, (void**) &pITPBTs)) {
		return false;
	}

	pITPBTs->SetTimeInterval(1);
*/
	bFileLoaded = true;

	currentPosition = 0;
	startPosition = 0;
	stopPosition = 0;

	return true;
}

BOOL CDmpCtrl::play() 
{
	logSession("Try play");
	

	if ((!bFileLoaded) || (!bInitialized)) return false;


	IMediaControl *pIMC;
	if (pIGB->QueryInterface(IID_IMediaControl, (void**) &pIMC) != S_OK) {
		logSession("Play failed"); return false; }
	
	

	bProcessEvents = true;

	if (pIMC->Run() == S_FALSE) {
		long state, i;

		for (i = 0; i < 100; i++) {
			pIMC->GetState(10, &state);
			if (state == State_Running) break;
		}
		
		if (i == 100) { 
			pIMC->Release(); 
			logSession("Play failed");
			return false; 
		}
	}

	//SetEndOfStream(false);

//	if (pITPBTs) pITPBTs->StartEventEngine();
//	ResumeThread(hEventHandler);

	pIMC->Release();

	logSession("Play");

	return TRUE;
}

BOOL CDmpCtrl::stop() 
{
	logSession("Try stop");
	if ((!bFileLoaded) || (!bInitialized)) return false;
	//if (pIGB == NULL) return false;

	//SuspendThread(hEventHandler); 
	bProcessEvents = false;
//	if (pITPBTs) pITPBTs->StopEventEngine();

	IMediaControl *pIMC;
	if (pIGB->QueryInterface(IID_IMediaControl, (void**) &pIMC) != S_OK) {
		logSession("Stop failed");
		return false;
	}

	if (pIMC->StopWhenReady() != S_OK) {
		long state, i;

		for (i = 0; i < 100; i++) {
			pIMC->GetState(10, &state);
			if (state == State_Stopped) break;
		}
		
		if (i == 100) { 
			pIMC->Release(); 
			logSession("Stop failed");
			bProcessEvents = true;
			return false; 
		}
	}

	//SetEndOfStream(false);

	pIMC->Release();

	logSession("Stop");

	return TRUE;
}

BOOL CDmpCtrl::pause()
{
	logSession("Try pause");
	if ((!bFileLoaded) || (!bInitialized)) return false;

//	SuspendThread(hEventHandler); 
	bProcessEvents = true;
//	if (pITPBTs) pITPBTs->StopEventEngine();

	IMediaControl *pIMC;
	if (pIGB->QueryInterface(IID_IMediaControl, (void**) &pIMC) != S_OK) {
		logSession("Pause failed"); return false; }

	if (pIMC->Pause() != S_OK) {
		long state, i;

		for (i = 0; i < 100; i++) {
			pIMC->GetState(10, &state);
			if (state == State_Paused) break;
		}
		
		if (i == 100) { 
			pIMC->Release(); 
			logSession("Pause failed");
			return false; 
		}
	}

	pIMC->Release();

	logSession("Pause");

	return TRUE;
}

long CDmpCtrl::getPosition() 
{
	if ((!bFileLoaded) || (!bInitialized)) return -1;

	LONGLONG llPosition = 0;

/*	if (pITPBTs) {
		REFERENCE_TIME timeWithRate;
		if (pITPBTs->GetTime(&timeWithRate) != S_OK) {
			currentPosition = 0;
		}
		currentPosition = (long) (timeWithRate / 10000);
	} else {*/
		IMediaSeeking *pIMS;
		pIGB->QueryInterface(IID_IMediaSeeking, (void**) &pIMS);

		if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) != S_OK) return -1;
		if (pIMS->GetPositions(&llPosition, NULL) != S_OK) return -1;
  		currentPosition = (long) ((llPosition / 10000) & 0xFFFFFFFF);

		pIMS->Release();
//	};

	return currentPosition;
}

BOOL CDmpCtrl::setPosition(long ilPosition) 
{
	logSession("Try set position");
	logSession(ilPosition);
	if ((!bFileLoaded) || (!bInitialized)) return false;
		
	bool bReturn = true;
	LONGLONG Value = (LONGLONG) (ilPosition);
	Value *= 10000;

	IMediaSeeking *pIMS;
	if (pIGB->QueryInterface(IID_IMediaSeeking, (void**) &pIMS) != S_OK) {
		logSession("Set position failed"); return false; }

	if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
		if (pIMS->SetPositions(&Value, AM_SEEKING_AbsolutePositioning, NULL, 
			AM_SEEKING_NoPositioning) != S_OK) bReturn = false;
	} else { bReturn = false; };

	//SetEndOfStream(false);

	if (bReturn == false) logSession("Set position failed");

	pIMS->Release();

	logSession("Set position");

	return bReturn;
}

long CDmpCtrl::getDuration() 
{
	if ((!bFileLoaded) || (!bInitialized)) return -1;

	bool bReturn = true;
	LONGLONG llDuration = 0;

	IMediaSeeking *pIMS;
	if (pIGB->QueryInterface(IID_IMediaSeeking, (void**) &pIMS) != S_OK) return -1;

	if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
		if (pIMS->GetPositions(NULL, &llDuration) != S_OK) llDuration = -10000;
	} else { llDuration = - 10000; };

	long retval = (long) ((llDuration / 10000) & 0xFFFFFFFF);	  

	pIMS->Release();
	  
	return retval;
}

BOOL CDmpCtrl::setStopPosition(long istopPosition)
{
	logSession("Try set stop position");
	logSession(istopPosition);
	if ((!bFileLoaded) || (!bInitialized)) return false;

	//stopPosition = istopPosition;

	BOOL bReturn = true;
	LONGLONG Value = (LONGLONG) (istopPosition);
	Value *= 10000;

	IMediaSeeking *pIMS;
	pIGB->QueryInterface(IID_IMediaSeeking, (void**) &pIMS);
	if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
		if (pIMS->SetPositions(NULL, AM_SEEKING_NoPositioning, &Value, 
			AM_SEEKING_AbsolutePositioning) != S_OK) bReturn = false;
	} else { bReturn = false; };

	pIMS->Release();
	logSession("Set stop position");
	
	//return TRUE;
	return bReturn;
}

BOOL CDmpCtrl::setRate(double idRate) {
	if (idRate != 1.0) {
		int i = 1; //dsfds
	} else {
		int i = 1; //dsfds
	}

	mRate = idRate;

	if ((!bFileLoaded) || (!bInitialized)) {
		return false;
	}
	
	/*IMediaSeeking *pIMS;
	pIGB->QueryInterface(IID_IMediaSeeking, (void**) &pIMS);
	pIMS->SetRate(idRate);
	pIMS->Release();*/

	if (pIVuppInterface) {
		short cs = (short)(idRate*(double)100.0);
		pIVuppInterface->setCycleSpeed(cs);
	}

	/*
	pIVuppInterface->setCrossFadeSpeed(idCrossFadeSpeed / 100);
	pIVuppInterface->setWindowLength(isWindowLength);
	pIVuppInterface->setSilenceLoudnessThreshold(isThreshold);
	pIVuppInterface->setSilenceRemainderLength(isRemainder);
	pIVuppInterface->setSilenceSpeed(isSilence);
	*/

	return TRUE;
}

double CDmpCtrl::getRate() 
{
	if ((!bFileLoaded) || (!bInitialized)) return -1;

	double dRate = 0;
//	if (pIMS->GetRate(&dRate) != S_OK) return -1;
	//pIVuppInterface->getCycleSpeed(&rate);
	return dRate;
}

BOOL CDmpCtrl::setVuppProperties(double idCrossFadeSpeed, short isWindowLength,
								 short isCycleSpeed, short isThreshold,
								 short isRemainder, short isSilence) 
{
	if ((!bFileLoaded) || (!bInitialized)) return false;

	if (!pIVuppInterface) return false;
		
	pIVuppInterface->setCrossFadeSpeed(idCrossFadeSpeed / 100);
	pIVuppInterface->setWindowLength(isWindowLength);
	pIVuppInterface->setCycleSpeed(isCycleSpeed);
	pIVuppInterface->setSilenceLoudnessThreshold(isThreshold);
	pIVuppInterface->setSilenceRemainderLength(isRemainder);
	pIVuppInterface->setSilenceSpeed(isSilence);

	return TRUE;
}

long CDmpCtrl::getState()
{
	if ((!bFileLoaded) || (!bInitialized)) return -1;

	IMediaControl *pIMC;
	if (pIGB->QueryInterface(IID_IMediaControl, (void**) &pIMC) != S_OK) return -1;

	long filterState;
	pIMC->GetState(1000, &filterState);
	pIMC->Release();

	return filterState;
}
void CDmpCtrl::setCallback(sendMessageCallbackFn pFunction)
{
	sendMessageCallback=pFunction;
}
void CDmpCtrl::setCallbackHwnd(HWND hWnd)
{
	parentWnd = hWnd;
}
void CDmpCtrl::handleEvent(long lEventCode)
{
	//__try {
/*		REFERENCE_TIME timeWithRate;
		pITPBTs->GetTime(&timeWithRate);
		WaitForSingleObject(hThread, 10);

		currentPosition = (long) (timeWithRate / 10000);

		if (((currentPosition >= stopPosition) && (stopPosition > 0)) || 
			(lEventCode == EC_COMPLETE)) {
*/			
			if (bProcessEvents) {
				stopPosition = 0;
				//SetEndOfStream(true);
				sendMessageCallback(parentWnd);
			//	pause();
			}
//		}
	/*} __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) {
		lEventCode = 2;
	}*/
}

/*
BOOL CDmpCtrl::GetEndOfStream() 
{
	if ((!bFileLoaded) || (!bInitialized)) return false;
	if (bEndOfStream) { bEndOfStream = false; return TRUE; }
	return FALSE;
}

void CDmpCtrl::SetEndOfStream(BOOL bNewValue) 
{
	logSession("Set end of stream");
	bEndOfStream = bNewValue;
	//NO_ACTIVEX SetModifiedFlag();
}
*/

BOOL CDmpCtrl::setVolume(long ilVolume) 
{
	// TODO: Add your dispatch handler code here
	logSession("Try set volume");

	IBasicAudio *pIBA;

	bool bReturn = true;

	ilVolume = 0 - ((10 - ilVolume) * 300);

	if (pIGB->QueryInterface(IID_IBasicAudio, (void**) &pIBA) != S_OK) {
		logSession("Set volume failed"); return false; }

	// Full volume is 0, and E0,000 is silence
	if (pIBA->put_Volume(ilVolume) != S_OK) bReturn = false;

	if (bReturn == false) logSession("Set volume failed");

	pIBA->Release();

	logSession("Set volume");

	return bReturn;
}

long CDmpCtrl::getVolume() 
{
	logSession("Try get volume");

	IBasicAudio *pIBA;

	bool bReturn = true;
	long lVolume = -1;

	if (pIGB->QueryInterface(IID_IBasicAudio, (void**) &pIBA) != S_OK) {
		logSession("Get volume failed"); return -1; }

	pIBA->get_Volume(&lVolume);

	if (lVolume == -1) 
		logSession("Get volume failed");
	else
		lVolume = -((0 - lVolume) / 300) - 10;

	pIBA->Release();

	logSession("Get volume");

	return lVolume;
}
//Tor 20030722, This is to stop the thread froom VB before exiting
/* void CDmpCtrl::stopDmpThread() 
{
	SuspendThread(hEventHandler);

}
*/

BOOL isSameCLSID(CLSID *compare1, const struct _GUID *compare2)
{  
	if ((compare1->Data1 != compare2->Data1) ||
	  (compare1->Data2 != compare2->Data2) ||
	  (compare1->Data3 != compare2->Data3)) return FALSE;

	int j = 0;
	while (j < 8)  {
	  if (*(compare1->Data4 + j) != *(compare2->Data4 + j)) return FALSE;
	  j++;
	};
  
	return TRUE;
}