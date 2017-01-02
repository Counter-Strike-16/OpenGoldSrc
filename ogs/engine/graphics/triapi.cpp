/// @file

#include "system/precompiled.h"
#include "common/triangleapi.h"

namespace
{
#ifndef
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