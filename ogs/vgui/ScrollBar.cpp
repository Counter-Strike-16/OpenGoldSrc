//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_ScrollBar.h"

using namespace vgui;

ScrollBar::ScrollBar(int x,int y,int wide,int tall,bool vertical)
{
}

void ScrollBar::setValue(int value)
{
}

int ScrollBar::getValue()
{
	return 0;
}

void ScrollBar::addIntChangeSignal(IntChangeSignal* s)
{
}
 
void ScrollBar::setRange(int min,int max)
{
}

void ScrollBar::setRangeWindow(int rangeWindow)
{
}

void ScrollBar::setRangeWindowEnabled(bool state)
{
}

void ScrollBar::setSize(int wide,int tall)
{
}

bool ScrollBar::isVertical()
{
	return false;
}

bool ScrollBar::hasFullRange()
{
	return false;
}

void ScrollBar::setButton(Button* button,int index)
{
}

Button* ScrollBar::getButton(int index)
{
	return null;
}

void ScrollBar::setSlider(Slider* slider)
{
}

Slider* ScrollBar::getSlider()
{
	return null;
}

void ScrollBar::doButtonPressed(int buttonIndex)
{
}

void ScrollBar::setButtonPressedScrollValue(int value)
{
}

void ScrollBar::validate()
{
}

void ScrollBar::fireIntChangeSignal()
{
}

void ScrollBar::performLayout()
{
}

