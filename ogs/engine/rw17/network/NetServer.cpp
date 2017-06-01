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
/// @brief network server; accepts clients and used for authority

#include "precompiled.hpp"
#include "network/NetServer.hpp"
#include "system/common.hpp"

bool CNetServer::Start(int anPort)
{
};

void CNetServer::Stop()
{
};

void CNetServer::Frame(float frametime)
{
	if(!g_psv.active)
		return;
	
	ReadPackets(frametime);
};

bool CNetServer::FilterPacket()
{
	for(int i = numipfilters - 1; i >= 0; i--)
	{
		ipfilter_t *curFilter = &ipfilters[i];
		
		if(curFilter->compare.u32 == 0xFFFFFFFF || curFilter->banEndTime == 0.0f || curFilter->banEndTime > realtime)
		{
			if((*(uint32 *)net_from.ip & curFilter->mask) == curFilter->compare.u32)
				return (int)sv_filterban.value;
		}
		else
		{
			if(i < numipfilters - 1)
				Q_memmove(curFilter, &curFilter[1], sizeof(ipfilter_t) * (numipfilters - i - 1));

			--numipfilters;
		};
	};
	
	return sv_filterban.value == 0.0f;
};

void CNetServer::SendBan()
{
	char szMessage[64];
	Q_snprintf(szMessage, sizeof(szMessage), "You have been banned from this server.\n");

	SZ_Clear(&net_message);

	MSG_WriteLong(&net_message, -1);
	MSG_WriteByte(&net_message, 108);
	MSG_WriteString(&net_message, szMessage);
	
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, net_from);

	SZ_Clear(&net_message);
};

void CNetServer::CheckForRcon()
{
	if(g_psv.active || cls.state != ca_dedicated || giActive == DLL_CLOSE || !host_initialized)
		return;

	while(NET_GetPacket(NS_SERVER))
	{
		if(FilterPacket())
			SendBan();
		else
		{
			if(*(uint32 *)net_message.data == 0xFFFFFFFF)
				HandleRconPacket();
		};
	};
};

void CNetServer::HandleRconPacket(netadr_t *adr)
{
	MSG_BeginReading();
	MSG_ReadLong();
	
	char *s = MSG_ReadStringLine();
	Cmd_TokenizeString(s);
	
	const char *c = Cmd_Argv(0);
	
	if(!Q_strcmp(c, "getchallenge"))
		SVC_GetChallenge();
	else if(!Q_stricmp(c, "challenge"))
		SVC_ServiceChallenge();
	else if(!Q_strcmp(c, "rcon"))
		HandleRcon(adr);
};

void CNetServer::HandleConnectionlessPacket(netadr_t *adr)
{
	MSG_BeginReading();
	MSG_ReadLong();
	
	char *args = MSG_ReadStringLine();
	Cmd_TokenizeString(args);
	
	const char *c = Cmd_Argv(0);
	
	if(!Q_strcmp(c, "getchallenge"))
		SVC_GetChallenge();
	else if(!Q_stricmp(c, "challenge"))
		SVC_ServiceChallenge();
	else if(!Q_strcmp(c, "rcon"))
		HandleRcon(adr);
	else if(!Q_strcmp(c, "connect"))
		ConnectClient(adr);
	
	HandleConnectionlessPacket(adr, c, args);
};

void CNetServer::BroadcastCommand(char *fmt, ...)
{
	if(!g_psv.active)
		return;
	
	va_list argptr;
	
	va_start(argptr, fmt);
	
	//INetMsg *pMsg = mpNetwork->BeginNetMsg();
	sizebuf_t msg;
	char data[128];
	
	msg.data = (byte *)data;
	msg.buffername = "Broadcast Command";
	msg.cursize = 0;
	msg.maxsize = sizeof(data);
	msg.flags = SIZEBUF_ALLOW_OVERFLOW;
	
	char string[1024];
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	
	va_end(argptr);

	MSG_WriteByte(&msg, svc_stufftext);
	MSG_WriteString(&msg, string);
	
	if(msg.flags & SIZEBUF_OVERFLOWED)
		CSystem::Error("SV_BroadcastCommand:  Overflowed on %s, %i is max size\n", string, msg.maxsize);

	for(int i = 0; i < g_psvs.maxclients; ++i)
	{
		client_t *cl = &g_psvs.clients[i];
		
		if(cl->active || cl->connected || (cl->spawned && !cl->fakeclient))
			SZ_Write(&cl->netchan.message, msg.data, msg.cursize);
	};
};

void CNetServer::BroadcastPrintf(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	va_start(argptr, fmt);
	Q_vsnprintf(string, ARRAYSIZE(string) - 1, fmt, argptr);
	va_end(argptr);

	string[ARRAYSIZE(string) - 1] = 0;

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		
		if((cl->active || cl->spawned) && !cl->fakeclient)
		{
			MSG_WriteByte(&cl->netchan.message, svc_print);
			MSG_WriteString(&cl->netchan.message, string);
		};
	};
	
	mpConsole->DPrintf("%s", string);
};

void CNetServer::ConnectClient(netadr_t *adr)
{
	if(mpConnectionHandler)
		mpConnectionHandler->ConnectClient(adr);
	
	//g_RehldsHookchains.m_SV_ConnectClient.callChain(SV_ConnectClient_internal);
	ConnectClient_internal();
};

void CNetServer::ReconnectClient(int anID)
{
};

void CNetServer::DisconnectClient(int anID)
{
};

void CNetServer::ReconnectAll()
{
};

void CNetServer::DisconnectAll()
{
};

NOXREF int CNetServer::GetChallengeNr(netadr_t *adr)
{
	NOXREFCHECK;
	
	int oldest = 0;
	int oldestTime = 0x7FFFFFFFu;
	int i;

	for(i = 0; i < MAX_CHALLENGES; i++)
	{
		if(NET_CompareBaseAdr(*adr, g_rg_sv_challenges[i].adr))
			break;

		if(g_rg_sv_challenges[i].time < oldestTime)
		{
			oldestTime = g_rg_sv_challenges[i].time;
			oldest = i;
		};
	};
	
	if(i == MAX_CHALLENGES)
	{
		g_rg_sv_challenges[oldest].challenge =
		RandomLong(0, 65535) | (RandomLong(0, 36863) << 16);
		g_rg_sv_challenges[oldest].adr = net_from;
		g_rg_sv_challenges[oldest].time = realtime;
		i = oldest;
	};
	
	return g_rg_sv_challenges[i].challenge;
};

NOXREF void CNetServer::ReplyServerChallenge(netadr_t *adr)
{
	NOXREFCHECK;
	
	char buffer[16];
	sizebuf_t buf;

	buf.buffername = "SVC_Challenge";
	buf.data = (byte *)buffer;
	buf.maxsize = sizeof(buffer);
	buf.cursize = 0;
	buf.flags = SIZEBUF_ALLOW_OVERFLOW;

	MSG_WriteLong(&buf, 0xffffffff);
	MSG_WriteByte(&buf, 65);
	MSG_WriteLong(&buf, GetChallengeNr(adr));
	
	NET_SendPacket(NS_SERVER, buf.cursize, (char *)buf.data, *adr);
};

int CNetServer::GetClientCount() const
{
};

int CNetServer::GetFakeClientCount() const
{
	int fakeclients = 0;

	for(int i = 0; i < g_psvs.maxclients; ++i)
	{
		client_t *client = &g_psvs.clients[i];

		if(client->fakeclient)
			fakeclients++;
	};
	
	return fakeclients;
};

int CNetServer::GetMaxClients() const
{
	return 0;
};

void CNetServer::ReadPackets(float frametime)
{
	while(mpNetwork->GetPacket(NS_SERVER)) // mpSocket->GetPacket
	{
		if(FilterPacket())
		{
			SendBan();
			continue;
		};

		bool pass = g_RehldsHookchains.m_PreprocessPacket.callChain(NET_GetPacketPreprocessor, net_message.data, net_message.cursize, net_from);
		
		if(!pass)
			continue;

		if(*(uint32 *)net_message.data == 0xFFFFFFFF)
		{
			// Connectionless packet
			if(CheckIP(net_from))
			{
				Steam_HandleIncomingPacket(net_message.data, net_message.cursize, ntohl(*(u_long *)&net_from.ip[0]), htons(net_from.port));
				HandleConnectionlessPacket(net_from);
			}
			else if(sv_logblocks.value != 0.0f)
				Log_Printf("Traffic from %s was blocked for exceeding rate limits\n", NET_AdrToString(net_from));
			
			continue;
		};

		for(int i = 0; i < g_psvs.maxclients; i++)
		{
			client_t *cl = &g_psvs.clients[i];
			
			if(!cl->connected && !cl->active && !cl->spawned)
				continue;

			if(NET_CompareAdr(net_from, cl->netchan.remote_address) != TRUE)
				continue;

			if(Netchan_Process(&cl->netchan))
			{
				if(g_psvs.maxclients == 1 || !cl->active || !cl->spawned || !cl->fully_connected)
					cl->send_message = TRUE;

				SV_ExecuteClientMessage(cl);
				gGlobalVariables.frametime = frametime;
			}

			if(Netchan_IncomingReady(&cl->netchan))
			{
				if(Netchan_CopyNormalFragments(&cl->netchan))
				{
					MSG_BeginReading();
					SV_ExecuteClientMessage(cl);
				};
				
				if(Netchan_CopyFileFragments(&cl->netchan))
				{
					host_client = cl;
					SV_ProcessFile(cl, cl->netchan.incomingfilename);
				};
			};
		};
	};
};

void CNetServer::SVC_GetChallenge(netadr_t *adr)
{
	qboolean steam = (Cmd_Argc() == 2 && !Q_stricmp(Cmd_Argv(1), "steam"));
	
	int challenge = GetChallenge(adr);
	
	char data[1024];
	
	if(steam)
		Q_snprintf(data, sizeof(data), "\xFF\xFF\xFF\xFF%c00000000 %u 3 %lld %d\n", S2C_CHALLENGE, challenge, g_RehldsHookchains.m_Steam_GSGetSteamID.callChain(Steam_GSGetSteamID), Steam_GSBSecure());
	else
	{
		mpConsole->DPrintf("Server requiring authentication\n");
		Q_snprintf(data, sizeof(data), "\xFF\xFF\xFF\xFF%c00000000 %u 2\n", S2C_CHALLENGE, challenge);
	};

	// Give 3-rd party plugins a chance to modify challenge response
	g_RehldsHookchains.m_SVC_GetChallenge_mod.callChain(NULL, data, challenge);
	NET_SendPacket(NS_SERVER, Q_strlen(data) + 1, data, adr);
};

void CNetServer::SVC_ServiceChallenge(netadr_t *adr)
{
	if(Cmd_Argc() != 2)
		return;

	const char *type = Cmd_Argv(1);
	
	if(!type)
		return;

	if(!type[0] || Q_stricmp(type, "rcon"))
		return;

	int challenge = GetChallenge(adr);
	
	char data[128];
	Q_snprintf(data, sizeof(data), "%c%c%c%cchallenge %s %u\n", 255, 255, 255, 255, type, challenge);

	NET_SendPacket(NS_SERVER, Q_strlen(data) + 1, data, adr);
};

void CNetServer::HandleRcon(netadr_t *net_from_)
{
	char remaining[512];
	char rcon_buff[1024];

	int invalid = Rcon_Validate();
	int len = net_message.cursize - Q_strlen("rcon");
	if(len <= 0 || len >= sizeof(remaining))
		return;

	Q_memcpy(remaining, &net_message.data[Q_strlen("rcon")], len);
	remaining[len] = 0;

#ifdef REHLDS_FIXES
	if(sv_rcon_condebug.value > 0.0f)
#endif
	{
		if(invalid)
		{
			mpConsole->Printf("Bad Rcon from %s:\n%s\n", NET_AdrToString(*net_from_), remaining);
			Log_Printf("Bad Rcon: \"%s\" from \"%s\"\n", remaining, NET_AdrToString(*net_from_));
		}
		else
		{
			mpConsole->Printf("Rcon from %s:\n%s\n", NET_AdrToString(*net_from_), remaining);
			Log_Printf("Rcon: \"%s\" from \"%s\"\n", remaining, NET_AdrToString(*net_from_));
		};
	};

	SV_BeginRedirect(RD_PACKET, net_from_);

	if(invalid)
	{
		if(invalid == 2)
			mpConsole->Printf("Bad rcon_password.\n");
		else if(Q_strlen(rcon_password.string) == 0)
			mpConsole->Printf("Bad rcon_password.\nNo password set for this server.\n");
		else
			mpConsole->Printf("Bad rcon_password.\n");

		SV_EndRedirect();
		return;
	}
	char *data = COM_Parse(COM_Parse(COM_Parse(remaining)));
	if(!data)
	{
		mpConsole->Printf("Empty rcon\n");

#ifdef REHLDS_FIXES
		// missing SV_EndRedirect()
		SV_EndRedirect();
#endif // REHLDS_FIXES
		return;
	}

	Q_strncpy(rcon_buff, data, sizeof(rcon_buff) - 1);
	rcon_buff[sizeof(rcon_buff) - 1] = 0;
	Cmd_ExecuteString(rcon_buff, src_command);
	SV_EndRedirect();
};

int CNetServer::Rcon_Validate()
{
	if(Cmd_Argc() < 3 || Q_strlen(rcon_password.string) == 0)
		return 1;

	if(sv_rcon_banpenalty.value < 0.0f)
		Cvar_SetValue("sv_rcon_banpenalty", 0.0);

	if(CheckRconFailure(&net_from))
	{
		mpConsole->Printf("Banning %s for rcon hacking attempts\n", NET_AdrToString(net_from));
		Cbuf_AddText(va("addip %i %s\n", (int)sv_rcon_banpenalty.value, NET_BaseAdrToString(net_from)));
		return 3;
	};

	if(!SV_CheckChallenge(&net_from, Q_atoi(Cmd_Argv(1))))
		return 2;

	if(Q_strcmp(Cmd_Argv(2), rcon_password.string))
	{
		AddFailedRcon(&net_from);
		return 1;
	};
	
	return 0;
};

bool CNetServer::CheckRconFailure(netadr_t *adr)
{
	for(int i = 0; i < MAX_RCON_FAILURES_STORAGE; i++)
	{
		rcon_failure_t *r = &g_rgRconFailures[i];
		
		if(NET_CompareAdr(*adr, r->adr))
		{
			if(r->shouldreject)
				return true;
		};
	};

	return false;
};

void CNetServer::AddFailedRcon(netadr_t *adr)
{
	int i;
	int best = 0;
	float best_update = -99999.0f;
	float time;
	bool found = false;
	rcon_failure_t *r;
	int failed;

	if(sv_rcon_minfailures.value < 1.0f)
		Cvar_SetValue("sv_rcon_minfailures", 1.0);
	else if(sv_rcon_minfailures.value > 20.0f)
		Cvar_SetValue("sv_rcon_minfailures", 20.0);
	
	if(sv_rcon_maxfailures.value < 1.0f)
		Cvar_SetValue("sv_rcon_maxfailures", 1.0);
	else if(sv_rcon_maxfailures.value > 20.0f)
		Cvar_SetValue("sv_rcon_maxfailures", 20.0);
	
	if(sv_rcon_maxfailures.value < sv_rcon_minfailures.value)
	{
		float temp = sv_rcon_maxfailures.value;
		Cvar_SetValue("sv_rcon_maxfailures", sv_rcon_minfailures.value);
		Cvar_SetValue("sv_rcon_minfailures", temp);
	};
	
	if(sv_rcon_minfailuretime.value < 1.0f)
		Cvar_SetValue("sv_rcon_minfailuretime", 1.0);

	for(i = 0; i < MAX_RCON_FAILURES_STORAGE; i++)
	{
		r = &g_rgRconFailures[i];
		
		if(!r->active)
			break;
		
		if(NET_CompareAdr(r->adr, *adr))
		{
			found = true;
			break;
		};
		
		time = (float)(realtime - r->last_update);
		
		if(time >= best_update)
		{
			best = i;
			best_update = time;
		};
	};

	// If no match found, take the oldest entry for usage
	if(i >= MAX_RCON_FAILURES_STORAGE)
		r = &g_rgRconFailures[best];

	// Prepare new or stale entry
	if(!found)
	{
		r->shouldreject = FALSE;
		r->num_failures = 0;
		Q_memcpy(&r->adr, adr, sizeof(netadr_t));
	}
	else if(r->shouldreject)
		return;

	r->active = TRUE;
	r->last_update = (float)realtime;

	if(r->num_failures >= sv_rcon_maxfailures.value)
	{
#ifdef REHLDS_FIXES
		// FIXED: Accessing beyond array
		for(i = 0; i < sv_rcon_maxfailures.value - 1; i++)
		{
			r->failure_times[i] = r->failure_times[i + 1];
		}
		r->num_failures = sv_rcon_maxfailures.value - 1;
#else  // REHLDS_FIXES
		for(i = 0; i < sv_rcon_maxfailures.value; i++)
		{
			r->failure_times[i] = r->failure_times[i + 1];
		}
		r->num_failures--;
#endif // REHLDS_FIXES
	}

	r->failure_times[r->num_failures] = (float)realtime;
	r->num_failures++;

	failed = 0;
	
	for(i = 0; i < r->num_failures; i++)
	{
		if(realtime - r->failure_times[i] <= sv_rcon_minfailuretime.value)
			failed++;
	};

	if(failed >= sv_rcon_minfailures.value)
	{
		mpConsole->Printf("User %s will be banned for rcon hacking\n", NET_AdrToString(*adr));
		r->shouldreject = TRUE;
	};
};