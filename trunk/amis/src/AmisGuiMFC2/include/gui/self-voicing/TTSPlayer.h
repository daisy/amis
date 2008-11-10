/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2008  DAISY Consortium

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

#ifndef TTSPLAYER_H
#define TTSPLAYER_H

#pragma once

#include <string>

#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif


#include <sapi.h>

namespace amis
{
	namespace tts
	{

		class TTSPlayer
		{
		protected:
			TTSPlayer(void);
		public:
			~TTSPlayer(void);

			typedef void (*sendMessageCallbackFn)(void);

			sendMessageCallbackFn sendMessageCallback;
			void setCallback(sendMessageCallbackFn pFunction);

			static void DestroyInstanceOne();
			static void DestroyInstanceTwo();
			static TTSPlayer* InstanceOne();
			static TTSPlayer* InstanceTwo();

			void Play(CString str);
			void Stop();
			void Pause();
			void Resume();

			void callback();
			void WaitUntilDone();

			long GetSpeechRate();
			bool SetSpeechRate(long newRate);

			std::string ChangeVoice(bool speakNotify);
			void ChangeVoice(int index);

			int initVoiceList(HWND);

			bool IsSpeaking(void);

		private:

			bool m_isSpeaking;
			static TTSPlayer* pinstance_one;
			static TTSPlayer* pinstance_two;
			CRITICAL_SECTION m_csSequence;
			ULONG m_currentVoiceNumber;
			bool mbDoNotProcessEndEvent;
			
			ISpVoice* m_iV; // Voice Interface
		};
	}
}
#endif //TTSPLAYER_H