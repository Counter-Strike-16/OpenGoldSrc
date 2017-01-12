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
/// @brief client side temporary entities

//#include "precompiled.hpp"
#include "client/client.hpp"

#define MAX_BEAMS 8
typedef struct
{
	int             entity;
	struct model_s *model;
	float           endtime;
	vec3_t          start, end;
} beam_t;

beam_t cl_beams[MAX_BEAMS];

#define MAX_EXPLOSIONS 8
typedef struct
{
	vec3_t   origin;
	float    start;
	model_t *model;
} explosion_t;

explosion_t cl_explosions[MAX_EXPLOSIONS];

sfx_t *cl_sfx_ric1;
sfx_t *cl_sfx_ric2;
sfx_t *cl_sfx_ric3;

/*
=================
CL_ParseTEnts
=================
*/
void CL_InitTEnts()
{
	cl_sfx_ric1 = S_PrecacheSound("weapons/ric1.wav");
	cl_sfx_ric2 = S_PrecacheSound("weapons/ric2.wav");
	cl_sfx_ric3 = S_PrecacheSound("weapons/ric3.wav");
	// ric4?
	// ric5?
}

/*
=================
CL_ClearTEnts
=================
*/
void CL_ClearTEnts()
{
	memset(&cl_beams, 0, sizeof(cl_beams));
	memset(&cl_explosions, 0, sizeof(cl_explosions));
}

/*
=================
CL_AllocExplosion
=================
*/
explosion_t *CL_AllocExplosion()
{
	int i;

	for(i = 0; i < MAX_EXPLOSIONS; ++i)
		if(!cl_explosions[i].model)
			return &cl_explosions[i];

	// find the oldest explosion
	float time  = cl.time;
	int   index = 0;

	for(i = 0; i < MAX_EXPLOSIONS; i++)
	{
		if(cl_explosions[i].start < time)
		{
			time  = cl_explosions[i].start;
			index = i;
		};
	};

	return &cl_explosions[index];
}

/*
=================
CL_ParseBeam
=================
*/
void CL_ParseBeam(model_t *m)
{
	int     ent;
	vec3_t  start, end;
	beam_t *b;
	int     i;

	ent = MSG_ReadShort();

	start[0] = MSG_ReadCoord();
	start[1] = MSG_ReadCoord();
	start[2] = MSG_ReadCoord();

	end[0] = MSG_ReadCoord();
	end[1] = MSG_ReadCoord();
	end[2] = MSG_ReadCoord();

	// override any beam with the same entity
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
		if(b->entity == ent)
		{
			b->entity  = ent;
			b->model   = m;
			b->endtime = cl.time + 0.2;
			VectorCopy(start, b->start);
			VectorCopy(end, b->end);
			return;
		}

	// find a free beam
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
	{
		if(!b->model || b->endtime < cl.time)
		{
			b->entity  = ent;
			b->model   = m;
			b->endtime = cl.time + 0.2;
			VectorCopy(start, b->start);
			VectorCopy(end, b->end);
			return;
		}
	}
	Con_Printf("beam list overflow!\n");
}

/*
=================
CL_ParseTEnt
=================
*/

// clang-format off

void CL_ParseTempEntity()
{
	byte nEntType = MSG_ReadByte();

	vec3_t vStartPos = vec3_origin;
	vec3_t vEndPos   = vec3_origin;

	switch(nEntType)
	{
	case TE_BEAMPOINTS:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMENTPOINT:
		short nStartEnt = MSG_ReadShort();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_GUNSHOT: // bullet hitting wall
		//cnt = MSG_ReadByte();
		
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		R_RunParticleEffect(vStartPos, vec3_origin, 0, 20 * cnt);
		break;
	case TE_EXPLOSION: // rocket explosion particles
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale     = MSG_ReadByte();
		byte nFrameRate = MSG_ReadByte();
		byte nFlags     = MSG_ReadByte();
		break;
	case TE_TAREXPLOSION: // tarbaby explosion
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		R_BlobExplosion(vStartPos);

		S_StartSound(-1, 0, cl_sfx_r_exp3, vStartPos, 1, 1);
		break;
	case TE_SMOKE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale     = MSG_ReadByte();
		byte nFrameRate = MSG_ReadByte();
		break;
	case TE_TRACER:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();
		break;
	case TE_LIGHTNING:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		byte nLifeTime  = MSG_ReadByte();
		byte nWidth     = MSG_ReadByte();
		byte nAmplitude = MSG_ReadByte();

		short nModelIndex = MSG_ReadShort();
		break;
	case TE_BEAMENTS:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_SPARKS:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_LAVASPLASH:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		R_LavaSplash(vStartPos);
		break;
	case TE_TELEPORT:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		R_TeleportSplash(vStartPos);
		break;
	case TE_EXPLOSION2:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		byte nStartColor = MSG_ReadByte();
		byte nColorCount = MSG_ReadByte();
		break;
	case TE_BSPDECAL:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nTextureIndex  = MSG_ReadShort();
		short nEntIndex      = MSG_ReadShort();
		short nEntModelIndex = 0;

		if(nEntIndex)
			nEntModelIndex = MSG_ReadShort();
		break;
	case TE_IMPLOSION:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		byte nRadius   = MSG_ReadByte();
		byte nCount    = MSG_ReadByte();
		byte nLifeTime = MSG_ReadByte();
		break;
	case TE_SPRITETRAIL:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nCount      = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nScale      = MSG_ReadByte();
		byte nVelocity   = MSG_ReadByte();
		byte nRandomness = MSG_ReadByte();
		break;
	case TE_BEAM:
		// obsolete, unused
		break;
	case TE_SPRITE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale      = MSG_ReadByte();
		byte nBrightness = MSG_ReadByte();
		break;
	case TE_BEAMSPRITE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nStartSprIndex = MSG_ReadShort();
		short nEndSprIndex   = MSG_ReadShort();
		break;
	case TE_BEAMTORUS:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMDISK:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartingFrame = MSG_ReadByte();
		byte nFrameRate     = MSG_ReadByte();
		byte nLifeTime      = MSG_ReadByte();
		byte nLineWidth     = MSG_ReadByte();
		byte nNoiseAmpl     = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMCYLINDER:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameTate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte, byte, byte(color)

		                byte nBrightness  = MSG_ReadByte();
		byte                 nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMFOLLOW:
		short(entity
		      : attachment to follow) short nSprIndex = MSG_ReadShort();

		byte nLifeTime  = MSG_ReadByte();
		byte nLineWidth = MSG_ReadByte();

		byte, byte, byte(color)

		                byte nBrightness = MSG_ReadByte();
		break;
	case TE_GLOWSPRITE:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte(scale / 10) = MSG_ReadByte();
		break;
	case TE_BEAMRING:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();
		
		byte nColor[3] = {0};
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_STREAK_SPLASH:
		coord coord coord(start position)
		    coord coord
		    coord(direction vector)

		        byte nColor = MSG_ReadByte();

		short nCount     = MSG_ReadShort();
		short nBaseSpeed = MSG_ReadShort();
		short nVelocity  = MSG_ReadShort();
		break;
	case TE_BEAMHOSE:
		// obsolete, unused
		break;
	case TE_DLIGHT:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		byte nRadius = MSG_ReadByte();
		
		byte nColor[3] = {0};
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nDecayRate  = MSG_ReadByte();
		break;
	case TE_ELIGHT:
		short(entity
		      : attachment to follow)

		    float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fRadius = MSG_ReadCoord();

		byte nColor[3] = {0};
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		byte nLifeTime = MSG_ReadByte();

		coord(decay rate) = MSG_ReadCoord();
		break;
	case TE_TEXTMESSAGE:
		short 1.2.13 x(-1 = center) short 1.2.13 y(-1 = center)

		    byte nEffect = MSG_ReadByte();
		// 0 = fade in/fade out
		// 1 is flickery credits
		// 2 is write out (training room)
		// 4 bytes r,g,b,a color1	(text color)
		// 4 bytes r,g,b,a color2	(effect color)

		ushort nFadeInTime
		    ushort nFadeOutTime
		        ushort nHoldTime

		            optional ushort
		            nFXTime(time the highlight lags behing the leading text in effect 2)

		                string text message(512 chars max sz string) break;
	case TE_LINE:
		coord, coord, coord startpos
		                  coord,
		    coord, coord endpos

		    short nLifeTime = MSG_ReadShort();

		3 bytes r, g, b break;
	case TE_BOX:
		coord, coord, coord boxmins
		                  coord,
		    coord, coord boxmaxs

		    short nLifeTime = MSG_ReadShort();

		3 bytes r, g, b break;
	case TE_KILLBEAM:
		short nEnt = MSG_ReadShort();
		break;
	case TE_LARGEFUNNEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();
		short nFlags    = MSG_ReadShort();
		break;
	case TE_BLOODSTREAM:
		coord coord coord(start position)
		    coord coord
		    coord(spray vector)

		        byte nColor = MSG_ReadByte();
		byte         nSpeed = MSG_ReadByte();
		break;
	case TE_SHOWLINE:
		coord coord coord(start position)
		    coord coord
		    coord(end position)
		break;
	case TE_BLOOD: // // bullets hitting body
		coord coord coord(start position)
		    coord coord
		    coord(spray vector)

		        byte nColor = MSG_ReadByte();
		byte         nSpeed = MSG_ReadByte();
		
		//
		
		cnt    = MSG_ReadByte();
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		
		R_RunParticleEffect(vStartPos, vec3_origin, 73, 20 * cnt);
		break;
	case TE_DECAL:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte(texture index of precached decal texture name)

		                                      short nEntIndex = MSG_ReadShort();
		break;
	case TE_FIZZ:
		short nEntity   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nDensity = MSG_ReadByte();
		break;
	case TE_MODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		angle(initial yaw)

		    short nModelIndex = MSG_ReadShort();

		byte(bounce sound type)     = MSG_ReadByte();
		byte              nLifeTime = MSG_ReadByte();
		break;
	case TE_EXPLODEMODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nCount      = MSG_ReadShort();

		byte nLifeTime = MSG_ReadByte();
		break;
	case TE_BREAKMODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fSize[3] = {0};
		for(int i    = 0; i < 3; ++i)
			fSize[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		byte nVelocity = MSG_ReadByte();

		short nModelIndex = MSG_ReadShort();

		byte nCount    = MSG_ReadByte();
		byte nLifeTime = MSG_ReadByte();
		byte nFlags    = MSG_ReadByte();
		break;
	case TE_GUNSHOTDECAL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nEntIndex = MSG_ReadShort();

		byte (decal???) = MSG_ReadByte();
		break;
	case TE_SPRITE_SPRAY:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();
		byte nSpeed = MSG_ReadByte();
		byte nNoise = MSG_ReadByte();
		break;
	case TE_ARMOR_RICOCHET:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		byte nScale = MSG_ReadByte();
		break;
	case TE_PLAYERDECAL:
		byte nPlayerIndex = MSG_ReadByte();

		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short (entity???) = MSG_ReadShort();

		byte nDecalNum = MSG_ReadByte();

		[optional] short (model index???)
		break;
	case TE_BUBBLES:
		float fMinPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BUBBLETRAIL:
		float fMinPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BLOODSPRITE:
		float fPosition[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fPosition[i] = MSG_ReadCoord();

		short nSpr1Index = MSG_ReadShort();
		short nSpr2Index = MSG_ReadShort();

		byte nColor = MSG_ReadByte();
		byte nScale = MSG_ReadByte();
		break;
	case TE_WORLDDECAL:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte nTextureIndex = MSG_ReadByte();
		break;
	case TE_WORLDDECALHIGH:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte(texture index of precached decal texture name - 256) break;
	case TE_DECALHIGH:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte nTextureIndex = MSG_ReadByte();

		short nEntIndex = MSG_ReadShort();
		break;
	case TE_PROJECTILE:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nLife  = MSG_ReadByte();
		byte nOwner = MSG_ReadByte();
		break;
	case TE_SPRAY:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fDir[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fDir[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount      = MSG_ReadByte();
		byte nSpeed      = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();
		byte nRenderMode = MSG_ReadByte();
		break;
	case TE_PLAYERSPRITES:
		byte nPlayerNum = MSG_ReadByte();

		short nSprModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();
		byte nVariance(0 = no variance in size)(10 = 10 % variance in size) break;
	case TE_PARTICLEBURST:
		float fOrigin = MSG_ReadCoord();

		short nRadius = MSG_ReadShort();

		byte nParticleColor = MSG_ReadByte();
		byte nDuration      = MSG_ReadByte();
		break;
	case TE_FIREFIELD:
		float fOrigin = MSG_ReadCoord();

		short nRadius     = MSG_ReadShort();
		short nModelIndex = MSG_ReadShort();

		byte nCount    = MSG_ReadByte();
		byte nFlags    = MSG_ReadByte();
		byte nDuration = MSG_ReadByte();
		break;
	case TE_PLAYERATTACHMENT:
		byte nPlayerEntIndex = MSG_ReadByte();

		float fVerticalOffset = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nLifeTime   = MSG_ReadShort();
		break;
	case TE_KILLPLAYERATTACHMENTS:
		byte nPlayerEntIndex = MSG_ReadByte();
		break;
	case TE_MULTIGUNSHOT:
		// This message is used to make a client approximate a 'spray' of gunfire.
		// Any weapon that fires more than one bullet per frame and fires in a bit of a spread is
		// a good candidate for MULTIGUNSHOT use. (shotguns)
		//
		// NOTE: This effect makes the client do traces for each bullet, these client traces ignore
		//		 entities that have studio models.Traces are 4096 long.
		//
		// coord (origin)
		// coord (origin)
		// coord (origin)
		// coord (direction)
		// coord (direction)
		// coord (direction)
		// coord (x noise * 100)
		// coord (y noise * 100)
		// byte (count)
		// byte (bullethole decal texture index)
		break;
	case TE_USERTRACER:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		byte nLifeTime   = MSG_ReadByte();
		byte nColorIndex = MSG_ReadByte();
		byte nLength     = MSG_ReadByte();
		break;
	default:
		//Sys_Error("CL_ParseTEnt: bad type");
		break;
	};
};

// clang-format on

/*
void CL_ParseTEnt()
{
	int          type;
	vec3_t       pos;
	dlight_t *   dl;
	int          rnd;
	explosion_t *ex;
	int          cnt;

	type = MSG_ReadByte();
	switch(type)
	{
	case TE_WIZSPIKE: // spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 20, 30);
		S_StartSound(-1, 0, cl_sfx_wizhit, pos, 1, 1);
		break;

	case TE_KNIGHTSPIKE: // spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 226, 20);
		S_StartSound(-1, 0, cl_sfx_knighthit, pos, 1, 1);
		break;

	case TE_SPIKE: // spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 0, 10);

		if(rand() % 5)
			S_StartSound(-1, 0, cl_sfx_tink1, pos, 1, 1);
		else
		{
			rnd = rand() & 3;
			if(rnd == 1)
				S_StartSound(-1, 0, cl_sfx_ric1, pos, 1, 1);
			else if(rnd == 2)
				S_StartSound(-1, 0, cl_sfx_ric2, pos, 1, 1);
			else
				S_StartSound(-1, 0, cl_sfx_ric3, pos, 1, 1);
		}
		break;
	case TE_SUPERSPIKE: // super spike hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 0, 20);

		if(rand() % 5)
			S_StartSound(-1, 0, cl_sfx_tink1, pos, 1, 1);
		else
		{
			rnd = rand() & 3;
			if(rnd == 1)
				S_StartSound(-1, 0, cl_sfx_ric1, pos, 1, 1);
			else if(rnd == 2)
				S_StartSound(-1, 0, cl_sfx_ric2, pos, 1, 1);
			else
				S_StartSound(-1, 0, cl_sfx_ric3, pos, 1, 1);
		}
		break;

	case TE_EXPLOSION: 
		               // 
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_ParticleExplosion(pos);

		// light
		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius   = 350;
		dl->die      = cl.time + 0.5;
		dl->decay    = 300;
		dl->color[0] = 0.2;
		dl->color[1] = 0.1;
		dl->color[2] = 0.05;
		dl->color[3] = 0.7;

		// sound
		S_StartSound(-1, 0, cl_sfx_r_exp3, pos, 1, 1);

		// sprite
		ex = CL_AllocExplosion();
		VectorCopy(pos, ex->origin);
		ex->start = cl.time;
		ex->model = Mod_ForName("progs/s_explod.spr", true);
		break;

	case TE_LIGHTNING1: // lightning bolts
		CL_ParseBeam(Mod_ForName("progs/bolt.mdl", true));
		break;

	case TE_LIGHTNING2: // lightning bolts
		CL_ParseBeam(Mod_ForName("progs/bolt2.mdl", true));
		break;

	case TE_LIGHTNING3: // lightning bolts
		CL_ParseBeam(Mod_ForName("progs/bolt3.mdl", true));
		break;

	case TE_LIGHTNINGBLOOD: // lightning hitting body
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 225, 50);
		break;
	}
}
*/

/*
=================
CL_NewTempEntity
=================
*/
cl_entity_t *CL_NewTempEntity()
{
	if(cl_numvisedicts == MAX_VISEDICTS)
		return NULL;

	cl_entity_t *ent = &cl_visedicts[cl_numvisedicts];

	cl_numvisedicts++;
	ent->keynum = 0;

	memset(ent, 0, sizeof(*ent));

	ent->colormap = vid.colormap;
	return ent;
}

/*
=================
CL_UpdateBeams
=================
*/
void CL_UpdateBeams()
{
	int          i;
	beam_t *     b;
	vec3_t       dist, org;
	float        d;
	cl_entity_t *ent;
	float        yaw, pitch;
	float        forward;

	// update lightning
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
	{
		if(!b->model || b->endtime < cl.time)
			continue;

		// if coming from the player, update the start position
		if(b->entity == cl.playernum + 1) // entity 0 is the world
		{
			VectorCopy(cl.simorg, b->start);
			//			b->start[2] -= 22;	// adjust for view height
		}

		// calculate pitch and yaw
		VectorSubtract(b->end, b->start, dist);

		if(dist[1] == 0 && dist[0] == 0)
		{
			yaw = 0;
			if(dist[2] > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			yaw = (int)(atan2(dist[1], dist[0]) * 180 / M_PI);
			if(yaw < 0)
				yaw += 360;

			forward = sqrt(dist[0] * dist[0] + dist[1] * dist[1]);
			pitch   = (int)(atan2(dist[2], forward) * 180 / M_PI);
			if(pitch < 0)
				pitch += 360;
		}

		// add new entities for the lightning
		VectorCopy(b->start, org);
		d = VectorNormalize(dist);
		while(d > 0)
		{
			ent = CL_NewTempEntity();
			if(!ent)
				return;
			VectorCopy(org, ent->origin);
			ent->model     = b->model;
			ent->angles[0] = pitch;
			ent->angles[1] = yaw;
			ent->angles[2] = rand() % 360;

			for(i = 0; i < 3; i++)
				org[i] += dist[i] * 30;
			d -= 30;
		}
	}
}

/*
=================
CL_UpdateExplosions
=================
*/
void CL_UpdateExplosions()
{
	int          i;
	int          f;
	explosion_t *ex;
	cl_entity_t *ent;

	for(i = 0, ex = cl_explosions; i < MAX_EXPLOSIONS; i++, ex++)
	{
		if(!ex->model)
			continue;
		f = 10 * (cl.time - ex->start);
		if(f >= ex->model->numframes)
		{
			ex->model = NULL;
			continue;
		}

		ent = CL_NewTempEntity();
		if(!ent)
			return;
		VectorCopy(ex->origin, ent->origin);
		ent->model = ex->model;
		ent->frame = f;
	}
}

/*
=================
CL_UpdateTEnts
=================
*/
void CL_UpdateTEnts()
{
	CL_UpdateBeams();
	CL_UpdateExplosions();
}