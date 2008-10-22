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

#ifndef PRETRANSMSGHANDLER_H
#define PRETRANSMSGHANDLER_H

#pragma once

#include "AudioSequencePlayer.h"
#include "promptresolver.h"

namespace amis
{
	namespace gui
	{
		namespace spoken
		{
			class PreTranslateMessageHandler
			{
			public:
				bool PreTranslateMessageHandler::isControlPressed();

				PreTranslateMessageHandler(int idUiDialog);
				~PreTranslateMessageHandler(void);

				CString normalizeTextEntry(CString str, int nStartChar, int nEndChar);
				void handle(PromptResolver * pResolver, 
					MSG* pMsg,
					int idUiFocus,
					bool ignoreSpaceKey = false,
					bool ignoreArrowKeys = false,
					CString strTextField = CString(L""),
					CString strTextFieldFULL = CString(L""),
					bool ignoreCharKeys = false);

				void handleCommand(WPARAM cmdid);

			private:

				CString convertKeystrokeToSpeakableString(MSG* pMsg, bool ignoreSpaceKey = false, bool ignoreArrowKeys = false, bool ignoreCharKeys = false);
				CString convertKeystrokeToSpeakableString(WPARAM ch, bool ignoreSpaceKey = false, bool ignoreArrowKeys = false, bool ignoreCharKeys = false);

				int m_instructionsDialogID;

				bool mbKeyControl;
				WPARAM mLastKeyDown;
			};
		}
	}
}
#endif //PRETRANSMSGHANDLER_H