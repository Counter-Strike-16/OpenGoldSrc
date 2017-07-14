/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief entity parsing and management

#include "precompiled.hpp"
#include "client/client.hpp"
#include "network/net_msg.hpp"

/*
===============
CL_SetSolid

Builds all the pmove physents for the current frame
Note that CL_SetUpPlayerPrediction() must be called first!
pmove must be setup with world and solid entity hulls before calling
(via CL_PredictMove)
===============
*/
void CL_SetSolidPlayers(int playernum)
{
	if(!cl_solid_players.value)
		return;

	extern vec3_t player_mins;
	extern vec3_t player_maxs;

	struct predicted_player *pplayer;

	physent_t *pent = pmove.physents + pmove.numphysent;

	for(int j = 0, pplayer = predicted_players; j < MAX_CLIENTS; j++, pplayer++)
	{
		if(!pplayer->active)
			continue; // not present this frame

		// the player object never gets added
		if(j == playernum)
			continue;

		if(pplayer->flags & PF_DEAD)
			continue; // dead players aren't solid

		pent->model = 0;
		VectorCopy(pplayer->origin, pent->origin);
		VectorCopy(player_mins, pent->mins);
		VectorCopy(player_maxs, pent->maxs);
		pmove.numphysent++;
		pent++;
	};
};

/*
===============
CL_SetSolid

Builds all the pmove physents for the current frame
===============
*/
void CL_SetSolidEntities()
{
	int i;
	frame_t *frame;
	packet_entities_t *pak;
	entity_state_t *state;

	pmove.physents[0].model = cl.worldmodel;
	VectorCopy(vec3_origin, pmove.physents[0].origin);
	pmove.physents[0].info = 0;
	pmove.numphysent = 1;

	frame = &cl.frames[parsecountmod];
	pak = &frame->packet_entities;

	for(i = 0; i < pak->num_entities; i++)
	{
		state = &pak->entities[i];

		if(!state->modelindex)
			continue;
		if(!cl.model_precache[state->modelindex])
			continue;
		if(cl.model_precache[state->modelindex]->hulls[1].firstclipnode ||
		   cl.model_precache[state->modelindex]->clipbox)
		{
			pmove.physents[pmove.numphysent].model =
			cl.model_precache[state->modelindex];
			VectorCopy(state->origin, pmove.physents[pmove.numphysent].origin);
			pmove.numphysent++;
		};
	};
};

/*
===============
CL_EmitEntities

Builds the visedicts array for cl.time

Made up of: clients, packet_entities, nails, and tents
===============
*/
void CL_EmitEntities()
{
#ifndef SWDS
	if(cls.state != ca_active)
		return;

	if(!cl.validsequence)
		return;

	cl_oldnumvisedicts = cl_numvisedicts;
	cl_oldvisedicts = cl_visedicts_list[(cls.netchan.incoming_sequence - 1) & 1];
	cl_visedicts = cl_visedicts_list[cls.netchan.incoming_sequence & 1];

	cl_numvisedicts = 0;

	CL_LinkPlayers();
	CL_LinkPacketEntities();
	CL_LinkProjectiles();
	CL_UpdateTEnts();
#endif // SWDS
};