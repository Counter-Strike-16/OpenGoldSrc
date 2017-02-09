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

typedef struct
{
	int entity;
	struct model_s *model;
	float endtime;
	vec3_t start, end;
} beam_t;

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
CL_ParseBeam
=================
*/
void CL_ParseBeam(model_t *m)
{
	int ent;
	vec3_t start, end;
	beam_t *b;
	int i;

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
			b->entity = ent;
			b->model = m;
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
			b->entity = ent;
			b->model = m;
			b->endtime = cl.time + 0.2;
			VectorCopy(start, b->start);
			VectorCopy(end, b->end);
			return;
		}
	}
	Con_Printf("beam list overflow!\n");
}

/*
==============
CL_ParseTempEntity

handle temp entity messages
==============
*/
void CL_ParseTempEntity()
{
	int nEntType = MSG_ReadByte();

	vec3_t vStartPos = vec3_origin;
	vec3_t vEndPos = vec3_origin;

	switch(nEntType)
	{
	case TE_BEAMPOINTS:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nStartFrame = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();
		int nNoise = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nBrightness = MSG_ReadByte();
		int nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMENTPOINT:
		short nStartEnt = MSG_ReadShort();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nStartFrame = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();
		int nNoise = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nBrightness = MSG_ReadByte();
		int nScrollSpeed = MSG_ReadByte();
		break;
	case TE_GUNSHOT: // bullet hitting wall
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		CL_RicochetSound(vStartPos);
		R_RunParticleEffect(vStartPos, vec3_origin, 0, 20);
		break;
	case TE_EXPLOSION: // rocket explosion particles
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nScale = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);
		int nFrameRate = MSG_ReadByte();
		int nFlags = MSG_ReadByte();

		CL_Explosion(vStartPos, nSprIndex, nScale, nFrameRate, nFlags);
		break;
	case TE_TAREXPLOSION: // tarbaby explosion
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		R_BlobExplosion(vStartPos); // CL_

		S_StartSound(-1, 0, cl_sfx_r_exp3, vStartPos, 1, 1);
		break;
	case TE_SMOKE:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nScale = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		break;
	case TE_TRACER:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		CL_TracerEffect(vStartPos, vEndPos);
		break;
	case TE_LIGHTNING:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		int nLifeTime = MSG_ReadByte();
		int nWidth = MSG_ReadByte();
		int nAmplitude = MSG_ReadByte();

		short nModelIndex = MSG_ReadShort();
		break;
	case TE_BEAMENTS:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		int nStartFrame = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();
		int nNoiseAmpl = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nBrightness = MSG_ReadByte();
		int nScrollSpeed = MSG_ReadByte();
		break;
	case TE_SPARKS:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		CL_SparkShower(vStartPos);
		break;
	case TE_LAVASPLASH:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		R_LavaSplash(vStartPos); // CL_
		break;
	case TE_TELEPORT:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		R_TeleportSplash(vStartPos); // CL_
		break;
	case TE_EXPLOSION2:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		int nStartColor = MSG_ReadByte();
		int nColorCount = MSG_ReadByte();

		CL_ParticleExplosion2(pos, color, count);
		break;
	case TE_BSPDECAL:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nTextureIndex = MSG_ReadShort();
		short nEntIndex = MSG_ReadShort();
		short nEntModelIndex = 0;

		if(nEntIndex)
			nEntModelIndex = MSG_ReadShort();

		CL_DecalShoot(CL_DecalIndex(decalIndex), entityIndex, modelIndex, pos, FDECAL_PERMANENT);
		break;
	case TE_IMPLOSION:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		int nRadius = MSG_ReadByte();
		int nCount = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		break;
	case TE_SPRITETRAIL:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nScale = MSG_ReadByte();
		int nVelocity = MSG_ReadByte();
		int nRandomness = MSG_ReadByte();

		CL_Sprite_Trail(type, pos, pos2, modelIndex, count, life, scale, random, 255, vel);
		break;
	case TE_BEAM:
		// obsolete, unused
		break;
	case TE_SPRITE:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nScale = MSG_ReadByte();
		int nBrightness = MSG_ReadByte();
		break;
	case TE_BEAMSPRITE:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nStartSprIndex = MSG_ReadShort();
		short nEndSprIndex = MSG_ReadShort();
		break;
	case TE_BEAMTORUS:
	case TE_BEAMDISK:
	case TE_BEAMCYLINDER:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		coord coord coord(axis and radius)

		short nSprIndex = MSG_ReadShort();

		int nStartFrame = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();
		int nNoiseAmpl = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		int nBrightness = MSG_ReadByte();
		int nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMFOLLOW:
		short nEntAttachment = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		int nBrightness = MSG_ReadByte();
		break;
	case TE_GLOWSPRITE:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		int nLifeTime = (float)(MSG_ReadByte() * 0.1f);
		int nScale = (float)(MSG_ReadByte() * 0.1f);
		int nAlpha = (float)MSG_ReadByte();
		
		if(( pTemp = CL_DefaultSprite( pos, modelIndex, 0 )) != NULL )
		{
			pTemp->entity.curstate.scale = scale;
			pTemp->entity.curstate.rendermode = kRenderGlow;
			pTemp->entity.curstate.renderfx = kRenderFxNoDissipation;
			pTemp->entity.baseline.renderamt = brightness;
			pTemp->entity.curstate.renderamt = brightness;
			pTemp->flags = FTENT_FADEOUT;
			pTemp->die = cl.time + life;
		};
		break;
	case TE_BEAMRING:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		int nStartFrame = MSG_ReadByte();
		int nFrameRate = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nLineWidth = MSG_ReadByte();
		int nNoiseAmpl = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nBrightness = MSG_ReadByte();
		int nScrollSpeed = MSG_ReadByte();
		break;
	case TE_STREAK_SPLASH:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		
		coord coord coord(direction vector)

		int nColor = MSG_ReadByte();

		short nCount = MSG_ReadShort();
		short nBaseSpeed = MSG_ReadShort();
		short nVelocity = MSG_ReadShort();
		
		CL_StreakSplash( pos, pos2, color, count, vel, -random, random );
		break;
	case TE_BEAMHOSE:
		// obsolete, unused
		break;
	case TE_DLIGHT:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		int nRadius = MSG_ReadByte();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nBrightness = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		int nDecayRate = MSG_ReadByte();
		break;
	case TE_ELIGHT:
		short(entity
		      : attachment to follow)

		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fRadius = MSG_ReadCoord();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		int nLifeTime = MSG_ReadByte();

		coord(decay rate) = MSG_ReadCoord();
		break;
	case TE_TEXTMESSAGE:
		short 1.2.13 x(-1 = center) short 1.2.13 y(-1 = center)

		int nEffect = MSG_ReadByte();
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
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nLifeTime = MSG_ReadShort();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		break;
	case TE_BOX:
		float fBoxMins[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fBoxMins[i] = MSG_ReadCoord();

		float fBoxMaxs[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fBoxMaxs[i] = MSG_ReadCoord();

		short nLifeTime = MSG_ReadShort();

		int nColor[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();
		
		break;
	case TE_KILLBEAM:
		short nEnt = MSG_ReadShort();
		break;
	case TE_LARGEFUNNEL:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();
		short nFlags = MSG_ReadShort();

		CL_FunnelSprite(pos, modelIndex, flags);
		break;
	case TE_BLOODSTREAM:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		vec3_t vSpray[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			vSpray[i] = MSG_ReadCoord();

		int nColor = MSG_ReadByte();
		int nSpeed = MSG_ReadByte();
		break;
	case TE_SHOWLINE:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		CL_ShowLine(pos, pos2);
		break;
	case TE_BLOOD: // // bullets hitting body
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		vec3_t vSpray[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			vSpray[i] = MSG_ReadCoord();

		int nColor = MSG_ReadByte();
		int nSpeed = MSG_ReadByte();

		//

		cnt = MSG_ReadByte();
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
		short nEntityIndex = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		int nDensity = MSG_ReadByte();

		pEnt = CL_GetEntityByIndex(entityIndex);
		CL_FizzEffect(pEnt, modelIndex, scale);
		break;
	case TE_MODEL:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		angle(initial yaw)

		short nModelIndex = MSG_ReadShort();

		int (bounce sound type) = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte();
		break;
	case TE_EXPLODEMODEL:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nCount = MSG_ReadShort();

		int nLifeTime = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);

		CL_TempSphereModel(pos, vel, life, count, modelIndex);
		break;
	case TE_BREAKMODEL:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fSize[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fSize[i] = MSG_ReadCoord();

		float fVelocity[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		int nVelocity = MSG_ReadByte(); // (float)BF_ReadByte( &buf );

		short nModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nLifeTime = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);
		int nFlags = MSG_ReadByte();

		CL_BreakModel(pos, pos2, ang, random, life, count, modelIndex, (char)flags);
		break;
	case TE_GUNSHOTDECAL:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nEntIndex = MSG_ReadShort();

		int nDecalIndex = MSG_ReadByte();
		
		pEnt = CL_GetEntityByIndex( entityIndex );
		CL_DecalShoot( CL_DecalIndex( decalIndex ), entityIndex, 0, pos, 0 );
		CL_BulletImpactParticles( pos );
		CL_RicochetSound( pos );
		break;
	case TE_SPRITE_SPRAY:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nSpeed = MSG_ReadByte();
		int nNoise = MSG_ReadByte();
		break;
	case TE_ARMOR_RICOCHET:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		int nScale = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);

		modelIndex = CL_FindModelIndex("sprites/richo1.spr");
		
		CL_RicochetSprite(pos, Mod_Handle(modelIndex), 0.0f, scale);
		CL_RicochetSound(pos);
		break;
	case TE_PLAYERDECAL:
		int nPlayerIndex = MSG_ReadByte();

		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nEntIndex = MSG_ReadShort();

		int nDecalNum = MSG_ReadByte();

		[optional] short (model index???)
		
		CL_PlayerDecal( CL_DecalIndex( decalIndex ), entityIndex, pos );
		break;
	case TE_BUBBLES:
		float fMinPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BUBBLETRAIL:
		float fMinPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BLOODSPRITE:
		float fPosition[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPosition[i] = MSG_ReadCoord();

		short nSpr1Index = MSG_ReadShort();
		short nSpr2Index = MSG_ReadShort();

		int nColor = MSG_ReadByte();
		int nScale = MSG_ReadByte(); // (float)BF_ReadByte( &buf );

		CL_BloodSprite(pos, color, nSpr1Index, nSpr2Index, scale);
		break;
	case TE_WORLDDECAL:
		coord, coord, coord(x, y, z),
		decal position(center of texture in world)
		
		int nTextureIndex = MSG_ReadByte();
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
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		int nLifeTime = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);
		int nOwner = MSG_ReadByte();

		CL_Projectile(fPos, fVelocity, modelIndex, life, nOwner, NULL);
		break;
	case TE_SPRAY:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fDir[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fDir[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nSpeed = MSG_ReadByte();
		int nNoise = MSG_ReadByte();
		int nRenderMode = MSG_ReadByte();
		break;
	case TE_PLAYERSPRITES:
		short nPlayerNum = MSG_ReadShort();
		short nSprModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nVariance = MSG_ReadByte(); // (float)BF_ReadByte( &buf );

		CL_PlayerSprites(nPlayerNum, nSprModelIndex, nCount, nVariance);
		break;
	case TE_PARTICLEBURST:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nRadius = MSG_ReadShort(); // (float)BF_ReadShort( &buf );

		int nParticleColor = MSG_ReadByte();
		int nDuration = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);

		CL_ParticleBurst(pos, scale, color, life);
		break;
	case TE_FIREFIELD:
		for(int i = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nRadius = MSG_ReadShort(); // (float)BF_ReadShort( &buf );
		short nModelIndex = MSG_ReadShort();

		int nCount = MSG_ReadByte();
		int nFlags = MSG_ReadByte();
		int nDuration = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);

		CL_FireField(pos, scale, modelIndex, count, flags, life);
		break;
	case TE_PLAYERATTACHMENT:
		int nPlayerEntIndex = MSG_ReadByte();

		float fVerticalOffset = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nLifeTime = MSG_ReadShort(); // (float)(BF_ReadShort( &buf ) * 0.1f);

		CL_AttachTentToPlayer(color, modelIndex, fVerticalOffset, life);
		break;
	case TE_KILLPLAYERATTACHMENTS:
		int nPlayerEntIndex = MSG_ReadByte();
		CL_KillAttachedTents(nPlayerEntIndex);
		break;
	case TE_MULTIGUNSHOT:
		//
		// This message is used to make a client approximate a 'spray' of gunfire.
		// Any weapon that fires more than one bullet per frame and fires in a bit of a spread is
		// a good candidate for MULTIGUNSHOT use. (shotguns)
		//
		// NOTE: This effect makes the client do traces for each bullet, these client traces ignore
		//		 entities that have studio models.Traces are 4096 long.
		//
		
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fDir[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fDir[i] = MSG_ReadCoord();
		
		// coord (x noise * 100)
		// coord (y noise * 100)
		
		int nCount = MSG_ReadByte();
		int nDecalIndex = MSG_ReadByte();
		
		CL_MultiGunshot( pos, pos2, ang, count, 1, decalIndices );
		break;
	case TE_USERTRACER:
		float fPos[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = { 0 };
		for(int i = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		int nLifeTime = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);
		int nColorIndex = MSG_ReadByte();
		int nLength = MSG_ReadByte(); // (float)(BF_ReadByte( &buf ) * 0.1f);

		CL_UserTracerParticle(pos, pos2, life, color, scale, 0, NULL);
		break;
	default:
		//Sys_Error("CL_ParseTEnt: bad type");
		break;
	};
};

/*
void CL_ParseTEnt()
{
        vec3_t       pos;
        dlight_t *   dl;
        int          rnd;
        explosion_t *ex;
        int          cnt;

        switch(type)
        {

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