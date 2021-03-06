/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amisproject.org

Copyright (c) 2004-2009  DAISY Consortium

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

#ifndef AudioSequence_H
#define AudioSequence_H

#pragma once

#include "AudioSequenceComponent.h"
#include <atlcoll.h>
#include "Media.h"
//#include <atlstr.h>

namespace amis
{
	namespace gui
	{
		namespace spoken
		{
			class AudioSequence
			{
			public:
				AudioSequence();
				~AudioSequence();

				UINT GetCount();
				bool IsEmpty();
				void appendAll(AudioSequence * seq);
				AudioSequenceComponent* RemoveTail();
				void AddTail(AudioSequenceComponent* comp);
				void append(CString strTTS);
				void prepend(CString strTTS);
				void append(amis::AudioNode* audioClip, CString strTTS);
				AudioSequence* clone();
			private:
				CAtlList<AudioSequenceComponent*> m_AudioSequence;
				void append_prepend(CString strTTS, bool prepend) ;
			};
		}
	}
}
#endif //AudioSequence_H
