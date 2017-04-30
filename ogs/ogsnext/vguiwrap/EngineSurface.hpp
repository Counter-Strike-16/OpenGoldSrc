/*
EngineSurface.hpp - engine vgui surface
Copyright (C) 2015 XashXT Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef XASH_ENGINESURFACE_HPP
#define XASH_ENGINESURFACE_HPP

#include<VGUI_App.h>
#include<VGUI_Font.h>
#include<VGUI_Panel.h>
#include<VGUI_Cursor.h>
#include<VGUI_SurfaceBase.h>

using namespace vgui;

class CEngineSurface : public SurfaceBase
{
public:
	CEngineSurface( Panel *embeddedPanel );
	~CEngineSurface();
	
	virtual Panel *getEmbeddedPanel( void );
	virtual bool setFullscreenMode( int wide, int tall, int bpp );
	virtual void setWindowedMode( void );
	virtual void setTitle( const char *title ) { }
	virtual void createPopup( Panel* embeddedPanel ) { }
	virtual bool isWithin( int x, int y ) { return true; }
	virtual bool hasFocus( void );
private:
	struct paintState_t
	{
		Panel	*m_pPanel;
		int	iTranslateX;
		int	iTranslateY;
		int	iScissorLeft;
		int	iScissorRight;
		int	iScissorTop;
		int	iScissorBottom;
	};

	// point translation for current panel
	int		_translateX;
	int		_translateY;

	// the size of the window to draw into
	int		_surfaceExtents[4];

	CUtlVector <paintState_t> _paintStack;

	void SetupPaintState( const paintState_t &paintState );
	void InitVertex( vpoint_t &vertex, int x, int y, float u, float v );
protected:
	Font* _hCurrentFont;
	Cursor* _hCurrentCursor;
	int _drawTextPos[2];
	int _drawColor[4];
	int _drawTextColor[4];
	
	friend class App;
	friend class Panel;
	
	virtual int createNewTextureID();
	virtual void drawSetColor( int r, int g, int b, int a );
	virtual void drawSetTextColor( int r, int g, int b, int a );
	virtual void drawFilledRect( int x0, int y0, int x1, int y1 );
	virtual void drawOutlinedRect( int x0,int y0,int x1,int y1 );
	virtual void drawSetTextFont( Font *font );
	virtual void drawSetTextPos( int x, int y );
	virtual void drawPrintText( const char* text, int textLen );
	virtual void drawSetTextureRGBA( int id, const char* rgba, int wide, int tall );
	virtual void drawSetTexture( int id );
	virtual void drawTexturedRect( int x0, int y0, int x1, int y1 );
	virtual bool createPlat( void ) { return false; }
	virtual bool recreateContext( void ) { return false; }
	virtual void setCursor( Cursor* cursor );
	virtual void pushMakeCurrent( Panel* panel, bool useInsets );
	virtual void popMakeCurrent( Panel* panel );

	// not used in engine instance
	virtual void enableMouseCapture( bool state ) { }
	virtual void invalidate( Panel *panel ) { }
	virtual void setAsTopMost( bool state ) { }
	virtual void applyChanges( void ) { }
	virtual void swapBuffers( void ) { }
};

#endif // XASH_ENGINESURFACE_HPP