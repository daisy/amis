// Microsoft C Compiler will give hundreds of warnings about
// unused inline functions in header files.  Try to disable them.
//#pragma warning(disable:4514)

//#include "..\directx\streams.h"
#include <dshow.h>

//#pragma comment(lib,"Strmiids.lib") 

#include <stdio.h>
#include <math.h>

#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
#include <iostream>

//#include "pins.h"
#include "vupp1.h"
#include "vuppInterface.h"

//#include "TransformDllHeader.h"

//ke20030630
//#include "..\Queuetest\tpbqueue.h"

// GUID for the CTransformSample class

DEFINE_GUID(CLSID_TPBVupp10, 
	0x66172967, 0x56c5, 0x4b89, 0xaa, 0x92, 0xc9, 0xef, 0xec, 0x56, 0x46, 0x7b);

#define NUM_BUFFS 1

// Active movie register filter information

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{ &MEDIATYPE_Audio, &MEDIASUBTYPE_PCM };

const AMOVIESETUP_PIN sudPins[] =
{	
	{ L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, L"", 1, &sudPinTypes },
	{ L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, L"", 1, &sudPinTypes }
};

const AMOVIESETUP_FILTER sudFilter = 
{ &CLSID_TPBVupp10, L"TPB VuppTest 1.0", MERIT_DO_NOT_USE, 2, sudPins };
	//Dll init functions
 


// Class begin

class CTransformSample
    : public CTransformFilter,
	  public IVuppInterface
{

public:

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
    
	DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	

	STDMETHODIMP setCrossFadeSpeed(double speed);
	STDMETHODIMP setCycleSpeed(short speed);
	STDMETHODIMP setSilenceLoudnessThreshold(short threshold);
	STDMETHODIMP setSilenceRemainderLength(short length);
	STDMETHODIMP setSilenceSpeed(short speed);
	STDMETHODIMP setWindowLength(int length);
	STDMETHODIMP getPosition(LONGLONG &position);

	// Get prefered mediaformat for output pin
	HRESULT GetMediaType(int position, CMediaType *pMediaTypeIn);
	
HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);

HRESULT CTransformSample::Receive(IMediaSample *pIn);

HRESULT CTransformSample::EndOfStream(void);

	//ke20030611
	short silenceThresholdPercent;
	short silenceSpeed;
	short silenceRemainderLength;
	short cycleSpeed;
	double crossfadeSpeed;
	int windowLength;

	long samplesPerSecond;
	long channels;
	WAVEFORMATEX *pWFEXIn;
	CVupp *vuppen;
	long MSCounter;

	CVuppSample *vuppSmp;
	bool vuppSmpInitialized; // TRUE when vuppSmp has been initialized
	long nbrOfvuppSmp;
	long MSampleDataLength;

    // Constructor - calls the base class constructor
    CTransformSample(TCHAR *pObjectName, LPUNKNOWN lpUnk, CLSID clsid);
	~CTransformSample(void);

    // Functions derived from CTransformFilter

    HRESULT CheckInputType(const CMediaType *pMediaTypeIn);

	HRESULT CheckTransform(const CMediaType *pMediaTypeIn, 
		const CMediaType *pMediaTypeOut);
		
	HRESULT DecideBufferSize(struct IMemAllocator *pMemAlloc, 
		struct _AllocatorProperties *pAllocProps);
	

};


