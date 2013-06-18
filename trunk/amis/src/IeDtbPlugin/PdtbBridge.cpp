/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

Originally created by CWI (http://www.cwi.nl)

Copyright (c) 2004 DAISY Consortium

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
// PdtbBridge.cpp : Implementation of CPdtbBridge

#include "stdafx.h"
#include "PdtbBridge.h"

DATASOURCEFACTORYPTR CPdtbBridge::s_datasource_factory = 0;
BOOL CPdtbBridge::s_process_dtbook = 0;

// CPdtbBridge

STDMETHODIMP CPdtbBridge::SetDatasourceFactory(DATASOURCEFACTORYPTR df)
{
	CPdtbBridge::s_datasource_factory = df;
	return S_OK;
}

STDMETHODIMP CPdtbBridge::SetDtbookProcessing(BOOL on)
{
	CPdtbBridge::s_process_dtbook = on;
	return S_OK;
}