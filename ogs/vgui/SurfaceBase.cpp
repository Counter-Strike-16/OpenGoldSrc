//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>
#include "VGUI.h"
#include "VGUI_SurfaceBase.h"
#include "VGUI_ImagePanel.h"
#include "VGUI_App.h"
#include "VGUI_Cursor.h"

using namespace vgui;

SurfaceBase::SurfaceBase(Panel* embeddedPanel)
{
	_embeddedPanel=embeddedPanel;
	_needsSwap=true;
	_emulatedCursor=new ImagePanel(null);
	_emulatedCursor->setVisible(false);
	_currentCursor=null;
	_embeddedPanel->setSurfaceBaseTraverse(this);
	getApp()->surfaceBaseCreated(this);
	_emulatedCursor->setParent(getPanel());
}

SurfaceBase::~SurfaceBase()
{
	getApp()->surfaceBaseDeleted(this);
}

Panel* SurfaceBase::getPanel()
{
	return _embeddedPanel;
}

void SurfaceBase::requestSwap()
{
	_needsSwap=true;
}

void SurfaceBase::resetModeInfo()
{
	_modeInfoDar.removeAll();
}

int SurfaceBase::getModeInfoCount()
{
	return _modeInfoDar.getCount();
}

bool SurfaceBase::getModeInfo(int mode,int& wide,int& tall,int& bpp)
{
	if((mode<0)||(mode>=_modeInfoDar.getCount()))
	{
		return false;
	}

	sscanf(_modeInfoDar[mode],"%dx%dx%d",&wide,&tall,&bpp);
	return true;
}

App* SurfaceBase::getApp()
{
	return App::getInstance();
}

void SurfaceBase::setEmulatedCursorVisible(bool state)
{
	_emulatedCursor->setVisible(state);
}

void SurfaceBase::setEmulatedCursorPos(int x,int y)
{
	getPanel()->removeChild(_emulatedCursor);
	getPanel()->addChild(_emulatedCursor);
	getPanel()->screenToLocal(x,y);

	if(_currentCursor&&_emulatedCursor->isVisible())
	{
		int cx,cy;
		_currentCursor->getHotspot(cx,cy);
		x-=cx;
		y-=cy;
	}

	_emulatedCursor->setPos(x,y);
}

void SurfaceBase::addModeInfo(int wide,int tall,int bpp)
{
	char buf[256];
	sprintf(buf,"%dx%dx%d",wide,tall,bpp);
	_modeInfoDar.putElement(vgui_strdup(buf));
}
