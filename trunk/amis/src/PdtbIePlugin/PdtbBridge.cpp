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