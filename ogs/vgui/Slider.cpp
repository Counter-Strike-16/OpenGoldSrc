//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_Slider.h"

using namespace vgui;

Slider::Slider(int x,int y,int wide,int tall,bool vertical)
{
}

void Slider::setValue(int value)
{
}

int Slider::getValue()
{
	return 0;
}

bool Slider::isVertical()
{
	return false;
}

void Slider::addIntChangeSignal(IntChangeSignal* s)
{
}

void Slider::setRange(int min,int max)
{
}

void Slider::getRange(int& min,int& max)
{
}

void Slider::setRangeWindow(int rangeWindow)
{
}

void Slider::setRangeWindowEnabled(bool state)
{
}

void Slider::setSize(int wide,int tall)
{
}

void Slider::getNobPos(int& min, int& max)
{
}

bool Slider::hasFullRange()
{
	return false;
}

void Slider::setButtonOffset(int buttonOffset)
{
}

void Slider::recomputeNobPosFromValue()
{
}

void Slider::recomputeValueFromNobPos()
{
}

void Slider::privateCursorMoved(int x,int y,Panel* panel)
{
}

void Slider::privateMousePressed(MouseCode code,Panel* panel)
{
}

void Slider::privateMouseReleased(MouseCode code,Panel* panel)
{
}

void Slider::fireIntChangeSignal()
{
}

void Slider::paintBackground()
{
}

