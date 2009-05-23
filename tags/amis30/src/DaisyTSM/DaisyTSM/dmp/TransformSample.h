
#ifndef TRANS_H
#define TRANS_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

class CTransformSample
    : public CTransformFilter,
	  public IVuppInterface
{

public:
	//Dll init functions
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    
	DECLARE_IUNKNOWN;
	STDMETHODIMP setCrossFadeSpeed(double speed);
	STDMETHODIMP setCycleSpeed(short speed);
	STDMETHODIMP setSilenceLoudnessThreshold(short threshold);
	STDMETHODIMP setSilenceRemainderLength(short length);
	STDMETHODIMP setSilenceSpeed(short speed);
	STDMETHODIMP setWindowLength(int length);
	STDMETHODIMP CTransformSample::getPosition(LONGLONG &position);

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);


	
    HRESULT CheckInputType(const CMediaType *pMediaTypeIn);
	HRESULT CheckTransform(const CMediaType *pMediaTypeIn, 
		const CMediaType *pMediaTypeOut);
	HRESULT DecideBufferSize(struct IMemAllocator *pMemAlloc, 
		struct _AllocatorProperties *pAllocProps);
	HRESULT GetMediaType(int position, CMediaType *pMediaTypeIn);
	
	HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);	
	HRESULT Receive(IMediaSample *pIn);
	HRESULT EndOfStream(void);
private:
	CTransformSample(TCHAR *pObjectName, LPUNKNOWN lpUnk, CLSID clsid);

	~CTransformSample(void);

};

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */
#endif
