#pragma once

#include <VGUI_BitmapTGA.h>
#include <VGUI_SurfaceBase.h>
#include <vgui/ISurface.h>

class IEngineSurface;

class CEngineSurfaceWrap : public vgui::SurfaceBase
{
public:
	CEngineSurfaceWrap( vgui::Panel* embeddedPanel, IEngineSurface* engineSurface );
	~CEngineSurfaceWrap();

	void setTitle( const char* title ) override;
	bool setFullscreenMode( int wide, int tall, int bpp ) override;
	void setWindowedMode() override;
	void setAsTopMost( bool state ) override;
	void createPopup( vgui::Panel* embeddedPanel ) override;
	bool hasFocus() override;
	bool isWithin( int x, int y ) override;
	int  createNewTextureID() override;
	void GetMousePos( int &x, int &y ) override;

	void drawSetColor( int r, int g, int b, int a ) override;
	void drawFilledRect( int x0, int y0, int x1, int y1 ) override;
	void drawOutlinedRect( int x0, int y0, int x1, int y1 ) override;
	void drawSetTextFont( vgui::Font* font ) override;
	void drawSetTextColor( int r, int g, int b, int a ) override;
	void drawSetTextPos( int x, int y ) override;
	void drawPrintText( const char* text, int textLen ) override;
	void drawSetTextureRGBA( int id, const char* rgba, int wide, int tall ) override;
	void drawSetTexture( int id ) override;
	void drawTexturedRect( int x0, int y0, int x1, int y1 ) override;
	void invalidate( vgui::Panel* panel ) override;
	void enableMouseCapture( bool state ) override;
	void setCursor( vgui::Cursor* cursor ) override;
	void swapBuffers() override;
	void pushMakeCurrent( vgui::Panel* panel, bool useInsets ) override;
	void popMakeCurrent( vgui::Panel* panel ) override;
	void applyChanges() override;

	// TODO: more virtual functions, possibly part of an interface
	virtual void AppHandler( void* event, void* userData );
	virtual void lockCursor();
	virtual void unlockCursor();

	virtual void drawLine( int x1, int y1, int x2, int y2 );
	virtual void drawPolyLine( int* px, int* py, int n );
	virtual void drawTexturedPolygon( vgui2::VGuiVertex* pVertices, int n );
	virtual void drawSetTextureBGRA( int id, const char* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel );
	virtual void drawUpdateRegionTextureBGRA( int nTextureID, int x, int y, const byte* pchData, int wide, int tall );
	virtual void drawGetTextPos( int& x, int& y );
	virtual void drawPrintChar( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 );
	virtual void drawPrintCharAdd( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 );
private:
	EngineSurfaceWrap( const EngineSurfaceWrap& ) = delete;
	EngineSurfaceWrap& operator=( const EngineSurfaceWrap& ) = delete;
	
	vgui::BitmapTGA* _emulatedMouseImage = nullptr;
	IEngineSurface* _engineSurface;
	bool _cursorLocked = false;
};