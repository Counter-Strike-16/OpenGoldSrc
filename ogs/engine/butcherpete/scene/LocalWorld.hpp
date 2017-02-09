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
/// @brief client side world representation

#pragma once

#include <list>
#include "IWorld.hpp"

const int MAX_BEAMS = 32; //8
const int MAX_EXPLOSIONS = 32; //8
const int MAX_LASERS = 32;

class CParicleEntity;
class CLightEntity;

typedef tParticleEntityList std::list<CParicleEntity*>;

typedef tLightEntityList std::list<CLightEntity*>;

typedef struct dlight_s dlight_t;

enum exptype_t
{
	ex_free,
	ex_explosion,
	ex_misc,
	ex_flash,
	ex_mflash,
	ex_poly,
	ex_poly2
};

typedef struct
{
	vec3_t origin;
	float start;
	model_t *model;
	
	exptype_t	type;
	cl_entity_t	ent;

	int			frames;
	float		light;
	vec3_t		lightcolor;
	int			baseframe;
} explosion_t;

typedef struct
{
	cl_entity_t ent;
	int endtime;
} laser_t;

class CLocalWorld : public IWorld
{
public:
	void Update(float afTimeStep);
	
	void Clear();
	
	void SetLightstyle(int anType);
	
	dlight_t *AllocDlight(int anKey);
	
	CParicleEntity *CreateParticle();
	CLightEntity *CreateLight();
	
	void SpawnSmokeAndFlash(vec3_t origin);
	void SpawnBigTeleportParticles(vec3_t org);
	void SpawnRocketTrail(vec3_t start, vec3_t end, cl_entity_t *old);
	void SpawnDiminishingTrail(vec3_t start, vec3_t end, cl_entity_t *old, int flags);
	void SpawnFlyEffect(cl_entity_t *ent, vec3_t origin);
	void TeleportParticles(vec3_t org);
	void BlasterParticles(vec3_t org, vec3_t dir);
	void ExplosionParticles(vec3_t org);
	void BlueBlasterParticles(vec3_t org, vec3_t dir);
	
	void BfgParticles(cl_entity_t *ent);
	void AddParticles();
	void EntityEvent(entity_state_t *ent);
	void TrapParticles(cl_entity_t *ent);
private:
	void UpdateTEnts();
	void UpdateBeams();
	void UpdateExplosions();
	void RunParticles();
	void RunDLights();
	void RunLightStyles();
	void DecayLights();
	
	void ClearTEnts();
	
	// FIXME: allocate dynamically
	beam_t cl_beams[MAX_BEAMS]; // player-linked beams; cl_playerbeams
	entity_state_t cl_baselines[MAX_EDICTS];
	efrag_t cl_efrags[MAX_EFRAGS];
	entity_t cl_static_entities[MAX_STATIC_ENTITIES];
	lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
	dlight_t cl_dlights[MAX_DLIGHTS];
	cl_entity_t cl_entities[MAX_EDICTS];
	explosion_t cl_explosions[MAX_EXPLOSIONS];
	laser_t cl_lasers[MAX_LASERS];
	cl_sustain_t cl_sustains[MAX_SUSTAINS];
};