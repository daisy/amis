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
// PdtbPluggableProtocol.cpp : Implementation of CPdtbPluggableProtocol

#include "stdafx.h"
#include "PdtbPluggableProtocol.h"
#include "PdtbBridge.h"
#include "ambulant/net/datasource.h"
#include "ambulant/lib/textptr.h"
#include "dtb/TransformDTBook.h"
#include "util/FilePathTools.h"
#include "util/Log.h"

// CPdtbPluggableProtocol

STDMETHODIMP CPdtbPluggableProtocol::Start(
	LPCWSTR szUrl,
	IInternetProtocolSink *pIProtSink,
	IInternetBindInfo *pIBindInfo,
	DWORD grfSTI,
	DWORD dwReserved)
{
	HRESULT hr = S_OK;
	std::string mimetype("text/html");
	if (m_buffer) {
		free(m_buffer);
		m_bufptr = 0;
		m_bufsize = 0;
	}
	
	ambulant::net::datasource_factory *df = (ambulant::net::datasource_factory *)CPdtbBridge::s_datasource_factory;
	if (df) 
	{
		// Convert the URL to the form we need, and strip "amisie:" prefix.
		ambulant::lib::textptr urlconv(szUrl);
		std::string urlstr = urlconv;
		if (urlstr.substr(0, 7) == "amisie:")
			urlstr = urlstr.substr(7);
		ambulant::net::url url = ambulant::net::url::from_url(urlstr);
		
		
		if (CPdtbBridge::s_process_dtbook == TRUE && 
			amis::util::FilePathTools::getExtension(url.get_url()) == "xml")
		{
			amis::dtb::TransformDTBook dtbook_xform;
			m_whole_file = dtbook_xform.getResults();
				
			long sz = m_whole_file.size();
					
			if (!m_whole_file.empty())
			{
				m_buffer = (BYTE*)m_whole_file.c_str();
				m_bufsize = sz;
				mimetype = "text/html";
			}
			else
			{
				mimetype = url.guesstype();
			}
		}
		else //this is a pdtb
		{
			// Get the data and store it
			char *buffer;
			size_t size;
			bool ok = ambulant::net::read_data_from_url(url, df, &buffer, &size);
			if (ok) 
			{
				mimetype = url.guesstype();
				m_buffer = (BYTE *)buffer;
				m_bufsize = size;
			}
			else 
			{
				m_buffer = (BYTE *)strdup("<pre>PDTB-reader could not open document</pre>");
				m_bufsize = strlen((char *)m_buffer);
			}
		}
	} 
	else //called from outside AMIS
	{
		m_buffer = (BYTE *)strdup("<pre>\"amisie:\" URLs only work from within AMIS!</pre>");
		m_bufsize = strlen((char *)m_buffer);
	}
	
	// Do various reports that the sink appears to need.
	pIProtSink->ReportProgress(BINDSTATUS_FINDINGRESOURCE, szUrl);
	pIProtSink->ReportProgress(BINDSTATUS_CONNECTING, szUrl);
	pIProtSink->ReportProgress(BINDSTATUS_SENDINGREQUEST, szUrl);
	ambulant::lib::textptr mtconv(mimetype.c_str());
	LPCWSTR szMimeType = mtconv;
	pIProtSink->ReportProgress(BINDSTATUS_MIMETYPEAVAILABLE, szMimeType);
	pIProtSink->ReportData(BSCF_FIRSTDATANOTIFICATION, 0, m_bufsize);
	pIProtSink->ReportData(BSCF_LASTDATANOTIFICATION | BSCF_DATAFULLYAVAILABLE, m_bufsize, m_bufsize);
	// XXX mimetype
	return hr;
}

STDMETHODIMP CPdtbPluggableProtocol::Continue(PROTOCOLDATA *pStateInfo)
{
	return S_OK;
}

STDMETHODIMP CPdtbPluggableProtocol::Abort(HRESULT hrReason,DWORD dwOptions)
{
	return S_OK;
}

STDMETHODIMP CPdtbPluggableProtocol::Terminate(DWORD dwOptions)
{
	return S_OK;
}

STDMETHODIMP CPdtbPluggableProtocol::Suspend()
{
	return E_NOTIMPL;
}

STDMETHODIMP CPdtbPluggableProtocol::Resume()
{
	return E_NOTIMPL;
}

STDMETHODIMP CPdtbPluggableProtocol::LockRequest(DWORD dwOptions)
{
	return S_OK;
}

STDMETHODIMP CPdtbPluggableProtocol::UnlockRequest()
{
	return S_OK;
}

STDMETHODIMP CPdtbPluggableProtocol::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	ATLTRACE(_T("READ  - requested=%8d\n"), cb);
	
	HRESULT hr = S_OK;

	if (m_buffer == NULL || m_bufptr >= m_bufsize)
		return S_FALSE;

	BYTE* pbData = m_buffer + m_bufptr;
	size_t cbAvail = m_bufsize - m_bufptr;
	if (cb > cbAvail) cb = cbAvail;

	memcpy(pv, pbData, cb);
	
	m_bufptr += cb;
	*pcbRead = cb;
	
	if (m_bufptr >= m_bufsize) {
		free(m_buffer);
		m_buffer = NULL;
		m_bufptr = 0;
		m_bufsize = 0;
	}
	return hr;
}

STDMETHODIMP CPdtbPluggableProtocol::Seek(
	LARGE_INTEGER dlibMove,
	DWORD dwOrigin,
	ULARGE_INTEGER *plibNewPosition)
{
	return E_NOTIMPL;
}


STDMETHODIMP CPdtbPluggableProtocol::CombineUrl(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags,
												LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved)
{
	ambulant::lib::textptr base_tp(pwzBaseUrl);
	ambulant::lib::textptr rel_tp(pwzRelativeUrl);
	ambulant::net::url base_url = ambulant::net::url::from_url(std::string(base_tp));
	ambulant::net::url rel_url;
	if (StrCmp(pwzRelativeUrl, _T("")))
	{
		rel_url = ambulant::net::url::from_url(std::string(rel_tp));
		rel_url = rel_url.join_to_base(base_url);
	}
	else
	{
		rel_url = base_url;
	}
	std::string joined_str = rel_url.get_url();
	ambulant::lib::textptr joined_tp(joined_str.c_str());
	LPCWSTR pwzJoined = joined_tp.c_wstr();
	size_t sizeJoined = (joined_tp.length()+1)*2;
	// XXXJack: size in bytes or wchars???!?
	if (sizeJoined <= cchResult) {
		memcpy(pwzResult, pwzJoined, sizeJoined);
		*pcchResult = sizeJoined;
		return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CPdtbPluggableProtocol::CompareUrl(LPCWSTR pwszUrl1, LPCWSTR pwszUrl2, DWORD dwCompareFlags)
{
#if 0
	ATLTRACE(_T("CompareUrl\n"));
	
	if (pwszUrl1 == NULL || pwszUrl2 == NULL)
		return E_POINTER;

	HRESULT hr = S_FALSE;

	CDataURL url1, url2;

	if (url1.Parse(pwszUrl1) && url2.Parse(pwszUrl2) && url1 == url2)
	{
		hr = S_OK;
	}

	return hr;
#endif
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CPdtbPluggableProtocol::ParseUrl(LPCWSTR pwzUrl, PARSEACTION parseAction, DWORD dwParseFlags,
											  LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CPdtbPluggableProtocol::QueryInfo( LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags,
											   LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}
