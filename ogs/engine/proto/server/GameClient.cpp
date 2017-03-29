/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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
#include "server/GameClient.hpp"

CGameClient::CGameClient(int id, client_t *apClientData) : mpClientData(apClientData)
{
};

CGameClient::~CGameClient()
{
};

void CGameClient::Disconnect(const char *asReason, ...)
{
};

void CGameClient::Reconnect(const char *message)
{
	//myNetChan->Clear();
	Netchan_Clear(&mpClientData->netchan);
	
	MSG_WriteByte(&mpClientData->netchan.message, svc_print);
	MSG_WriteString(&mpClientData->netchan.message, message);

	MSG_WriteByte(&mpClientData->netchan.message, svc_stufftext);
	MSG_WriteString(&mpClientData->netchan.message, "retry\n");

	Drop(false, message);
};

void CGameClient::Drop(bool crash, const char *fmt, ...)
{
	char buf[1024];
	va_list argptr;

	va_start(argptr, fmt);
	Q_vsnprintf(buf, ARRAYSIZE(buf) - 1, fmt, argptr);
	va_end(argptr);

	g_RehldsHookchains.m_SV_DropClient.callChain(SV_DropClient_hook, GetRehldsApiClient(mpClientData), crash != false, buf);
};

void CGameClient::Drop_internal(bool crash, const char *string)
{
	unsigned char final[512];
	float connection_time;

	int i = 0;

	if(!crash)
	{
		if(!mpClientData->fakeclient)
		{
			MSG_WriteByte(&mpClientData->netchan.message, svc_disconnect);
			MSG_WriteString(&mpClientData->netchan.message, string);
			final[0] = svc_disconnect;
			Q_strncpy((char *)& final[1], string, Q_min(sizeof(final) - 1, Q_strlen(string) + 1));
			final[sizeof(final) - 1] = 0;
			i = 1 + Q_min(sizeof(final) - 1, Q_strlen(string) + 1);
		};
		
		if(mpClientData->edict && mpClientData->spawned)
			gEntityInterface.pfnClientDisconnect(mpClientData->edict);

		CSystem::Printf("Dropped %s from server\nReason:  %s\n", mpClientData->name, string);

		if(!mpClientData->fakeclient)
			Netchan_Transmit(&mpClientData->netchan, i, final);
	};

	connection_time = realtime - mpClientData->netchan.connect_time;
	
	if(connection_time > 60.0f)
	{
		++g_psvs.stats.num_sessions;
		g_psvs.stats.cumulative_sessiontime += connection_time;
	};

#ifdef REHLDS_FIXES
	// prevent message reading after disconnect
	if(mpClientData == host_client)
		msg_readcount = net_message.cursize;
#endif // REHLDS_FIXES
	
	// myNetChan->Clear();
	Netchan_Clear(&mpClientData->netchan);

	Steam_NotifyClientDisconnect(mpClientData);

	mpClientData->active = FALSE;
	mpClientData->connected = FALSE;
	mpClientData->hasusrmsgs = FALSE;
	mpClientData->fakeclient = FALSE;
	mpClientData->spawned = FALSE;
	mpClientData->fully_connected = FALSE;
	mpClientData->name[0] = 0;
	mpClientData->connection_started = realtime;
	mpClientData->proxy = FALSE;
	
	COM_ClearCustomizationList(&mpClientData->customdata, FALSE);
	
	mpClientData->edict = NULL;
	
	Q_memset(mpClientData->userinfo, 0, sizeof(mpClientData->userinfo));
	Q_memset(mpClientData->physinfo, 0, sizeof(mpClientData->physinfo));

#ifdef REHLDS_FIXES
	g_GameClients[mpClientData - g_psvs.clients]->SetSpawnedOnce(false);
#endif // REHLDS_FIXES

	mpServer->SendFullClientUpdateForAll(mpClientData);

	//NotifyDedicatedServerUI("UpdatePlayers");
};

void CGameClient::Kick(const char *asReason, ...)
{
};

void CGameClient::Ban()
{
};

void CGameClient::BanEx(const char *asReason, ...)
{
};

void CGameClient::TimeBan(uint anTimeInMins)
{
};

void CGameClient::SetConnected(bool connected)
{
};

bool CGameClient::IsConnected()
{
};

void CGameClient::SetActive(bool active)
{
};

bool CGameClient::IsActive()
{
};

void CGameClient::Inactivate()
{
	mpClientData->active = FALSE;
	mpClientData->connected = TRUE;
	mpClientData->spawned = FALSE;
	mpClientData->fully_connected = FALSE;

	SZ_Clear(&mpClientData->netchan.message);
	SZ_Clear(&mpClientData->datagram);

	COM_ClearCustomizationList(&mpClientData->customdata, FALSE);
	Q_memset(mpClientData->physinfo, 0, MAX_PHYSINFO_STRING);
};

void CGameClient::UpdateUserInfo()
{
	mpClientData->sendinfo = FALSE;
	mpClientData->sendinfo_time = realtime + 1.0;
	
	ExtractFromUserinfo();
	mpServer->SendFullClientUpdateForAll(mpClientData);
};

void CGameClient::ExtractFromUserinfo()
{
	int i;
	char newname[MAX_NAME];
	
	char *userinfo = mpClientData->userinfo;

	const char *val = Info_ValueForKey(userinfo, "name");
	
#ifdef REHLDS_FIXES
	ReplaceSpecialCharactersInName(newname, val);
#else  // REHLDS_FIXES
	
	Q_strncpy(newname, val, sizeof(newname) - 1);
	newname[sizeof(newname) - 1] = '\0';

	for(char *p = newname; *p; p++)
	{
		if(*p == '%' || *p == '&')
			*p = ' ';
	}

	// Fix name to not start with '#', so it will not resemble userid
	for(char *p = newname; *p == '#'; p++)
		*p = ' ';
#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
	Q_StripUnprintableAndSpace(newname);
#else  // REHLDS_FIXES
	TrimSpace(newname, newname);
#endif // REHLDS_FIXES

	if(!Q_UnicodeValidate(newname))
		Q_UnicodeRepair(newname);

	if(newname[0] == '\0' || !Q_stricmp(newname, "console")
#ifdef REHLDS_FIXES
	   ||
	   Q_strstr(newname, "..") != NULL)
#else  // REHLDS_FIXES
	   )
#endif // REHLDS_FIXES
	{
		Info_SetValueForKey(userinfo, "name", "unnamed", MAX_INFO_STRING);
	}
	else if(Q_strcmp(val, newname))
		Info_SetValueForKey(userinfo, "name", newname, MAX_INFO_STRING);

	// Check for duplicate names
	mpServer->CheckForDuplicateNames(userinfo, TRUE, mpClientData - g_psvs.clients);

	gEntityInterface.pfnClientUserInfoChanged(mpClientData->edict, userinfo);

	val = Info_ValueForKey(userinfo, "name");
	
	Q_strncpy(mpClientData->name, val, sizeof(mpClientData->name) - 1);
	mpClientData->name[sizeof(mpClientData->name) - 1] = '\0';

	//ISteamGameServer_BUpdateUserData(mpClientData->network_userid.m_SteamID, mpClientData->name, 0);

	val = Info_ValueForKey(userinfo, "rate");
	
	if(val[0] != 0)
	{
		i = Q_atoi(val);
		mpClientData->netchan.rate = Q_clamp(float(i), MIN_RATE, MAX_RATE);
	};

	val = Info_ValueForKey(userinfo, "topcolor");
	if(val[0] != 0)
		mpClientData->topcolor = Q_atoi(val);
	else
		mpConsole->DPrintf("topcolor unchanged for %s\n", mpClientData->name);

	val = Info_ValueForKey(userinfo, "bottomcolor");
	if(val[0] != 0)
		mpClientData->bottomcolor = Q_atoi(val);
	else
		mpConsole->DPrintf("bottomcolor unchanged for %s\n", mpClientData->name);

	val = Info_ValueForKey(userinfo, "cl_updaterate");
	if(val[0] != 0)
	{
		i = Q_atoi(val);
		
		if(i >= 10)
			mpClientData->next_messageinterval = 1.0f / i;
		else
			mpClientData->next_messageinterval = 0.1f;
	};

	val = Info_ValueForKey(userinfo, "cl_lw");
	mpClientData->lw = val[0] != 0 ? Q_atoi(val) != 0 : 0;

	val = Info_ValueForKey(userinfo, "cl_lc");
	mpClientData->lc = val[0] != 0 ? Q_atoi(val) != 0 : 0;

	val = Info_ValueForKey(userinfo, "*hltv");
	mpClientData->proxy = val[0] != 0 ? Q_atoi(val) == 1 : 0;

	mpServer->CheckUpdateRate(&mpClientData->next_messageinterval);
	CheckRate();
};

#ifdef REHLDS_FIXES
void CGameClient::ReplaceSpecialCharactersInName(char *newname, const char *oldname)
{
	size_t remainChars = MAX_NAME - 1;
	size_t n = 0;
	
	for(const char *s = oldname; *s != '\0' && remainChars; s++)
	{
		if(*s == '#' || *s == '%' || *s == '&' || (n && newname[n - 1] == '+' && (signed char)*s > 0 && isalnum(*s)))
		{
			if(remainChars < 3)
				break;

			// http://unicode-table.com/blocks/halfwidth-and-fullwidth-forms/
			newname[n++] = char(0xEF);
			newname[n++] = 0xBC | (((*s - 0x20) & 0x40) >> 6);
			newname[n++] = 0x80 + ((*s - 0x20) & 0x3F);

			remainChars -= 3;
		}
		else
		{
			newname[n++] = *s;
			remainChars--;
		};
	};
	
	newname[n] = '\0';
};
#endif // REHLDS_FIXES

int CGameClient::CalcPing()
{
	float ping;
	int i;
	int count;
	int back;
	client_frame_t *frame;
	int idx;

	if(mpClientData->fakeclient)
		return 0;

	if(SV_UPDATE_BACKUP <= 31)
	{
		back = SV_UPDATE_BACKUP * 0.5f;
		
		if(back <= 0)
			return 0;
	}
	else
		back = 16;

	ping = 0.0f;
	count = 0;
	
	for(i = 0; i < back; i++)
	{
		idx = mpClientData->netchan.incoming_acknowledged + ~i;
		frame = &mpClientData->frames[SV_UPDATE_MASK & idx];

		if(frame->ping_time > 0.0f)
		{
			ping += frame->ping_time;
			count++;
		};
	};

	if(count)
	{
		ping /= count;
		
		if(ping > 0.0f)
			return ping * 1000.0f;
	};
	
	return 0;
};

bool CGameClient::ShouldUpdatePing()
{
	if(mpClientData->proxy)
	{
		if(realtime < mpClientData->nextping)
			return FALSE;

		mpClientData->nextping = realtime + 2.0f;
		return true;
	};

	// useless call
	//CalcPing();

	return mpClientData->lastcmd.buttons & IN_SCORE;
};

float CGameClient::CalcClientTime()
{
	float ping = 0.0f;
	int count = 0;
	
	int backtrack = (int)sv_unlagsamples.value;

	if(backtrack < 1)
		backtrack = 1;

	if(backtrack >= (SV_UPDATE_BACKUP <= 16 ? SV_UPDATE_BACKUP : 16))
		backtrack = (SV_UPDATE_BACKUP <= 16 ? SV_UPDATE_BACKUP : 16);

	if(backtrack <= 0)
		return 0.0f;

	for(int i = 0; i < backtrack; i++)
	{
		client_frame_t *frame = &mpClientData->frames[SV_UPDATE_MASK & (mpClientData->netchan.incoming_acknowledged - i)];
		
		if(frame->ping_time <= 0.0f)
			continue;

		++count;
		ping += frame->ping_time;
	};

	if(!count)
		return 0.0f;
	
	// Aren't these two supposed to be reversed?
	float minping = 9999.0f;
	float maxping = -9999.0f;
	
	ping /= count;

	for(int i = 0; i < (SV_UPDATE_BACKUP <= 4 ? SV_UPDATE_BACKUP : 4); i++)
	{
		client_frame_t *frame = &mpClientData->frames[SV_UPDATE_MASK & (mpClientData->netchan.incoming_acknowledged - i)];
		
		if(frame->ping_time <= 0.0f)
			continue;

		if(frame->ping_time < minping)
			minping = frame->ping_time;

		if(frame->ping_time > maxping)
			maxping = frame->ping_time;
	};

	if(maxping < minping || fabs(maxping - minping) <= 0.2f)
		return ping;

	return 0.0f;
};

void CGameClient::ComputeLatency()
{
	mpClientData->latency = CalcTime();
};

void CGameClient::CheckRate()
{
	if(sv_maxrate.value > 0.0f)
	{
		if(mpClientData->netchan.rate > sv_maxrate.value)
		{
			if(sv_maxrate.value > MAX_RATE)
				mpClientData->netchan.rate = MAX_RATE;
			else
				mpClientData->netchan.rate = sv_maxrate.value;
		};
	};
	
	if(sv_minrate.value > 0.0f)
	{
		if(mpClientData->netchan.rate < sv_minrate.value)
		{
			if(sv_minrate.value < MIN_RATE)
				mpClientData->netchan.rate = MIN_RATE;
			else
				mpClientData->netchan.rate = sv_minrate.value;
		};
	};
};

void CGameClient::BuildFrame()
{
};

void CGameClient::WriteEntities(sizebuf_t *msg)
{
	client_frame_t *frame = &mpClientData->frames[SV_UPDATE_MASK & mpClientData->netchan.outgoing_sequence];

	unsigned char *pvs = NULL;
	unsigned char *pas = NULL;
	
	gEntityInterface.pfnSetupVisibility((edict_t *)mpClientData->pViewEntity,
	                                    mpClientData->edict,
	                                    &pvs,
	                                    &pas);
	unsigned char *pSet = pvs;

	packet_entities_t *pack = &frame->entities;

// for REHLDS_OPT_PEDANTIC: Allocate the MAX_PACKET_ENTITIES ents in the frame's
// storage
// This allows us to avoid intermediate 'fullpack' storage
#ifdef REHLDS_OPT_PEDANTIC
	SV_AllocPacketEntities(frame, MAX_PACKET_ENTITIES);
	packet_entities_t *curPack = &frame->entities;
	curPack->num_entities = 0;
#else
	SV_ClearPacketEntities(frame);
	full_packet_entities_t fullpack;
	fullpack.num_entities = 0;
	full_packet_entities_t *curPack = &fullpack;
#endif // REHLDS_OPT_PEDANTIC

	qboolean sendping = ShouldUpdatePing();
	int flags = mpClientData->lw != 0;

	int e;
	for(e = 1; e <= g_psvs.maxclients; e++)
	{
		client_t *cl = &g_psvs.clients[e - 1];
		if((!cl->active && !cl->spawned) || cl->proxy)
			continue;

		qboolean add = gEntityInterface.pfnAddToFullPack(
		&curPack->entities[curPack->num_entities], e, &g_psv.edicts[e], host_client->edict, flags, TRUE, pSet);
		if(add)
			++curPack->num_entities;
	}

	for(; e < g_psv.num_edicts; e++)
	{
		if(curPack->num_entities >= MAX_PACKET_ENTITIES)
		{
			mpConsole->DPrintf("Too many entities in visible packet list.\n");
			break;
		}

		edict_t *ent = &g_psv.edicts[e];

#ifdef REHLDS_OPT_PEDANTIC
		// Part of gamedll's code is moved here to decrease amount of calls to
		// AddToFullPack()
		// We don't even try to transmit entities without model as well as invisible
		// entities
		if(ent->v.modelindex && !(ent->v.effects & EF_NODRAW))
		{
			qboolean add = gEntityInterface.pfnAddToFullPack(
			&curPack->entities[curPack->num_entities], e, &g_psv.edicts[e], host_client->edict, flags, FALSE, pSet);
			if(add)
				++curPack->num_entities;
		}
#else
		qboolean add = gEntityInterface.pfnAddToFullPack(
		&curPack->entities[curPack->num_entities], e, &g_psv.edicts[e], host_client->edict, flags, FALSE, pSet);
		if(add)
			++curPack->num_entities;
#endif // REHLDS_OPT_PEDANTIC
	}

#ifdef REHLDS_FIXES
	if (sv_rehlds_attachedentities_playeranimationspeed_fix.value != 0)
	{
		int attachedEntCount[MAX_CLIENTS + 1] = {};
		for (int i = curPack->num_entities - 1; i >= 0; i--)
		{
			auto &entityState = curPack->entities[i];
			if (entityState.number > MAX_CLIENTS)
			{
				if (entityState.movetype == MOVETYPE_FOLLOW
					&& 1 <= entityState.aiment && entityState.aiment <= MAX_CLIENTS)
				{
					attachedEntCount[entityState.aiment]++;
				}
			}
			else
			{
				if (attachedEntCount[entityState.number] != 0)
				{
					// Each attached entity causes StudioProcessGait for player
					// But this will slow down normal animation predicting on client
					entityState.framerate /= (1 + attachedEntCount[entityState.number]);
				}
			}
		}
	}
#endif

// for REHLDS_FIXES: Entities are already in the frame's storage, no need to
// copy them
#ifndef REHLDS_OPT_PEDANTIC
	SV_AllocPacketEntities(frame, fullpack.num_entities);
	
	if(pack->num_entities)
		Q_memcpy(pack->entities, fullpack.entities, sizeof(entity_state_t) * pack->num_entities);
#endif

	SV_EmitPacketEntities(mpClientData, pack, msg);
	SV_EmitEvents(mpClientData, pack, msg);
	if(sendping)
		SV_EmitPings(mpClientData, msg);
};

void CGameClient::WriteFrame(sizebuf_t *msg)
{
};

void CGameClient::ProcessFile(char *filename)
{
};

bool CGameClient::SendDatagram()
{
	unsigned char buf[MAX_DATAGRAM];
	sizebuf_t msg;

	msg.buffername = "Client Datagram";
	msg.data = buf;
	msg.maxsize = sizeof(buf);
	msg.cursize = 0;
	msg.flags = SIZEBUF_ALLOW_OVERFLOW;

	MSG_WriteByte(&msg, svc_time);
	
#ifdef REHLDS_FIXES
	if(sv_rehlds_local_gametime.value != 0.0f)
		MSG_WriteFloat(&msg, (float)g_GameClients[mpClientData - g_psvs.clients]->GetLocalGameTime());
	else
#endif
	{
		MSG_WriteFloat(&msg, g_psv.time);
	};

	WriteClientdata(&msg);
	WriteEntities(&msg);

	if(mpClientData->datagram.flags & SIZEBUF_OVERFLOWED)
		mpConsole->Printf("WARNING: datagram overflowed for %s\n", mpClientData->name);
	else
	{
#ifdef REHLDS_FIXES
		if(msg.cursize + mpClientData->datagram.cursize > msg.maxsize)
			mpConsole->DPrintf("Warning: Ignoring unreliable datagram for %s, would "
			            "overflow on msg\n",
			            mpClientData->name);
		else
			SZ_Write(&msg, mpClientData->datagram.data, mpClientData->datagram.cursize);
#else
		SZ_Write(&msg, mpClientData->datagram.data, mpClientData->datagram.cursize);
#endif
	}

	SZ_Clear(&mpClientData->datagram);

	if(msg.flags & SIZEBUF_OVERFLOWED)
	{
		mpConsole->Printf("WARNING: msg overflowed for %s\n", mpClientData->name);
		SZ_Clear(&msg);
	};

	Netchan_Transmit(&mpClientData->netchan, msg.cursize, buf);
	return true;
};

void CGameClient::SendServerInfo()
{
};

void CGameClient::ExtractFromUserInfo()
{
};

void CGameClient::SendFullUpdate(sizebuf_t *sb)
{
};

void CGameClient::ClearResourceLists()
{
	if(!mpClientData)
		CSystem::Error("SV_ClearResourceLists with NULL client!");

	mpServer->ClearResourceList(&mpClientData->resourcesneeded);
	mpServer->ClearResourceList(&mpClientData->resourcesonhand);
};

char *CGameClient::GetClientIDString()
{
	static char idstr[64];

	idstr[0] = 0;

	if(!mpClientData)
		return idstr;

	if(mpClientData->netchan.remote_address.type == NA_LOOPBACK && mpClientData->network_userid.idtype == AUTH_IDTYPE_VALVE)
		Q_snprintf(idstr, ARRAYSIZE(idstr) - 1, "VALVE_ID_LOOPBACK");
	else
	{
		USERID_t *id = &mpClientData->network_userid;
		Q_snprintf(idstr, ARRAYSIZE(idstr) - 1, "%s", SV_GetIDString(id));
		idstr[ARRAYSIZE(idstr) - 1] = 0;
	};

	return idstr;
};

void CGameClient::WriteClientdata(client_t *client, sizebuf_t *msg)
{
	edict_t *ent = client->edict;
	client_frame_t *frame = &client->frames[SV_UPDATE_MASK & client->netchan.outgoing_sequence];
	int bits = (SV_UPDATE_MASK & host_client->delta_sequence);

	frame->senttime = realtime;
	frame->ping_time = -1.0f;

	if(client->chokecount)
	{
		MSG_WriteByte(msg, svc_choke);
		client->chokecount = 0;
	};

	if(ent->v.fixangle)
	{
		if(ent->v.fixangle == 2)
		{
			MSG_WriteByte(msg, svc_addangle);
			MSG_WriteHiresAngle(msg, ent->v.avelocity[1]);
			ent->v.avelocity[1] = 0;
		}
		else
		{
			MSG_WriteByte(msg, svc_setangle);
			MSG_WriteHiresAngle(msg, ent->v.angles[0]);
			MSG_WriteHiresAngle(msg, ent->v.angles[1]);
			MSG_WriteHiresAngle(msg, ent->v.angles[2]);
		};
		
		ent->v.fixangle = 0;
	};

	Q_memset(&frame->clientdata, 0, sizeof(frame->clientdata));
	gEntityInterface.pfnUpdateClientData(ent, host_client->lw != 0, &frame->clientdata);

	MSG_WriteByte(msg, svc_clientdata);

	if(client->proxy)
		return;

	MSG_StartBitWriting(msg);

	clientdata_t baseline;
	Q_memset(&baseline, 0, sizeof(baseline));

	clientdata_t *from = &baseline;
	clientdata_t *to = &frame->clientdata;

	if(host_client->delta_sequence == -1)
		MSG_WriteBits(0, 1);
	else
	{
		MSG_WriteBits(1, 1);
		MSG_WriteBits(host_client->delta_sequence, 8);
		from = &host_client->frames[bits].clientdata;
	};

	DELTA_WriteDelta((byte *)from, (byte *)to, TRUE, (delta_t *)g_pclientdelta, NULL);

	if(host_client->lw && gEntityInterface.pfnGetWeaponData(host_client->edict, frame->weapondata))
	{
		weapon_data_t wbaseline;
		Q_memset(&wbaseline, 0, sizeof(wbaseline));

		weapon_data_t *fdata = NULL;
		weapon_data_t *tdata = frame->weapondata;

		for(int i = 0; i < 64; i++, tdata++)
		{
#ifdef REHLDS_FIXES
			// So, HL and CS games send absolute gametime in these vars, DMC and Ricochet games don't send absolute gametime
			// TODO: idk about other games
			// FIXME: there is a loss of precision, because gamedll has already written float gametime in them 
			if (sv_rehlds_local_gametime.value != 0.0f)
			{
				auto convertGlobalGameTimeToLocal = 
					[](float &globalGameTime)
				{
					if(globalGameTime > 0.0f)
						globalGameTime -= (float)g_GameClients[host_client - g_psvs.clients]->GetLocalGameTimeBase();
				};
				
				if (g_bIsCStrike || g_bIsCZero)
					convertGlobalGameTimeToLocal(std::ref(tdata->m_fAimedDamage));
				
				if (g_bIsHL1 || g_bIsCStrike || g_bIsCZero)
				{
					convertGlobalGameTimeToLocal(std::ref(tdata->fuser2));
					convertGlobalGameTimeToLocal(std::ref(tdata->fuser3));
				};
			};
#endif
			
			if(host_client->delta_sequence == -1)
				fdata = &wbaseline;
			else
				fdata = &host_client->frames[bits].weapondata[i];

			if(DELTA_CheckDelta((byte *)fdata, (byte *)tdata, g_pweapondelta))
			{
				MSG_WriteBits(1, 1);
				MSG_WriteBits(i, 6);

#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
				// all calculations are already done
				_DELTA_WriteDelta((byte *)fdata, (byte *)tdata, TRUE, g_pweapondelta, NULL, TRUE);
#else
				DELTA_WriteDelta((byte *)fdata, (byte *)tdata, TRUE, g_pweapondelta, NULL);
#endif
			};
		};
	};

	MSG_WriteBits(0, 1);
	MSG_EndBitWriting(msg);
};