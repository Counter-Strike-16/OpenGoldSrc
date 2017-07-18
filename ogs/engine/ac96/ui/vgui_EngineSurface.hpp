#pragma once

#include "EngineSurface.h"

class EngineSurfaceRenderPlat;

class EngineSurface : public IEngineSurface
{
public:
	EngineSurface();
	virtual ~EngineSurface();

	void pushMakeCurrent( int* insets, int* absExtents, int* clipRect, bool translateToScreenSpace ) override;
	void popMakeCurrent() override;

	void drawFilledRect( int x0, int y0, int x1, int y1 ) override;
	void drawOutlinedRect( int x0, int y0, int x1, int y1 ) override;
	void drawLine( int x1, int y1, int x2, int y2 ) override;
	void drawPolyLine( int* px, int* py, int numPoints ) override;

	void drawTexturedPolygon( vgui2::VGuiVertex* pVertices, int n ) override;
	void drawSetTextureRGBA( int id, const byte* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel ) override;
	void drawSetTexture( int id ) override;
	void drawTexturedRect( int x0, int y0, int x1, int y1 ) override;
	int createNewTextureID() override;

	void drawSetColor( int r, int g, int b, int a ) override;

	void drawSetTextColor( int r, int g, int b, int a ) override;
	void drawSetTextPos( int x, int y ) override;
	void drawGetTextPos( int& x, int& y ) override;
	void drawPrintChar( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 ) override;
	void drawPrintCharAdd( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 ) override;

	void drawSetTextureFile( int id, const char* filename, int hardwareFilter, bool forceReload ) override;
	void drawGetTextureSize( int id, int& wide, int& tall ) override;
	int isTextureIdValid( int id ) override;

	void drawSetSubTextureRGBA( int textureID, int drawX, int drawY, const byte* rgba, int subTextureWide, int subTextureTall ) override;

	void drawFlushText() override;
	void resetViewPort() override;

	void drawSetTextureBGRA( int id, const byte* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel ) override;
	void drawUpdateRegionTextureBGRA( int nTextureID, int x, int y, const byte* pchData, int wide, int tall ) override;

	void createRenderPlat();
	void deleteRenderPlat();

	static void freeEngineSurface();

private:
	static EngineSurface* EngineSurface::s_pEngineSurface;

	EngineSurfaceRenderPlat* _renderPlat;
	int _drawColor[ 4 ];
	int _drawTextColor[ 4 ];
	int _drawTextPos[ 2 ];

private:
	EngineSurface( const EngineSurface& ) = delete;
	EngineSurface& operator=( const EngineSurface& ) = delete;
};