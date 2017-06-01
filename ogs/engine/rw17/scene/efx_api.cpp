/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file
/// @brief client-side effects api impl

#include "precompiled.hpp"

#define OGS_EFX_API_NULL_IMPL

namespace
{

#ifndef OGS_EFX_API_NULL_IMPL

particle_t *R_AllocParticle(void (*callback)(struct particle_s *particle, float frametime))
{
	return nullptr;
};

void R_BlobExplosion(float *org)
{
};

void R_Blood(float *org, float *dir, int pcolor, int speed)
{
};

/*
==============
CL_BloodSprite

Create a high priority blood sprite
and some blood drops. This is high-priority tent
==============
*/
void R_BloodSprite(float *org, int colorindex, int modelIndex, int modelIndex2, float size)
{
	TEMPENTITY	*pTemp;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	// large, single blood sprite is a high-priority tent
	if(( pTemp = CL_TempEntAllocHigh( org, Mod_Handle( modelIndex ))) != NULL )
	{
		int	i, frameCount;
		vec3_t	offset, dir;
		vec3_t	forward, right, up;

		colorIndex = bound( 0, colorIndex, 256 );

		Mod_GetFrames( modelIndex, &frameCount );
		pTemp->entity.curstate.rendermode = kRenderTransTexture;
		pTemp->entity.curstate.renderfx = kRenderFxClampMinScale;
		pTemp->entity.curstate.scale = Com_RandomFloat(( size / 25.0f ), ( size / 35.0f ));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = clgame.palette[colorIndex][0];
		pTemp->entity.curstate.rendercolor.g = clgame.palette[colorIndex][1];
		pTemp->entity.curstate.rendercolor.b = clgame.palette[colorIndex][2];
		pTemp->entity.curstate.framerate = frameCount * 4; // Finish in 0.250 seconds
		// play the whole thing once
		pTemp->die = cl.time + (frameCount / pTemp->entity.curstate.framerate);

		pTemp->entity.angles[2] = Com_RandomLong( 0, 360 );
		pTemp->bounceFactor = 0;

		VectorSet( forward, 0.0f, 0.0f, 1.0f );	// up-vector
		VectorVectors( forward, right, up );

		Mod_GetFrames( modelIndex2, &frameCount );

		// create blood drops
		for( i = 0; i < 14; i++ )
		{
			// originate from within a circle 'scale' inches in diameter.
			VectorCopy( org, offset );
			VectorMA( offset, Com_RandomFloat( -0.5f, 0.5f ) * size, right, offset ); 
			VectorMA( offset, Com_RandomFloat( -0.5f, 0.5f ) * size, up, offset ); 

			pTemp = CL_TempEntAlloc( org, Mod_Handle( modelIndex2 ));
			if( !pTemp ) return;

			pTemp->flags = FTENT_COLLIDEWORLD|FTENT_SLOWGRAVITY;

			pTemp->entity.curstate.rendermode = kRenderTransTexture;
			pTemp->entity.curstate.renderfx = kRenderFxClampMinScale; 
			pTemp->entity.curstate.scale = Com_RandomFloat(( size / 25.0f), ( size / 35.0f ));
			pTemp->entity.curstate.rendercolor.r = clgame.palette[colorIndex][0];
			pTemp->entity.curstate.rendercolor.g = clgame.palette[colorIndex][1];
			pTemp->entity.curstate.rendercolor.b = clgame.palette[colorIndex][2];
			pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
			pTemp->die = cl.time + Com_RandomFloat( 1.0f, 3.0f );

			pTemp->entity.angles[2] = Com_RandomLong( 0, 360 );
			pTemp->bounceFactor = 0;

			dir[0] = forward[0] + Com_RandomFloat( -0.8f, 0.8f );
			dir[1] = forward[1] + Com_RandomFloat( -0.8f, 0.8f );
			dir[2] = forward[2];

			VectorScale( dir, Com_RandomFloat( 8.0f * size, 20.0f * size ), pTemp->entity.baseline.origin );
			pTemp->entity.baseline.origin[2] += Com_RandomFloat( 4.0f, 16.0f ) * size;
		}
	}
};

void R_BloodStream(float *org, float *dir, int pcolor, int speed)
{
};

/*
==============
CL_BreakModel

Create a shards
==============
*/
void R_BreakModel(float *pos, float *size, float *dir, float random, float life, int count, int modelIndex, char flags)
{
	int		i, frameCount;
	int		numtries = 0;
	TEMPENTITY	*pTemp;
	char		type;
	vec3_t		dir;

	if( !modelIndex ) return;
	type = flags & BREAK_TYPEMASK;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	Mod_GetFrames( modelIndex, &frameCount );
		
	if( count == 0 )
	{
		// assume surface (not volume)
		count = (size[0] * size[1] + size[1] * size[2] + size[2] * size[0]) / (3 * SHARD_VOLUME * SHARD_VOLUME);
	}

	VectorCopy( direction, dir );

	// limit to 100 pieces
	if( count > 100 ) count = 100;

	if( VectorIsNull( direction ))
		random *= 10;

	for( i = 0; i < count; i++ ) 
	{
		vec3_t	vecSpot;

		if( VectorIsNull( direction ))
		{
			// random direction for each piece
			dir[0] = Com_RandomFloat( -1.0f, 1.0f );
			dir[1] = Com_RandomFloat( -1.0f, 1.0f );
			dir[2] = Com_RandomFloat( -1.0f, 1.0f );

			VectorNormalize( dir );
		}
		numtries = 0;
tryagain:
		// fill up the box with stuff
		vecSpot[0] = pos[0] + Com_RandomFloat( -0.5f, 0.5f ) * size[0];
		vecSpot[1] = pos[1] + Com_RandomFloat( -0.5f, 0.5f ) * size[1];
		vecSpot[2] = pos[2] + Com_RandomFloat( -0.5f, 0.5f ) * size[2];

		if( CL_PointContents( vecSpot ) == CONTENTS_SOLID )
		{
			if( ++numtries < 32 )
				goto tryagain;
			continue;	// a piece completely stuck in the wall, ignore it
                    }

		pTemp = CL_TempEntAlloc( vecSpot, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		// keep track of break_type, so we know how to play sound on collision
		pTemp->hitSound = type;
		
		if( Mod_GetType( modelIndex ) == mod_sprite )
			pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		else if( Mod_GetType( modelIndex ) == mod_studio )
			pTemp->entity.curstate.body = Com_RandomLong( 0, frameCount - 1 );

		pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_FADEOUT | FTENT_SLOWGRAVITY;

		if ( Com_RandomLong( 0, 255 ) < 200 ) 
		{
			pTemp->flags |= FTENT_ROTATE;
			pTemp->entity.baseline.angles[0] = Com_RandomFloat( -256, 255 );
			pTemp->entity.baseline.angles[1] = Com_RandomFloat( -256, 255 );
			pTemp->entity.baseline.angles[2] = Com_RandomFloat( -256, 255 );
		}

		if (( Com_RandomLong( 0, 255 ) < 100 ) && ( flags & BREAK_SMOKE ))
			pTemp->flags |= FTENT_SMOKETRAIL;

		if (( type == BREAK_GLASS ) || ( flags & BREAK_TRANS ))
		{
			pTemp->entity.curstate.rendermode = kRenderTransTexture;
			pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 128;
		}
		else
		{
			pTemp->entity.curstate.rendermode = kRenderNormal;
			pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 255; // set this for fadeout
		}

		pTemp->entity.baseline.origin[0] = dir[0] + Com_RandomFloat( -random, random );
		pTemp->entity.baseline.origin[1] = dir[1] + Com_RandomFloat( -random, random );
		pTemp->entity.baseline.origin[2] = dir[2] + Com_RandomFloat( 0, random );

		pTemp->die = cl.time + life + Com_RandomFloat( 0.0f, 1.0f ); // Add an extra 0-1 secs of life
	}
};

/*
==============
CL_Bubbles

Create bubbles
==============
*/
void R_Bubbles(float *mins, float *maxs, float height, int modelIndex, int count, float speed)
{
	TEMPENTITY	*pTemp;
	int		i, frameCount;
	float		sine, cosine, zspeed;
	float		angle;
	vec3_t		origin;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	Mod_GetFrames( modelIndex, &frameCount );

	for ( i = 0; i < count; i++ )
	{
		origin[0] = Com_RandomLong( mins[0], maxs[0] );
		origin[1] = Com_RandomLong( mins[1], maxs[1] );
		origin[2] = Com_RandomLong( mins[2], maxs[2] );
		pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];
		angle = Com_RandomFloat( -M_PI, M_PI );
		SinCos( angle, &sine, &cosine );
		
		zspeed = Com_RandomLong( 80, 140 );
		VectorSet( pTemp->entity.baseline.origin, speed * cosine, speed * sine, zspeed );
		pTemp->die = cl.time + ((height - (origin[2] - mins[2])) / zspeed) - 0.1f;
		pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		
		// Set sprite scale
		pTemp->entity.curstate.scale = 1.0f / Com_RandomFloat( 4.0f, 16.0f );
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 192; // g-cont. why difference with FizzEffect ???		
	}
};

/*
==============
CL_BubbleTrail

Create bubble trail
==============
*/
//void CL_BubbleTrail( const vec3_t start, const vec3_t end, float flWaterZ, int modelIndex, int count, float speed )
void R_BubbleTrail(float *start, float *end, float height, int modelIndex, int count, float speed)
{
	TEMPENTITY	*pTemp;
	int		i, frameCount;
	float		dist, angle, zspeed;
	vec3_t		origin;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	Mod_GetFrames( modelIndex, &frameCount );

	for( i = 0; i < count; i++ )
	{
		dist = Com_RandomFloat( 0, 1.0 );
		VectorLerp( start, dist, end, origin );
		pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];
		angle = Com_RandomFloat( -M_PI, M_PI );

		zspeed = Com_RandomLong( 80, 140 );
		VectorSet( pTemp->entity.baseline.origin, speed * cos( angle ), speed * sin( angle ), zspeed );
		pTemp->die = cl.time + ((flWaterZ - (origin[2] - start[2])) / zspeed) - 0.1f;
		pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		// Set sprite scale
		pTemp->entity.curstate.scale = 1.0 / Com_RandomFloat( 4, 8 );
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 192;
	}
};

void R_BulletImpactParticles(float *pos)
{
};

void R_EntityParticles(struct cl_entity_s *ent)
{
};

/*
==============
CL_Explosion

Create an explosion (scale is magnitude)
==============
*/
void R_Explosion(float *pos, int model, float scale, float framerate, int flags)
{
	TEMPENTITY	*pTemp;
	sound_t		hSound;

	if( scale != 0.0f )
	{
		// create explosion sprite
		pTemp = CL_DefaultSprite( pos, model, framerate );
		CL_Sprite_Explode( pTemp, scale, flags );

		if( !( flags & TE_EXPLFLAG_NODLIGHTS ))
		{
			dlight_t	*dl;

			// big flash
			dl = CL_AllocDlight( 0 );
			VectorCopy( pos, dl->origin );
			dl->radius = 200;
			dl->color.r = dl->color.g = 250;
			dl->color.b = 150;
			dl->die = cl.time + 0.25f;
			dl->decay = 800;

			// red glow
			dl = CL_AllocDlight( 0 );
			VectorCopy( pos, dl->origin );
			dl->radius = 150;
			dl->color.r = 255;
			dl->color.g= 190;
			dl->color.b = 40;
			dl->die = cl.time + 1.0f;
			dl->decay = 200;
		}
	}

	if(!( flags & TE_EXPLFLAG_NOPARTICLES ))
		CL_FlickerParticles( pos );

	if( flags & TE_EXPLFLAG_NOSOUND ) return;

	hSound = S_RegisterSound( va( "weapons/explode%i.wav", Com_RandomLong( 3, 5 )));
	S_StartSound( pos, 0, CHAN_AUTO, hSound, VOL_NORM, ATTN_NORM, PITCH_NORM, 0 );
};

/*
==============
CL_FizzEffect

Create a fizz effect
==============
*/
void R_FizzEffect(struct cl_entity_s *pent, int modelIndex, int density)
{
	TEMPENTITY	*pTemp;
	int		i, width, depth, count, frameCount;
	float		angle, maxHeight, speed;
	float		xspeed, yspeed, zspeed;
	vec3_t		origin, mins, maxs;

	if( !pent || Mod_GetType( modelIndex ) == mod_bad )
		return;

	if( pent->curstate.modelindex <= 0 )
		return;

	count = density + 1;
	density = count * 3 + 6;

	Mod_GetBounds( pent->curstate.modelindex, mins, maxs );

	maxHeight = maxs[2] - mins[2];
	width = maxs[0] - mins[0];
	depth = maxs[1] - mins[1];
	speed = ( pent->curstate.rendercolor.r<<8 | pent->curstate.rendercolor.g );
	if( pent->curstate.rendercolor.b ) speed = -speed;
	if( speed == 0.0f ) speed = 100.0f;	// apply default value

	if( pent->angles[YAW] != 0.0f )
	{
		angle = pent->angles[YAW] * M_PI / 180;
		yspeed = sin( angle );
		xspeed = cos( angle );

		xspeed *= speed;
		yspeed *= speed;
	}
	else xspeed = yspeed = 0.0f;	// z only

	Mod_GetFrames( modelIndex, &frameCount );

	for( i = 0; i < count; i++ )
	{
		origin[0] = mins[0] + Com_RandomLong( 0, width - 1 );
		origin[1] = mins[1] + Com_RandomLong( 0, depth - 1 );
		origin[2] = mins[2];
		pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));

		if ( !pTemp ) return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];

		zspeed = Com_RandomLong( 80, 140 );
		VectorSet( pTemp->entity.baseline.origin, xspeed, yspeed, zspeed );
		pTemp->die = cl.time + ( maxHeight / zspeed ) - 0.1f;
		pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		// Set sprite scale
		pTemp->entity.curstate.scale = 1.0f / Com_RandomFloat( 2.0f, 5.0f );
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 255;
	}
};

/*
==============
CL_FireField

Makes a field of fire
==============
*/
void R_FireField(float *org, int radius, int modelIndex, int count, int flags, float life)
{
	TEMPENTITY	*pTemp;
	float		radius2;
	vec3_t		dir, m_vecPos;
	int		i;

	for( i = 0; i < count; i++ )
	{
		dir[0] = Com_RandomFloat( -1.0f, 1.0f );
		dir[1] = Com_RandomFloat( -1.0f, 1.0f );

		if( flags & TEFIRE_FLAG_PLANAR ) dir[2] = 0.0f;
		else dir[2] = Com_RandomFloat( -1.0f, 1.0f );
		VectorNormalize( dir );

		radius2 = Com_RandomFloat( 0.0f, radius );
		VectorMA( org, -radius2, dir, m_vecPos );

		pTemp = CL_DefaultSprite( m_vecPos, modelIndex, 0 );
		if( !pTemp ) return;

		if( flags & TEFIRE_FLAG_ALLFLOAT )
			pTemp->entity.baseline.origin[2] = 30; // drift sprite upward
		else if( flags & TEFIRE_FLAG_SOMEFLOAT && Com_RandomLong( 0, 1 ))
			pTemp->entity.baseline.origin[2] = 30; // drift sprite upward

		if( flags & TEFIRE_FLAG_LOOP )
		{
			pTemp->entity.curstate.framerate = 15;
			pTemp->flags |= FTENT_SPRANIMATELOOP;
		}

		if( flags & TEFIRE_FLAG_ALPHA )
		{
			pTemp->entity.curstate.rendermode = kRenderTransTexture;
			pTemp->entity.curstate.renderamt = 128;
		}

		pTemp->die += life;
	}
};

void R_FlickerParticles(float *org)
{
};

/*
===============
CL_FunnelSprite

Create a funnel effect with custom sprite
===============
*/
//void CL_FunnelSprite( const vec3_t pos, int spriteIndex, int flags )
void R_FunnelSprite(float *org, int modelIndex, int reverse)
{
	TEMPENTITY	*pTemp = NULL;
	particle_t	*pPart = NULL;
	vec3_t		dir, dest;
	vec3_t		m_vecPos;
	float		flDist, life, vel;
	int		i, j, colorIndex;

	colorIndex = CL_LookupColor( 0, 255, 0 ); // green color

	for( i = -256; i <= 256; i += 32 )
	{
		for( j = -256; j <= 256; j += 32 )
		{
			if( flags & SF_FUNNEL_REVERSE )
			{
				VectorCopy( pos, m_vecPos );

				dest[0] = pos[0] + i;
				dest[1] = pos[1] + j;
				dest[2] = pos[2] + Com_RandomFloat( 100, 800 );

				// send particle heading to dest at a random speed
				VectorSubtract( dest, m_vecPos, dir );

				// velocity based on how far particle has to travel away from org
				vel = dest[2] / 8;
			}
			else
			{
				m_vecPos[0] = pos[0] + i;
				m_vecPos[1] = pos[1] + j;
				m_vecPos[2] = pos[2] + Com_RandomFloat( 100, 800 );

				// send particle heading to org at a random speed
				VectorSubtract( pos, m_vecPos, dir );

				// velocity based on how far particle starts from org
				vel = m_vecPos[2] / 8;
			}

			if( pPart && spriteIndex && CL_PointContents( m_vecPos ) == CONTENTS_EMPTY )
			{
				pTemp = CL_TempEntAlloc( pos, Mod_Handle( spriteIndex ));
				pPart = NULL;
			}
			else
			{
				pPart = CL_AllocParticle( NULL );
				pTemp = NULL;
			}

			if( pTemp || pPart )
			{
				flDist = VectorNormalizeLength( dir );	// save the distance
				if( vel < 64 ) vel = 64;
				
				vel += Com_RandomFloat( 64, 128  );
				life = ( flDist / vel );

				if( pTemp )
				{
					VectorCopy( m_vecPos, pTemp->entity.origin );
					VectorScale( dir, vel, pTemp->entity.baseline.origin );
					pTemp->entity.curstate.rendermode = kRenderTransAdd;
					pTemp->flags |= FTENT_FADEOUT;
					pTemp->fadeSpeed = 3.0f;
					pTemp->die = cl.time + life - Com_RandomFloat( 0.5f, 0.6f );
					pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 255;
					pTemp->entity.curstate.scale = 0.75f;
				}
				
				if( pPart )
				{
					VectorCopy( m_vecPos, pPart->org );
					pPart->color = colorIndex;
					pPart->type = pt_static;

					VectorScale( dir, vel, pPart->vel );
					// die right when you get there
					pPart->die += life;
				}
			}
		}
	}
};

void R_Implosion(float *end, float radius, int count, float life)
{
};

/*
===============
CL_Large_Funnel

Create a funnel effect (particles only)
===============
*/
void R_LargeFunnel(float *org, int reverse)
{
	R_FunnelSprite(org, 0, flags);
};

void R_LavaSplash(float *org)
{
};

/*
==============
CL_MultiGunshot

Client version of shotgun shot
==============
*/
void R_MultiGunshot(float *org, float *dir, float *noise, int count, int decalCount, int *decalIndices)
{
	pmtrace_t	trace;
	vec3_t	right, up;
	vec3_t	vecSrc, vecDir, vecEnd;
	int	i, j, decalIndex;

	VectorVectors( dir, right, up );
	VectorCopy( org, vecSrc );

	for( i = 1; i <= count; i++ )
	{
		// get circular gaussian spread
		float x, y, z;
		do {
			x = Com_RandomFloat( -0.5f, 0.5f ) + Com_RandomFloat( -0.5f, 0.5f );
			y = Com_RandomFloat( -0.5f, 0.5f ) + Com_RandomFloat( -0.5f, 0.5f );
			z = x * x + y * y;
		} while( z > 1.0f );

		for( j = 0; j < 3; j++ )
		{
			vecDir[j] = dir[i] + x * noise[0] * right[j] + y * noise[1] * up[j];
			vecEnd[j] = vecSrc[j] + 2048.0f * vecDir[j];
		}

		trace = CL_TraceLine( vecSrc, vecEnd, PM_STUDIO_BOX );

		// paint decals
		if( trace.fraction != 1.0f )
		{
			physent_t	*pe = NULL;

			if( trace.ent >= 0 && trace.ent < clgame.pmove->numphysent )
				pe = &clgame.pmove->physents[trace.ent];

			if( pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ))
			{
				cl_entity_t *e = CL_GetEntityByIndex( pe->info );
				decalIndex = CL_DecalIndex( decalIndices[Com_RandomLong( 0, decalCount-1 )] );
				CL_DecalShoot( decalIndex, e->index, 0, trace.endpos, 0 );
			};
		};
	};
};

/*
==============
CL_MuzzleFlash

Do muzzleflash
==============
*/
void R_MuzzleFlash(float *pos, int type)
{
	TEMPENTITY	*pTemp;
	int		index, modelIndex, frameCount;
	float		scale;

	index = bound( 0, type % 5, MAX_MUZZLEFLASH - 1 );
	scale = (type / 5) * 0.1f;
	if( scale == 0.0f ) scale = 0.5f;

	modelIndex = cl_muzzleflash[index];
	if( !modelIndex ) return;

	Mod_GetFrames( modelIndex, &frameCount );

	// must set position for right culling on render
	pTemp = CL_TempEntAllocHigh( pos, Mod_Handle( modelIndex ));
	if( !pTemp ) return;
	pTemp->entity.curstate.rendermode = kRenderTransAdd;
	pTemp->entity.curstate.renderamt = 255;
	pTemp->entity.curstate.framerate = 10;
	pTemp->entity.curstate.renderfx = 0;
	pTemp->die = cl.time + 0.01; // die at next frame
	pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
	pTemp->flags |= FTENT_SPRANIMATE|FTENT_SPRANIMATELOOP;
	pTemp->frameMax = frameCount - 1;

	if( index == 0 )
	{
		// Rifle flash
		pTemp->entity.curstate.scale = scale * Com_RandomFloat( 0.5f, 0.6f );
		pTemp->entity.angles[2] = 90 * Com_RandomLong( 0, 3 );
	}
	else
	{
		pTemp->entity.curstate.scale = scale;
		pTemp->entity.angles[2] = Com_RandomLong( 0, 359 );
	}

	// play playermodel muzzleflashes only for mirror pass
	if( RP_LOCALCLIENT( RI.currententity ) && !RI.thirdPerson && ( RI.params & RP_MIRRORVIEW ))
		pTemp->entity.curstate.effects |= EF_REFLECTONLY;

	CL_TEntAddEntity( &pTemp->entity );
};

void R_ParticleBox(float *mins, float *maxs, unsigned char r, unsigned char g, unsigned char b, float life)
{
};

void R_ParticleBurst(float *pos, int size, int color, float life)
{
};

void R_ParticleExplosion(float *org)
{
};

void R_ParticleExplosion2(float *org, int colorStart, int colorLength)
{
};

void R_ParticleLine(float *start, float *end, unsigned char r, unsigned char g, unsigned char b, float life)
{
};

/*
==============
CL_PlayerSprites

Create a particle smoke around player
==============
*/
void R_PlayerSprites(int client, int modelIndex, int count, int size)
{
	TEMPENTITY	*pTemp;
	cl_entity_t	*pEnt;
	float		vel;
	int		i;

	pEnt = CL_GetEntityByIndex( client );

	if( !pEnt || !pEnt->player )
	{
		MsgDev( D_INFO, "Bad ent %i in R_PlayerSprites()!\n", client );
		return;
	}

	vel = 128;

	for( i = 0; i < count; i++ )
	{
		pTemp = CL_DefaultSprite( pEnt->origin, modelIndex, 15 );
		if( !pTemp ) return;

		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderfx = kRenderFxNone;
		pTemp->entity.baseline.origin[0] = Com_RandomFloat( -1.0f, 1.0f ) * vel;
		pTemp->entity.baseline.origin[1] = Com_RandomFloat( -1.0f, 1.0f ) * vel;
		pTemp->entity.baseline.origin[2] = Com_RandomFloat( 0.0f, 1.0f ) * vel;
		pTemp->entity.curstate.rendercolor.r = 192;
		pTemp->entity.curstate.rendercolor.g = 192;
		pTemp->entity.curstate.rendercolor.b = 192;
		pTemp->entity.curstate.renderamt = 64;
		pTemp->entity.curstate.scale = size;
	}
};

/*
==============
CL_Projectile

Create an projectile entity
==============
*/
void R_Projectile(float *origin, float *velocity, int modelIndex, int life, int owner, void (*hitcallback)(struct tempent_s *ent, struct pmtrace_s *ptr))
{
	// alloc a new tempent
	TEMPENTITY *pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));
	
	if( !pTemp )
		return;

	VectorAngles( velocity, pTemp->entity.angles );
	VectorCopy( velocity, pTemp->entity.baseline.origin );

	pTemp->entity.curstate.body = 0;
	pTemp->flags = FTENT_COLLIDEALL|FTENT_COLLIDEKILL;
	pTemp->entity.curstate.rendermode = kRenderNormal;
	pTemp->entity.baseline.renderamt = 255;
	pTemp->hitcallback = hitcallback;
	pTemp->die = cl.time + life;
	pTemp->clientIndex = owner;
};

/*
==============
CL_RicochetSound

Make a random ricochet sound
==============
*/
void R_RicochetSound(float *pos)
{
	char soundpath[32];
	int	iPitch = Com_RandomLong( 90, 105 );
	float fvol = Com_RandomFloat( 0.7f, 0.9f );
	sound_t	handle;

	Q_snprintf( soundpath, sizeof( soundpath ), "weapons/ric%i.wav", Com_RandomLong( 1, 5 ));
	handle = S_RegisterSound( soundpath );

	S_StartSound( pos, 0, CHAN_AUTO, handle, fvol, ATTN_NORM, iPitch, 0 );
};

void R_RicochetSprite(float *pos, struct model_s *pmodel, float duration, float scale)
{
};

void R_RocketFlare(float *pos)
{
};

void R_RocketTrail(float *start, float *end, int type)
{
};

void R_RunParticleEffect(float *org, float *dir, int color, int count)
{
};

void R_ShowLine(float *start, float *end)
{
};

void R_SparkEffect(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_SparkShower(float *pos)
{
};

void R_SparkStreaks(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_Spray(float *pos, float *dir, int modelIndex, int count, int speed, int spread, int rendermode)
{
};

void R_Sprite_Explode(TEMPENTITY *pTemp, float scale, int flags)
{
};

void R_Sprite_Smoke(TEMPENTITY *pTemp, float scale)
{
};

void R_Sprite_Spray(float *pos, float *dir, int modelIndex, int count, int speed, int iRand)
{
};

void R_Sprite_Trail(int type, float *start, float *end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed)
{
};

void R_Sprite_WallPuff(TEMPENTITY *pTemp, float scale)
{
};

void R_StreakSplash(float *pos, float *dir, int color, int count, float speed, int velocityMin, int velocityMax)
{
};

void R_TracerEffect(float *start, float *end)
{
};

void R_UserTracerParticle(float *org, float *vel, float life, int colorIndex, float length, unsigned char deathcontext, void (*deathfunc)(struct particle_s *particle))
{
};

particle_t *R_TracerParticles(float *org, float *vel, float life)
{
	return nullptr;
};

void R_TeleportSplash(float *org)
{
};

void R_TempSphereModel(float *pos, float speed, float life, int count, int modelIndex)
{
};

TEMPENTITY *R_TempModel(float *pos, float *dir, float *angles, float life, int modelIndex, int soundtype)
{
	return nullptr;
};

TEMPENTITY *R_DefaultSprite(float *pos, int spriteIndex, float framerate)
{
	return nullptr;
};

TEMPENTITY *R_TempSprite(float *pos, float *dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags)
{
	return nullptr;
};

int Draw_DecalIndex(int id)
{
	return 0;
};

int Draw_DecalIndexFromName(char *name)
{
	return 0;
};

void R_DecalShoot(int textureIndex, int entity, int modelIndex, float *position, int flags)
{
};

void R_AttachTentToPlayer(int client, int modelIndex, float zoffset, float life)
{
};

void R_KillAttachedTents(int client)
{
};

BEAM *R_BeamCirclePoints(int type, float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamEntPoint(int startEnt, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamEnts(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamFollow(int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness)
{
	return nullptr;
};

void R_BeamKill(int deadEntity)
{
};

BEAM *R_BeamLightning(float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed)
{
	return nullptr;
};

BEAM *R_BeamPoints(float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamRing(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

/*
===============
CL_AllocDlight

===============
*/
dlight_t *CL_AllocDlight(int key)
{
	dlight_t	*dl;
	int	i;

	// first look for an exact key match
	if( key )
	{
		for( i = 0, dl = cl_dlights; i < MAX_DLIGHTS; i++, dl++ )
		{
			if( dl->key == key )
			{
				// reuse this light
				Q_memset( dl, 0, sizeof( *dl ));
				dl->key = key;
				return dl;
			}
		}
	}

	// then look for anything else
	for( i = 0, dl = cl_dlights; i < MAX_DLIGHTS; i++, dl++ )
	{
		if( dl->die < cl.time && dl->key == 0 )
		{
			Q_memset( dl, 0, sizeof( *dl ));
			dl->key = key;
			return dl;
		}
	}

	// otherwise grab first dlight
	dl = &cl_dlights[0];
	Q_memset( dl, 0, sizeof( *dl ));
	dl->key = key;

	return dl;
};

/*
===============
CL_AllocElight

===============
*/
dlight_t *CL_AllocElight(int key)
{
	dlight_t	*dl;
	int	i;

	// first look for an exact key match
	if( key )
	{
		for( i = 0, dl = cl_elights; i < MAX_ELIGHTS; i++, dl++ )
		{
			if( dl->key == key )
			{
				// reuse this light
				Q_memset( dl, 0, sizeof( *dl ));
				dl->key = key;
				return dl;
			}
		}
	}

	// then look for anything else
	for( i = 0, dl = cl_elights; i < MAX_ELIGHTS; i++, dl++ )
	{
		if( dl->die < cl.time && dl->key == 0 )
		{
			Q_memset( dl, 0, sizeof( *dl ));
			dl->key = key;
			return dl;
		}
	}

	// otherwise grab first dlight
	dl = &cl_elights[0];
	Q_memset( dl, 0, sizeof( *dl ));
	dl->key = key;

	return dl;
};

TEMPENTITY *CL_TempEntAlloc(float *org, struct model_s *model)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocNoModel(float *org)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocHigh(float *org, struct model_s *model)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocCustom(float *origin, struct model_s *model, int high, void (*callback)(struct tempent_s *ent, float frametime, float currenttime))
{
	return nullptr;
};

void R_GetPackedColor(short *packed, short color)
{
};

short R_LookupColor(unsigned char r, unsigned char g, unsigned char b)
{
	return 0;
};

/*
===============
CL_DecalRemoveAll

remove all decals with specified texture
===============
*/
void R_DecalRemoveAll(int textureIndex)
{
	//textureIndex points to the decal index in the array, not the actual texture index
	int id = bound(0, textureIndex, MAX_DECALS - 1);	
	R_DecalRemoveAll(cl.decal_index[id]);
};

void R_FireCustomDecal(int textureIndex, int entity, int modelIndex, float *position, int flags, float scale)
{
};

#else // if defined(OGS_EFX_API_NULL_IMPL)

particle_t *R_AllocParticle(void (*callback)(struct particle_s *particle, float frametime))
{
	return nullptr;
};

void R_BlobExplosion(float *org)
{
};

void R_Blood(float *org, float *dir, int pcolor, int speed)
{
};

void R_BloodSprite(float *org, int colorindex, int modelIndex, int modelIndex2, float size)
{
};

void R_BloodStream(float *org, float *dir, int pcolor, int speed)
{
};

void R_BreakModel(float *pos, float *size, float *dir, float random, float life, int count, int modelIndex, char flags)
{
};

void R_Bubbles(float *mins, float *maxs, float height, int modelIndex, int count, float speed)
{
};

void R_BubbleTrail(float *start, float *end, float height, int modelIndex, int count, float speed)
{
};

void R_BulletImpactParticles(float *pos)
{
};

void R_EntityParticles(struct cl_entity_s *ent)
{
};

void R_Explosion(float *pos, int model, float scale, float framerate, int flags)
{
};

void R_FizzEffect(struct cl_entity_s *pent, int modelIndex, int density)
{
};

void R_FireField(float *org, int radius, int modelIndex, int count, int flags, float life)
{
};

void R_FlickerParticles(float *org)
{
};

void R_FunnelSprite(float *org, int modelIndex, int reverse)
{
};

void R_Implosion(float *end, float radius, int count, float life)
{
};

void R_LargeFunnel(float *org, int reverse)
{
};

void R_LavaSplash(float *org)
{
};

void R_MultiGunshot(float *org, float *dir, float *noise, int count, int decalCount, int *decalIndices)
{
};

void R_MuzzleFlash(float *pos1, int type)
{
};

void R_ParticleBox(float *mins, float *maxs, unsigned char r, unsigned char g, unsigned char b, float life)
{
};

void R_ParticleBurst(float *pos, int size, int color, float life)
{
};

void R_ParticleExplosion(float *org)
{
};

void R_ParticleExplosion2(float *org, int colorStart, int colorLength)
{
};

void R_ParticleLine(float *start, float *end, unsigned char r, unsigned char g, unsigned char b, float life)
{
};

void R_PlayerSprites(int client, int modelIndex, int count, int size)
{
};

void R_Projectile(float *origin, float *velocity, int modelIndex, int life, int owner, void (*hitcallback)(struct tempent_s *ent, struct pmtrace_s *ptr))
{
};

void R_RicochetSound(float *pos)
{
};

void R_RicochetSprite(float *pos, struct model_s *pmodel, float duration, float scale)
{
};

void R_RocketFlare(float *pos)
{
};

void R_RocketTrail(float *start, float *end, int type)
{
};

void R_RunParticleEffect(float *org, float *dir, int color, int count)
{
};

void R_ShowLine(float *start, float *end)
{
};

void R_SparkEffect(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_SparkShower(float *pos)
{
};

void R_SparkStreaks(float *pos, int count, int velocityMin, int velocityMax)
{
};

void R_Spray(float *pos, float *dir, int modelIndex, int count, int speed, int spread, int rendermode)
{
};

void R_Sprite_Explode(TEMPENTITY *pTemp, float scale, int flags)
{
};

void R_Sprite_Smoke(TEMPENTITY *pTemp, float scale)
{
};

void R_Sprite_Spray(float *pos, float *dir, int modelIndex, int count, int speed, int iRand)
{
};

void R_Sprite_Trail(int type, float *start, float *end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed)
{
};

void R_Sprite_WallPuff(TEMPENTITY *pTemp, float scale)
{
};

void R_StreakSplash(float *pos, float *dir, int color, int count, float speed, int velocityMin, int velocityMax)
{
};

void R_TracerEffect(float *start, float *end)
{
};

void R_UserTracerParticle(float *org, float *vel, float life, int colorIndex, float length, unsigned char deathcontext, void (*deathfunc)(struct particle_s *particle))
{
};

particle_t *R_TracerParticles(float *org, float *vel, float life)
{
	return nullptr;
};

void R_TeleportSplash(float *org)
{
};

void R_TempSphereModel(float *pos, float speed, float life, int count, int modelIndex)
{
};

TEMPENTITY *R_TempModel(float *pos, float *dir, float *angles, float life, int modelIndex, int soundtype)
{
	return nullptr;
};

TEMPENTITY *R_DefaultSprite(float *pos, int spriteIndex, float framerate)
{
	return nullptr;
};

TEMPENTITY *R_TempSprite(float *pos, float *dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags)
{
	return nullptr;
};

int Draw_DecalIndex(int id)
{
	return 0;
};

int Draw_DecalIndexFromName(char *name)
{
	return 0;
};

void R_DecalShoot(int textureIndex, int entity, int modelIndex, float *position, int flags)
{
};

void R_AttachTentToPlayer(int client, int modelIndex, float zoffset, float life)
{
};

void R_KillAttachedTents(int client)
{
};

BEAM *R_BeamCirclePoints(int type, float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamEntPoint(int startEnt, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamEnts(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamFollow(int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness)
{
	return nullptr;
};

void R_BeamKill(int deadEntity)
{
};

BEAM *R_BeamLightning(float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed)
{
	return nullptr;
};

BEAM *R_BeamPoints(float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

BEAM *R_BeamRing(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b)
{
	return nullptr;
};

dlight_t *CL_AllocDlight(int key)
{
	return nullptr;
};

dlight_t *CL_AllocElight(int key)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAlloc(float *org, struct model_s *model)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocNoModel(float *org)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocHigh(float *org, struct model_s *model)
{
	return nullptr;
};

TEMPENTITY *CL_TempEntAllocCustom(float *origin, struct model_s *model, int high, void (*callback)(struct tempent_s *ent, float frametime, float currenttime))
{
	return nullptr;
};

void R_GetPackedColor(short *packed, short color)
{
};

short R_LookupColor(unsigned char r, unsigned char g, unsigned char b)
{
	return 0;
};

void R_DecalRemoveAll(int textureIndex)
{
	//textureIndex points to the decal index in the array, not the actual texture index
};

void R_FireCustomDecal(int textureIndex, int entity, int modelIndex, float *position, int flags, float scale)
{
};

#endif // OGS_EFX_API_NULL_IMPL

}; // namespace

efx_api_t gEfxAPI =
    {
        R_AllocParticle,
        (void *)R_BlobExplosion,
		
        (void *)R_Blood,
        (void *)R_BloodSprite,
        (void *)R_BloodStream,
		
        (void *)R_BreakModel,
		
        (void *)R_Bubbles,
        (void *)R_BubbleTrail,
		
        (void *)R_BulletImpactParticles,
        R_EntityParticles,
        R_Explosion,
        R_FizzEffect,
        R_FireField,
        (void *)R_FlickerParticles,
        (void *)R_FunnelSprite,
        (void *)R_Implosion,
        (void *)R_LargeFunnel,
        (void *)R_LavaSplash,
        (void *)R_MultiGunshot,
        (void *)R_MuzzleFlash,
		
        (void *)R_ParticleBox,
        (void *)R_ParticleBurst,
		(void *)R_ParticleExplosion,
        (void *)R_ParticleExplosion2,
		(void *)R_ParticleLine,
		
        R_PlayerSprites,
        (void *)R_Projectile,
		
        (void *)R_RicochetSound,
		(void *)R_RicochetSprite,
		
        (void *)R_RocketFlare,
        R_RocketTrail,
		
        (void *)R_RunParticleEffect,
        (void *)R_ShowLine,
		
        (void *)R_SparkEffect,
        (void *)R_SparkShower,
        (void *)R_SparkStreaks,
		
		(void *)R_Spray,
        R_Sprite_Explode,
        R_Sprite_Smoke,
        (void *)R_Sprite_Spray,
        (void *)R_Sprite_Trail,
        R_Sprite_WallPuff,
        (void *)R_StreakSplash,
        (void *)R_TracerEffect,
        R_UserTracerParticle,
        R_TracerParticles,
        (void *)R_TeleportSplash,
        (void *)R_TempSphereModel,
        (void *)R_TempModel,
        (void *)R_DefaultSprite,
        (void *)R_TempSprite,
		
        Draw_DecalIndex,
        (void *)Draw_DecalIndexFromName,
		
        R_DecalShoot,
		
        R_AttachTentToPlayer,
        R_KillAttachedTents,
		
		(void *)R_BeamCirclePoints,
        (void *)R_BeamEntPoint,
		
        R_BeamEnts,
        R_BeamFollow,
        R_BeamKill,
		
        (void *)R_BeamLightning,
        (void *)R_BeamPoints,
        (void *)R_BeamRing,
		
        CL_AllocDlight,
        CL_AllocElight,
		
        (void *)CL_TempEntAlloc,
        (void *)CL_TempEntAllocNoModel,
        (void *)CL_TempEntAllocHigh,
        (void *)CL_TempEntAllocCustom,
		
        R_GetPackedColor,
        R_LookupColor,
        R_DecalRemoveAll,
        R_FireCustomDecal
		};