// PdtbPluggableProtocol.h : Declaration of the CPdtbPluggableProtocol

#pragma once
#include "resource.h"       // main symbols
#include <string>

// IPdtbPluggableProtocol
[
	object,
	uuid("BFB4C04E-70F7-4512-966F-A2B27524F87A"),
	dual,	helpstring("IPdtbPluggableProtocol Interface"),
	pointer_default(unique)
]
__interface IPdtbPluggableProtocol : IDispatch
{
};



// CPdtbPluggableProtocol

[
	coclass,
	threading("apartment"),
	vi_progid("PdtbIePlugin.PdtbPluggableProtocol"),
	progid("PdtbIePlugin.PdtbPluggableProtocol.1"),
	version(1.0),
	uuid("183A003A-3D01-4E94-A2C5-AD0108C68370"),
	helpstring("PdtbPluggableProtocol Class"),
	registration_script("PdtbPluggableProtocol.rgs")
]
class ATL_NO_VTABLE CPdtbPluggableProtocol : 
	public IPdtbPluggableProtocol,
	public IInternetProtocol,
	public IInternetProtocolInfo

{
public:
	CPdtbPluggableProtocol()
	:	m_buffer(NULL),
		m_bufsize(0),
		m_bufptr(0)
	{
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_DATAPLUGGABLEPROTOCOL)


BEGIN_COM_MAP(CPdtbPluggableProtocol)
	COM_INTERFACE_ENTRY(IInternetProtocolInfo)
	COM_INTERFACE_ENTRY(IInternetProtocol)
	COM_INTERFACE_ENTRY(IInternetProtocolRoot)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
		if (m_buffer) {
			free(m_buffer);
			m_buffer = NULL;
		}
	}

public:
// IInternetProtocol interface
    STDMETHOD(Start)(
            LPCWSTR szUrl,
            IInternetProtocolSink *pIProtSink,
            IInternetBindInfo *pIBindInfo,
            DWORD grfSTI,
            DWORD dwReserved);
    STDMETHOD(Continue)(PROTOCOLDATA *pStateInfo);
    STDMETHOD(Abort)(HRESULT hrReason,DWORD dwOptions);
    STDMETHOD(Terminate)(DWORD dwOptions);
    STDMETHOD(Suspend)();
    STDMETHOD(Resume)();
    STDMETHOD(Read)(void *pv,ULONG cb,ULONG *pcbRead);
    STDMETHOD(Seek)(
            LARGE_INTEGER dlibMove,
            DWORD dwOrigin,
            ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(LockRequest)(DWORD dwOptions);
    STDMETHOD(UnlockRequest)();

// IInternetProtocolInfo interface
	STDMETHOD(CombineUrl)(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags,
		LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(CompareUrl)(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags);
	STDMETHOD(ParseUrl)(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags,
		LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(QueryInfo)( LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags,
		LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved);
private:
	BYTE *m_buffer;
	size_t m_bufsize;
	size_t m_bufptr;
	std::string m_whole_file;
};

//OBJECT_ENTRY_AUTO(__uuidof(PdtbPluggableProtocol), CPdtbPluggableProtocol)

