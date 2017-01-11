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

void TriAPI_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a){};

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

int TriAPI_SpriteTexture(struct model_s *pSpriteModel, int frame)
{
	return 0;
};

int TriAPI_WorldToScreen(float *world, float *screen)
{
	return 0;
};

void TriAPI_Fog(float flFogColor[3], float flStart, float flEnd, int bOn)
{
};

void TriAPI_ScreenToWorld(float *screen, float *world)
{
};

void TriAPI_GetMatrix(const int pname, float *matrix)
{
};

int TriAPI_BoxInPVS(float *mins, float *maxs)
{
	return 0;
};

void TriAPI_LightAtPoint(float *pos, float *value){};

void TriAPI_Color4fRendermode(float r, float g, float b, float a, int rendermode){};

void TriAPI_FogParams(float flDensity, int iFogSkybox){};
#else
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