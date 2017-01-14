//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_Scheme.h"
#include "VGUI_Font.h"
#include "VGUI_Cursor.h"

using namespace vgui;

Scheme::Scheme()
{
	setColor(sc_black,0,0,0,0);
	setColor(sc_white,255,255,255,0);
	setColor(sc_primary1,102,102,153,0);
	setColor(sc_primary2,153,153,204,0);
	setColor(sc_primary3,204,204,255,0);
	setColor(sc_secondary1,102,102,102,0);
	setColor(sc_secondary2,153,153,153,0);
	setColor(sc_secondary3,204,204,204,0);
	setColor(sc_user,0,0,0,0);
	setFont(sf_primary1,new Font("Arial",0,0,20,0,0,400,false,false,false,false));
	setFont(sf_primary2,new Font("FixedSys",0,0,18,0,0,400,false,false,false,false));
	setFont(sf_primary3,new Font("Arial",0,0,12,0,0,400,false,false,false,false));
	setFont(sf_secondary1,new Font("Marlett",0,0,16,0,0,0,false,false,false,true));
	setCursor(scu_none,new Cursor(Cursor::dc_none));
	setCursor(scu_arrow,new Cursor(Cursor::dc_arrow));
	setCursor(scu_ibeam,new Cursor(Cursor::dc_ibeam));
	setCursor(scu_hourglass,new Cursor(Cursor::dc_hourglass));
	setCursor(scu_crosshair,new Cursor(Cursor::dc_crosshair));
	setCursor(scu_up,new Cursor(Cursor::dc_up));
	setCursor(scu_sizenwse,new Cursor(Cursor::dc_sizenwse));
	setCursor(scu_sizenesw,new Cursor(Cursor::dc_sizenesw));
	setCursor(scu_sizewe,new Cursor(Cursor::dc_sizewe));
	setCursor(scu_sizens,new Cursor(Cursor::dc_sizens));
	setCursor(scu_sizeall,new Cursor(Cursor::dc_sizeall));
	setCursor(scu_no,new Cursor(Cursor::dc_no));
	setCursor(scu_hand,new Cursor(Cursor::dc_hand));
}

void Scheme::setColor(SchemeColor sc,int r,int g,int b,int a)
{
	_color[sc][0]=r;
	_color[sc][1]=g;
	_color[sc][2]=b;
	_color[sc][3]=a;
}

void Scheme::getColor(SchemeColor sc,int& r,int& g,int& b,int& a)
{
	r=_color[sc][0];
	g=_color[sc][1];
	b=_color[sc][2];
	a=_color[sc][3];
}

void Scheme::setFont(SchemeFont sf,Font* font)
{
	_font[sf]=font;
}

Font* Scheme::getFont(SchemeFont sf)
{
	return _font[sf];
}

void Scheme::setCursor(SchemeCursor sc,Cursor* cursor)
{
	_cursor[sc]=cursor;
}

Cursor* Scheme::getCursor(SchemeCursor sc)
{
	return _cursor[sc];;
}
