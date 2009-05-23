CFactoryTemplate g_Templates[]=
	{ { L"TPB Vupp 1.0", &CLSID_TPBVupp10, CTransformSample::CreateInstance, NULL, 
	&sudFilter } };

int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

// Provide the way for COM to create a TransformSample object
CUnknown * WINAPI CTransformSample::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {
	
	CTransformSample *pNewObject = new CTransformSample(NAME("TPB Vupp 1.0"), punk, F
		CLSID_TPBVupp10);

    
	if (pNewObject == NULL) *phr = E_OUTOFMEMORY;
    return pNewObject;
} 

STDAPI
DllRegisterServer()
{
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI
DllUnregisterServer()
{
  return AMovieDllRegisterServer2( FALSE );
}

// Microsoft C Compiler will give hundreds of warnings about
// unused inline functions in header files.  Try to disable them.
#pragma warning( disable:4514)
