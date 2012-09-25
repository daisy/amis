

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Sep 25 15:28:16 2012
 */
/* Compiler settings for _PdtbIePlugin.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef ___PdtbIePlugin_h__
#define ___PdtbIePlugin_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPdtbBridge_FWD_DEFINED__
#define __IPdtbBridge_FWD_DEFINED__
typedef interface IPdtbBridge IPdtbBridge;
#endif 	/* __IPdtbBridge_FWD_DEFINED__ */


#ifndef __IPdtbPluggableProtocol_FWD_DEFINED__
#define __IPdtbPluggableProtocol_FWD_DEFINED__
typedef interface IPdtbPluggableProtocol IPdtbPluggableProtocol;
#endif 	/* __IPdtbPluggableProtocol_FWD_DEFINED__ */


#ifndef __CPdtbBridge_FWD_DEFINED__
#define __CPdtbBridge_FWD_DEFINED__

#ifdef __cplusplus
typedef class CPdtbBridge CPdtbBridge;
#else
typedef struct CPdtbBridge CPdtbBridge;
#endif /* __cplusplus */

#endif 	/* __CPdtbBridge_FWD_DEFINED__ */


#ifndef __CPdtbPluggableProtocol_FWD_DEFINED__
#define __CPdtbPluggableProtocol_FWD_DEFINED__

#ifdef __cplusplus
typedef class CPdtbPluggableProtocol CPdtbPluggableProtocol;
#else
typedef struct CPdtbPluggableProtocol CPdtbPluggableProtocol;
#endif /* __cplusplus */

#endif 	/* __CPdtbPluggableProtocol_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IPdtbBridge_INTERFACE_DEFINED__
#define __IPdtbBridge_INTERFACE_DEFINED__

/* interface IPdtbBridge */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPdtbBridge;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("659B0CA3-0746-4050-8505-9C132A3BBCC5")
    IPdtbBridge : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetDatasourceFactory( 
            /* [in] */ LPDWORD df) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDtbookProcessing( 
            /* [in] */ BOOL on) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPdtbBridgeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPdtbBridge * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPdtbBridge * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPdtbBridge * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPdtbBridge * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPdtbBridge * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPdtbBridge * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPdtbBridge * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *SetDatasourceFactory )( 
            IPdtbBridge * This,
            /* [in] */ LPDWORD df);
        
        HRESULT ( STDMETHODCALLTYPE *SetDtbookProcessing )( 
            IPdtbBridge * This,
            /* [in] */ BOOL on);
        
        END_INTERFACE
    } IPdtbBridgeVtbl;

    interface IPdtbBridge
    {
        CONST_VTBL struct IPdtbBridgeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPdtbBridge_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPdtbBridge_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPdtbBridge_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPdtbBridge_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPdtbBridge_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPdtbBridge_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPdtbBridge_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPdtbBridge_SetDatasourceFactory(This,df)	\
    (This)->lpVtbl -> SetDatasourceFactory(This,df)

#define IPdtbBridge_SetDtbookProcessing(This,on)	\
    (This)->lpVtbl -> SetDtbookProcessing(This,on)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPdtbBridge_SetDatasourceFactory_Proxy( 
    IPdtbBridge * This,
    /* [in] */ LPDWORD df);


void __RPC_STUB IPdtbBridge_SetDatasourceFactory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPdtbBridge_SetDtbookProcessing_Proxy( 
    IPdtbBridge * This,
    /* [in] */ BOOL on);


void __RPC_STUB IPdtbBridge_SetDtbookProcessing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPdtbBridge_INTERFACE_DEFINED__ */


#ifndef __IPdtbPluggableProtocol_INTERFACE_DEFINED__
#define __IPdtbPluggableProtocol_INTERFACE_DEFINED__

/* interface IPdtbPluggableProtocol */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPdtbPluggableProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB4C04E-70F7-4512-966F-A2B27524F87A")
    IPdtbPluggableProtocol : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IPdtbPluggableProtocolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPdtbPluggableProtocol * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPdtbPluggableProtocol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPdtbPluggableProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPdtbPluggableProtocol * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPdtbPluggableProtocol * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPdtbPluggableProtocol * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPdtbPluggableProtocol * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IPdtbPluggableProtocolVtbl;

    interface IPdtbPluggableProtocol
    {
        CONST_VTBL struct IPdtbPluggableProtocolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPdtbPluggableProtocol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPdtbPluggableProtocol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPdtbPluggableProtocol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPdtbPluggableProtocol_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPdtbPluggableProtocol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPdtbPluggableProtocol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPdtbPluggableProtocol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPdtbPluggableProtocol_INTERFACE_DEFINED__ */



#ifndef __PdtbIePlugin_LIBRARY_DEFINED__
#define __PdtbIePlugin_LIBRARY_DEFINED__

/* library PdtbIePlugin */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_PdtbIePlugin;

EXTERN_C const CLSID CLSID_CPdtbBridge;

#ifdef __cplusplus

class DECLSPEC_UUID("F326B6FD-40F5-44D7-A87A-E690500C27F1")
CPdtbBridge;
#endif

EXTERN_C const CLSID CLSID_CPdtbPluggableProtocol;

#ifdef __cplusplus

class DECLSPEC_UUID("183A003A-3D01-4E94-A2C5-AD0108C68370")
CPdtbPluggableProtocol;
#endif
#endif /* __PdtbIePlugin_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


