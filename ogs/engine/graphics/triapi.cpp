/// @file

//#include "precompiled.hpp"
#include "common/triangleapi.h"

/*
=================

TriApi implementation

=================
*/

namespace
{
#ifndef OGS_TRIAPI_NULL_IMPL

/*
=============
TriRenderMode

set rendermode
=============
*/
void TriAPI_SetRenderMode(int mode)
{
	switch( mode )
	{
	case kRenderNormal:
	default:
		pglDisable( GL_BLEND );
		pglDisable( GL_ALPHA_TEST );
		pglTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		break;
	case kRenderTransColor:
	case kRenderTransAlpha:
	case kRenderTransTexture:
		// NOTE: TriAPI doesn't have 'solid' mode
		pglEnable( GL_BLEND );
		pglDisable( GL_ALPHA_TEST );
		pglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		pglTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		break;
	case kRenderGlow:
	case kRenderTransAdd:
		pglEnable( GL_BLEND );
		pglDisable( GL_ALPHA_TEST );
		pglBlendFunc( GL_SRC_ALPHA, GL_ONE );
		pglTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		break;
	}
};

/*
=============
TriBegin

begin triangle sequence
=============
*/
void TriAPI_Begin(int primitiveCode)
{
	gpRender->BeginRendering();
};

/*
=============
TriEnd

end triangle sequence
=============
*/
void TriAPI_End()
{
	gpRender->EndRendering();
};

void TriAPI_Color4f(float r, float g, float b, float a)
{
	clgame.ds.triColor[0] = (byte)bound( 0, (r * 255.0f), 255 );
	clgame.ds.triColor[1] = (byte)bound( 0, (g * 255.0f), 255 );
	clgame.ds.triColor[2] = (byte)bound( 0, (b * 255.0f), 255 );
	clgame.ds.triColor[3] = (byte)bound( 0, (a * 255.0f), 255 );
	pglColor4ub( clgame.ds.triColor[0], clgame.ds.triColor[1], clgame.ds.triColor[2], clgame.ds.triColor[3] );
};

void TriAPI_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	clgame.ds.triColor[0] = r;
	clgame.ds.triColor[1] = g;
	clgame.ds.triColor[2] = b;
	clgame.ds.triColor[3] = a;
	pglColor4ub( r, g, b, a );
};

void TriAPI_TexCoord2f(float u, float v)
{
	pglTexCoord2f( u, v );
};

void TriAPI_Vertex3fv(float *worldPnt)
{
	pglVertex3fv(worldPnt);
};

void TriAPI_Vertex3f(float x, float y, float z)
{
	pglVertex3f( x, y, z );
};

void TriAPI_Brightness(float brightness)
{
	rgba_t	rgba;
	
	brightness = max( 0.0f, brightness );
	
	rgba[0] = clgame.ds.triColor[0] * brightness;
	rgba[1] = clgame.ds.triColor[1] * brightness;
	rgba[2] = clgame.ds.triColor[2] * brightness;
	
	pglColor3ub( rgba[0], rgba[1], rgba[2] );
};

void TriAPI_CullFace(TRICULLSTYLE style)
{
	switch( mode )
	{
	case TRI_FRONT:
		clgame.ds.cullMode = GL_FRONT;
		break;
	default:
		clgame.ds.cullMode = GL_NONE;
		break;
	};
	
	GL_Cull( clgame.ds.cullMode );
};

/*
=============
TriSpriteTexture

bind current texture
=============
*/
int TriAPI_SpriteTexture(struct model_s *pSpriteModel, int frame)
{
	int	gl_texturenum;
	msprite_t	*psprite;

	if(( gl_texturenum = R_GetSpriteTexture( pSpriteModel, frame )) == 0 )
		return 0;

	if( gl_texturenum <= 0 || gl_texturenum > MAX_TEXTURES )
	{
		MsgDev( D_ERROR, "TriSpriteTexture: bad index %i\n", gl_texturenum );
		gl_texturenum = tr.defaultTexture;
	}

	psprite = pSpriteModel->cache.data;
	if( psprite->texFormat == SPR_ALPHTEST )
	{
		pglEnable( GL_ALPHA_TEST );
		pglAlphaFunc( GL_GREATER, 0.0f );
	}

	GL_Bind( GL_TEXTURE0, gl_texturenum );
	return 1;
};

/*
=============
TriWorldToScreen

convert world coordinates (x,y,z) into screen (x, y)
=============
*/
int TriAPI_WorldToScreen(float *world, float *screen)
{
	int retval = R_WorldToScreen( world, screen );

	screen[0] =  0.5f * screen[0] * (float)cl.refdef.viewport[2];
	screen[1] = -0.5f * screen[1] * (float)cl.refdef.viewport[3];
	screen[0] += 0.5f * (float)cl.refdef.viewport[2];
	screen[1] += 0.5f * (float)cl.refdef.viewport[3];

	return retval;
};

/*
=============
TriFog

enables global fog on the level
=============
*/
void TriAPI_Fog(float flFogColor[3], float flStart, float flEnd, int bOn)
{
	if( RI.fogEnabled )
		return;
	
	RI.fogCustom = true;
	
	if( !bOn )
	{
		pglDisable( GL_FOG );
		RI.fogCustom = false;
		return;
	}
	
	// copy fog params
	RI.fogColor[0] = flFogColor[0] / 255.0f;
	RI.fogColor[1] = flFogColor[1] / 255.0f;
	RI.fogColor[2] = flFogColor[2] / 255.0f;
	RI.fogStart = flStart;
	RI.fogDensity = 0.0f;
	RI.fogEnd = flEnd;
	
	if( VectorIsNull( RI.fogColor ))
	{
		pglDisable( GL_FOG );
		return;	
	}
	
	pglEnable( GL_FOG );
	pglFogi( GL_FOG_MODE, GL_LINEAR );
	pglFogf( GL_FOG_START, RI.fogStart );
	pglFogf( GL_FOG_END, RI.fogEnd );
	pglFogfv( GL_FOG_COLOR, RI.fogColor );
	pglHint( GL_FOG_HINT, GL_NICEST );
};

void TriAPI_ScreenToWorld(float *screen, float *world)
{
};

void TriAPI_GetMatrix(const int pname, float *matrix)
{
	pglGetFloatv( pname, matrix );
};

/*
=============
TriBoxInPVS

check for box presense in pvs (absmin, absmax)
=============
*/
int TriAPI_BoxInPVS(float *mins, float *maxs)
{
	return Mod_BoxVisible( mins, maxs, Mod_GetCurrentVis( ));
};

/*
=============
TriLightAtPoint

NOTE: dlights are ignored
=============
*/
void TriAPI_LightAtPoint(float *pos, float *value)
{
	color24	ambient;

	if( !pos || !value )
		return;

	R_LightForPoint( pos, &ambient, false, false, 0.0f );

	value[0] = (float)ambient.r * 255.0f;
	value[1] = (float)ambient.g * 255.0f;
	value[2] = (float)ambient.b * 255.0f;
};

/*
=============
TriColor4fRendermode

Heavy legacy of Quake...
=============
*/
void TriAPI_Color4fRendermode(float r, float g, float b, float a, int rendermode)
{
	if( rendermode == kRenderTransAlpha )
		pglColor4f( r, g, b, a );
	else
		pglColor4f( r * a, g * a, b * a, 1.0f );
};

void TriAPI_FogParams(float flDensity, int iFogSkybox)
{
	RI.fogDensity = flDensity;
	RI.fogCustom = iFogSkybox;
};
#else // if OGS_TRIAPI_NULL_IMPL defined
void TriAPI_RenderMode(int mode){};

void TriAPI_Begin(int primitiveCode){};

void TriAPI_End(){};

void TriAPI_Color4f(float r, float g, float b, float a){};

void TriAPI_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a){};

void TriAPI_TexCoord2f(float u, float v){};

void TriAPI_Vertex3fv(float *worldPnt){};

void TriAPI_Vertex3f(float x, float y, float z){};

void TriAPI_Brightness(float brightness){};

void TriAPI_CullFace(TRICULLSTYLE style){};

int TriAPI_SpriteTexture(struct model_s *pSpriteModel, int frame)
{
	return 0;
};

int TriAPI_WorldToScreen(float *world, float *screen)
{
	return 0;
};

void TriAPI_Fog(float flFogColor[3], float flStart, float flEnd, int bOn){};

void TriAPI_ScreenToWorld(float *screen, float *world){};

void TriAPI_GetMatrix(const int pname, float *matrix){};

int TriAPI_BoxInPVS(float *mins, float *maxs)
{
	return 0;
};

void TriAPI_LightAtPoint(float *pos, float *value){};

void TriAPI_Color4fRendermode(float r, float g, float b, float a, int rendermode){};

void TriAPI_FogParams(float flDensity, int iFogSkybox){};
#endif

}; // namespace

triangleapi_t gTriAPI =
    {
        TRI_API_VERSION,

        TriAPI_RenderMode,
        TriAPI_Begin,
        TriAPI_End,

        TriAPI_Color4f,
        TriAPI_Color4ub,

        TriAPI_TexCoord2f,

        TriAPI_Vertex3fv,
        TriAPI_Vertex3f,

        TriAPI_Brightness,
        TriAPI_CullFace,
        TriAPI_SpriteTexture,
        TriAPI_WorldToScreen,
        TriAPI_Fog,
        TriAPI_ScreenToWorld,
        TriAPI_GetMatrix,
        TriAPI_BoxInPVS,
        TriAPI_LightAtPoint,
        TriAPI_Color4fRendermode,
        TriAPI_FogParams};