#ifdef __cplusplus
extern "C" {
#endif


// {A33E626E-D6C4-4559-A1D6-9F1D95F0D8E2}
DEFINE_GUID(IID_IVuppInterface, 
0xa33e626e, 0xd6c4, 0x4559, 0xa1, 0xd6, 0x9f, 0x1d, 0x95, 0xf0, 0xd8, 0xe2);



	DECLARE_INTERFACE_(IVuppInterface, IUnknown) { 

		//Deklarera metoder:
		STDMETHOD(setCrossFadeSpeed)
			( THIS_
			  double speed 
			) PURE;

		STDMETHOD(setWindowLength)
			( THIS_
			  int length
			) PURE;

		STDMETHOD(setCycleSpeed)
			( THIS_
			  short speed
			) PURE;

		STDMETHOD(setSilenceLoudnessThreshold)
			( THIS_
			  short threshold
			) PURE;

		STDMETHOD(setSilenceRemainderLength)
			( THIS_
			  short length
		    ) PURE;

		STDMETHOD(setSilenceSpeed)
			( THIS_
			  short speed
			) PURE;

		STDMETHOD(getPosition)
			( THIS_
			  LONGLONG &position
			) PURE;
	};

#ifdef __cplusplus
}
#endif

