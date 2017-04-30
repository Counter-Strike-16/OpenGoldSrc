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
/// @brief authoritative server side world

#include "precompiled.hpp"
#include "world/GameWorld.hpp"

CGameWorld::CGameWorld()
{
};

CGameWorld::~CGameWorld()
{
};

void CGameWorld::Init()
{
};

void CGameWorld::Shutdown()
{
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

void CGameWorld::Update()
{
	if(!mbActive)
		return;
};

void CGameWorld::SetActive(bool abActive)
{
	mbActive = abActive;
};

bool CGameWorld::IsActive()
{
	return mbActive;
};

edict_t *CGameWorld::CreateEntity(int className)
{
	edict_t *pEdict = ED_Alloc();
	
	if(className)
	{
		pEdict->v.classname = className;
		
		ENTITYINIT pEntityInit = GetEntityInit(&pr_strings[className]);
		
		if(pEntityInit)
		{
			pEntityInit(&pEdict->v);
			return pEdict;
		}
		else
		{
			ED_Free(pEdict);
			mpConsole->DevPrintf("Can't create entity: %s\n", &pr_strings[className]);
			return nullptr;
		};
	};
	
	return pEdict;
};

void CGameWorld::RemoveEntity(edict_t *ed)
{
	g_RehldsHookchains.m_PF_Remove_I.callChain(PF_Remove_I_internal, ed);
};

edict_t *CGameWorld::FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue) const
{
	if(!pszValue)
		return NULL;

	int iField = iGetIndex(pszField);
	
	if(iField == -1)
		return NULL;

	return PF_find_Shared(pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0, iField, pszValue);
};

edict_t *CGameWorld::FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad) const
{
	int e = pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0;

	for(int i = e + 1; i < g_psv.num_edicts; i++)
	{
		edict_t *ent = &g_psv.edicts[i];
		if(ent->free || !ent->v.classname)
			continue;

		if(i <= g_psvs.maxclients && !g_psvs.clients[i - 1].active)
			continue;

		float distSquared = 0.0;
		for(int j = 0; j < 3 && distSquared <= (rad * rad); j++)
		{
			float eorg;
			if(org[j] >= ent->v.absmin[j])
				eorg = (org[j] <= ent->v.absmax[j]) ? 0.0f : org[j] - ent->v.absmax[j];
			else
				eorg = org[j] - ent->v.absmin[j];
			distSquared = eorg * eorg + distSquared;
		}

		if(distSquared <= ((rad * rad)))
			return ent;
	}

	return &g_psv.edicts[0];
};

edict_t *CGameWorld::FindClientInPVS(edict_t *pEdict) const
{
	return nullptr;
};

edict_t *CGameWorld::GetEntitiesInPVS(edict_t *pplayer) const
{
	return nullptr;
};

void CGameWorld::EmitSound(edict_t *entity, int channel, const char *sample, /*int*/ float volume, float attenuation, int fFlags, int pitch)
{
};

void CGameWorld::EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	int i;
	int soundnum;
	int ent;
	sizebuf_t *pout;

	if(samp[0] == '!')
	{
		fFlags |= SND_FL_SENTENCE;
		soundnum = Q_atoi(samp + 1);
		if(soundnum >= CVOXFILESENTENCEMAX)
		{
			mpConsole->Printf("invalid sentence number: %s", &samp[1]);
			return;
		}
	}
	else
	{
		for(i = 0; i < MAX_SOUNDS; i++)
		{
			if(g_psv.sound_precache[i] &&
			   !Q_stricmp(g_psv.sound_precache[i], samp))
			{
				soundnum = i;
				break;
			}
		}

		if(i == MAX_SOUNDS)
		{
			mpConsole->Printf("no precache: %s\n", samp);
			return;
		}
	}

	ent = NUM_FOR_EDICT(entity);
	pout = &g_psv.signon;
	if(!(fFlags & SND_FL_SPAWNING))
		pout = &g_psv.datagram;

	MSG_WriteByte(pout, svc_spawnstaticsound);
	MSG_WriteCoord(pout, pos[0]);
	MSG_WriteCoord(pout, pos[1]);
	MSG_WriteCoord(pout, pos[2]);

	MSG_WriteShort(pout, soundnum);
	MSG_WriteByte(pout, (vol * 255.0));
	MSG_WriteByte(pout, (attenuation * 64.0));
	MSG_WriteShort(pout, ent);
	MSG_WriteByte(pout, pitch);
	MSG_WriteByte(pout, fFlags);
};

void CGameWorld::SetLightStyle(int style, char *val)
{
	g_psv.lightstyles[style] = val;
	
	if(g_psv.state != ss_active)
		return;

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		if((cl->active || cl->spawned) && !cl->fakeclient)
		{
			MSG_WriteChar(&cl->netchan.message, svc_lightstyle);
			MSG_WriteChar(&cl->netchan.message, style);
			MSG_WriteString(&cl->netchan.message, val);
		}
	}
};

void *CGameWorld::AllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	return nullptr;
};

edict_t *CGameWorld::CreateFakeClient(const char *netname) const
{
	client_t *fakeclient;
	edict_t *ent;

	int i = 0;
	fakeclient = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, fakeclient++)
	{
		if(!fakeclient->active && !fakeclient->spawned && !fakeclient->connected)
			break;
	}

	if(i >= g_psvs.maxclients)
		return NULL;

	ent = EDICT_NUM(i + 1);
	if(fakeclient->frames)
		SV_ClearFrames(&fakeclient->frames);

	Q_memset(fakeclient, 0, sizeof(client_t));
	fakeclient->resourcesneeded.pPrev = &fakeclient->resourcesneeded;
	fakeclient->resourcesneeded.pNext = &fakeclient->resourcesneeded;
	fakeclient->resourcesonhand.pPrev = &fakeclient->resourcesonhand;
	fakeclient->resourcesonhand.pNext = &fakeclient->resourcesonhand;

	Q_strncpy(fakeclient->name, netname, sizeof(fakeclient->name) - 1);
	fakeclient->name[sizeof(fakeclient->name) - 1] = 0;

	fakeclient->active = TRUE;
	fakeclient->spawned = TRUE;
	fakeclient->fully_connected = TRUE;
	fakeclient->connected = TRUE;
	fakeclient->fakeclient = TRUE;
	fakeclient->userid = g_userid++;
	fakeclient->uploading = FALSE;
	fakeclient->edict = ent;
	ent->v.netname = (size_t)fakeclient->name - (size_t)pr_strings;
	ent->v.pContainingEntity = ent;
	ent->v.flags = FL_FAKECLIENT | FL_CLIENT;

	Info_SetValueForKey(fakeclient->userinfo, "name", netname, MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "model", "gordon", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "topcolor", "1", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "bottomcolor", "1", MAX_INFO_STRING);
	
	fakeclient->sendinfo = TRUE;
	
	SV_ExtractFromUserinfo(fakeclient);

	fakeclient->network_userid.m_SteamID =
	//ISteamGameServer_CreateUnauthenticatedUserConnection();
	fakeclient->network_userid.idtype = AUTH_IDTYPE_STEAM;
	//ISteamGameServer_BUpdateUserData(fakeclient->network_userid.m_SteamID, netname, 0);

	return ent;
};

int CGameWorld::CreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	extra_baselines_t *bls = g_psv.instance_baselines;
	if(bls->number >= NUM_BASELINES)
		return 0;

	bls->classname[bls->number] = classname;
	Q_memcpy(&bls->baseline[bls->number], baseline, sizeof(struct entity_state_s));
	bls->number += 1;
	return bls->number;
};

int CGameWorld::GetNumberOfEntities() const
{
	int ent_count = 0;
	
	for(int i = 1; i < g_psv.num_edicts; i++)
	{
		if(!g_psv.edicts[i].free)
			++ent_count;
	}

	return ent_count;
};

char *CGameWorld::GetInfoKeyBuffer(edict_t *e) const
{
	return nullptr;
};

void CGameWorld::TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
};

void CGameWorld::TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
};

int CGameWorld::TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	return 0;
};

void CGameWorld::TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
};

void CGameWorld::TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
};

const char *CGameWorld::TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	return "";
};

void CGameWorld::TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
};

void CGameWorld::SpawnParticleEffect(const float *org, const float *dir, float color, float count)
{
};

void CGameWorld::SpawnStaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
};

const char *CGameWorld::GetMapName() const
{
	return "";
};