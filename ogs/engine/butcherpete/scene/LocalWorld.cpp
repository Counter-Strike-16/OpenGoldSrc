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

#include "LocalWorld.hpp"

void CLocalWorld::Update(float afTimeStep)
{
	RunParticles();
	RunDLights();
	RunLightStyles();
};

dlight_t *CLocalWorld::AllocDlight(int key)
{
	int i;
	dlight_t *dl;

	// first look for an exact key match
	if(key)
	{
		dl = cl_dlights;
		for(i = 0; i < MAX_DLIGHTS; i++, dl++)
		{
			if(dl->key == key)
			{
				memset(dl, 0, sizeof(*dl));
				dl->key = key;
				return dl;
			}
		}
	}

	// then look for anything else
	dl = cl_dlights;
	for(i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if(dl->die < cl.time)
		{
			memset(dl, 0, sizeof(*dl));
			dl->key = key;
			return dl;
		}
	}

	dl = &cl_dlights[0];
	memset(dl, 0, sizeof(*dl));
	dl->key = key;
	return dl;
};

/*
=================
CL_NewTempEntity
=================
*/
cl_entity_t *CLocalWorld::NewTempEntity()
{
	if(cl_numvisedicts == MAX_VISEDICTS)
		return NULL;
	
	cl_entity_t *ent = &cl_visedicts[cl_numvisedicts];

	cl_numvisedicts++;
	ent->keynum = 0;

	memset(ent, 0, sizeof(*ent));

	ent->colormap = vid.colormap;
	return ent;
};

/*
=================
CL_AllocExplosion
=================
*/
explosion_t *CLocalWorld::AllocExplosion()
{
	int i;

	for(i = 0; i < MAX_EXPLOSIONS; ++i)
		if(!cl_explosions[i].model)
			return &cl_explosions[i];

	// find the oldest explosion
	float time = cl.time; // int
	int index = 0;

	for(i = 0; i < MAX_EXPLOSIONS; i++)
	{
		if(cl_explosions[i].start < time)
		{
			time = cl_explosions[i].start;
			index = i;
		};
	};

	return &cl_explosions[index];
};

/*
=================
CL_UpdateTEnts
=================
*/
void CLocalWorld::UpdateTEnts()
{
	UpdateBeams();
	UpdateExplosions();
};

/*
=================
CL_UpdateBeams
=================
*/
void CLocalWorld::UpdateBeams()
{
	int i;
	beam_t *b;
	vec3_t dist, org;
	float d;
	cl_entity_t *ent;
	float yaw, pitch;
	float forward;

	// update lightning
	for(i = 0, b = cl_beams; i < MAX_BEAMS; i++, b++)
	{
		if(!b->model || b->endtime < cl.time)
			continue;

		// if coming from the player, update the start position
		if(b->entity == cl.playernum + 1) // entity 0 is the world
		{
			VectorCopy(cl.simorg, b->start);
			//			b->start[2] -= 22;	// adjust for view
			//height
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
			pitch = (int)(atan2(dist[2], forward) * 180 / M_PI);
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
			ent->model = b->model;
			ent->angles[0] = pitch;
			ent->angles[1] = yaw;
			ent->angles[2] = rand() % 360;

			for(i = 0; i < 3; i++)
				org[i] += dist[i] * 30;
			d -= 30;
		}
	}
};


/*
=================
CL_UpdateExplosions
=================
*/
void CLocalWorld::UpdateExplosions()
{
	int i;
	int f;
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
};

/*
===============
CL_DecayLights
===============
*/
void CLocalWorld::DecayLights()
{
	dlight_t *dl = cl_dlights;

	for(int i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if(dl->die < cl.time || !dl->radius)
			continue;

		dl->radius -= host_frametime * dl->decay;
		if(dl->radius < 0)
			dl->radius = 0;
	};
};

/*
=================
CL_ClearTEnts
=================
*/
void CLocalWorld::ClearTEnts()
{
	memset(&cl_beams, 0, sizeof(cl_beams));
	memset(&cl_explosions, 0, sizeof(cl_explosions));
	memset(cl_lasers, 0, sizeof(cl_lasers));
	memset(cl_playerbeams, 0, sizeof(cl_playerbeams));
	memset(cl_sustains, 0, sizeof(cl_sustains));
};