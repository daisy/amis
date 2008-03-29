
#include <string>
#include "stdafx.h"

#include "gui/MainFrm.h"

namespace amis
{
	namespace gui
	{
			class MenuVoicing
			{
			private:
				CMainFrame * mFrm;
			public:
				MenuVoicing(CMainFrame * frm);
				~MenuVoicing(void);
				
		AudioSequence* createSeqPrepend(UINT nFlags);
				
				std::string computeRootMenuFromFirstChildID(unsigned int firstItemId, bool playPrompt);
				void OnMenuSelect(UINT	nItemID, UINT nFlags, HMENU	hSysMenu);
			};
		}
	
}