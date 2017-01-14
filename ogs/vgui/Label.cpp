//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <stdarg.h>
#include <stdio.h>
#include "VGUI.h"
#include "VGUI_Label.h"
#include "VGUI_TextImage.h"

using namespace vgui;

Label::Label(int textBufferLen,const char* text,int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
	init(textBufferLen,text,true);
}

Label::Label(const char* text,int x,int y,int wide,int tall): Panel(x,y,wide,tall)
{
	init(strlen(text)+1,text,true);
}

Label::Label(const char* text,int x,int y) : Panel(x,y,10,10)
{
	init(strlen(text)+1,text,true);
}

Label::Label(const char* text) : Panel(0,0,10,10)
{
	init(strlen(text)+1,text,true);
}

void Label::init(int textBufferLen,const char* text,bool textFitted)
{
	_contentFitted=textFitted;
	_textAlignment=a_center;
	_contentAlignment=a_center;
	_textImage=new TextImage(textBufferLen,text);
	_textImage->setColor(Color(Scheme::sc_black));
	_image=null;

	setText(textBufferLen,text);
}

void Label::setImage(Image* image)
{
	_image=image;
	recomputeMinimumSize();

	if(_image)
	{
		repaint();
	}
}

void Label::setText(int textBufferLen,const char* text)
{
	_textImage->setText(textBufferLen,text);
	recomputeMinimumSize();
	repaint();
}

void Label::setText(const char* format,...)
{
	char buf[4096];
	va_list argList;

	va_start(argList,format);
	int ret=vsnprintf(buf,sizeof(buf),format,argList);
	buf[sizeof(buf)-1]=0;
	va_end(argList);

	setText(strlen(buf)+1,buf);
}

void Label::setFont(Scheme::SchemeFont schemeFont)
{
	_textImage->setFont(schemeFont);
	recomputeMinimumSize();
	repaint();
}

void Label::setFont(Font* font)
{
	_textImage->setFont(font);
	recomputeMinimumSize();
	repaint();
}

void Label::getTextSize(int& wide,int& tall)
{
	_textImage->getTextSize(wide,tall);
}

void Label::getContentSize(int& wide,int& tall)
{
	int tx0,ty0,tx1,ty1,ix0,iy0,ix1,iy1,minX,minY,maxX,maxY;
	computeAlignment(tx0,ty0,tx1,ty1,ix0,iy0,ix1,iy1,minX,minY,maxX,maxY);
	wide=maxX-minX;
	tall=maxY-minY;
}

void Label::setTextAlignment(Alignment alignment)
{
	_textAlignment=alignment;
	recomputeMinimumSize();
	repaint();
}

void Label::setContentAlignment(Alignment alignment)
{
	_contentAlignment=alignment;
	recomputeMinimumSize();
	repaint();
}

Panel* Label::createPropertyPanel()
{
	// dummy
	return null;
}

void Label::setFgColor(int r,int g,int b,int a)
{
	Panel::setFgColor(r,g,b,a);
	_textImage->setColor(Color(r,g,b,a));
	repaint();
}

void Label::setFgColor(vgui::Scheme::SchemeColor sc)
{
	Panel::setFgColor(sc);
	_textImage->setColor(sc);
	repaint();
}

void Label::setContentFitted(bool state)
{
	_contentFitted=state;
	recomputeMinimumSize();
	repaint();
}

void Label::computeAlignment(int& tx0,int& ty0,int& tx1,int& ty1,int& ix0,int& iy0,int& ix1,int& iy1,int& minX,int& minY,int& maxX,int& maxY)
{
	// dummy
}

void Label::paint()
{
	int tx0,ty0,tx1,ty1,ix0,iy0,ix1,iy1,minX,minY,maxX,maxY;
	computeAlignment(tx0,ty0,tx1,ty1,ix0,iy0,ix1,iy1,minX,minY,maxX,maxY);

	if(_image)
	{
		_image->setPos(ix0,iy0);
		_image->doPaint(this);
	}

	if(_textImage)
	{
		_textImage->setPos(tx0,ty0);
		_textImage->doPaint(this);
	}

	if(hasFocus())
	{
		drawSetColor(Scheme::sc_primary2);
		drawOutlinedRect(tx0,ty0,tx1,ty1);
	}
}

void Label::recomputeMinimumSize()
{
	int wide,tall;
	getContentSize(wide,tall);
	setPreferredSize(wide,tall);

	if(_contentFitted)
	{
		setSize(wide,tall);
	}
}
