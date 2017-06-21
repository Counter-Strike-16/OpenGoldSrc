#include "quakedef.h"
#include "r_part.h"

particle_t* R_AllocParticle( void( *callback )( particle_t*, float ) )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void R_BlobExplosion( vec_t* org )
{
	//TODO: implement - Solokiller
}

void R_Blood( vec_t* org, vec_t* dir, int pcolor, int speed )
{
	//TODO: implement - Solokiller
}

void R_BloodStream( vec_t* org, vec_t* dir, int pcolor, int speed )
{
	//TODO: implement - Solokiller
}

void R_BulletImpactParticles( vec_t* pos )
{
	//TODO: implement - Solokiller
}

void R_EntityParticles( cl_entity_t* ent )
{
	//TODO: implement - Solokiller
}

void R_FlickerParticles( vec_t* org )
{
	//TODO: implement - Solokiller
}

void R_Implosion( vec_t* end, float radius, int count, float life )
{
	//TODO: implement - Solokiller
}

void R_LargeFunnel( vec_t* org, int reverse )
{
	//TODO: implement - Solokiller
}

void R_LavaSplash( vec_t* org )
{
	//TODO: implement - Solokiller
}

void R_ParticleBurst( vec_t* pos, int size, int color, float life )
{
	//TODO: implement - Solokiller
}

void R_ParticleExplosion( vec_t* org )
{
	//TODO: implement - Solokiller
}

void R_ParticleExplosion2( vec_t* org, int colorStart, int colorLength )
{
	//TODO: implement - Solokiller
}

void R_RocketTrail( vec_t* start, vec_t* end, int type )
{
	//TODO: implement - Solokiller
}

void R_RunParticleEffect( vec_t* org, vec_t* dir, int color, int count )
{
	//TODO: implement - Solokiller
}

void R_ShowLine( vec_t* start, vec_t* end )
{
	//TODO: implement - Solokiller
}

void R_SparkStreaks( vec_t* pos, int count, int velocityMin, int velocityMax )
{
	//TODO: implement - Solokiller
}

void R_StreakSplash( vec_t* pos, vec_t* dir, int color, int count,
					 float speed, int velocityMin, int velocityMax )
{
	//TODO: implement - Solokiller
}

void R_UserTracerParticle( vec_t* org, vec_t* vel,
						   float life, int colorIndex, float length,
						   byte context, void( *deathfunc )( particle_t* ) )
{
	//TODO: implement - Solokiller
}

particle_t* R_TracerParticles( vec_t* org, vec_t* vel, float life )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void R_TeleportSplash( vec_t* org )
{
	//TODO: implement - Solokiller
}

BEAM* R_BeamCirclePoints( int type, vec_t* start, vec_t* end,
						  int modelIndex, float life, float width, float amplitude,
						  float brightness, float speed, int startFrame, float framerate,
						  float r, float g, float b )
{
	//TODO: implement - Solokiller
	return nullptr;
}

BEAM* R_BeamEntPoint( int startEnt, vec_t* end,
					  int modelIndex, float life, float width, float amplitude,
					  float brightness, float speed, int startFrame, float framerate,
					  float r, float g, float b )
{
	//TODO: implement - Solokiller
	return nullptr;
}

BEAM* R_BeamEnts( int startEnt, int endEnt,
				  int modelIndex, float life, float width, float amplitude,
				  float brightness, float speed, int startFrame, float framerate,
				  float r, float g, float b )
{
	//TODO: implement - Solokiller
	return nullptr;
}

BEAM* R_BeamFollow( int startEnt,
					int modelIndex, float life, float width,
					float r, float g, float b,
					float brightness )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void R_BeamKill( int deadEntity )
{
	//TODO: implement - Solokiller
}

BEAM* R_BeamLightning( vec_t* start, vec_t* end,
					   int modelIndex, float life, float width, float amplitude,
					   float brightness, float speed )
{
	//TODO/ implement - Solokiller
	return nullptr;
}

BEAM* R_BeamPoints( vec_t* start, vec_t* end,
					int modelIndex, float life, float width, float amplitude,
					float brightness, float speed, int startFrame, float framerate,
					float r, float g, float b )
{
	//TODO: implement - Solokiller
	return nullptr;
}

BEAM* R_BeamRing( int startEnt, int endEnt,
				  int modelIndex, float life, float width, float amplitude,
				  float brightness, float speed, int startFrame, float framerate,
				  float r, float g, float b )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void R_GetPackedColor( short* packed, short color )
{
	//TODO: implement - Solokiller
}

short R_LookupColor( byte r, byte g, byte b )
{
	//TODO: implement - Solokiller
	return 0;
}
