#include "quakedef.h"
#include "r_triangle.h"

triangleapi_t tri =
{
	TRI_API_VERSION,
	&tri_GL_RenderMode,
	&tri_GL_Begin,
	&tri_GL_End,
	&tri_GL_Color4f,
	&tri_GL_Color4ub,
	&tri_GL_TexCoord2f,
	&tri_GL_Vertex3fv,
	&tri_GL_Vertex3f,
	&tri_GL_Brightness,
	&tri_GL_CullFace,
	&R_TriangleSpriteTexture,
	&tri_ScreenTransform,
	&R_RenderFog,
	&tri_WorldTransform,
	&tri_GetMatrix,
	&tri_BoxinPVS,
	&tri_LightAtPoint,
	&tri_GL_Color4fRendermode,
	&R_FogParams
};

void tri_GL_RenderMode( int mode )
{
	//TODO: implement - Solokiller
}

void tri_GL_Begin( int primitiveCode )
{
	//TODO: implement - Solokiller
}

void tri_GL_End()
{
	//TODO: implement - Solokiller
}

void tri_GL_Color4f( float x, float y, float z, float w )
{
	//TODO: implement - Solokiller
}

void tri_GL_Color4ub( byte r, byte g, byte b, byte a )
{
	//TODO: implement - Solokiller
}

void tri_GL_TexCoord2f( float u, float v )
{
	//TODO: implement - Solokiller
}

void tri_GL_Vertex3fv( float* worldPnt )
{
	//TODO: implement - Solokiller
}

void tri_GL_Vertex3f( float x, float y, float z )
{
	//TODO: implement - Solokiller
}

void tri_GL_Brightness( float x )
{
	//TODO: implement - Solokiller
}

void tri_GL_CullFace( TRICULLSTYLE style )
{
	//TODO: implement - Solokiller
}

int R_TriangleSpriteTexture( model_t *pSpriteModel, int frame )
{
	//TODO: implement - Solokiller
	return false;
}

int tri_ScreenTransform( float* world, float* screen )
{
	//TODO: implement - Solokiller
	return false;
}

void R_RenderFog( float* flFogColor, float flStart, float flEnd, int bOn )
{
	//TODO: implement - Solokiller
}

void tri_WorldTransform( float* screen, float* world )
{
	//TODO: implement - Solokiller
}

void tri_GetMatrix( const int pname, float* matrix )
{
	//TODO: implement - Solokiller
}

int tri_BoxinPVS( float* mins, float* maxs )
{
	//TODO: implement - Solokiller
	return false;
}

void tri_LightAtPoint( float* vPos, float* value )
{
	//TODO: implement - Solokiller
}

void tri_GL_Color4fRendermode( float x, float y, float z, float w, int rendermode )
{
	//TODO: implement - Solokiller
}

void R_FogParams( float flDensity, int iFogSkybox )
{
	//TODO: implement - Solokiller
}
