/*
 * This file is part of Ambulant Player, www.ambulantplayer.org.
 *
 * Copyright (C) 2003-2008 Stichting CWI, 
 * Kruislaan 413, 1098 SJ Amsterdam, The Netherlands.
 *
 * Ambulant Player is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Ambulant Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Ambulant Player; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* 
 * @$Id: dx_audio_player.h,v 1.21 2008/01/15 10:54:48 jackjansen Exp $ 
 */

#ifndef AMBULANT_GUI_DX_AUDIO_PLAYERX_H
#define AMBULANT_GUI_DX_AUDIO_PLAYERX_H

//#include "ambulant/config/config.h"

#include <string>
#include <cmath>

#include <objbase.h>
#include <strmif.h>
#include <control.h>

#include <initguid.h>


//#include "ambulant/common/playable.h"
//#include "ambulant/lib/win32/win32_error.h"
//#include "ambulant/lib/logger.h"

/*
#ifdef _DEBUG
#pragma comment(lib, "msvcrtd.lib") 
#else
#pragma comment(lib, "msvcrt.lib") 
#endif
*/

/*
#ifndef AMBULANT_PLATFORM_WIN32_WCE
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"amstrmid.lib")
#endif
#pragma comment (lib,"strmiids.lib")
#pragma comment (lib,"uuid.lib")
*/

#ifndef AMBULANT_PLATFORM_WIN32_WCE
#define WITH_TPB_AUDIO_SPEEDUP
#endif

#ifdef WITH_TPB_AUDIO_SPEEDUP
// If this option is active during build (which is true when building
// for AmisAmbulant) we try to insert the TPB Audio Speedup/Slowdown
// filter into our filtergraph. This allows for changing audio playback
// speed without altering pitch.
// If the TPB DirectShow filter is not available we do nothing.
#include <set>

DEFINE_GUID(CLSID_TPBVupp69, 
	0x66172967, 0x56c5, 0x4b89, 0xaa, 0x92, 0xc9, 0xef, 0xec, 0x56, 0x46, 0x7b);


#ifdef __cplusplus
extern "C" {
#endif

// {A33E626E-D6C4-4559-A1D6-9F1D95F0D8E2}
DEFINE_GUID(IID_IVuppInterface, 
0xa33e626e, 0xd6c4, 0x4559, 0xa1, 0xd6, 0x9f, 0x1d, 0x95, 0xf0, 0xd8, 0xe2);

DECLARE_INTERFACE_(IVuppInterface, IUnknown) { 

	//Deklarera metoder:
	STDMETHOD(setCrossFadeSpeed)
		( THIS_
			double speed 
		) PURE;

	STDMETHOD(setWindowLength)
		( THIS_
			int length
		) PURE;

	STDMETHOD(setCycleSpeed)
		( THIS_
			short speed
		) PURE;

	STDMETHOD(setSilenceLoudnessThreshold)
		( THIS_
			short threshold
		) PURE;

	STDMETHOD(setSilenceRemainderLength)
		( THIS_
			short length
		) PURE;

	STDMETHOD(setSilenceSpeed)
		( THIS_
			short speed
		) PURE;

	STDMETHOD(getPosition)
		( THIS_
			LONGLONG &position
		) PURE;
};

#ifdef __cplusplus
}
#endif



#endif // WITH_TPB_AUDIO_SPEEDUP

typedef void (*sendMessageCallbackFn)(void);

static double s_current_playback_rate = 1.0;

#define SINGLE_THREAD_HACK

namespace ambulantX {

namespace gui {

namespace dx {

class audio_playerX {
	
	typedef std::pair<bool, double> duration;
	
  public:
	  
	audio_playerX();
	  void setCallback(sendMessageCallbackFn pFunction);

	  static audio_playerX* Instance();
	  static void DestroyInstance();

#ifdef SINGLE_THREAD_HACK
	HANDLE m_hEventWakeup;
#endif
	~audio_playerX();
		
	void start(double t);
	void stop(bool fromPlay, bool fromThread=false);
	//void pause(common::pause_display d=common::display_show);
	void pause(void);
	void resume();
	void seek(double t);
	void endseek(double t);
	duration get_dur();
	void wantclicks(bool want) {}
	void preroll(double when, double where, double how_much) {}

	bool can_play();
	bool is_playing();
	double get_position();
		
	// -val is the attenuation in decibels 
	// can be 0 to 100
	void set_volume(long val);
		
	// can be -100 to 100
	// 0 sets a neutral balance
	// and 10 sets -10 db to right and -90 db to left
	void set_balance(long val);
	bool play(const char * url);

  private:

	void end_thread();

	void release_player();
	
	std::string m_url;

	IGraphBuilder *m_graph_builder;
	IMediaControl *m_media_control;
	IMediaPosition *m_media_position;
	IBasicAudio *m_basic_audio;
	
	HANDLE hEventHandler;

public:
	IMediaEvent *m_media_event;
	sendMessageCallbackFn sendMessageCallback;

private:
	
	CRITICAL_SECTION m_csSequence;
	static audio_playerX* pinstance;

#ifdef WITH_TPB_AUDIO_SPEEDUP
  public:
	void set_rate(double rate);
	double get_rate();

  private:
	IVuppInterface *m_audio_speedup;
	void initialize_speedup_filter();
#endif
};
	
} // namespace dx

} // namespace gui
 
} // namespace ambulantX

#endif // AMBULANT_GUI_DX_AUDIO_PLAYERX_H

