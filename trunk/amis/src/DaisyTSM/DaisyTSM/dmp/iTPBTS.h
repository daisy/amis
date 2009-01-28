//------------------------------------------------------------------------------
// File: ITPBTS.h
//
// Desc: ITPBTS interface which is implemented by TPBTSFilter class.
//       which provides access to the additional methods to control the filter.
//
// 
//------------------------------------------------------------------------------


#ifndef __ITPBTS__
#define __ITPBTS__

#ifdef __cplusplus
extern "C" {
#endif
// constant for the event.

#define EC_USER_TIME_ELAPSED EC_USER+1


//----------------------------------------------------------------------------
// ITPBTS's GUID
//
// {5DD2D89B-6925-4ceb-9C20-AD790F76E638}
DEFINE_GUID(IID_ITPBTS, 
0x5dd2d89b, 0x6925, 0x4ceb, 0x9c, 0x20, 0xad, 0x79, 0xf, 0x76, 0xe6, 0x38);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ITPBTS
//----------------------------------------------------------------------------
DECLARE_INTERFACE_(ITPBTS, IUnknown)
{

    STDMETHOD(put_MediaType) (THIS_
    				  CMediaType *pmt       /* [in] */	// the media type selected
				 ) PURE;

    STDMETHOD(get_MediaType) (THIS_
    				  CMediaType **pmt      /* [out] */	// the media type selected
				 ) PURE;

    STDMETHOD(get_IPin) (THIS_
    				  IPin **pPin          /* [out] */	// the source pin
				 ) PURE;


    STDMETHOD(get_State) (THIS_
    				  FILTER_STATE *state  /* [out] */	// the filter state
				 ) PURE;
		STDMETHOD (GetTime) (THIS_ REFERENCE_TIME * time) PURE;
STDMETHOD (GetTimeWithRate) (THIS_ DOUBLE * timeWithRate) PURE;
STDMETHOD(SetTimeWithRate) (THIS_ BOOL b) PURE;
STDMETHOD(SetTimeInterval) (THIS_ DWORD interval) PURE;
STDMETHOD(StartEventEngine) (THIS_) PURE;
STDMETHOD(StopEventEngine) (THIS_) PURE;


};
//----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // __ITPBTS__
