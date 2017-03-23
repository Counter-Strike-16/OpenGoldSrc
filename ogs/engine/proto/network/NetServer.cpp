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

void SVC_GetChallenge()
{
	char data[1024];
	qboolean steam = (Cmd_Argc() == 2 && !Q_stricmp(Cmd_Argv(1), "steam"));
	int challenge = SV_GetChallenge(net_from);

	if(steam)
		Q_snprintf(data, sizeof(data), "\xFF\xFF\xFF\xFF%c00000000 %u 3 %lld %d\n", S2C_CHALLENGE, challenge, g_RehldsHookchains.m_Steam_GSGetSteamID.callChain(Steam_GSGetSteamID), Steam_GSBSecure());
	else
	{
		Con_DPrintf("Server requiring authentication\n");
		Q_snprintf(data, sizeof(data), "\xFF\xFF\xFF\xFF%c00000000 %u 2\n", S2C_CHALLENGE, challenge);
	}

	// Give 3-rd party plugins a chance to modify challenge response
	g_RehldsHookchains.m_SVC_GetChallenge_mod.callChain(NULL, data, challenge);
	NET_SendPacket(NS_SERVER, Q_strlen(data) + 1, data, net_from);
};

void SVC_ServiceChallenge()
{
	char data[128];
	const char *type;
	int challenge;

	if(Cmd_Argc() != 2)
		return;

	type = Cmd_Argv(1);
	if(!type)
		return;

	if(!type[0] || Q_stricmp(type, "rcon"))
		return;

	challenge = SV_GetChallenge(net_from);

	Q_snprintf(data, sizeof(data), "%c%c%c%cchallenge %s %u\n", 255, 255, 255, 255, type, challenge);

	NET_SendPacket(NS_SERVER, Q_strlen(data) + 1, data, net_from);
};

void CNetServer::Start(int anPort)
{
};

void CNetServer::Stop()
{
};

void CNetServer::Frame()
{
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
	
	HandleConnectionlessPacket(c, args);
};

void CNetServer::BroadcastCommand(char *fmt, ...)
{
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

int CNetServer::GetMaxClients()
{
	return 0;
};

void CNetServer::HandleRcon(netadr_t *net_from_)
{
	char remaining[512];
	char rcon_buff[1024];

	int invalid = SV_Rcon_Validate();
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
			Con_Printf("Bad Rcon from %s:\n%s\n", NET_AdrToString(*net_from_), remaining);
			Log_Printf("Bad Rcon: \"%s\" from \"%s\"\n", remaining, NET_AdrToString(*net_from_));
		}
		else
		{
			Con_Printf("Rcon from %s:\n%s\n", NET_AdrToString(*net_from_), remaining);
			Log_Printf("Rcon: \"%s\" from \"%s\"\n", remaining, NET_AdrToString(*net_from_));
		}
	}

	SV_BeginRedirect(RD_PACKET, net_from_);

	if(invalid)
	{
		if(invalid == 2)
			Con_Printf("Bad rcon_password.\n");
		else if(Q_strlen(rcon_password.string) == 0)
			Con_Printf("Bad rcon_password.\nNo password set for this server.\n");
		else
			Con_Printf("Bad rcon_password.\n");

		SV_EndRedirect();
		return;
	}
	char *data = COM_Parse(COM_Parse(COM_Parse(remaining)));
	if(!data)
	{
		Con_Printf("Empty rcon\n");

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
		Con_Printf("Banning %s for rcon hacking attempts\n",
		           NET_AdrToString(net_from));
		Cbuf_AddText(va("addip %i %s\n", (int)sv_rcon_banpenalty.value, NET_BaseAdrToString(net_from)));
		return 3;
	};

	if(!SV_CheckChallenge(&net_from, Q_atoi(Cmd_Argv(1))))
		return 2;

	if(Q_strcmp(Cmd_Argv(2), rcon_password.string))
	{
		SV_AddFailedRcon(&net_from);
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