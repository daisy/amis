/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Originally created by CWI (http://www.cwi.nl)

Copyright (c) 2009  DAISY Consortium

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// PdtbBridge.h : Declaration of the CPdtbBridge

#pragma once
#include "resource.h"       // main symbols

#define DATASOURCEFACTORYPTR LPDWORD

DEFINE_GUID(CLSID_CPdtbBridge,
			0xF326B6FD,0x40F5,0x44D7,0xA8,0x7A,0xE6,0x90,0x50,0x0C,0x27,0xF1);
DEFINE_GUID(IID_PdtbBridge,
			0x659B0CA3,0x0746,0x4050,0x85,0x05,0x9C,0x13,0x2A,0x3B,0xBC,0xC5);

#ifdef INTERFACES_AND_IMPLEMENTATIONS

// IPdtbBridge
[
	object,
	uuid("659B0CA3-0746-4050-8505-9C132A3BBCC5"),
	dual,	helpstring("IPdtbBridge Interface"),
	pointer_default(unique)
]
#endif
__interface IPdtbBridge : IDispatch
{
	STDMETHOD(SetDatasourceFactory)(DATASOURCEFACTORYPTR df) = 0;
	STDMETHOD(SetDtbookProcessing)(BOOL on) = 0;
};

// Stupid hack by Jack (who doesn't know what he's doing):
// define INTERFACES_AND_IMPLEMENTATIONS while building the
// COM object (through the project properties) but not while
// using it.
#ifdef INTERFACES_AND_IMPLEMENTATIONS
// CPdtbBridge

[
	coclass,
	threading("apartment"),
	vi_progid("PdtbIePlugin.PdtbBridge"),
	progid("PdtbIePlugin.PdtbBridge.1"),
	version(1.0),
	uuid("F326B6FD-40F5-44D7-A87A-E690500C27F1"),
	helpstring("PdtbBridge Class")
]
class ATL_NO_VTABLE CPdtbBridge : 
	public IPdtbBridge
{
public:
	CPdtbBridge()
	{
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		s_datasource_factory = 0;
		s_process_dtbook = 0;
		return S_OK;
	}
	
	void FinalRelease() 
	{
		s_datasource_factory = 0;
		s_process_dtbook = 0;
	}

public:
	STDMETHOD(SetDatasourceFactory)(DATASOURCEFACTORYPTR df);
	STDMETHOD(SetDtbookProcessing)(BOOL on);
public:  // Sigh: I don't know how to make PdtbPluggableProtocol a friend...
	static DATASOURCEFACTORYPTR s_datasource_factory;
	static BOOL s_process_dtbook;
};
#endif
