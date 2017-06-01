//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_TextImage.h"

using namespace vgui;

TextImage::TextImage(int textBufferLen,const char* text)
{
}

TextImage::TextImage(const char* text)
{
}

void TextImage::init(int textBufferLen,const char* text)
{
}

void TextImage::getTextSize(int& wide,int& tall)
{
}

void TextImage::getTextSizeWrapped(int& wide,int& tall)
{
}

Font* TextImage::getFont()
{
	return null;
}

void TextImage::setText(int textBufferLen,const char* text)
{
}

void TextImage::setText(const char* text)
{
}

void TextImage::setFont(vgui::Scheme::SchemeFont schemeFont)
{
}

void TextImage::setFont(vgui::Font* font)
{
}

void TextImage::setSize(int wide,int tall)
{
}

void TextImage::paint(Panel* panel)
{
}
