#include "quakedef.h"
#include "cdll_int.h"
#include "decals.h"
#include "gl_draw.h"
#include "gl_rmain.h"
#include "gl_screen.h"
#include "gl_vidnt.h"
#include "qgl.h"
#include "wad.h"

#include "vgui2/text_draw.h"

cvar_t gl_ansio = { "gl_ansio", "16" };

qpic_t* draw_disc = nullptr;

void Draw_Init()
{
	m_bDrawInitialized = true;
	VGUI2_Draw_Init();

	//TODO: implement - Solokiller

	Cvar_RegisterVariable( &gl_ansio );

	if( Host_GetVideoLevel() > 0 )
	{
		Cvar_DirectSet( &gl_ansio, "4" );
		qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ansio.value );
	}
	//TODO: implement - Solokiller
}

void Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

int Draw_Character( int x, int y, int num, unsigned int font )
{
	return VGUI2_Draw_Character( x, y, num, font );
}

int Draw_MessageCharacterAdd( int x, int y, int num, int rr, int gg, int bb, unsigned int font )
{
	return VGUI2_Draw_CharacterAdd( x, y, num, rr, gg, bb, font );
}

int Draw_String( int x, int y, char* str )
{
	const auto iWidth = VGUI2_DrawString( x, y, str, VGUI2_GetConsoleFont() );

	Draw_ResetTextColor();

	return iWidth + x;
}

int Draw_StringLen( const char* psz, unsigned int font )
{
	return VGUI2_Draw_StringLen( psz, font );
}

void Draw_SetTextColor( float r, float g, float b )
{
	g_engdstAddrs.pfnDrawSetTextColor( &r, &g, &b );

	VGUI2_Draw_SetTextColor(
		static_cast<int>( r * 255.0 ),
		static_cast<int>( g * 255.0 ),
		static_cast<int>( b * 255.0 )
	);
}

void Draw_GetDefaultColor()
{
	int r, g, b;

	if( sscanf( con_color.string, "%i %i %i", &r, &g, &b ) == 3 )
		VGUI2_Draw_SetTextColor( r, g, b );
}

void Draw_ResetTextColor()
{
	Draw_GetDefaultColor();
}

void Draw_FillRGBABlend( int x, int y, int w, int h, int r, int g, int b, int a )
{
	//TODO: implement - Solokiller
}

GLuint GL_GenTexture()
{
	GLuint tex;

	qglGenTextures( 1, &tex );

	return tex;
}

GLenum oldtarget = TEXTURE0_SGIS;

void GL_SelectTexture( GLenum target )
{
	if( !gl_mtexable )
		return;

	qglSelectTextureSGIS( target );

	if( target == oldtarget )
		return;

	cnttextures[ oldtarget - TEXTURE0_SGIS ] = currenttexture;
	currenttexture = cnttextures[ target - TEXTURE0_SGIS ];
	oldtarget = target;
}

void Draw_Pic( int x, int y, qpic_t* pic )
{
	//TODO: implement - Solokiller
}

void Draw_BeginDisc()
{
	if( draw_disc )
		Draw_CenterPic( draw_disc );
}