/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2008  DAISY for All Project

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

#include "stdafx.h"
#include "gui/toolbar/Toolbar.h"
#include "gui/AmisApp.h"
#include "util/Log.h"

using namespace amis::gui::toolbar;

/**
* abstract base class ToolbarItem
**/
ToolbarItem::ToolbarItem(ToolbarItemType type)
{
	mType = type;
}
ToolbarItem::~ToolbarItem() 
{
}
ToolbarItemType ToolbarItem::getType()
{
	return mType;
}

/**
* A separator that usually goes between groups of buttons
**/
ToolbarSeparator::ToolbarSeparator() : ToolbarItem(amis::gui::toolbar::SEPARATOR)
{
}
ToolbarSeparator::~ToolbarSeparator()
{
}

/**
* A toolbar button
**/
ToolbarButton::ToolbarButton() : ToolbarItem(amis::gui::toolbar::BUTTON)
{
}
ToolbarButton::~ToolbarButton()
{
}

void ToolbarButton::setImageSrc(ambulant::net::url src)
{
	mSrc = src;
}
void ToolbarButton::setCommandId(UINT id)
{
	mMfcId = id;
}

const ambulant::net::url* ToolbarButton::getImageSrc()
{
	return &mSrc;
}

UINT ToolbarButton::getCommandId()
{
	return mMfcId;
}
void ToolbarButton::setId(string id)
{
	mId = id;
}
string ToolbarButton::getId()
{
	return mId;
}
void ToolbarButton::setImageIndex(int idx)
{
	mImageIndex = idx;
}
int ToolbarButton::getImageIndex()
{
	return mImageIndex;
}
/**
* a toggle button
**/
ToolbarToggleButton::ToolbarToggleButton() : ToolbarItem(amis::gui::toolbar::TOGGLE)
{
	mpButtonOne = NULL;
	mpButtonTwo = NULL;
}
ToolbarToggleButton::~ToolbarToggleButton()
{
	if (mpButtonOne != NULL) delete mpButtonOne;
	if (mpButtonTwo != NULL) delete mpButtonTwo;
}
//add a button as the first or second (or don't add it if there are no empty slots)
void ToolbarToggleButton::addButton(ToolbarButton* pButton)
{
	if (mpButtonOne == NULL) setButtonOne(pButton);
	else if (mpButtonTwo == NULL) setButtonTwo(pButton);
}
void ToolbarToggleButton::setButtonOne(ToolbarButton* pButton)
{
	mpButtonOne = pButton;
	//default is button one
	mpCurrent = mpButtonOne;
}
void ToolbarToggleButton::setButtonTwo(ToolbarButton* pButton)
{
	mpButtonTwo = pButton;
}
ToolbarButton* ToolbarToggleButton::getButtonOne()
{
	return mpButtonOne;
}
ToolbarButton* ToolbarToggleButton::getButtonTwo()
{
	return mpButtonTwo;
}
void ToolbarToggleButton::setCurrent(ToolbarButton* pButton)
{
	mpCurrent = pButton;
}
ToolbarButton* ToolbarToggleButton::getCurrent()
{
	return mpCurrent;
}
/**
* settings for the toolbar
**/
ToolbarSettings::ToolbarSettings()
{
	//defaults
	mButtonWidth = 32;
	mButtonHeight = 32;
	mPlacement = TOP;
	mView = DEFAULT;
}
ToolbarSettings::~ToolbarSettings()
{
	cleanUpVector();
}
void ToolbarSettings::cleanUpVector()
{
	int sz = mItems.size();
	ToolbarItem* p_item = NULL;
	for (int i = sz-1; i>=0; i--)
	{
		p_item = mItems[i];
		if (p_item != NULL) delete p_item;
		mItems.pop_back();
	}
}
void ToolbarSettings::setView(ToolbarView view)
{
	mView = view;
}
void ToolbarSettings::setPlacement(ToolbarPlacement placement)
{
	mPlacement = placement;
}
void ToolbarSettings::setButtonResolution(int width, int height)
{
	mButtonWidth = width;
	mButtonHeight = height;
}
ToolbarView ToolbarSettings::getView()
{
	return mView;
}
ToolbarPlacement ToolbarSettings::getPlacement()
{
	return mPlacement;
}
int ToolbarSettings::getButtonWidth()
{
	return mButtonWidth;
}
int ToolbarSettings::getButtonHeight()
{
	return mButtonHeight;
}
void ToolbarSettings::addItem(ToolbarItem* pItem)
{
	mItems.push_back(pItem);
}
ToolbarItems ToolbarSettings::getItems()
{
	return mItems;
}
//get the number of items that are buttons (regular or toggle); don't count separators.
int ToolbarSettings::getNumberOfButtons()
{
	int count = 0;
	for (int i = 0; i<mItems.size(); i++)
	{
		if (mItems[i]->getType() == BUTTON || mItems[i]->getType() == TOGGLE) count++;
	}
	return count;
}

/**
* The toolbar itself
**/
BEGIN_MESSAGE_MAP(Toolbar, CToolBarCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
END_MESSAGE_MAP()

Toolbar::Toolbar()
{
	mpSettings = NULL;
}
Toolbar::~Toolbar()
{
	deleteAll();
}
void Toolbar::deleteAll()
{
	m_images.DeleteImageList();
	if (mpSettings != NULL) delete mpSettings;
}
void Toolbar::OnDestroy()
{
	deleteAll();
}
void Toolbar::setSettings(ToolbarSettings* pSettings)
{
	mpSettings = pSettings;
}
void Toolbar::togglePlayPause(bool bShowPlay)
{
	//find the toggle button that has the ID_AMIS_PLAYPAUSE command
	ToolbarToggleButton* play_pause_button = NULL;
	ToolbarItems items = mpSettings->getItems();
	int index_of_toggle = 0;
	for (int i = 0; i<items.size(); i++)
	{
		if (items[i]->getType() == TOGGLE)
		{
			ToolbarToggleButton* p_tmp = (ToolbarToggleButton*)items[i];
			if (p_tmp->getButtonOne()->getCommandId() == ID_AMIS_PLAYPAUSE)
			{
				play_pause_button = p_tmp;
				index_of_toggle = i;
				break;
			}
		}
	}
	if (play_pause_button == NULL)
	{
		amis::util::Log::Instance()->writeError("Play/pause button not found on toolbar", "Toolbar::togglePlayPause");
		return;
	}
	
	ToolbarButton* play_button = NULL;
	ToolbarButton* pause_button = NULL;
	if (play_pause_button->getButtonOne()->getId() == "play")
	{
		play_button = play_pause_button->getButtonOne();
		pause_button = play_pause_button->getButtonTwo();
	}
	else
	{
		play_button = play_pause_button->getButtonTwo();
		pause_button = play_pause_button->getButtonOne();
	}

	TBBUTTON play_or_pause;
	GetButton(index_of_toggle, &play_or_pause);
	if (bShowPlay == true)
	{
		play_or_pause.iBitmap = play_button->getImageIndex();
		play_pause_button->setCurrent(play_button);
	}
	else
	{
		play_or_pause.iBitmap = pause_button->getImageIndex();
		play_pause_button->setCurrent(pause_button); 
	}
	DeleteButton(index_of_toggle);
	InsertButton(index_of_toggle, &play_or_pause);
	
}
HICON Toolbar::makeIcon(ToolbarButton* pButton)
{
	USES_CONVERSION;
	if (pButton == NULL) return 0;
	std::string src = pButton->getImageSrc()->get_file();
	CString cstr_src = A2CW(src.c_str());
	HICON icon = (HICON)LoadImage(NULL, cstr_src, IMAGE_ICON, 
		mpSettings->getButtonWidth(), mpSettings->getButtonHeight(), LR_LOADFROMFILE);
	if (icon == NULL || icon < 0)
	{
		TRACE(_T("error loading icon: "));
		TRACE(cstr_src);
		return 0;
	}
	else
		return icon;
}
int Toolbar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBarCtrl::OnCreate(lpCreateStruct) == -1) return -1;
	m_images.Create(mpSettings->getButtonWidth(), mpSettings->getButtonHeight(), 
		ILC_COLOR24 | ILC_MASK, mpSettings->getNumberOfButtons(), AMIS_MAX_ANYTHING);
	HICON hIcon[AMIS_MAX_ANYTHING];
	int i = 0;
	int j = 0;
	ToolbarItems items = mpSettings->getItems();
	for (i=0; i<items.size(); i++)
	{
		//if this is a button
		if (items[i]->getType() == BUTTON)
		{
			HICON icon = makeIcon((ToolbarButton*)items[i]);
			hIcon[j] = icon;
			j++;
		}
		else if (items[i]->getType() == TOGGLE)
		{
			ToolbarToggleButton* p_toggle = (ToolbarToggleButton*)items[i];
			HICON icon = makeIcon(p_toggle->getButtonOne());
			hIcon[j] = icon;
			j++;
			HICON icon2 = makeIcon(p_toggle->getButtonTwo());
			hIcon[j] = icon2;
			j++;
		}
	}
	for (i=0; i<j; i++) m_images.Add(hIcon[i]);
	this->SetImageList(&m_images);
	this->SetHotImageList(&m_images);

	TBBUTTON buttons[AMIS_MAX_ANYTHING];
	j = 0;

	for (i=0; i<items.size(); i++)
	{
		if (items[i]->getType() == SEPARATOR)
		{
			buttons[i].fsState = TBSTATE_ENABLED;
			buttons[i].fsStyle = TBSTYLE_SEP;
			buttons[i].idCommand = 0;
			buttons[i].iString = 0;
			buttons[i].iBitmap = 0;
			buttons[i].dwData = 0;
		}
		else if (items[i]->getType() == BUTTON)
		{
			ToolbarButton* p_button = (ToolbarButton*)items[i];
			p_button->setImageIndex(j);
			buttons[i].iBitmap = j;
			buttons[i].fsState = TBSTATE_ENABLED;
			buttons[i].fsStyle=TBSTYLE_BUTTON;
			buttons[i].iString = p_button->getCommandId();
			buttons[i].idCommand = p_button->getCommandId();
			j++;
		}
		else if (items[i]->getType() == TOGGLE)
		{
			ToolbarToggleButton* p_button = (ToolbarToggleButton*)items[i];
			buttons[i].iBitmap = j;
			p_button->getButtonOne()->setImageIndex(j);
			p_button->getButtonTwo()->setImageIndex(j+1);
			buttons[i].fsState = TBSTATE_ENABLED;
			buttons[i].fsStyle=TBSTYLE_BUTTON;
			buttons[i].iString = p_button->getButtonOne()->getCommandId();
			buttons[i].idCommand = p_button->getButtonOne()->getCommandId();
			j+=2;
		}
	}
	
	this->AddButtons(items.size(), &buttons[0]);
	this->AutoSize();

	return 0;
}
BOOL Toolbar::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
   // Cast the NMHDR pointer to a tooltiptext struct
   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
   // Get the control ID
   UINT nID = pNMHDR->idFrom;
   // Make sure we should do this...
   if (pTTT->uFlags && TTF_IDISHWND)
   {	
	   ToolbarItems items = mpSettings->getItems();
	   for (unsigned int i=0; i<items.size(); i++)
	   {
		   if ((items[i]->getType() == BUTTON && 
			   ((ToolbarButton*)items[i])->getCommandId() == nID)
				||
			   (items[i]->getType() == TOGGLE && 
			   ((ToolbarToggleButton*)items[i])->getCurrent()->getCommandId() == nID)
			   )

		  {
			pTTT->lpszText = (unsigned short*)nID;
			pTTT->hinst = AfxGetResourceHandle();
			break;
		  }
		}
	   //Success
	   return(TRUE);
   }
   // Failure
   return(FALSE);
}

void Toolbar::enable(UINT cmdId, bool value)
{
	this->EnableButton(cmdId, value);
}
void Toolbar::enableAll(bool value)
{
	ToolbarItems items = mpSettings->getItems();
	for (int i = 0; i<items.size(); i++)
	{
		if (items[i]->getType() == amis::gui::toolbar::TOGGLE)
		{
			// get the commands
			toolbar::ToolbarToggleButton* p_toggle = (ToolbarToggleButton*)items[i];
			enable(p_toggle->getButtonOne()->getCommandId(), value);
			enable(p_toggle->getButtonTwo()->getCommandId(), value);
		}
		else if (items[i]->getType() == amis::gui::toolbar::BUTTON)
		{
			toolbar::ToolbarButton* p_button = (ToolbarButton*)items[i];
			enable(p_button->getCommandId(), value);
		}
	}
}