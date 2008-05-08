/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

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

#include "gui/self-voicing/dialogs/skipdialogvoicing.h"

namespace amis
{
namespace gui
{
namespace dialogs
{
SkipDialogVoicing::SkipDialogVoicing(amis::gui::dialogs::SkipDialog * dlg)
{
	mpDialog = dlg;
}

SkipDialogVoicing::~SkipDialogVoicing(void)
{
	mpDialog = NULL;
}

amis::gui::spoken::PromptItem* SkipDialogVoicing::getPromptItemForReadingOptionName(string item_id)
{
	amis::gui::spoken::PromptItem* pi = NULL;
	
	if (item_id.compare("prodnote") == 0 ||
		item_id.compare("optional-prodnote") == 0)
	{
		pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("prodnotes");
		
	}
	else if (item_id.compare("pages") == 0 || item_id.compare("pagenumber") == 0)
	{
		pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("pages");
		
	}
	else if (item_id.compare("sections") == 0)
	{
		pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("sections");
		
	}
	else if (item_id.compare("sidebar") == 0)
	{
		
		pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("sidebars");
		
	}
	else if (item_id.compare("footnote") == 0 ||
		item_id.compare("noteref") == 0)
	{
		
		pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("noterefs");
		
	}
	return pi;
}
void SkipDialogVoicing::appendSeqListContent(AudioSequence* seq, amis::dtb::CustomTest* p_custom_test, int i)
{
	string item_id = p_custom_test->getId();
	amis::gui::spoken::PromptItem* pi = getPromptItemForReadingOptionName(item_id);
	if (pi != NULL)
		{
			AudioSequencePlayer::fillSequenceContents(seq, pi);
		}

	string strSelect;

		if (p_custom_test != NULL)
		{
			//false = skip this item
			if (mpDialog->m_ListSkips.GetCheck(i) == 1) //p_custom_test->getCurrentState() == false)
			{
				strSelect = "item_selected";
			}
			else
			{
				strSelect = "item_deselected";
			}
		}

	pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("thisItemIsCurrently");
	if (pi != NULL)
	{
		AudioSequencePlayer::fillSequenceContents(seq, pi);
	}
	pi = amis::gui::spoken::DataTree::Instance()->findPromptItem(strSelect);
	if (pi != NULL)
	{
		AudioSequencePlayer::fillSequenceContents(seq, pi);
	}
}

void SkipDialogVoicing::OnSelchangeSkips() 
{
	int i = mpDialog->m_ListSkips.GetCurSel();

	amis::dtb::CustomTest* p_test = (amis::dtb::CustomTest*)mpDialog->m_ListSkips.GetItemData(i);

	AudioSequence* seq = new AudioSequence;
	appendSeqListContent(seq, p_test, i);
	amis::gui::spoken::PromptItem * pi = amis::gui::spoken::DataTree::Instance()->findPromptItem("spaceToToggle"); 
	if (pi != NULL)
	{
		AudioSequencePlayer::fillSequenceContents(seq, pi);
	}
	
	AudioSequencePlayer::Instance()->Play(seq);
}
}}}