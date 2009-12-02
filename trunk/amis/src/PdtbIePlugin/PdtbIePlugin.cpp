// PdtbIePlugin.cpp : Implementation of DLL Exports.
//
// Note: COM+ 1.0 Information:
//      Please remember to run Microsoft Transaction Explorer to install the component(s).
//      Registration is not done by default. 

#include "stdafx.h"
#include "resource.h"

// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer to be automatically implemented for you
[ module(dll, uuid = "{929C9C6A-5978-4CEB-B13F-A2852D54DEAC}", 
		 name = "PdtbIePlugin", 
		 helpstring = "PdtbIePlugin 1.0 Type Library",
		 resource_name = "IDR_PDTBIEPLUGIN") ]
class CPdtbIePluginModule
{
public:
// Override CAtlDllModuleT members
};
		 
