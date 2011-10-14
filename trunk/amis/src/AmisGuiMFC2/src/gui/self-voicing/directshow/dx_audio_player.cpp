/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://daisy.org/amis

This is a modified version of an Ambulant component.
See below for license details.
*/

// This file is part of Ambulant Player, www.ambulantplayer.org.
//
// Copyright (C) 2003-2008 Stichting CWI, 
// Kruislaan 413, 1098 SJ Amsterdam, The Netherlands.
//
// Ambulant Player is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// Ambulant Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ambulant Player; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

/* 
* @$Id: dx_audio_player.cpp,v 1.22 2008/01/15 10:55:59 jackjansen Exp $ 
*/

#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif

#include "gui/self-voicing/directshow/dx_audio_player.h"

#include "gui\self-voicing\mffmTimeCode\SmilTimeCode.H"

#include "preferences.h"

//#include "ambulant/lib/logger.h"
//#include "ambulant/lib/textptr.h"
#include <math.h>
//#include <vfwmsgs.h>
// CLSID_FilterGraph
//#include <uuids.h>
#include <dshow.h>


#include "util/Log.h"

#ifndef AM_DBG
#define AM_DBG if(0)
#endif

//#include <process.h>

using namespace ambulantX;

//using ambulant::lib::win32::win_report_error;
//using ambulant::lib::win32::win_trace_error;
//using ambulant::lib::logger;

#ifdef WITH_TPB_AUDIO_SPEEDUP
bool speedup_filter_available;
bool speedup_filter_available_valid;
#endif 

#define win_report_error
#define win_trace_error
#define debugX
#define errorX
#define traceX


#ifdef SINGLE_THREAD_HACK
	static bool bDestroyBreak = false;
#endif

static int nTimes = 0;

//unsigned __stdcall eventHandler(void* lpParam) {
DWORD __stdcall eventHandler(LPVOID lpParam) 
{
	long lEventCode, lParam1, lParam2;
	HRESULT hResult = E_FAIL;
	bool bSelfBreak = false;

	gui::dx::audio_playerX *pPlayer = (gui::dx::audio_playerX*)lpParam;

	int nn = ++nTimes;
	char strOTHER[10];
	char strNN[5];
	sprintf(strNN, "%d", nn);
	TRACE(L"THREAD BEGIN %d\n", nn);

	amis::util::Log* p_log = amis::util::Log::Instance();
	string log_msg;
	log_msg = "Thread before event, # times=";
	log_msg.append(strNN);
	p_log->writeTrace(log_msg, "dx_audio_player eventHandler");

#ifdef SINGLE_THREAD_HACK
	while(!bSelfBreak && !bDestroyBreak)
#else
	while (!bSelfBreak)
#endif
	{
		hResult = E_FAIL;
#ifdef SINGLE_THREAD_HACK
		if (pPlayer->m_media_event == NULL) 
		{
			//Sleep(100);
			continue;
		}
#endif	
		TRACE(L"\n== THREAD BEFORE EVENT %d\n", nn);
		log_msg = "Thread before event, # times=";
		log_msg.append(strNN);
		p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
	
		hResult = pPlayer->m_media_event->GetEvent(&lEventCode, &lParam1, &lParam2, INFINITE);
		pPlayer->m_media_event->FreeEventParams(lEventCode, lParam1, lParam2);
		
		TRACE(L"\n== THREAD AFTER EVENT %d\n", nn);
		log_msg = "Thread after event, # times=";
		log_msg.append(strNN);
		p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
	
		if (hResult == S_OK) 
		{
			switch(lEventCode) 
			{
				case EC_COMPLETE:
				{
					TRACE(L"\n== THREAD EVENT COMPLETE %d\n", nn);		
					log_msg = "Thread event complete, # times=";
					log_msg.append(strNN);
					p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
					
					pPlayer->stop(false, true);
					TRACE(L"\n== THREAD BEFORE CALLBACK %d\n", nn);		
					log_msg = "Thread before callback, # times=";
					log_msg.append(strNN);
					p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
	
					pPlayer->sendMessageCallback();
#ifndef SINGLE_THREAD_HACK
					bSelfBreak = true;
#endif
					break;
				}
				case EC_USER + 4: 
				{
					TRACE(L"\n== THREAD EVENT USER+4 %d\n", nn);		
					log_msg = "Thread event user+4, # times=";
					log_msg.append(strNN);
					p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
					//pPlayer->stop(false, true);
					//pPlayer->sendMessageCallback();
					bSelfBreak = true;
					break;
				}
#ifdef SINGLE_THREAD_HACK
				case EC_USER + 5: 
				{
					TRACE(L"\n== THREAD BEFORE WAIT TOGGLE %d\n", nn);
					log_msg = "Thread before wait toggle, # times=";
					log_msg.append(strNN);
					p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
					
					DWORD hr = WaitForSingleObject(pPlayer->m_hEventWakeup, INFINITE);
					switch (hr) 
					{
						case WAIT_FAILED: 
						{
							int i = 0;
							break;
						}
						case WAIT_ABANDONED: 
						{
							int i = 0;
							break;
						}
						case WAIT_OBJECT_0: 
						{
							int i = 0;
							break;
						}
						case WAIT_TIMEOUT: 
						{
							int i = 0;
							break;
						}
					}
					TRACE(L"\n== THREAD AFTER WAIT TOGGLE %d\n", nn);
	
					//TODO: re-instates this logging message (causes app crash if called during exit)
					////p_log->writeMessage("== THREAD AFTER WAIT TOGGLE");
					////p_log->writeMessage(strNN);
					break;
				}
#endif
				default:
				{
					TRACE(L"\n== THREAD EVENT OTHER %d (%d)\n", nn, lEventCode);
					log_msg = "Thread event other, # times=";
					log_msg.append(strNN);
					log_msg.append(", event code = ");
					log_msg.append(strOTHER);
					p_log->writeTrace(log_msg, "dx_audio_player, eventHandler");
				}
			} //end switch
		} //end if (hResult == S_OK)
	} //end while

	TRACE(L"\n== THREAD END %d\n", nn);
	//todo: potentially makes app crash at exit time, because logger has been destroyed
	////p_log->writeMessage("== THREAD END");
	////p_log->writeMessage(strNN);

	//_endthreadex( 0 );
	return 0;
}


gui::dx::audio_playerX::audio_playerX()
:
//m_url(url),
m_graph_builder(0),
m_media_control(0),
m_media_position(0),
m_media_event(0),
#ifdef WITH_TPB_AUDIO_SPEEDUP
m_audio_speedup(0),
#endif
m_basic_audio(0),
hEventHandler(0)
{
#ifdef _DEBUG
	assert(!amis::Preferences::Instance()->getMustAvoidDirectX());
#endif

	set_rate(0);
	set_volume(100);

#ifdef SINGLE_THREAD_HACK
	bDestroyBreak = false;
	m_hEventWakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(m_hEventWakeup);
	ResetEvent(m_hEventWakeup);
#endif

	InitializeCriticalSection(&m_csSequence);
}

gui::dx::audio_playerX::~audio_playerX()
{
	stop(true);
	
#ifdef SINGLE_THREAD_HACK
	
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("SetEvent(m_hEventWakeup)", "audio_playerX::~audio_playerX");
	TRACE(L"%s", L"\n####### ~audio_playerX || SetEvent(m_hEventWakeup)\n");
	//end_thread();
	bDestroyBreak = true;
	SetEvent(m_hEventWakeup);
	CloseHandle(m_hEventWakeup);
#endif
	DeleteCriticalSection(&m_csSequence);
}

gui::dx::audio_playerX* gui::dx::audio_playerX::pinstance = 0;

gui::dx::audio_playerX* gui::dx::audio_playerX::Instance()
{
	if (pinstance == 0) pinstance = new gui::dx::audio_playerX();
	return pinstance;
}
void gui::dx::audio_playerX::DestroyInstance()
{
	if (pinstance) delete pinstance;
}


void gui::dx::audio_playerX::start(double t) 
{
	if(is_playing()) pause();
	seek(t);
	resume();
}

void gui::dx::audio_playerX::end_thread() 
{
	if (hEventHandler==NULL) return;

	amis::util::Log* p_log = amis::util::Log::Instance();
	//p_log->writeMessage("Wait for thread end.", "audio_playerX::end_thread");
	TRACE(L"%s", "\nWAIT for thread end.\n");

	IMediaEventSink *pIMES = NULL;
	m_graph_builder->QueryInterface(IID_IMediaEventSink, (void**) &pIMES);
	pIMES->Notify(EC_USER + 4, 0, 0);
	pIMES->Release();
	pIMES = NULL;

	//Sleep(200);

	DWORD hr = WaitForSingleObject(hEventHandler, 10000);
	switch (hr) 
	{
		case WAIT_FAILED: 
		{
			int i = 0;
			break;
		}
		case WAIT_ABANDONED: 
		{
			int i = 0;
			break;
		}
		case WAIT_OBJECT_0: 
		{
			int i = 0;
			break;
		}
		case WAIT_TIMEOUT: 
		{
			int i = 0;
			break;
		}
	}
	p_log->writeTrace("Wait for thread end: DONE.", "audio_playerX::end_thread");
	TRACE(L"%s", "\nWAIT for thread end DONE.\n");
	hEventHandler = NULL;
}

void gui::dx::audio_playerX::release_player() 
{
	if(m_graph_builder) 
	{
		if(m_media_event) 
		{
			m_media_event->Release();
			m_media_event = 0;
		}
		if(m_media_position) 
		{
			m_media_position->Release();
			m_media_position = 0;
		}
		if(m_media_control) 
		{ 
			m_media_control->Release();
			m_media_control = 0;
		}
		if(m_basic_audio) 
		{
			m_basic_audio->Release();
			m_basic_audio = 0;
		}
#ifdef WITH_TPB_AUDIO_SPEEDUP
		if(m_audio_speedup) 
		{
			m_audio_speedup->Release();
			m_audio_speedup = 0;
		}
#endif
		m_graph_builder->Release();
		m_graph_builder = 0;
	}
}

void gui::dx::audio_playerX::stop(bool fromPlay, bool fromThread) 
{

#ifdef SINGLE_THREAD_HACK
	ResetEvent(m_hEventWakeup);
#endif

		if (fromThread) 
		{

#ifndef SINGLE_THREAD_HACK
			hEventHandler = NULL;
#endif
			return;
		}

		amis::util::Log* p_log = amis::util::Log::Instance();
		p_log->writeTrace("Stop DX", "audio_playerX::stop");
		TRACE(L"%s", L"\n####### -- STOP DX\n");

		if(m_media_control == 0) 
		{			
			TRACE(L"%s", L"\n####### -- STOP DX || m_media_control == 0\n");
			p_log->writeTrace("Stop DX, m_media_control == 0", "audio_playerX::stop");
			return;
		}
		{ //TODO: what is this curly brace for??  i left it in ...
		if (!fromPlay) 
		{
			TRACE(L"%s", L"\n####### -- STOP DX || CS IN\n");
			p_log->writeTrace("Stop DX, CS IN", "audio_playerX::stop");
			EnterCriticalSection(&m_csSequence);
		}

#ifndef SINGLE_THREAD_HACK
		TRACE(L"%s", L"\n####### -- STOP DX || BEFORE END THREAD\n");
		end_thread();
#else
	IMediaEventSink *pIMES = NULL;
	m_graph_builder->QueryInterface(IID_IMediaEventSink, (void**) &pIMES);
	pIMES->Notify(EC_USER + 5, 0, 0);
	pIMES->Release();
	pIMES = NULL;
#endif

	//Sleep(100);

	TRACE(L"%s", L"\n####### -- STOP DX || AFTER END THREAD\n");
	//p_log->writeMessage("Stop DX, after thread end", "audio_playerX::stop");

	HRESULT hr = m_media_control->Stop();
	if(FAILED(hr)) 
	{
		win_report_error("IMediaControl::stop()", hr);	
	}
	TRACE(L"%s", L"\n####### -- STOP DX || AFTER STOP\n");
	p_log->writeTrace("Stop DX, After stop", "audio_playerX::stop");

	if (m_media_control->StopWhenReady() != S_OK) 
	{
		long state, i;
		for (i = 0; i < 100; i++) 
		{
			m_media_control->GetState(10, &state);
			if (state == State_Stopped) break;
		}
		if (i == 100) 
		{
			int debug = 0;
		}
	}
	TRACE(L"%s", L"\n####### -- STOP DX || AFTER STOP WHEN READY\n");
	//p_log->writeMessage("Stop DX, After stop when ready", "audio_playerX::stop");
	release_player();
	//p_log->writeMessage("Stop DX, After release", "audio_playerX::stop");
	TRACE(L"%s", L"\n####### -- STOP DX || AFTER RELEASE\n");

	if (!fromPlay) 
	{
		LeaveCriticalSection(&m_csSequence);	
		//p_log->writeMessage("Stop DX, CS out", "audio_playerX::stop");
		TRACE(L"%s", L"\n####### -- STOP DX ||  CS OUT\n");
	}
	} //counterpart to mysterious lone curly brace seen earlier
}

void gui::dx::audio_playerX::pause(void) {
	if(m_media_control == 0) return;
	HRESULT hr = m_media_control->Pause();
	if(FAILED(hr)) {
		win_report_error("IMediaControl::pause()", hr);	
	}
}

void gui::dx::audio_playerX::resume() {
	if(m_media_control == 0) {
		debugX("Invalid call to audio_playerX::run");
		return;
	}
	HRESULT hr = m_media_control->Run();
	if(FAILED(hr)) {
		win_report_error("IMediaControl::run()", hr);	
	}
}

void gui::dx::audio_playerX::seek(double t) {
	if(m_media_position == 0) return;
	HRESULT hr = m_media_position->put_CurrentPosition(REFTIME(t));
	if(FAILED(hr))
		win_report_error("IMediaPosition::put_CurrentPosition()", hr);	
}

void gui::dx::audio_playerX::endseek(double t) {
	if(m_media_position == 0) return;
	HRESULT hr = m_media_position->put_StopTime(REFTIME(t));
	if(FAILED(hr))
		win_report_error("IMediaPosition::put_StopTime()", hr);	
}

std::pair<bool, double> gui::dx::audio_playerX::get_dur() {
	if(m_media_position == 0) {
		debugX("Invalid call to audio_playerX::get_duration");
		return std::pair<bool, double>(false, 0);
	}
	REFTIME dur = 0.0;
	HRESULT hr = m_media_position->get_Duration(&dur);
	if(FAILED(hr)) {
		win_report_error("IMediaPosition::get_Duration()", hr);	
		return std::pair<bool, double>(false, 0);
	}
	return std::pair<bool, double>(dur>0, dur);
}

bool gui::dx::audio_playerX::can_play() {
	return m_graph_builder && 
		m_media_event &&
		m_media_position &&
		m_media_control &&
		m_media_event;
}

bool gui::dx::audio_playerX::is_playing() {

	//EnterCriticalSection(&m_csSequence);

	if(m_media_control == 0) {
		//LeaveCriticalSection(&m_csSequence);
		return false;}

	long msTimeout = 0;
	long state;
	m_media_control->GetState(msTimeout, &state);

	//LeaveCriticalSection(&m_csSequence);

	return (state == State_Running);

	/*

	if(m_media_event == 0) {
	LeaveCriticalSection(&m_csSequence);return false;}

	long evCode = 0;
	HRESULT hr = m_media_event->WaitForCompletion(msTimeout, &evCode);
	if(hr == E_ABORT) return true;
	else if(hr == S_OK) return false;
	else if(FAILED(hr)) {
	// XXXJack: this error occurs all the time...
	if (hr == 0x80040227) return false;
	win_trace_error("IMediaEvent::WaitForCompletion()", hr);	
	return false;
	}

	LeaveCriticalSection(&m_csSequence);
	return evCode == 0;
	*/
}

#if 0
double gui::dx::audio_playerX::get_position() {
	if(m_media_position == 0) {
		debugX("Invalid call to audio_playerX::get_current_position");
		return 0.0;
	}
	REFTIME pos = 0.0;
	HRESULT hr = m_media_position->get_CurrentPosition(&pos);
	if(FAILED(hr)) {
		win_report_error("IMediaPosition::get_CurrentPosition()", hr);	
		return 0.0;
	}
	return pos;
}	
#endif


#ifdef WITH_TPB_AUDIO_SPEEDUP
void gui::dx::audio_playerX::initialize_speedup_filter() {
	if (speedup_filter_available_valid && !speedup_filter_available) {
		// We don't seem to have the filter. Too bad.
		return;
	}
	// Either the filter exists or we haven't tried yet. Let's try to create
	// it and remember whether it worked.
	IBaseFilter *pNewFilter = NULL;

	HRESULT res;
	res = CoCreateInstance(CLSID_TPBVupp69, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&pNewFilter);

	if (res != S_OK) {
		traceX("dx_audio_player: Speedup filter not available, error 0x%x", res);
		speedup_filter_available = false;
		speedup_filter_available_valid = true;
		return;
	}
	res = m_graph_builder->AddFilter(pNewFilter, NULL);
	if (res != S_OK) {
		traceX("dx_audio_player: AddFilter(Speedup filter): error 0x%x", res);
		pNewFilter->Release();
		return;
	}
	speedup_filter_available = true;
	speedup_filter_available_valid = true;
	//	AM_DBG lib::debugX("dx_audio_player: added speedup filter to graph");

	// Next step: find out where we want to add the filter to the graph.
	// We iterate over the filter graph, then for each item in the graph
	// we iterate over the connected output pins util we find one we like.
	IPin *pOutputPin = NULL;
	IPin *pInputPin = NULL;
	IEnumFilters *pEnumFilters = NULL;
	res = m_graph_builder->EnumFilters(&pEnumFilters);
	if (res != S_OK) {
		traceX("dx_audio_filter: EnumFilters: error 0x%x", res);
		return;
	}

	IBaseFilter *pCurFilter;
	while (pOutputPin == NULL && (res=pEnumFilters->Next(1, &pCurFilter, NULL)) == S_OK) {
		AM_DBG {
			FILTER_INFO info;
			LPWSTR vendorInfo;
			res = pCurFilter->QueryFilterInfo(&info);
			if (res != S_OK) info.achName[0] = 0;
			res = pCurFilter->QueryVendorInfo(&vendorInfo);
			if (res != S_OK) vendorInfo = L"";
			//ambulant::lib::textptr tInfo(info.achName);
			//ambulant::lib::textptr tVendorInfo(vendorInfo);
			//lib::debugX("dx_audio_filter: filter found: '%s' vendor '%s'", tInfo.c_str(), tVendorInfo.c_str());
		}
		IEnumPins *pEnumPins;
		res = pCurFilter->EnumPins(&pEnumPins);
		IPin *pCurPin;
		while (pOutputPin == NULL && (res=pEnumPins->Next(1, &pCurPin, NULL)) == S_OK) {
			AM_MEDIA_TYPE mediaType;
			PIN_DIRECTION curPinDir;
			res = pCurPin->QueryDirection(&curPinDir);
			HRESULT res2 = pCurPin->ConnectionMediaType(&mediaType);
			if (res == S_OK && 
				res2 == S_OK && 
				curPinDir == PINDIR_OUTPUT &&
				mediaType.majortype == MEDIATYPE_Audio&& 
				mediaType.subtype == MEDIASUBTYPE_PCM){
					pOutputPin = pCurPin;
					res = pOutputPin->ConnectedTo(&pInputPin);
					if (res != S_OK) {
						// This output pin was the correct type, but not connected.
						// So it cannot be the one we're looking for.
						pOutputPin = pInputPin = NULL;
					} else {
						// Found it!
						pOutputPin->AddRef();
						pInputPin->AddRef();
					}
				}
				if (res2 == S_OK) {
					if (mediaType.cbFormat != 0) {
						CoTaskMemFree((PVOID)mediaType.pbFormat);
					}
				}
				pCurPin->Release();
		}
		if (res != S_FALSE && res != S_OK) 
			traceX("dx_audio_filter: enumerating pins: error 0x%x", res);
		pEnumPins->Release();
		pCurFilter->Release();
	}
	if (res != S_FALSE && res != S_OK)
		traceX("dx_audio_filter: enumerating filters: error 0x%x", res);

	pEnumFilters->Release();
	// We have the correct pins now.
	if (pOutputPin) {
		traceX("dx_audio_filter: found the right pins!");
	} else {
		traceX("dx_audio_filter: could not find a good pin");
		pOutputPin->Release();
		pInputPin->Release();
		return;
	}
	// Now we need to find the pins on our speedup filter.
	IPin *pFilterInputPin = NULL;
	IPin *pFilterOutputPin = NULL;
	IEnumPins *pEnumPins;
	res = pNewFilter->EnumPins(&pEnumPins);
	IPin *pCurPin;
	while (res=pEnumPins->Next(1, &pCurPin, NULL) == S_OK) {
		PIN_DIRECTION pinDir;
		res = pCurPin->QueryDirection(&pinDir);
		//assert(res == S_OK);
		if (pinDir == PINDIR_INPUT) {
			if (pFilterInputPin) {
				traceX("dx_audio_filter: multiple input pins on filter");
				goto bad;
			}
			pFilterInputPin = pCurPin;
			pFilterInputPin->AddRef();
		} else {
			if (pFilterOutputPin) {
				traceX("dx_audio_filter: multiple output pins on filter");
				goto bad;
			}
			pFilterOutputPin = pCurPin;
			pFilterOutputPin->AddRef();
		}
	}
	if (!pFilterInputPin) {
		traceX("dx_audio_filter: no input pin on filter");
		goto bad;
	}
	if (!pFilterOutputPin) {
		traceX("dx_audio_filter: no output pin on filter");
		goto bad;
	}
	// We have everything. Sever the old connection and insert the filter.
	res = m_graph_builder->Disconnect(pOutputPin);
	if (res) {
		traceX("dx_audio_filter: Severing old connection: error 0x%x", res);
		goto bad;
	}
	res = m_graph_builder->Disconnect(pInputPin);
	if (res) {
		traceX("dx_audio_filter: Severing old connection: error 0x%x", res);
		goto bad;
	}
	res = m_graph_builder->Connect(pOutputPin, pFilterInputPin);
	if (res) {
		traceX("dx_audio_filter: Creating filter input connection: error 0x%x", res);
		goto bad;
	}
	res = m_graph_builder->Connect(pFilterOutputPin, pInputPin);
	if (res) {
		traceX("dx_audio_filter: Creating filter output connection: error 0x%x", res);
		goto bad;
	}
	// Finally remember the interface to set speedup/slowdown, and register ourselves
	// in the global pool (so Amis can change our speed).
	res = pNewFilter->QueryInterface(IID_IVuppInterface, (void**) &m_audio_speedup);
	if (res != S_OK) {
		traceX("dx_audio_filter: filter does not provide IVuppInterface");
		goto bad;
	}
	set_rate(s_current_playback_rate);

bad:
	if (pOutputPin) pOutputPin->Release();
	if (pInputPin) pInputPin->Release();
	if (pFilterOutputPin) pFilterOutputPin->Release();
	if (pFilterInputPin) pFilterInputPin->Release();
	return;

}

int gui::dx::audio_playerX::get_rate() {
	return s_current_playback_rate;
}
void gui::dx::audio_playerX::set_rate(int rate) {

	 s_current_playback_rate = rate;
	 if (m_audio_speedup) {
	     char ch_rate[25];
	     sprintf(ch_rate, "AmbX Rate = %d", rate);
         amis::util::Log::Instance()->writeTrace(ch_rate);
		//The rate value sets come from TPB Reader settings
		//there are 15 distinct settings, from approximately 
		//60% speed to 2x speed
		//default rate = 0
		//single step increments = +/- 1

		if (rate <= -5)
		{
			//slowest speed
			set_rate_values(.60, 2500, 100, 20, 1000, 100);
		}
		else if (rate == -4)
		{
			set_rate_values(.75, 3000, 100, 20, 1000, 100);
		}
		else if (rate == -3)
		{
			set_rate_values(.80, 3000, 100, 20, 1000, 100);
		}
		else if (rate == -2)
		{
			set_rate_values(.85, 3000, 100, 20, 1000, 100);
		}
		else if (rate == -1)
		{
			set_rate_values(.90, 3000, 100, 20, 1000, 100);
		}
		else if (rate == 0)
		{
			//normal speed
			set_rate_values(1.00, 3000, 100, 20, 1000, 100);
		}
		else if (rate == 1)
		{
			set_rate_values(1.00, 3000, 120, 20, 1000, 110);
		}
		else if (rate == 2)
		{
			set_rate_values(1.00, 3000, 140, 25, 1000, 150);
		}
		else if (rate == 3)
		{
			set_rate_values(1.00, 3000, 170, 25, 1000, 150);			
		}
		else if (rate == 4)
		{
			set_rate_values(1.00, 3000, 200, 25, 1000, 150);
		}
		else if (rate == 5)
		{
			set_rate_values(1.05, 3000, 250, 30, 1000, 150);			
		}
		else if (rate == 6)
		{
			set_rate_values(1.10, 3000, 300, 30, 1000, 150);
		}
		else if (rate == 7)
		{
			set_rate_values(1.15, 3000, 200, 30, 1000, 175);
		}
   		else if (rate == 8)
		{
			set_rate_values(1.30, 3000, 200, 30, 1000, 175);
		}
		else if (rate == 9)
		{
			set_rate_values(1.50, 3000, 200, 30, 1000, 200);
		}
		else if (rate >= 10)
		{
			set_rate_values(2.00, 3000, 300, 30, 500, 500);
		}
	 }
}
void gui::dx::audio_playerX::set_rate_values(double crossFadeSpeed, int crossFadeWindowLength, 
											int cycleSpeed, int silenceLoudnessThreshold,
											int silenceRemainderLength, int silenceSpeed)
{	
		m_audio_speedup->setCrossFadeSpeed(crossFadeSpeed);
		m_audio_speedup->setWindowLength(crossFadeWindowLength);
		m_audio_speedup->setCycleSpeed(cycleSpeed);
		m_audio_speedup->setSilenceLoudnessThreshold(silenceLoudnessThreshold);
		m_audio_speedup->setSilenceRemainderLength(silenceRemainderLength);
		m_audio_speedup->setSilenceSpeed(silenceSpeed);
}

#endif

//void
//gui::dx::audio_playerX::set_global_level(double level)
//{
//	s_global_level = level;
//	ambulantX::gui::dx::audio_playerX::Instance()->set_volume((long)(s_global_level*100));
//}
//
//double
//gui::dx::audio_playerX::change_global_level(double factor)
//{
//	s_global_level *= factor;
//	set_global_level(s_global_level);
//	return s_global_level;
//}


// 0 - 100
long gui::dx::audio_playerX::get_volume() {
	return s_current_volume;
}

// 0 - 100
void gui::dx::audio_playerX::set_volume(long val) {
	if (val < 0) val = 0;
	if (val > 100) val = 100;
	s_current_volume = val;
	if(m_basic_audio == 0) return;
	long cdb = (long)(20.0*log10((double)val/100.0)*100);
	m_basic_audio->put_Volume(cdb);
}

// can be -100 to 100
// 0 sets a neutral balance
// and 10 sets -10 db to right and -90 db to left
void gui::dx::audio_playerX::set_balance(long val) {
	if(m_basic_audio == 0) return;
	val = (val>=-100)?val:-100;
	val = (val<=100)?val:100;
	long cdb = val*100;
	m_basic_audio->put_Balance(cdb);
}


void gui::dx::audio_playerX::setCallback(sendMessageCallbackFn pFunction)
{
	sendMessageCallback=pFunction;
}


//////////////////////////

bool gui::dx::audio_playerX::play(const char * url, char* clipBegin, char* clipEnd) {

	EnterCriticalSection(&m_csSequence);

	TRACE(L"%s", L"\n####### -- PLAY DX\n");
	amis::util::Log* p_log = amis::util::Log::Instance();
	p_log->writeTrace("Play DX", "audio_playerX::play");

	m_url.assign(url);

	if (m_graph_builder != NULL)
	{ 
		stop(true);
	}

	if (m_graph_builder == NULL)
	{
		HRESULT hr = CoCreateInstance(CLSID_FilterGraph,0,CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder,(void**)&m_graph_builder);
		if(FAILED(hr))
		{
			win_report_error("CoCreateInstance(CLSID_FilterGraph, ...)", hr);	

			LeaveCriticalSection(&m_csSequence);
			return false;
		}
	}

	//WCHAR wsz[MAX_PATH];
	//MultiByteToWideChar(CP_ACP,0, url.c_str(), -1, wsz, MAX_PATH);

	std::string strFileName;
	strFileName.assign(url);

	USES_CONVERSION;
	LPCWSTR str = A2CW(strFileName.c_str());

	HRESULT hr = m_graph_builder->RenderFile(str, 0);
	if(FAILED(hr)){
		
		amis::util::Log* p_log = amis::util::Log::Instance();
		string log_msg = "CANNOT PLAY: ";	
		log_msg.append(strFileName);
		p_log->writeError(log_msg, "audio_playerX::play");

		//m_graph_builder->Release();
		//m_graph_builder = 0;
		/* 
		if (hr == 0x800c000d)  // XXX This value experimentally determined:-)
		errorX("%s: Unsupported URL protocol", url.c_str());
		else if (hr == VFW_E_CANNOT_CONNECT)
		errorX("%s: Unsupported video format", url.c_str());
		else
		errorX("%s: DirectX error 0x%x", url.c_str(), hr);
		*/
		release_player();
		LeaveCriticalSection(&m_csSequence);
		return false;
	}
#ifdef WITH_TPB_AUDIO_SPEEDUP
	initialize_speedup_filter();
#endif

	if (m_media_control == NULL) { 
		HRESULT hr = m_graph_builder->QueryInterface(IID_IMediaControl, (void **) &m_media_control);
		if(FAILED(hr)) {
			win_report_error("QueryInterface(IID_IMediaControl, ...)", hr);	

			LeaveCriticalSection(&m_csSequence);
			return false;
		}
	}

	if (m_media_position == NULL) { 
		m_graph_builder->QueryInterface(IID_IMediaPosition, (void **) &m_media_position);
		if(FAILED(hr)) {
			win_report_error("QueryInterface(IID_IMediaPosition, ...)", hr);	

			LeaveCriticalSection(&m_csSequence);
			return false;
		}
	}

	if (m_media_event == NULL) { 
		m_graph_builder->QueryInterface(IID_IMediaEvent, (void **) &m_media_event);
		if(FAILED(hr)) {
			win_report_error("QueryInterface(IID_IMediaEvent, ...)", hr);	

			LeaveCriticalSection(&m_csSequence);
			return false;
		}
	}

	if (m_basic_audio == NULL) { 
		m_graph_builder->QueryInterface(IID_IBasicAudio, (void **) &m_basic_audio);
		if(FAILED(hr)) {
			win_report_error("QueryInterface(IID_IBasicAudio, ...)", hr);	
		}
	}

	SmilTimeCode startStop(clipBegin, clipBegin, clipEnd);
	unsigned long begin = startStop.getStart();
	unsigned long end = startStop.getEnd();

	LONGLONG llDuration = 0;

	IMediaSeeking *pIMS;
	if (m_graph_builder->QueryInterface(IID_IMediaSeeking, (void**) &pIMS) == S_OK)
	{

		if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
			if (pIMS->GetPositions(NULL, &llDuration) != S_OK) llDuration = -10000;
		}
		else
		{
			llDuration = - 10000;
		}

		long duration = (long) ((llDuration / 10000) & 0xFFFFFFFF);	  

		if (! (begin == 0 && end == 0)) {

			if (begin >= 0 && begin < duration) {

				LONGLONG Value = (LONGLONG) (begin);
				Value *= 10000;
				if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
					if (pIMS->SetPositions(&Value, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning) != S_OK){}
				}
			}
			
			if (end >= 0 && end > begin && end < duration) {
				
				LONGLONG Value = (LONGLONG) (end);
				Value *= 10000;
				if (pIMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) == S_OK) {
					if (pIMS->SetPositions(NULL, AM_SEEKING_NoPositioning, &Value, AM_SEEKING_AbsolutePositioning) != S_OK) {}
				}
			}
		}
		pIMS->Release();
	}

	if (hEventHandler == NULL) {
		unsigned long lpdwThreadID;
		//hEventHandler = (HANDLE) _beginthreadex(NULL, 0, eventHandler, (void*) this, 0, &lpdwThreadID);
		hEventHandler = CreateThread(NULL, 0, &eventHandler, this, 0, &lpdwThreadID);
		//GetCurrentThreadId
		TRACE("\nTHREAD ID (DX_AUDIO_PLAYER): %x\n", lpdwThreadID);
		
		amis::util::Log* p_log = amis::util::Log::Instance();
		string log_msg = "Thread ID: ";
		char strID[10];
		sprintf(strID, "%x", lpdwThreadID);			
		log_msg.append(strID);
		p_log->writeTrace(log_msg, "audio_playerX::play");
	}
#ifdef SINGLE_THREAD_HACK
	SetEvent(m_hEventWakeup);
#endif

	set_volume(s_current_volume);

	hr = m_media_control->Run();
	if(FAILED(hr)) {
		win_report_error("IMediaControl::run()", hr);	
	}

	LeaveCriticalSection(&m_csSequence);
	return true;
}
