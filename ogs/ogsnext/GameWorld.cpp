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

#include "world/GameWorld.hpp"

CGameWorld::CGameWorld(){};

CGameWorld::~CGameWorld(){};

void CGameWorld::Init()
{
	Con_DPrintf("World module initialized.\n");
};

void CGameWorld::Shutdown()
{
	Con_DPrintf("World module shutdown.\n");
};

/*
=============================================================================

EVENT MESSAGES

=============================================================================
*/

/*  
==================
SV_StartSound

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

Channel 0 is an auto-allocate channel, the others override anything
allready running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.  (max 4 attenuation)

==================
*/
void CGameWorld::StartSound(edict_t *entity, int channel, char *sample, int volume, float attenuation)
{
	int sound_num;
	int field_mask;
	int i;
	int ent;
	vec3_t origin;
	qboolean use_phs;
	qboolean reliable = false;

	if(volume < 0 || volume > 255)
		Sys_Error("SV_StartSound: volume = %i", volume);

	if(attenuation < 0 || attenuation > 4)
		Sys_Error("SV_StartSound: attenuation = %f", attenuation);

	if(channel < 0 || channel > 15) // 7
		Sys_Error("SV_StartSound: channel = %i", channel);

	// find precache number for sound
	for(sound_num = 1; sound_num < MAX_SOUNDS && sv.sound_precache[sound_num]; sound_num++)
		if(!strcmp(sample, sv.sound_precache[sound_num]))
			break;

	if(sound_num == MAX_SOUNDS || !sv.sound_precache[sound_num])
	{
		Con_Printf("SV_StartSound: %s not precacheed\n", sample);
		return;
	}

	ent = NUM_FOR_EDICT(entity);

	if((channel & 8) || !sv_phs.value) // no PHS flag
	{
		if(channel & 8)
			reliable = true; // sounds that break the phs are reliable
		use_phs = false;
		channel &= 7;
	}
	else
		use_phs = true;

	//	if (channel == CHAN_BODY || channel == CHAN_VOICE)
	//		reliable = true;

	channel = (ent << 3) | channel;

	field_mask = 0;
	if(volume != DEFAULT_SOUND_PACKET_VOLUME)
		channel |= SND_VOLUME;
	if(attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
		channel |= SND_ATTENUATION;

	// use the entity origin unless it is a bmodel
	if(entity->v.solid == SOLID_BSP)
	{
		for(i = 0; i < 3; i++)
			origin[i] = entity->v.origin[i] + 0.5 * (entity->v.mins[i] + entity->v.maxs[i]);
	}
	else
		VectorCopy(entity->v.origin, origin);

	MSG_WriteByte(&sv.multicast, svc_sound);
	MSG_WriteShort(&sv.multicast, channel);
	if(channel & SND_VOLUME)
		MSG_WriteByte(&sv.multicast, volume);
	if(channel & SND_ATTENUATION)
		MSG_WriteByte(&sv.multicast, attenuation * 64);
	MSG_WriteByte(&sv.multicast, sound_num);
	for(i = 0; i < 3; i++)
		MSG_WriteCoord(&sv.multicast, origin[i]);

	if(use_phs)
		SV_Multicast(origin, reliable ? MULTICAST_PHS_R : MULTICAST_PHS);
	else
		SV_Multicast(origin, reliable ? MULTICAST_ALL_R : MULTICAST_ALL);
};

/*  
==================
SV_StartParticle

Make sure the event gets sent to all clients
==================
*/
void CGameWorld::StartParticle(vec3_t org, vec3_t dir, int color, int count)
{
	int i, v;

	if(sv.datagram.cursize > MAX_DATAGRAM - 16)
		return;

	MSG_WriteByte(&sv.datagram, svc_particle);

	MSG_WriteCoord(&sv.datagram, org[0]);
	MSG_WriteCoord(&sv.datagram, org[1]);
	MSG_WriteCoord(&sv.datagram, org[2]);

	for(i = 0; i < 3; i++)
	{
		v = dir[i] * 16;
		if(v > 127)
			v = 127;
		else if(v < -128)
			v = -128;
		MSG_WriteChar(&sv.datagram, v);
	};

	MSG_WriteByte(&sv.datagram, count);
	MSG_WriteByte(&sv.datagram, color);
};