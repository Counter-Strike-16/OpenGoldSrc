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

//#include "precompiled.hpp"
#include "common/event_api.h"
#include "physics/sv_pmove.hpp"
#include "client/client.hpp"
#include "pm_shared/pm_defs.h"
#include <cstddef>

namespace
{

#ifndef OGS_EVENTAPI_NULL_IMPL
void EventAPI_PlaySound(int ent, float *origin, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch){};

void EventAPI_StopSound(int ent, int channel, const char *sample){};

int EventAPI_FindModelIndex(const char *pmodel)
{
	return 0;
};

int EventAPI_IsLocal /*Player*/ (int playernum)
{
	if(playernum == cl.playernum)
		return true;

	return false;
};

int EventAPI_LocalPlayerDucking()
{
	//return cl.predicted.usehull == 1;
	return 0;
};

void EventAPI_LocalPlayerViewheight(float *view_ofs)
{
	// predicted or smoothed
	if(!view_ofs)
		return;

	//if(CL_IsPredictionActive())
		//VectorCopy(cl.predicted.viewofs, view_ofs);
	//else
		//VectorCopy(cl.frame.client.view_ofs, view_ofs);
};

void EventAPI_LocalPlayerBounds(int hull, float *mins, float *maxs)
{
	if(hull >= 0 && hull < 4)
	{
		if(mins)
			VectorCopy(g_clmove.player_mins[hull], mins);

		if(maxs)
			VectorCopy(g_clmove.player_maxs[hull], maxs);
	};
};

int EventAPI_IndexFromTrace(struct pmtrace_s *pTrace)
{
	if(pTrace->ent >= 0 && pTrace->ent < g_clmove.numphysent)
	{
		// return cl.entities number
		return g_clmove.physents[pTrace->ent].info;
	};

	return -1;
};

struct physent_s *EventAPI_GetPhysent(int idx)
{
	if(idx >= 0 && idx < g_clmove.numphysent)
	{
		// return physent
		return &g_clmove.physents[idx];
	};

	return NULL;
};

void EventAPI_SetUpPlayerPrediction(int dopred, int bIncludeLocalClient){};

void EventAPI_PushPMStates(){};

void EventAPI_PopPMStates(){};

void EventAPI_SetSolidPlayers(int playernum){};

void EventAPI_SetTraceHull(int hull){};

void EventAPI_PlayerTrace(float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr){};

void EventAPI_WeaponAnimation(int sequence, int body){};

unsigned short EventAPI_PrecacheEvent(int type, const char *psz)
{
	//return CL_EventIndex(psz);
	return 0;
};

void EventAPI_PlaybackEvent(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2){};

const char *EventAPI_TraceTexture(int ground, float *vstart, float *vend)
{
	if(ground < 0 || ground >= g_clmove.numphysent)
		return NULL; // bad ground

	physent_t *pe = &g_clmove.physents[ground];

	return PM_SV_TraceTexture(pe, vstart, vend);
};

void EventAPI_StopAllSounds(int entnum, int entchannel){};

void EventAPI_KillEvents(int entnum, const char *eventname)
{
	int eventIndex = 0; //CL_EventIndex(eventname);

	//if(eventIndex < 0 || eventIndex >= MAX_EVENTS)
		//return;

	//if(entnum < 0 || entnum > clgame.maxEntities)
		//return;

	event_state_t *es = &cl.events;
	event_info_t *ei = nullptr;

	// find all events with specified index and kill it
	for(int i = 0; i < MAX_EVENT_QUEUE; ++i)
	{
		ei = &es->ei[i];

		if(ei->index == eventIndex && ei->entity_index == entnum)
		{
			//CL_ResetEvent(ei);
			break;
		}
	}
};
#else // if OGS_EVENTAPI_NULL_IMPL
void EventAPI_PlaySound(int ent, float *origin, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch){};

void EventAPI_StopSound(int ent, int channel, const char *sample){};

int EventAPI_FindModelIndex(const char *pmodel)
{
	return 0;
};

int EventAPI_IsLocal /*Player*/ (int playernum)
{
	return 0;
};

int EventAPI_LocalPlayerDucking()
{
	return 0;
};

void EventAPI_LocalPlayerViewheight(float *){};

void EventAPI_LocalPlayerBounds(int hull, float *mins, float *maxs){};

int EventAPI_IndexFromTrace(struct pmtrace_s *pTrace)
{
	return 0;
};

struct physent_s *EventAPI_GetPhysent(int idx)
{
	return NULL;
};

void EventAPI_SetUpPlayerPrediction(int dopred, int bIncludeLocalClient){};

void EventAPI_PushPMStates(){};

void EventAPI_PopPMStates(){};

void EventAPI_SetSolidPlayers(int playernum){};

void EventAPI_SetTraceHull(int hull){};

void EventAPI_PlayerTrace(float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr){};

void EventAPI_WeaponAnimation(int sequence, int body){};

unsigned short EventAPI_PrecacheEvent(int type, const char *psz)
{
	return 0;
};

void EventAPI_PlaybackEvent(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2){};

const char *EventAPI_TraceTexture(int ground, float *vstart, float *vend)
{
	return "";
};

void EventAPI_StopAllSounds(int entnum, int entchannel){};

void EventAPI_KillEvents(int entnum, const char *eventname){};
#endif // OGS_EVENTAPI_NULL_IMPL

}; // namespace

/*
static event_api_t gEventApi =
{
	pfnPlaySound,
	pfnIsLocal,
	pfnLocalPlayerDucking,
	pfnLocalPlayerViewheight,
	pfnLocalPlayerBounds,
	pfnIndexFromTrace,
	pfnGetPhysent,
	pfnPrecacheEvent,
	pfnTraceTexture,
	pfnStopAllSounds,
	pfnKillEvents,
	
	CL_EventIndex,
	CL_IndexEvent,
	CL_PlayerTraceExt,
	CL_SoundFromIndex,
	pfnTraceSurface,
};

event_api_t gEventAPI = { EVENT_API_VERSION,

	                      EventAPI_PlaySound,
	                      EventAPI_StopSound,

	                      EventAPI_FindModelIndex,

	                      EventAPI_IsLocal /*Player*,
	                      EventAPI_LocalPlayerDucking,
	                      EventAPI_LocalPlayerViewheight,
	                      EventAPI_LocalPlayerBounds,

	                      EventAPI_IndexFromTrace,

	                      EventAPI_GetPhysent,

	                      EventAPI_SetUpPlayerPrediction,

	                      EventAPI_PushPMStates,
	                      EventAPI_PopPMStates,

	                      EventAPI_SetSolidPlayers,

	                      EventAPI_SetTraceHull,
	                      EventAPI_PlayerTrace,

	                      EventAPI_WeaponAnimation,

	                      EventAPI_PrecacheEvent,
	                      EventAPI_PlaybackEvent,

	                      EventAPI_TraceTexture,
	                      EventAPI_StopAllSounds,
	                      EventAPI_KillEvents };
*/

event_api_t gEventAPI =
    {
        EVENT_API_VERSION,

        EventAPI_PlaySound,
        EventAPI_StopSound, // S_StopSound

        EventAPI_FindModelIndex, // CL_FindModelIndex

        EventAPI_IsLocal /*Player*/,
        EventAPI_LocalPlayerDucking,
        EventAPI_LocalPlayerViewheight,
        EventAPI_LocalPlayerBounds,

        EventAPI_IndexFromTrace,

        EventAPI_GetPhysent,

        EventAPI_SetUpPlayerPrediction, // CL_SetUpPlayerPrediction

        EventAPI_PushPMStates, // CL_PushPMStates
        EventAPI_PopPMStates,  // CL_PopPMStates

        EventAPI_SetSolidPlayers, // CL_SetSolidPlayers

        EventAPI_SetTraceHull, // CL_SetTraceHull
        EventAPI_PlayerTrace,  // CL_PlayerTrace

        EventAPI_WeaponAnimation, // CL_WeaponAnim

        EventAPI_PrecacheEvent,
        EventAPI_PlaybackEvent, // CL_PlaybackEvent,

        EventAPI_TraceTexture,
        EventAPI_StopAllSounds,
        EventAPI_KillEvents};