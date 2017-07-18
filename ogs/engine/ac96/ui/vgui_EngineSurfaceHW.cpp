#include "precompiled.hpp"
#include "quakedef.hpp"
#include "cl_main.hpp"
#include "qgl.hpp"
#include "gl_rmain.hpp"
#include "render.hpp"
#include "sys_getmodes.h"
#include "vgui_EngineSurface.hpp"
#include "vgui_int.hpp"

#include <UtlRBTree.h>

bool g_bScissor = false;
Rect_t g_ScissorRect = {};

struct TCoordRect
{
	float s0;
	float t0;
	float s1;
	float t1;
};

float InterpTCoord( int val, int min, int max, float tMin, float tMax )
{
	return tMin + ( tMax - tMin ) * ( static_cast<float>( val - min ) / static_cast<float>( max - min ) );
}

bool IsRectEmpty( const Rect_t* rect )
		{
	return rect->x == rect->width && rect->y == rect->height;
			}

bool IntersectRect( Rect_t* dest, const Rect_t* src1, const Rect_t* src2 )
{
	if( !src1 || !dest || !src2 )
		return false;

	if( !IsRectEmpty( src1 ) && !IsRectEmpty( src2 ) &&
		src2->width > src1->x &&
		src2->x < src1->width &&
		src2->height > src1->y &&
		src2->y < src1->height )
	{
		dest->x = max( src1->x, src2->x );
		dest->width = min( src1->width, src2->width );

		dest->y = max( src1->y, src2->y );
		dest->height = min( src1->height, src2->height );

		return true;
	}

	dest->x = 0;
	dest->y = 0;
	dest->width = 0;
	dest->height = 0;

	return false;
}

bool ScissorRect_TCoords( int x0, int y0, int x1, int y1,
						  float s0, float t0, float s1, float t1,
						  Rect_t* pOut, TCoordRect* pTCoords )
{
	if( g_bScissor )
	{
		if( pOut )
		{
			Rect_t in =
			{
				x0, y0, x1, y1
			};

			if( IntersectRect( pOut, &g_ScissorRect, &in ) )
			{
				if( pTCoords )
				{
					pTCoords->s0 = InterpTCoord( pOut->x, x0, x1, s0, s1 );
					pTCoords->s1 = InterpTCoord( pOut->width, x0, x1, s0, s1 );
				
					pTCoords->t0 = InterpTCoord( pOut->y, y0, y1, t0, t1 );
					pTCoords->t1 = InterpTCoord( pOut->height, y0, y1, t0, t1 );
				}

				return true;
			}
		}
	}
	else
	{
		pOut->x = x0;
		pOut->width = x1;
		pOut->y = y0;
		pOut->height = y1;

		if( pTCoords )
		{
			pTCoords->s0 = s0;
			pTCoords->t0 = t0;
			pTCoords->s1 = s1;
			pTCoords->t1 = t1;
		}

		return true;
	}

	return false;
	}

bool ScissorRect( int x0, int y0, int x1, int y1, Rect_t* pOut )
{
	return ScissorRect_TCoords( x0, y0, x1, y1, 0, 0, 0, 0, pOut, nullptr );
}

namespace
{
struct Texture
{
	int _id;
	int _wide;
	int _tall;
	float _s0;
	float _t0;
	float _s1;
	float _t1;
};
}

struct VertexBuffer_t
{
	float texcoords[ 2 ];
	float vertex[ 2 ];
};

static bool VguiSurfaceTexturesLessFunc( const Texture& lhs, const Texture& rhs )
{
	return lhs._id < rhs._id;
}

static CUtlRBTree<Texture, int> g_VGuiSurfaceTextures( &VguiSurfaceTexturesLessFunc );
static Texture* staticTextureCurrent = nullptr;

static VertexBuffer_t g_VertexBuffer[ 256 ];
static int g_iVertexBufferEntriesUsed = 0;
static float g_vgui_projection_matrix[ 16 ];

static Texture* staticGetTextureById( int id )
{
	Texture dummy;
	dummy._id = id;

	auto index = g_VGuiSurfaceTextures.Find( dummy );

	if( index != g_VGuiSurfaceTextures.InvalidIndex() )
		return &g_VGuiSurfaceTextures[ index ];

	return nullptr;
}

void EngineSurface::createRenderPlat()
{
	//Nothing
}

void EngineSurface::deleteRenderPlat()
{
	//Nothing
}

void EngineSurface::pushMakeCurrent( int* insets, int* absExtents, int* clipRect, bool translateToScreenSpace )
{
	int x = 0, y = 0;

	if( translateToScreenSpace )
	{
		if( VideoMode_IsWindowed() )
		{
			SDL_GetWindowPosition( pmainwindow, &x, &y );
		}
	}

	Rect_t rect;

	rect.y = 0;

	if( VideoMode_IsWindowed() )
	{
		SDL_GetWindowSize( pmainwindow, &rect.width, &rect.height );
	}
	else
	{
		VideoMode_GetCurrentVideoMode( &rect.width, &rect.height, nullptr );
	}

	rect.height += rect.y;

	//TODO: operations out of order? - Solokiller
	qglPushMatrix();
	qglMatrixMode( GL_PROJECTION );
	qglLoadIdentity();
	qglOrtho( 0, rect.width, rect.height, 0, -1, 1 );

	qglMatrixMode( GL_MODELVIEW );
	qglPushMatrix();
	qglLoadIdentity();

	int wide = insets[ 0 ];
	int tall = insets[ 1 ];

	qglTranslatef( wide, tall, 0 );

	const int iOffsetX = absExtents[ 0 ] - x;
	const int iOffsetY = absExtents[ 1 ] - y;
	qglTranslatef( iOffsetX, iOffsetY, 0 );

	g_bScissor = true;

	g_ScissorRect.x = clipRect[ 0 ] - x - ( insets[ 0 ] + iOffsetX );
	g_ScissorRect.width = clipRect[ 2 ] - x - ( insets[ 0 ] + iOffsetX );
	g_ScissorRect.y = clipRect[ 1 ] - y - ( insets[ 1 ] + iOffsetY );
	g_ScissorRect.height = clipRect[ 3 ] - y - ( insets[ 1 ] + iOffsetY );

	qglEnable( GL_BLEND );
	qglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void EngineSurface::popMakeCurrent()
{
	drawFlushText();

	//TODO: operations out of order? - Solokiller
	qglMatrixMode( GL_PROJECTION );
	qglMatrixMode( GL_MODELVIEW );
	qglPopMatrix();
	qglPopMatrix();

	g_bScissor = false;

	qglEnable( GL_TEXTURE_2D );
}

void EngineSurface::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	if( _drawColor[ 3 ] != 255 )
	{
		Rect_t rcOut;

		if( ScissorRect( x0, y0, x1, y1, &rcOut ) )
		{
			qglDisable( GL_TEXTURE_2D );

			qglColor4ub( _drawColor[ 0 ], _drawColor[ 1 ], _drawColor[ 2 ], 255 - _drawColor[ 3 ] );
		
			qglBegin( GL_QUADS );

			qglVertex2f( rcOut.x, rcOut.y );
			qglVertex2f( rcOut.width, rcOut.y );
			qglVertex2f( rcOut.width, rcOut.height );
			qglVertex2f( rcOut.x, rcOut.height );

			qglEnd();

			qglEnable( GL_TEXTURE_2D );
		}
	}
}

void EngineSurface::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	if( _drawColor[ 3 ] != 255 )
	{
		qglDisable( GL_TEXTURE_2D );
		qglColor4ub( _drawColor[ 0 ], _drawColor[ 1 ], _drawColor[ 2 ], 255 - _drawColor[ 3 ] );

		drawFilledRect( x0, y0, x1, y0 + 1 );
		drawFilledRect( x0, y1 - 1, x1, y1 );
		drawFilledRect( x0, y0 + 1, x0 + 1, y1 - 1 );
		drawFilledRect( x1 - 1, y0 + 1, x1, y1 - 1 );
	}
}

void EngineSurface::drawLine( int x1, int y1, int x2, int y2 )
{
	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_BLEND );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	qglBlendFunc( GL_SRC_ALPHA, GL_ONE );

	qglColor4ub( _drawColor[ 0 ], _drawColor[ 1 ], _drawColor[ 2 ], 255 - _drawColor[ 3 ] );

	qglBegin( GL_LINES );

	qglVertex2f( x1, y1 );
	qglVertex2f( x2, y2 );

	qglEnd();

	qglColor3f( 1, 1, 1 );

	qglEnable( GL_TEXTURE_2D );
	qglDisable( GL_BLEND );
}

void EngineSurface::drawPolyLine( int* px, int* py, int numPoints )
{
	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_BLEND );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	qglBlendFunc( GL_SRC_ALPHA, GL_ONE );

	qglColor4ub( _drawColor[ 0 ], _drawColor[ 1 ], _drawColor[ 2 ], 255 - _drawColor[ 3 ] );

	qglBegin( GL_LINES );

	for( int i = 1; i < numPoints; ++i )
	{
		qglVertex2f( px[ i - 1 ], py[ i - 1 ] );
		qglVertex2f( px[ i ], py[ i ] );
	}

	qglEnd();

	qglColor3f( 1, 1, 1 );

	qglEnable( GL_TEXTURE_2D );
	qglDisable( GL_BLEND );
}

void EngineSurface::drawTexturedPolygon( vgui2::VGuiVertex* pVertices, int n )
{
	if( _drawColor[ 3 ] == 255 )
		return;

	qglColor4ub(
		_drawColor[ 0 ],
		_drawColor[ 1 ],
		_drawColor[ 2 ],
		255 - _drawColor[ 3 ]
	);

	qglEnable( GL_TEXTURE_2D );

	auto pNext = pVertices;

	qglBegin( GL_POLYGON );

	for( int i = 0; i < n; ++i, ++pNext )
	{
		qglTexCoord2f( pNext->u, pNext->v );
		qglVertex2f( pNext->x, pNext->y );
	}

	qglEnd();
}

void EngineSurface::drawSetTextureRGBA( int id, const byte* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel )
{
	auto pTexture = staticGetTextureById( id );

	if( !pTexture )
	{
		Texture newTexture;

		memset( &newTexture, 0, sizeof( newTexture ) );

		newTexture._id = id;
		
		pTexture = &g_VGuiSurfaceTextures[ g_VGuiSurfaceTextures.Insert( newTexture ) ];
	}

	if( !pTexture )
		return;

	pTexture->_id = id;
	pTexture->_wide = wide;
	pTexture->_tall = tall;

	int pow2Wide;
	int pow2Tall;

	for( int i = 0; i < 32; ++i )
	{
		pow2Wide = 1 << i;

		if( wide <= pow2Wide )
			break;
	}

	for( int i = 0; i < 32; ++i )
	{
		pow2Tall = 1 << i;

		if( tall <= pow2Tall )
			break;
	}

	int* pExpanded = nullptr;

	const void* pData = rgba;

	//Convert to power of 2 texture.
	if( wide != pow2Wide || tall != pow2Tall )
	{
		pExpanded = new int[ pow2Wide * pow2Tall ];

		pData = pExpanded;

		memset( pExpanded, 0, 4 * pow2Wide * pow2Tall );

		const int* pSrc = reinterpret_cast<const int*>( rgba );
		int* pDest = pExpanded;

		for( int y = 0; y < tall; ++y )
		{
			for( int x = 0; x < wide; ++x )
			{
				pDest[ x ] = pSrc[ x ];
			}

			pDest += pow2Wide;
			pSrc += wide;
		}
	}

	pTexture->_s0 = 0;
	pTexture->_t0 = 0;
	pTexture->_s1 = static_cast<double>( wide ) / pow2Wide;
	pTexture->_t1 = static_cast<double>( tall ) / pow2Tall;

	staticTextureCurrent = pTexture;

	currenttexture = id;

	qglBindTexture( GL_TEXTURE_2D, id );

	if( hardwareFilter )
	{
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	int w, h, bpp;
	VideoMode_GetCurrentVideoMode( &w, &h, &bpp );

	if( hasAlphaChannel || bpp == 32 )
	{
		qglTexImage2D(
			GL_TEXTURE_2D,
			0, GL_RGBA8,
			pow2Wide, pow2Tall,
			0,
			GL_RGBA, GL_UNSIGNED_BYTE,
			pData
		);
	}
	else
	{
		qglTexImage2D(
			GL_TEXTURE_2D,
			0, GL_RGB5_A1,
			pow2Wide, pow2Tall,
			0,
			GL_RGBA, GL_UNSIGNED_BYTE,
			pData
		);
	}

		if( pExpanded )
			delete[] pExpanded;
}

void EngineSurface::drawSetTexture( int id )
{
	if( id != currenttexture )
	{
		drawFlushText();
		currenttexture = id;
	}

	staticTextureCurrent = staticGetTextureById( id );

	qglBindTexture( GL_TEXTURE_2D, id );
}

void EngineSurface::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	if( !staticTextureCurrent || _drawColor[ 3 ] == 255 )
		return;

	Rect_t rcOut;
	TCoordRect tRect;

	if( !ScissorRect_TCoords(
		x0, y0, x1, y1,
		staticTextureCurrent->_s0,
		staticTextureCurrent->_t0,
		staticTextureCurrent->_s1,
		staticTextureCurrent->_t1,
		&rcOut,
		&tRect )
	)
	{
		return;
	}

	qglEnable( GL_TEXTURE_2D );

	qglBindTexture( GL_TEXTURE_2D, staticTextureCurrent->_id );

	currenttexture = staticTextureCurrent->_id;

	qglColor4ub(
		_drawColor[ 0 ],
		_drawColor[ 1 ],
		_drawColor[ 2 ],
		255 - _drawColor[ 3 ]
	);

	qglBegin( GL_QUADS );

	qglTexCoord2f( tRect.s0, tRect.t0 );
	qglVertex2f( rcOut.x, rcOut.y );

	qglTexCoord2f( tRect.s1, tRect.t0 );
	qglVertex2f( rcOut.width, rcOut.y );

	qglTexCoord2f( tRect.s1, tRect.t1 );
	qglVertex2f( rcOut.width, rcOut.height );

	qglTexCoord2f( tRect.s0, tRect.t1 );
	qglVertex2f( rcOut.x, rcOut.height );

	qglEnd();
}

void EngineSurface::drawPrintChar( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 )
{
	Rect_t rcOut;
	TCoordRect tRect;

	if( _drawTextColor[ 3 ] == 255 ||
		!ScissorRect_TCoords(
			x, y, 
			x + wide, y + tall,
			s0, t0, s1, t1,
			&rcOut, &tRect )
	)
		return;

	if( g_iVertexBufferEntriesUsed >= ARRAYSIZE( g_VertexBuffer ) )
	{
		drawFlushText();
		g_iVertexBufferEntriesUsed = 0;
	}

	auto& vertex1 = g_VertexBuffer[ g_iVertexBufferEntriesUsed++ ];

	vertex1.texcoords[ 0 ] = tRect.s0;
	vertex1.texcoords[ 1 ] = tRect.t0;
	vertex1.vertex[ 0 ] = rcOut.x;
	vertex1.vertex[ 1 ] = rcOut.y;

	auto& vertex2 = g_VertexBuffer[ g_iVertexBufferEntriesUsed++ ];

	vertex2.texcoords[ 0 ] = tRect.s1;
	vertex2.texcoords[ 1 ] = tRect.t0;
	vertex2.vertex[ 0 ] = rcOut.width;
	vertex2.vertex[ 1 ] = rcOut.y;

	auto& vertex3 = g_VertexBuffer[ g_iVertexBufferEntriesUsed++ ];

	vertex3.texcoords[ 0 ] = tRect.s1;
	vertex3.texcoords[ 1 ] = tRect.t1;
	vertex3.vertex[ 0 ] = rcOut.width;
	vertex3.vertex[ 1 ] = rcOut.height;

	auto& vertex4 = g_VertexBuffer[ g_iVertexBufferEntriesUsed++ ];

	vertex4.texcoords[ 0 ] = tRect.s0;
	vertex4.texcoords[ 1 ] = tRect.t1;
	vertex4.vertex[ 0 ] = rcOut.x;
	vertex4.vertex[ 1 ] = rcOut.height;
}

void EngineSurface::drawPrintCharAdd( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 )
{
	Rect_t rcOut;
	TCoordRect tRect;

	if( _drawTextColor[ 3 ] == 255 ||
		!ScissorRect_TCoords(
			x, y,
			x + wide, y + tall,
			s0, t0, s1, t1,
			&rcOut, &tRect )
		)
		return;

	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	qglBlendFunc( GL_EQUAL, GL_ONE );
	qglEnable( GL_BLEND );
	qglEnable( GL_ALPHA_TEST );

	qglColor4ub(
		_drawTextColor[ 0 ],
		_drawTextColor[ 1 ],
		_drawTextColor[ 2 ],
		255 - _drawTextColor[ 3 ]
	);

	qglBegin( GL_QUADS );

	qglTexCoord2f( tRect.s0, tRect.t0 );
	qglVertex2f( rcOut.x, rcOut.y );

	qglTexCoord2f( tRect.s1, tRect.t0 );
	qglVertex2f( rcOut.width, rcOut.y );

	qglTexCoord2f( tRect.s1, tRect.t1 );
	qglVertex2f( rcOut.width, rcOut.height );

	qglTexCoord2f( tRect.s0, tRect.t1 );
	qglVertex2f( rcOut.x, rcOut.height );

	qglEnd();

	qglDisable( GL_ALPHA_TEST );
	qglDisable( GL_BLEND );
}

void EngineSurface::drawSetTextureFile( int id, const char* filename, int hardwareFilter, bool forceReload )
{
	auto pTexture = staticGetTextureById( id );

	if( !pTexture || forceReload )
	{
		char name[ 512 ];

		byte buf[ 512 * 512 ];

		int width, height;

		snprintf( name, ARRAYSIZE( name ), "%s.tga", filename );

		bool bLoaded = false;

		if( LoadTGA( name, buf, sizeof( buf ), &width, &height ) )
		{
			bLoaded = true;
		}
		else
		{
			snprintf( name, ARRAYSIZE( name ), "%s.bmp", filename );

			FileHandle_t hFile = FS_Open( name, "rb" );

			if( hFile != FILESYSTEM_INVALID_HANDLE )
			{
				VGui_LoadBMP( hFile, buf, sizeof( buf ), &width, &height );
				//TODO: check result of call - Solokiller
				bLoaded = true;
			}
		}

		if( bLoaded )
		{
			drawSetTextureRGBA( id, buf, width, height, hardwareFilter, false );

			pTexture = staticGetTextureById( id );
		}

		if( fs_perf_warnings.value && ( !IsPowerOfTwo( width ) || !IsPowerOfTwo( height ) ) )
		{
			Con_DPrintf( "fs_perf_warnings: Resampling non-power-of-2 image '%s' (%dx%d)\n", filename, width, height );
		}
	}

	if( pTexture )
		drawSetTexture( id );
}

void EngineSurface::drawGetTextureSize( int id, int& wide, int& tall )
{
	auto pTexture = staticGetTextureById( id );

	if( pTexture )
	{
		wide = pTexture->_wide;
		tall = pTexture->_tall;
	}
	else
	{
		wide = 0;
		tall = 0;
	}
}

int EngineSurface::isTextureIdValid( int id )
{
	return staticGetTextureById( id ) != nullptr;
}

void EngineSurface::drawSetSubTextureRGBA( int textureID, int drawX, int drawY, const byte* rgba, int subTextureWide, int subTextureTall )
{
	qglTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		drawX, drawY,
		subTextureWide, subTextureTall,
		GL_RGBA, GL_UNSIGNED_BYTE,
		rgba
	);
}

void EngineSurface::drawFlushText()
{
	if( g_iVertexBufferEntriesUsed )
	{
		qglEnableClientState( GL_VERTEX_ARRAY );
		qglEnableClientState( GL_TEXTURE_COORD_ARRAY );

		qglEnable( GL_TEXTURE_2D );
		qglEnable( GL_BLEND );

		qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		qglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		qglColor4ub(
			_drawTextColor[ 0 ],
			_drawTextColor[ 1 ],
			_drawTextColor[ 2 ],
			255 - _drawTextColor[ 3 ]
			);

		qglTexCoordPointer( 2, GL_FLOAT, sizeof( float ) * 4, g_VertexBuffer[ 0 ].texcoords );
		qglVertexPointer( 2, GL_FLOAT, sizeof( float ) * 4, g_VertexBuffer[ 0 ].vertex );

		qglDrawArrays( GL_QUADS, 0, g_iVertexBufferEntriesUsed );

		qglDisable( GL_BLEND );

		g_iVertexBufferEntriesUsed = 0;
	}
}

void EngineSurface::resetViewPort()
{
	//Nothing
}

void EngineSurface::drawSetTextureBGRA( int id, const byte* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel )
{
	auto pTexture = staticGetTextureById( id );

	if( !pTexture )
	{
		Texture newTexture;

		memset( &newTexture, 0, sizeof( newTexture ) );

		newTexture._id = id;

		pTexture = &g_VGuiSurfaceTextures[ g_VGuiSurfaceTextures.Insert( newTexture ) ];
	}

	if( !pTexture )
		return;

	pTexture->_id = id;
	pTexture->_wide = wide;
	pTexture->_tall = tall;

	int pow2Wide;
	int pow2Tall;

	for( int i = 0; i < 32; ++i )
	{
		pow2Wide = 1 << i;

		if( wide <= pow2Wide )
			break;
	}

	for( int i = 0; i < 32; ++i )
	{
		pow2Tall = 1 << i;

		if( tall <= pow2Tall )
			break;
	}

	int* pExpanded = nullptr;

	const void* pData = rgba;

	//Convert to power of 2 texture.
	if( wide != pow2Wide || tall != pow2Tall )
	{
		pExpanded = new int[ pow2Wide * pow2Tall ];

		pData = pExpanded;

		memset( pExpanded, 0, 4 * pow2Wide * pow2Tall );

		const int* pSrc = reinterpret_cast<const int*>( rgba );
		int* pDest = pExpanded;

		for( int y = 0; y < tall; ++y )
		{
			for( int x = 0; x < wide; ++x )
			{
				pDest[ x ] = pSrc[ x ];
			}

			pDest += pow2Wide;
			pSrc += wide;
		}
	}

	pTexture->_s0 = 0;
	pTexture->_t0 = 0;
	pTexture->_s1 = static_cast<double>( wide ) / pow2Wide;
	pTexture->_t1 = static_cast<double>( tall ) / pow2Tall;

	staticTextureCurrent = pTexture;

	currenttexture = id;

	qglBindTexture( GL_TEXTURE_2D, id );

	if( hardwareFilter )
	{
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	qglTexImage2D(
		GL_TEXTURE_2D,
		0, GL_RGBA8,
		pow2Wide, pow2Tall,
		0,
		GL_BGRA, GL_UNSIGNED_BYTE,
		pData
	);

	if( pExpanded )
		delete[] pExpanded;
}

void EngineSurface::drawUpdateRegionTextureBGRA( int nTextureID, int x, int y, const byte* pchData, int wide, int tall )
{
	auto pTexture = staticGetTextureById( nTextureID );

	qglBindTexture( GL_TEXTURE_2D, nTextureID );

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	qglPixelStorei( GL_UNPACK_ROW_LENGTH, pTexture->_wide );

	qglTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0, y,
		pTexture->_wide, tall,
		GL_BGRA, GL_UNSIGNED_BYTE,
		&pchData[ 4 * y * pTexture->_wide ]
	);

	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
}

void VGui_ViewportPaintBackground( int* extents )
{
	//TODO: implement - Solokiller
}