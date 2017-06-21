#ifndef ENGINE_R_PART_H
#define ENGINE_R_PART_H

#include "r_efx.h"

particle_t* R_AllocParticle( void( *callback )( particle_t*, float ) );

void R_BlobExplosion( vec_t* org );

void R_Blood( vec_t* org, vec_t* dir, int pcolor, int speed );

void R_BloodStream( vec_t* org, vec_t* dir, int pcolor, int speed );

void R_BulletImpactParticles( vec_t* pos );

void R_EntityParticles( cl_entity_t* ent );

void R_FlickerParticles( vec_t* org );

void R_Implosion( vec_t* end, float radius, int count, float life );

void R_LargeFunnel( vec_t* org, int reverse );

void R_LavaSplash( vec_t* org );

void R_ParticleBurst( vec_t* pos, int size, int color, float life );

void R_ParticleExplosion( vec_t* org );

void R_ParticleExplosion2( vec_t* org, int colorStart, int colorLength );

void R_RocketTrail( vec_t* start, vec_t* end, int type );

void R_RunParticleEffect( vec_t* org, vec_t* dir, int color, int count );

void R_ShowLine( vec_t* start, vec_t* end );

void R_SparkStreaks( vec_t* pos, int count, int velocityMin, int velocityMax );

void R_StreakSplash( vec_t* pos, vec_t* dir, int color, int count, 
					 float speed, int velocityMin, int velocityMax );

void R_UserTracerParticle( vec_t* org, vec_t* vel, 
						   float life, int colorIndex, float length, 
						   byte context, void( *deathfunc )( particle_t* ) );

particle_t* R_TracerParticles( vec_t* org, vec_t* vel, float life );

void R_TeleportSplash( vec_t* org );

BEAM* R_BeamCirclePoints( int type, vec_t* start, vec_t* end,
						  int modelIndex, float life, float width, float amplitude,
						  float brightness, float speed, int startFrame, float framerate,
						  float r, float g, float b );

BEAM* R_BeamEntPoint( int startEnt, vec_t* end,
					  int modelIndex, float life, float width, float amplitude,
					  float brightness, float speed, int startFrame, float framerate,
					  float r, float g, float b );

BEAM* R_BeamEnts( int startEnt, int endEnt,
				  int modelIndex, float life, float width, float amplitude,
				  float brightness, float speed, int startFrame, float framerate,
				  float r, float g, float b );

BEAM* R_BeamFollow( int startEnt,
					int modelIndex, float life, float width,
					float r, float g, float b,
					float brightness );

void R_BeamKill( int deadEntity );

BEAM* R_BeamLightning( vec_t* start, vec_t* end,
					   int modelIndex, float life, float width, float amplitude,
					   float brightness, float speed );

BEAM* R_BeamPoints( vec_t* start, vec_t* end,
					int modelIndex, float life, float width, float amplitude,
					float brightness, float speed, int startFrame, float framerate,
					float r, float g, float b );

BEAM* R_BeamRing( int startEnt, int endEnt,
				  int modelIndex, float life, float width, float amplitude,
				  float brightness, float speed, int startFrame, float framerate,
				  float r, float g, float b );

void R_GetPackedColor( short* packed, short color );

short R_LookupColor( byte r, byte g, byte b );

#endif //ENGINE_R_PART_H
