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
/// @brief client side prediction impl

#include "precompiled.hpp"
#include "client/client.hpp"
#include "console/console.hpp"
#include "console/cvar.hpp"

cvar_t cl_nopred = { "cl_nopred", "0" };
cvar_t cl_pushlatency = { "pushlatency", "-999" };

extern frame_t *view_frame;

/*
=================
CL_NudgePosition

If pmove.origin is in a solid position,
try nudging slightly on all axis to
allow for the cut precision of the net coordinates
=================
*/
void CL_NudgePosition()
{
	/*
	vec3_t base;
	int x, y;

	if(PM_HullPointContents(&cl.model_precache[1]->hulls[1], 0, pmove.origin) ==
	   CONTENTS_EMPTY)
		return;

	VectorCopy(pmove.origin, base);
	for(x = -1; x <= 1; x++)
	{
		for(y = -1; y <= 1; y++)
		{
			pmove.origin[0] = base[0] + x * 1.0 / 8;
			pmove.origin[1] = base[1] + y * 1.0 / 8;
			if(PM_HullPointContents(&cl.model_precache[1]->hulls[1], 0, pmove.origin) == CONTENTS_EMPTY)
				return;
		};
	};

	Con_DPrintf("CL_NudgePosition: stuck\n");
	*/
};

/*
==============
CL_PredictUsercmd
==============
*/
void CL_PredictUsercmd(entity_state_t *from, entity_state_t *to, usercmd_t *u, 
qboolean spectator) // was player_state_t, but should be local_state_t i think
{
	/*
	// split up very long moves
	if(u->msec > 50)
	{
		entity_state_t temp;
		usercmd_t split;

		split = *u;
		split.msec /= 2;

		CL_PredictUsercmd(from, &temp, &split, spectator);
		CL_PredictUsercmd(&temp, to, &split, spectator);
		return;
	};

	VectorCopy(from->origin, pmove.origin);
	// VectorCopy(from->viewangles, pmove.angles);
	VectorCopy(u->angles, pmove.angles);
	VectorCopy(from->velocity, pmove.velocity);

	pmove.oldbuttons = from->oldbuttons;
	pmove.waterjumptime = from->waterjumptime;
	pmove.dead = cl.stats[STAT_HEALTH] <= 0;
	pmove.spectator = spectator;

	pmove.cmd = *u;

	ClientDLL_MoveClient(&pmove);

	// for (i=0 ; i<3 ; i++)
	// pmove.origin[i] = ((int)(pmove.origin[i]*8))*0.125;

	to->waterjumptime = pmove.waterjumptime;
	to->oldbuttons = pmove.cmd.buttons;
	VectorCopy(pmove.origin, to->origin);
	VectorCopy(pmove.angles, to->viewangles);
	VectorCopy(pmove.velocity, to->velocity);
	to->onground = onground;

	to->weaponframe = from->weaponframe;
	*/
};

/*
==============
CL_PredictMove
==============
*/
void CL_PredictMove(qboolean repredicting)
{
	/*
	int i;
	float f;
	frame_t *from, *to = NULL;
	int oldphysent;

	if(cl_pushlatency.value > 0)
		Cvar_Set("pushlatency", "0");

	if(cl.paused)
		return;

	cl.time = realtime - cls.latency - cl_pushlatency.value * 0.001;
	if(cl.time > realtime)
		cl.time = realtime;

	if(cl.intermission)
		return;

	if(!cl.validsequence)
		return;

	if(cls.netchan.outgoing_sequence - cls.netchan.incoming_sequence >=
	   SV_UPDATE_BACKUP - 1)
		return;

	VectorCopy(cl.viewangles, cl.simangles);

	// this is the last frame received from the server
	from = &cl.frames[cls.netchan.incoming_sequence & SV_UPDATE_MASK];

	// we can now render a frame
	if(cls.state == ca_connected)
	{
		// first update is the final signon stage
		cls.state = ca_active;
	};

	if(cl_nopred.value)
	{
		VectorCopy(from->playerstate[cl.playernum].velocity, cl.simvel);
		VectorCopy(from->playerstate[cl.playernum].origin, cl.simorg);
		return;
	};

	// predict forward until cl.time <= to->senttime
	oldphysent = pmove.numphysent;
	CL_SetSolidPlayers(cl.playernum);

	//	to = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];

	for(i = 1; i < SV_UPDATE_BACKUP - 1 &&
	    cls.netchan.incoming_sequence + i < cls.netchan.outgoing_sequence;
	    i++)
	{
		to = &cl.frames[(cls.netchan.incoming_sequence + i) & SV_UPDATE_MASK];
		CL_PredictUsercmd(&from->playerstate[cl.playernum],
		                  &to->playerstate[cl.playernum],
		                  &to->cmd,
		                  cls.spectator);
		if(to->senttime >= cl.time)
			break;
		from = to;
	};

	pmove.numphysent = oldphysent;

	if(i == SV_UPDATE_BACKUP - 1 || !to)
		return; // net hasn't deliver packets in a long time...

	// now interpolate some fraction of the final frame
	if(to->senttime == from->senttime)
		f = 0;
	else
	{
		f = (cl.time - from->senttime) / (to->senttime - from->senttime);

		if(f < 0)
			f = 0;
		if(f > 1)
			f = 1;
	};

	for(i = 0; i < 3; i++)
		if(fabs(from->playerstate[cl.playernum].origin[i] -
		        to->playerstate[cl.playernum].origin[i]) >
		   128)
		{ // teleported, so don't lerp
			VectorCopy(to->playerstate[cl.playernum].velocity, cl.simvel);
			VectorCopy(to->playerstate[cl.playernum].origin, cl.simorg);
			return;
		};

	for(i = 0; i < 3; i++)
	{
		cl.simorg[i] = from->playerstate[cl.playernum].origin[i] +
		f * (to->playerstate[cl.playernum].origin[i] -
		     from->playerstate[cl.playernum].origin[i]);
		cl.simvel[i] = from->playerstate[cl.playernum].velocity[i] +
		f * (to->playerstate[cl.playernum].velocity[i] -
		     from->playerstate[cl.playernum].velocity[i]);
	};
	*/
};

/*
==============
CL_InitPrediction
==============
*/
void CL_InitPrediction()
{
	Cvar_RegisterVariable(&cl_pushlatency);
	Cvar_RegisterVariable(&cl_nopred);
};

void CL_RedoPrediction(){};