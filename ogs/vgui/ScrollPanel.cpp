//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_ScrollPanel.h"

using namespace vgui;

ScrollPanel::ScrollPanel(int x,int y,int wide,int tall)
{
}

void ScrollPanel::setSize(int wide,int tall)
{
}

void ScrollPanel::setScrollBarVisible(bool horizontal,bool vertical)
{
}

void ScrollPanel::setScrollBarAutoVisible(bool horizontal,bool vertical)
{
}

Panel* ScrollPanel::getClient()
{
	return null;
}

Panel* ScrollPanel::getClientClip()
{
	return null;
}

void ScrollPanel::setScrollValue(int horizontal,int vertical)
{
}

void ScrollPanel::getScrollValue(int& horizontal,int& vertical)
{
}

void ScrollPanel::recomputeClientSize()
{
}

ScrollBar* ScrollPanel::getHorizontalScrollBar()
{
	return null;
}

ScrollBar* ScrollPanel::getVerticalScrollBar()
{
	return null;
}

void ScrollPanel::validate()
{
}

void ScrollPanel::recomputeScroll()
{
}
