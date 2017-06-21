#ifndef ENGINE_CL_TENT_H
#define ENGINE_CL_TENT_H

#include "pmtrace.h"
#include "r_efx.h"

void CL_TempEntInit();

TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model );

TEMPENTITY* CL_TempEntAllocNoModel( vec_t* org );

TEMPENTITY* CL_TempEntAllocHigh( vec_t* org, model_t* model );

TEMPENTITY* CL_AllocCustomTempEntity( float* origin, model_t* model, int high, void ( *callback )( TEMPENTITY*, float, float ) );

void R_BloodSprite( vec_t* org, int colorindex, int modelIndex, int modelIndex2, float size );

void R_BreakModel( float* pos, float* size, float* dir, float random, float life, int count, int modelIndex, char flags );

void R_Bubbles( vec_t* mins, vec_t* maxs, float height, int modelIndex, int count, float speed );

void R_BubbleTrail( vec_t* start, vec_t* end, float height, int modelIndex, int count, float speed );

void R_Explosion( float* pos, int model, float scale, float framerate, int flags );

void R_FizzEffect( cl_entity_t* pent, int modelIndex, int density );

void R_FireField( vec_t* org, int radius, int modelIndex, int count, int flags, float life );

void R_FunnelSprite( float* org, int modelIndex, int reverse );

void R_MultiGunshot( vec_t* org, vec_t* dir, vec_t* noise, int count, int decalCount, int* decalIndices );

void R_MuzzleFlash( float* pos1, int type );

void R_ParticleBox( float* mins, float* maxs, byte r, byte g, byte b, float life );

void R_ParticleLine( float* start, float* end, byte r, byte g, byte b, float life );

void R_PlayerSprites( int client, int modelIndex, int count, int size );

void R_Projectile( vec_t* origin, vec_t* velocity, int modelIndex, int life, int owner, void( *hitcallback )( TEMPENTITY*, pmtrace_t* ) );

void R_RicochetSound( vec_t* pos );

void R_RicochetSprite( float* pos, model_t* pmodel, float duration, float scale );

void R_RocketFlare( float* pos );

void R_SparkEffect( float* pos, int count, int velocityMin, int velocityMax );

void R_SparkShower( float* pos );

void R_Spray( vec_t* pos, vec_t* dir, int modelIndex, int count, int speed, int spread, int rendermode );

void R_Sprite_Explode( TEMPENTITY* pTemp, float scale, int flags );

void R_Sprite_Smoke( TEMPENTITY* pTemp, float scale );

void R_Sprite_Spray( vec_t* pos, vec_t* dir, int modelIndex, int count, int speed, int iRand );

void R_Sprite_Trail( int type, vec_t* start, vec_t* end, 
					 int modelIndex, int count, float life, float size, 
					 float amplitude, int renderamt, float speed );

void R_Sprite_WallPuff( TEMPENTITY* pTemp, float scale );

void R_TracerEffect( vec_t* start, vec_t* end );

void R_TempSphereModel( float* pos, float speed, float life, int count, int modelIndex );

TEMPENTITY* R_TempModel( float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype );

TEMPENTITY* R_DefaultSprite( float* pos, int spriteIndex, float framerate );

TEMPENTITY* R_TempSprite( float* pos, float* dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags );

void R_AttachTentToPlayer( int client, int modelIndex, float zoffset, float life );

void R_KillAttachedTents( int client );

#endif //ENGINE_CL_TENT_H
