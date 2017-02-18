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

#else // if OGS_EFX_API_NULL_IMPL defined

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