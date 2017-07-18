#include "precompiled.hpp"
#include "quakedef.hpp"
#include "gl_draw.hpp"
#include "vgui_EngineSurface.hpp"

//TODO: never set - Solokiller
EngineSurface* EngineSurface::s_pEngineSurface = nullptr;

EXPOSE_SINGLE_INTERFACE( EngineSurface, IEngineSurface, ENGINESURFACE_INTERFACE_VERSION );

EngineSurface::~EngineSurface()
{
	deleteRenderPlat();
	_renderPlat = nullptr;
	s_pEngineSurface = nullptr;
}

int EngineSurface::createNewTextureID()
{
	return static_cast<int>( GL_GenTexture() );
}

void EngineSurface::drawGetTextPos( int& x, int& y )
{
	x = _drawTextPos[ 0 ];
	y = _drawTextPos[ 1 ];
}

void EngineSurface::drawSetColor( int r, int g, int b, int a )
{
	_drawColor[ 0 ] = r;
	_drawColor[ 1 ] = g;
	_drawColor[ 2 ] = b;
	_drawColor[ 3 ] = a;
}

void EngineSurface::drawSetTextColor( int r, int g, int b, int a )
{
	if( _drawTextColor[ 0 ] != r ||
		_drawTextColor[ 1 ] != g ||
		_drawTextColor[ 2 ] != b ||
		_drawTextColor[ 3 ] != a )
	{
		drawFlushText();

		_drawTextColor[ 0 ] = r;
		_drawTextColor[ 1 ] = g;
		_drawTextColor[ 2 ] = b;
		_drawTextColor[ 3 ] = a;
	}
}

void EngineSurface::drawSetTextPos( int x, int y )
{
	_drawTextPos[ 0 ] = x;
	_drawTextPos[ 1 ] = y;
}

void EngineSurface::freeEngineSurface()
{
	//TODO: doesn't work because this pointer is never set. - Solokiller
	if( s_pEngineSurface )
	{
		delete s_pEngineSurface;
		s_pEngineSurface = nullptr;
	}
}