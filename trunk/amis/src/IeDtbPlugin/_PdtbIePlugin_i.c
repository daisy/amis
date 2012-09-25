

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IPdtbBridge,0x659B0CA3,0x0746,0x4050,0x85,0x05,0x9C,0x13,0x2A,0x3B,0xBC,0xC5);


MIDL_DEFINE_GUID(IID, IID_IPdtbPluggableProtocol,0xBFB4C04E,0x70F7,0x4512,0x96,0x6F,0xA2,0xB2,0x75,0x24,0xF8,0x7A);


MIDL_DEFINE_GUID(IID, LIBID_PdtbIePlugin,0x929C9C6A,0x5978,0x4CEB,0xB1,0x3F,0xA2,0x85,0x2D,0x54,0xDE,0xAC);


MIDL_DEFINE_GUID(CLSID, CLSID_CPdtbBridge,0xF326B6FD,0x40F5,0x44D7,0xA8,0x7A,0xE6,0x90,0x50,0x0C,0x27,0xF1);


MIDL_DEFINE_GUID(CLSID, CLSID_CPdtbPluggableProtocol,0x183A003A,0x3D01,0x4E94,0xA2,0xC5,0xAD,0x01,0x08,0xC6,0x83,0x70);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



