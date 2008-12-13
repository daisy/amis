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
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <string>
#include <vector>
#include "stdafx.h"
#include "ambulant/net/url.h"

namespace amis
{
namespace gui
{
namespace toolbar
{
enum ToolbarItemType {SEPARATOR, BUTTON, TOGGLE};

//abstract base class
class ToolbarItem
{
public:
	ToolbarItem(ToolbarItemType);
	virtual ~ToolbarItem() = 0;
	ToolbarItemType getType();
private:
	ToolbarItemType mType;
};

typedef std::vector<ToolbarItem*> ToolbarItems;

class ToolbarSeparator : public ToolbarItem
{
public:
	ToolbarSeparator();
	~ToolbarSeparator();
};
class ToolbarButton : public ToolbarItem
{
public:
	ToolbarButton();
	~ToolbarButton();
	void setImageSrc(ambulant::net::url);
	void setCommandId(UINT);
	const ambulant::net::url* getImageSrc();
	UINT getCommandId();
	std::string getId();
	void setId(std::string);
	void setImageIndex(int);
	int getImageIndex();
	void setTooltipId(UINT);
	UINT getTooltipId();
	void setTooltipText(CString);
	CString getTooltipText();
private:
	ambulant::net::url mSrc;
	UINT mMfcId;
	std::string mId;
	int mImageIndex;
	UINT mTooltipId;
	CString mTooltipText;
};
class ToolbarToggleButton : public ToolbarItem
{
public:
	ToolbarToggleButton();
	~ToolbarToggleButton();
	void setButtonOne(ToolbarButton*);
	void setButtonTwo(ToolbarButton*);
	void addButton(ToolbarButton*);
	ToolbarButton* getButtonOne();
	ToolbarButton* getButtonTwo();
	void setCurrent(ToolbarButton*);
	ToolbarButton* getCurrent();
private:
	ToolbarButton* mpButtonOne;
	ToolbarButton* mpButtonTwo;
	ToolbarButton* mpCurrent;
};

enum ToolbarPlacement {TOP, BOTTOM, LEFT, RIGHT};
enum ToolbarView {DEFAULT, BASIC};
class ToolbarSettings
{
public:
	ToolbarSettings();
	~ToolbarSettings();
	void setView(ToolbarView);
	void setPlacement(ToolbarPlacement);
	//width, height
	void setButtonResolution(int, int);
	ToolbarView getView();
	ToolbarPlacement getPlacement();
	int getButtonWidth();
	int getButtonHeight();
	void addItem(ToolbarItem*);
	ToolbarItems getItems();
	int getNumberOfButtons();
private:
	void cleanUpVector();
	ToolbarItems mItems;
	int mButtonHeight;
	int mButtonWidth;
	ToolbarPlacement mPlacement;
	ToolbarView mView;
};
class Toolbar : public CToolBarCtrl
{
public:
	Toolbar();
	virtual ~Toolbar();
	void setSettings(ToolbarSettings*);
	void togglePlayPause(bool);
	void enable(UINT, bool);
	void enableAll(bool);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR*, LRESULT*);
	afx_msg void OnDestroy();
private:
	HICON makeIcon(ToolbarButton*);
	void deleteAll();
	CToolTipCtrl* mpToolTips;
	CImageList m_images;
	ToolbarSettings* mpSettings;
	ToolbarButton* mpCurrentForPlayOrPause;
	DECLARE_MESSAGE_MAP()

};
}
}
}
#endif