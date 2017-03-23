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
/// @brief server connection handler component

#include "precompiled.hpp"
#include "network/LoginServer.hpp"

bool EXT_FUNC CLoginServer::ConnectClient(netadr_t *adr)
{
	int nClientSlot = 0;
#ifdef REHLDS_FIXES
	char userinfo[MAX_INFO_STRING];
#else
	char userinfo[1024];
#endif
	char protinfo[1024];
	char cdkey[64];
	IInfoKeyBuffer *s;
	char name[32];
	char szRawCertificate[512];
	int nAuthProtocol;
	unsigned short port;
	qboolean reconnect;
	qboolean bIsSecure;

	client_t *client = NULL;
	
	nAuthProtocol = -1;
	reconnect = FALSE;
	port = Q_atoi("27005");
	
	if(Cmd_Argc() < 5)
	{
		RejectConnection(adr, "Insufficient connection info\n");
		return;
	};

	if(!CheckProtocol(adr, Q_atoi(Cmd_Argv(1))))
		return;

	if(!CheckChallenge(adr, Q_atoi(Cmd_Argv(2))))
		return;

	Q_memset(szRawCertificate, 0, sizeof(szRawCertificate));
	s = Cmd_Argv(3);
	
	if(!s->IsValid(s))
	{
		RejectConnection(adr, "Invalid protinfo in connect command\n");
		return;
	};

	Q_strncpy(protinfo, s, sizeof(protinfo) - 1);
	protinfo[sizeof(protinfo) - 1] = 0;
	
	if(!CheckKeyInfo(adr, protinfo, &port, &nAuthProtocol, szRawCertificate, cdkey))
		return;

	if(!CheckIPRestrictions(adr, nAuthProtocol))
	{
		RejectConnection(adr, "LAN servers are restricted to local clients (class C).\n");
		return;
	}

	s = Cmd_Argv(4);
#ifdef REHLDS_FIXES
	// truncate to 255 before sanity checks
	if(Q_strlen(s) > MAX_INFO_STRING - 1 || !Info_IsValid(s))
#else
	if(Q_strlen(s) > 256 || !Info_IsValid(s))
#endif
	{
		RejectConnection(adr, "Invalid userinfo in connect command\n");
		return;
	};
	
	Q_strncpy(userinfo, s, sizeof(userinfo) - 1);
	userinfo[sizeof(userinfo) - 1] = 0;
	
	if(Master_IsLanGame() || nAuthProtocol == 2 || nAuthProtocol == 3)
	{
		for(nClientSlot = 0; nClientSlot < g_psvs.maxclients; nClientSlot++)
		{
			client = &g_psvs.clients[nClientSlot];
#ifndef REHLDS_FIXES
			if(NET_CompareAdr(adr, client->netchan.remote_address))
			{
				reconnect = TRUE;
				break;
			};
#else  // REHLDS_FIXES
			auto isTimedOut = [](client_t *cl) {
				return realtime - cl->netchan.last_received > 10.0;
			};

			if(NET_CompareBaseAdr(adr, client->netchan.remote_address) &&
			   (adr.port == client->netchan.remote_address.port ||
			    isTimedOut(client)))
			{
				reconnect = TRUE;
				break;
			};
#endif // REHLDS_FIXES
		};
	};

	if(!CheckUserInfo(adr, userinfo, reconnect, nClientSlot, name))
		return;

	if(!FinishCertificateCheck(adr, nAuthProtocol, szRawCertificate, userinfo))
		return;

	if(reconnect)
	{
		Steam_NotifyClientDisconnect(client);
		
		if((client->active || client->spawned) && client->edict)
			gEntityInterface.pfnClientDisconnect(client->edict);

		mpConsole->Printf("%s:reconnect\n", NET_AdrToString(adr));
	}
	else
	{
		if(!mpServer->FindEmptySlot(&nClientSlot, &client))
		{
			RejectConnection(adr, "Server is full.\n");
			return;
		};
	};

	if(!CheckIPConnectionReuse(adr))
		return;

	host_client = client;
	client->userid = g_userid++;
	
	if(nAuthProtocol == 3)
	{
		char szSteamAuthBuf[1024];
		int len = net_message.cursize - msg_readcount;
		
		if(net_message.cursize - msg_readcount <= 0 || len >= sizeof(szSteamAuthBuf))
		{
			RejectConnection(adr, "STEAM certificate length error! %i/%i\n", net_message.cursize - msg_readcount, sizeof(szSteamAuthBuf));
			return;
		};
		
		Q_memcpy(szSteamAuthBuf, &net_message.data[msg_readcount], len);
		client->network_userid.clientip = *(uint32 *)&adr.ip[0];
		
		if(adr.type == NA_LOOPBACK)
		{
			if(sv_lan.value <= 0.0f)
				client->network_userid.clientip = *(uint32 *)&adr.ip[0];
			else
				client->network_userid.clientip = 0x7F000001; // 127.0.0.1
		};

		client->netchan.remote_address.port = adr.port ? adr.port : port;
		
		if(!Steam_NotifyClientConnect(client, szSteamAuthBuf, len))
		{
			if(sv_lan.value == 0.0f)
			{
				RejectConnection(&adr, "STEAM validation rejected\n");
				return;
			};
			
			host_client->network_userid.idtype = AUTH_IDTYPE_STEAM;
			host_client->network_userid.m_SteamID = 0;
		};
	}
	else
	{
		if(nAuthProtocol != 2)
		{
			RejectConnection(adr, "Invalid authentication type\n");
			return;
		};

		const char *val = userinfo->ValueForKey("*hltv");
		
		if(!Q_strlen(val))
		{
			RejectConnection(adr, "Invalid validation type\n");
			return;
		};
		
		if(Q_atoi(val) != 1)
		{
			RejectConnection(adr, "Invalid validation type\n");
			return;
		};
		
		host_client->network_userid.idtype = AUTH_IDTYPE_LOCAL;
		host_client->network_userid.m_SteamID = 0;
		host_client->network_userid.clientip = *(uint32 *)&adr.ip[0];
		
		Steam_NotifyBotConnect(client);
	};
};

void EXT_FUNC CLoginServer::RejectConnection(netadr_t *adr, char *fmt, ...)
{
	va_list argptr;
	char text[1024];
	va_start(argptr, fmt);
	Q_vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	SZ_Clear(&net_message);
	MSG_WriteLong(&net_message, -1);
	MSG_WriteByte(&net_message, '9');
	MSG_WriteString(&net_message, text);
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr);
	SZ_Clear(&net_message);
};

void CLoginServer::RejectConnectionForPassword(netadr_t *adr)
{
	SZ_Clear(&net_message);
	MSG_WriteLong(&net_message, -1);
	MSG_WriteByte(&net_message, '8');
	MSG_WriteString(&net_message, "BADPASSWORD");
	NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr);
	SZ_Clear(&net_message);
};

int CLoginServer::CheckProtocol(netadr_t *adr, int nProtocol)
{
	return g_RehldsHookchains.m_SV_CheckProtocol.callChain(SV_CheckProtocol_internal, adr, nProtocol);
};

int EXT_FUNC CLoginServer::CheckProtocol_internal(netadr_t *adr, int nProtocol)
{
	if(adr == NULL)
		Sys_Error("%s:  Null address\n", __FUNCTION__);

	if(nProtocol == PROTOCOL_VERSION)
		return TRUE;

	if(nProtocol < PROTOCOL_VERSION)
	{
		RejectConnection(adr, "This server is using a newer protocol ( %i ) "
		                         "than your client ( %i ).  You should check for "
		                         "updates to your client.\n",
		                    PROTOCOL_VERSION,
		                    nProtocol);
	}
	else
	{
		char *contact = "(no email address specified)";
		if(sv_contact.string[0] != 0)
			contact = sv_contact.string;

		RejectConnection(adr, "This server is using an older protocol ( %i ) "
		                         "than your client ( %i ).  If you believe this "
		                         "server is outdated, you can contact the server "
		                         "administrator at %s.\n",
		                    PROTOCOL_VERSION,
		                    nProtocol,
		                    contact);
	};

	return FALSE;
};

typedef struct challenge_s
{
	netadr_t adr;
	int challenge;
	int time;
} challenge_t;

challenge_t g_rg_sv_challenges[MAX_CHALLENGES];
#ifdef REHLDS_OPT_PEDANTIC
int g_oldest_challenge = 0;
#endif

bool EXT_FUNC CLoginServer::CheckChallenge_api(const netadr_t &adr, int nChallengeValue)
{
	netadr_t localAdr = adr;
	return CheckChallenge(&localAdr, nChallengeValue) != 0;
};

int CLoginServer::CheckChallenge(netadr_t *adr, int nChallengeValue)
{
	if(!adr)
		Sys_Error("%s:  Null address\n", __FUNCTION__);

	if(NET_IsLocalAddress(*adr))
		return 1;

	for(int i = 0; i < MAX_CHALLENGES; i++)
	{
		if(NET_CompareBaseAdr(net_from, g_rg_sv_challenges[i].adr))
		{
			if(nChallengeValue != g_rg_sv_challenges[i].challenge)
			{
				RejectConnection(adr, "Bad challenge.\n");
				return 0;
			};
			
			return 1;
		};
	};
	
	RejectConnection(adr, "No challenge for your address.\n");
	return 0;
};

int CLoginServer::CheckKeyInfo(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey)
{
	return g_RehldsHookchains.m_SV_CheckKeyInfo.callChain(SV_CheckKeyInfo_internal, adr, protinfo, port, pAuthProtocol, pszRaw, cdkey);
};

int EXT_FUNC CLoginServer::CheckKeyInfo_internal(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey)
{
	const char *s = Info_ValueForKey(protinfo, "prot");
	int nAuthProtocol = Q_atoi(s);

	if(nAuthProtocol <= 0 || nAuthProtocol > 4)
	{
		RejectConnection(adr, "Invalid connection.\n");
		return 0;
	};

	s = Info_ValueForKey(protinfo, "raw");

	if(s[0] == 0 || (nAuthProtocol == 2 && Q_strlen(s) != 32))
	{
		RejectConnection(adr, "Invalid authentication certificate length.\n");
		return 0;
	};

	Q_strcpy(pszRaw, s);

	if(nAuthProtocol != 2)
	{
		s = Info_ValueForKey(protinfo, "cdkey");

		if(Q_strlen(s) != 32)
		{
			RejectConnection(adr, "Invalid hashed CD key.\n");
			return 0;
		};
	};

	Q_snprintf(cdkey, 64, "%s", s);
	*pAuthProtocol = nAuthProtocol;
	*port = Q_atoi("27005");

	return 1;
};

int CLoginServer::CheckIPRestrictions(netadr_t *adr, int nAuthProtocol)
{
	return g_RehldsHookchains.m_SV_CheckIPRestrictions.callChain(SV_CheckIPRestrictions_internal, adr, nAuthProtocol);
};

int EXT_FUNC CLoginServer::CheckIPRestrictions_internal(netadr_t *adr, int nAuthProtocol)
{
	if(sv_lan.value || nAuthProtocol != 3)
	{
		if(nAuthProtocol == 2)
			return 1;

		return (sv_lan.value && (NET_CompareClassBAdr(*adr, net_local_adr) ||
		                         NET_IsReservedAdr(*adr)));
	};
	
	return 1;
};

int CLoginServer::CheckUserInfo(netadr_t *adr, char *userinfo, qboolean bIsReconnecting, int nReconnectSlot, char *name)
{
	const char *s;
	char newname[MAX_NAME];
	int proxies;
	int i;

	if(!NET_IsLocalAddress(*adr))
	{
		const char *password = Info_ValueForKey(userinfo, "password");

		if(sv_password.string[0] != '\0' &&
		   Q_stricmp(sv_password.string, "none") &&
		   Q_strcmp(sv_password.string, password))
		{
#ifdef REHLDS_FIXES
			if(password[0] == '\0')
			{
				mpConsole->Printf("%s:  connect without password\n", NET_AdrToString(*adr));
				RejectConnection(adr, "No password set. Clean your userinfo.\n");
			}
			else
#endif // REHLDS_FIXES
			{
				mpConsole->Printf("%s:  password failed\n", NET_AdrToString(*adr));
				RejectConnectionForPassword(adr);
			};

			return 0;
		};
	};

	i = Q_strlen(userinfo);
	if(i <= 4 || Q_strstr(userinfo, "\\\\") || userinfo[i - 1] == '\\')
	{
		RejectConnection(adr, "Unknown HLTV client type.\n");
		return 0;
	};

	Info_RemoveKey(userinfo, "password");

	s = Info_ValueForKey(userinfo, "name");
#ifdef REHLDS_FIXES
	SV_ReplaceSpecialCharactersInName(newname, s);
#else  // REHLDS_FIXES
	Q_strncpy(newname, s, sizeof(newname) - 1);
	newname[sizeof(newname) - 1] = '\0';

	for(char *pChar = newname; *pChar; pChar++)
	{
		if(*pChar == '%' || *pChar == '&')
			*pChar = ' ';
	};
#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
	Q_strcpy(name, newname);
	Q_StripUnprintableAndSpace(name);
#else  // REHLDS_FIXES
	TrimSpace(newname, name);
#endif // REHLDS_FIXES

	if(!Q_UnicodeValidate(name))
		Q_UnicodeRepair(name);

#ifndef REHLDS_FIXES
	for(char *pChar = newname; *pChar == '#'; pChar++)
		*pChar = ' ';
#endif

	if(name[0] == 0 || !Q_stricmp(name, "console") || Q_strstr(name, "..") != NULL)
		Info_SetValueForKey(userinfo, "name", "unnamed", MAX_INFO_STRING);
	else
		Info_SetValueForKey(userinfo, "name", name, MAX_INFO_STRING);

	if(SV_CheckForDuplicateNames(userinfo, bIsReconnecting, nReconnectSlot))
	{
		Q_strncpy(name, Info_ValueForKey(userinfo, "name"), MAX_NAME - 1);
		name[MAX_NAME - 1] = 0;
	};

	s = Info_ValueForKey(userinfo, "*hltv");

	if(!s[0])
		return 1;

	switch(Q_atoi(s))
	{
	case 0:
		return 1;

	case 1:
		SV_CountProxies(&proxies);
		
		if(proxies >= sv_proxies.value && !bIsReconnecting)
		{
			RejectConnection(adr, "Proxy slots are full.\n");
			return 0;
		};
		
		return 1;

	case 3:
		RejectConnection(adr, "Please connect to HLTV master proxy.\n");
		return 0;

	default:
		RejectConnection(adr, "Unknown HLTV client type.\n");
		return 0;
	};
};

int CLoginServer::FinishCertificateCheck(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo)
{
	return g_RehldsHookchains.m_SV_FinishCertificateCheck.callChain(SV_FinishCertificateCheck_internal, adr, nAuthProtocol, szRawCertificate, userinfo);
};

int EXT_FUNC CLoginServer::FinishCertificateCheck_internal(netadr_t *adr,
                                                int nAuthProtocol,
                                                char *szRawCertificate,
                                                char *userinfo)
{
	if(nAuthProtocol != 2)
	{
		if(Q_stricmp(szRawCertificate, "steam"))
		{
			SV_RejectConnection(adr, "Expecting STEAM authentication USERID ticket!\n");
			return 0;
		};

		return 1;
	};

	if(Q_strlen(szRawCertificate) != 32)
	{
		SV_RejectConnection(adr, "Invalid CD Key.\n");
		return 0;
	};

	if(adr->type == NA_LOOPBACK)
		return 1;

	const char *val = Info_ValueForKey(userinfo, "*hltv");

	if(val[0] == 0 || Q_atoi(val) != 1)
	{
		SV_RejectConnection(adr, "Invalid CD Key.\n");
		return 0;
	};

	return 1;
};

int CLoginServer::CheckIPConnectionReuse(netadr_t *adr)
{
	int count = 0;

	client_t *cl = g_psvs.clients;
	for(int i = 0; i < g_psvs.maxclients; i++, cl++)
	{
		if(cl->connected && !cl->fully_connected && NET_CompareBaseAdr(cl->netchan.remote_address, *adr))
			count++;
	};

	if(count > 5)
	{
		Log_Printf("Too many connect packets from %s\n", NET_AdrToString(*adr));
		return 0;
	};

	return 1;
};