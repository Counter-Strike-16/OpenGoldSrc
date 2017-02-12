//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_Surface.h"
#include "VGUI_Panel.h"

#if defined ( WIN32 )
#include <windows.h>

namespace vgui
{
class SurfacePlat
{
public:
	HWND    hwnd;
	HDC     hdc;
	HDC     hwndDC;
	HDC	  textureDC;
	HGLRC   hglrc;
	HRGN    clipRgn;
	HBITMAP bitmap;
	int     bitmapSize[2];
	int     restoreInfo[4];
	bool    isFullscreen;
	int     fullscreenInfo[3];
};
}
#endif

namespace {
#ifdef _WIN32
class Texture
{
public:
	int     _id;
	HBITMAP _bitmap;
	HBITMAP _maskBitmap;
	bool    _bMask;
	HICON	_icon;
	int     _wide;
	int     _tall;
	void   *_dib;
	void   *_maskDib;
	const char *_filename;
};
#else
// from DWARF data
class Texture
{
public:
	int     _id;
	int     _wide;
	int     _tall;
	void   *_dib;
};
#endif
}

using namespace vgui;

namespace vgui
{
static int staticBindIndex=2700;
static Texture* staticTextureCurrent=null;

Texture* GetTextureById(int id)
{
	return null;
}
}

Surface::Surface(Panel* embeddedPanel) : SurfaceBase(embeddedPanel)
{
	createPlat();
	recreateContext();
}

void Surface::setTitle(const char* title)
{
#if defined ( WIN32 )
	SetWindowText(_plat->hwnd,title);
#endif
}

bool Surface::setFullscreenMode(int wide,int tall,int bpp)
{
	// dummy
	return false;
}

void Surface::setWindowedMode()
{
	// dummy
}

void Surface::setAsTopMost(bool state)
{
#if defined ( WIN32 )
	if (state)
		SetWindowPos(_plat->hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(_plat->hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
#endif
}

int Surface::getModeInfoCount()
{
	resetModeInfo();
	addModeInfo(640,480,16);
	addModeInfo(800,600,16);
	return SurfaceBase::getModeInfoCount();
}

void Surface::createPopup(Panel* embeddedPanel)
{
	embeddedPanel->setParent(null);
	new Surface(embeddedPanel); // ?
}

bool Surface::hasFocus()
{
#if defined ( WIN32 )
	return GetFocus()==_plat->hwnd;
#else
	return false;
#endif
}

bool Surface::isWithin(int x,int y)
{
#if defined ( WIN32 )
	POINT pnt={x,y};
	return WindowFromPoint(pnt)==_plat->hwnd;
#else
	return false;
#endif
}

void Surface::GetMousePos( int &x, int &y )
{
}

int Surface::createNewTextureID(void)
{
	return staticBindIndex++;
}

void Surface::drawSetColor(int r,int g,int b,int a)
{
#if defined ( WIN32 )
	SetBkColor(_plat->hdc,RGB(r,g,b));
#endif
}

void Surface::drawFilledRect(int x0,int y0,int x1,int y1)
{
#if defined ( WIN32 )
	// trick to draw filled rectangles using current background color
	RECT rect = { x0, y0, x1, y1};
	BOOL res = ExtTextOut(_plat->hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
#endif
}

void Surface::drawOutlinedRect(int x0,int y0,int x1,int y1)
{
	// draw an outline of a rectangle using 4 filledRect
	drawFilledRect(x0,y0,x1,y0+1);     // top
	drawFilledRect(x0,y1-1,x1,y1);	   // bottom
	drawFilledRect(x0,y0+1,x0+1,y1-1); // left
	drawFilledRect(x1-1,y0+1,x1,y1-1); // right
}

void Surface::drawSetTextFont(Font* font)
{
	// dummy
}

void Surface::drawSetTextColor(int r,int g,int b,int a)
{
#if defined ( WIN32 )
	SetTextColor(_plat->hdc,RGB(r,g,b));
#endif
}

void Surface::drawSetTextPos(int x,int y)
{
#if defined ( WIN32 )
	MoveToEx(_plat->hdc,x,y,null);	
#endif
}

void Surface::drawPrintText(const char* text,int textLen)
{
	if (!text)
		return;

	if (textLen < 1)
		return;

#ifdef _WIN32
	ExtTextOut(_plat->hdc,0,0,0,NULL,text,textLen,NULL);
#endif
}

void Surface::drawSetTextureRGBA(int id,const char* rgba,int wide,int tall)
{
	// dummy
}

void Surface::drawSetTexture(int id)
{
	staticTextureCurrent = GetTextureById(id);
}

void Surface::drawTexturedRect(int x0,int y0,int x1,int y1)
{
	if (staticTextureCurrent == null)
	{
		return;
	}

#ifdef _WIN32


	if (_plat->textureDC == null)
	{
		return;
	}

	HBITMAP bitmap = staticTextureCurrent->_bitmap;
	int wide = staticTextureCurrent->_wide;
	int tall = staticTextureCurrent->_tall;

	::SelectObject(_plat->textureDC, bitmap);
	::StretchBlt(_plat->hdc,x0,y0,x1-x0,y1-y0,_plat->textureDC,0,0,wide,tall,SRCCOPY);
#endif
}

void Surface::invalidate(Panel *panel)
{
#ifdef _WIN32
	InvalidateRect(_plat->hwnd, NULL, false);
#endif
}

bool Surface::createPlat()
{
	// dummy
	return false;
}

bool Surface::recreateContext()
{
	// dummy
	return false;
}

void Surface::enableMouseCapture(bool state)
{
#ifdef _WIN32
	if (state)
		::SetCapture(_plat->hwnd);
	else
		::ReleaseCapture();
#endif
}

void Surface::setCursor(Cursor* cursor)
{
	// dummy
}

void Surface::swapBuffers()
{
	// dummy
}

void Surface::pushMakeCurrent(Panel* panel,bool useInsets)
{
	// dummy
}

void Surface::popMakeCurrent(Panel* panel)
{
	// dummy
}

void Surface::applyChanges()
{
	// dummy
}