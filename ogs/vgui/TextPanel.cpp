//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_TextPanel.h"

using namespace vgui;

TextPanel::TextPanel(const char* text,int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
}

void TextPanel::setText(const char* text)
{
}

void TextPanel::setFont(vgui::Scheme::SchemeFont schemeFont)
{
}

void TextPanel::setFont(vgui::Font* font)
{
}

void TextPanel::setSize(int wide,int tall)
{
}

void TextPanel::setFgColor(int r,int g,int b,int a)
{
}

void TextPanel::setFgColor(Scheme::SchemeColor sc)
{
}

TextImage* TextPanel::getTextImage()
{
	return null;
}

void TextPanel::paint()
{
}
