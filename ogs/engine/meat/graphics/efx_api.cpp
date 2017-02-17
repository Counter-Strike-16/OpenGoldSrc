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

#include "precompiled.hpp"

efx_api_t gEfxAPI =
    {
        CL_AllocParticle,
        (void *)R_BlobExplosion,
		
        (void *)R_Blood,
        (void *)R_BloodSprite,
        (void *)R_BloodStream,
		
        (void *)CL_BreakModel,
		
        (void *)R_Bubbles,
        (void *)CL_BubbleTrail,
		
        (void *)CL_BulletImpactParticles,
        CL_EntityParticles,
        R_Explosion,
        R_FizzEffect,
        R_FireField,
        (void *)CL_FlickerParticles,
        (void *)CL_FunnelSprite,
        (void *)R_Implosion,
        (void *)CL_Large_Funnel,
        (void *)R_LavaSplash,
        (void *)R_MultiGunshot,
        (void *)R_MuzzleFlash,
		
        (void *)CL_ParticleBox,
        (void *)CL_ParticleBurst,
        (void *)CL_ParticleExplosion,
        (void *)CL_ParticleExplosion2,
        (void *)CL_ParticleLine,
		
        CL_PlayerSprites,
        (void *)CL_Projectile,
		
        (void *)CL_RicochetSound,
        (void *)CL_RicochetSprite,
		
        (void *)CL_RocketFlare,
        CL_RocketTrail,
		
        (void *)CL_RunParticleEffect,
        (void *)CL_ShowLine,
		
        (void *)CL_SparkEffect,
        (void *)CL_SparkShower,
        (void *)CL_SparkStreaks,
		
        (void *)CL_Spray,
        CL_Sprite_Explode,
        CL_Sprite_Smoke,
        (void *)CL_Sprite_Spray,
        (void *)CL_Sprite_Trail,
        CL_Sprite_WallPuff,
        (void *)CL_StreakSplash,
        (void *)CL_TracerEffect,
        CL_UserTracerParticle,
        CL_TracerParticles,
        (void *)CL_TeleportSplash,
        (void *)CL_TempSphereModel,
        (void *)CL_TempModel,
        (void *)CL_DefaultSprite,
        (void *)CL_TempSprite,
        CL_DecalIndex,
        (void *)CL_DecalIndexFromName,
        CL_DecalShoot,
        CL_AttachTentToPlayer,
        CL_KillAttachedTents,
        (void *)CL_BeamCirclePoints,
        (void *)CL_BeamEntPoint,
        CL_BeamEnts,
        CL_BeamFollow,
        CL_BeamKill,
        (void *)CL_BeamLightning,
        (void *)CL_BeamPoints,
        (void *)CL_BeamRing,
        CL_AllocDlight,
        CL_AllocElight,
        (void *)CL_TempEntAlloc,
        (void *)CL_TempEntAllocNoModel,
        (void *)CL_TempEntAllocHigh,
        (void *)CL_TempEntAllocCustom,
        CL_GetPackedColor,
        CL_LookupColor,
        CL_DecalRemoveAll,
        CL_FireCustomDecal};