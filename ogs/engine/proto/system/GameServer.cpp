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

#include "system/GameServer.hpp"

void CGameServer::Init()
{
#ifdef HOOK_ENGINE

	Cmd_AddCommand("banid", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_BanId_f", (void *)SV_BanId_f));
	Cmd_AddCommand("removeid", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_RemoveId_f", (void *)SV_RemoveId_f));
	Cmd_AddCommand("listid", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_ListId_f", (void *)SV_ListId_f));
	Cmd_AddCommand("writeid", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_WriteId_f", (void *)SV_WriteId_f));
	Cmd_AddCommand("resetrcon", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_ResetRcon_f", (void *)SV_ResetRcon_f));
	Cmd_AddCommand("logaddress",
	               (xcommand_t)GetOriginalFuncAddrOrDefault(
	               "SV_SetLogAddress_f", (void *)SV_SetLogAddress_f));
	Cmd_AddCommand("logaddress_add",
	               (xcommand_t)GetOriginalFuncAddrOrDefault(
	               "SV_AddLogAddress_f", (void *)SV_AddLogAddress_f));
	Cmd_AddCommand("logaddress_del",
	               (xcommand_t)GetOriginalFuncAddrOrDefault(
	               "SV_DelLogAddress_f", (void *)SV_DelLogAddress_f));
	Cmd_AddCommand("log", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_ServerLog_f", (void *)SV_ServerLog_f));
	Cmd_AddCommand("serverinfo", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_Serverinfo_f", (void *)SV_Serverinfo_f));
	Cmd_AddCommand("localinfo", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_Localinfo_f", (void *)SV_Localinfo_f));
	Cmd_AddCommand("showinfo",
	               (xcommand_t)GetOriginalFuncAddrOrDefault(
	               "SV_ShowServerinfo_f", (void *)SV_ShowServerinfo_f));
	Cmd_AddCommand("user", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_User_f", (void *)SV_User_f));
	Cmd_AddCommand("users", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_Users_f", (void *)SV_Users_f));
	Cmd_AddCommand("dlfile",
	               (xcommand_t)GetOriginalFuncAddrOrDefault(
	               "SV_BeginFileDownload_f", (void *)SV_BeginFileDownload_f));
	Cmd_AddCommand("addip", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_AddIP_f", (void *)SV_AddIP_f));
	Cmd_AddCommand("removeip", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_RemoveIP_f", (void *)SV_RemoveIP_f));
	Cmd_AddCommand("listip", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_ListIP_f", (void *)SV_ListIP_f));
	Cmd_AddCommand("writeip", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_WriteIP_f", (void *)SV_WriteIP_f));
	Cmd_AddCommand("dropclient", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_Drop_f", (void *)SV_Drop_f));
	Cmd_AddCommand("spawn", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_Spawn_f", (void *)SV_Spawn_f));
	Cmd_AddCommand("new", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_New_f", (void *)SV_New_f));
	Cmd_AddCommand("sendres", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_SendRes_f", (void *)SV_SendRes_f));
	Cmd_AddCommand("sendents", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_SendEnts_f", (void *)SV_SendEnts_f));
	Cmd_AddCommand("fullupdate", (xcommand_t)GetOriginalFuncAddrOrDefault("SV_FullUpdate_f", (void *)SV_FullUpdate_f));

#else // HOOK_ENGINE

	Cmd_AddCommand("banid", SV_BanId_f);
	Cmd_AddCommand("removeid", SV_RemoveId_f);
	Cmd_AddCommand("listid", SV_ListId_f);
	Cmd_AddCommand("writeid", SV_WriteId_f);
	Cmd_AddCommand("resetrcon", SV_ResetRcon_f);
	Cmd_AddCommand("logaddress", SV_SetLogAddress_f);
	Cmd_AddCommand("logaddress_add", SV_AddLogAddress_f);
	Cmd_AddCommand("logaddress_del", SV_DelLogAddress_f);
	Cmd_AddCommand("log", SV_ServerLog_f);
	Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
	Cmd_AddCommand("localinfo", SV_Localinfo_f);
	Cmd_AddCommand("showinfo", SV_ShowServerinfo_f);
	Cmd_AddCommand("user", SV_User_f);
	Cmd_AddCommand("users", SV_Users_f);
	Cmd_AddCommand("dlfile", SV_BeginFileDownload_f);
	Cmd_AddCommand("addip", SV_AddIP_f);
	Cmd_AddCommand("removeip", SV_RemoveIP_f);
	Cmd_AddCommand("listip", SV_ListIP_f);
	Cmd_AddCommand("writeip", SV_WriteIP_f);
	Cmd_AddCommand("dropclient", SV_Drop_f);
	Cmd_AddCommand("spawn", SV_Spawn_f);
	Cmd_AddCommand("new", SV_New_f);
	Cmd_AddCommand("sendres", SV_SendRes_f);
	Cmd_AddCommand("sendents", SV_SendEnts_f);
	Cmd_AddCommand("fullupdate", SV_FullUpdate_f);

#endif // HOOK_ENGINE

	Cvar_RegisterVariable(&sv_failuretime);
	Cvar_RegisterVariable(&sv_voiceenable);
	Cvar_RegisterVariable(&rcon_password);
	Cvar_RegisterVariable(&sv_enableoldqueries);
	Cvar_RegisterVariable(&mp_consistency);
	Cvar_RegisterVariable(&sv_instancedbaseline);
	Cvar_RegisterVariable(&sv_contact);
	Cvar_RegisterVariable(&sv_unlag);
	Cvar_RegisterVariable(&sv_maxunlag);
	Cvar_RegisterVariable(&sv_unlagpush);
	Cvar_RegisterVariable(&sv_unlagsamples);
	Cvar_RegisterVariable(&sv_filterban);
	Cvar_RegisterVariable(&sv_maxupdaterate);
	Cvar_RegisterVariable(&sv_minupdaterate);
	Cvar_RegisterVariable(&sv_logrelay);
	Cvar_RegisterVariable(&sv_lan);
	
	Cvar_DirectSet(&sv_lan, PF_IsDedicatedServer() ? "0" : "1");
	
	Cvar_RegisterVariable(&sv_lan_rate);
	Cvar_RegisterVariable(&sv_proxies);
	Cvar_RegisterVariable(&sv_outofdatetime);
	Cvar_RegisterVariable(&sv_visiblemaxplayers);
	Cvar_RegisterVariable(&sv_password);
	Cvar_RegisterVariable(&sv_aim);
	Cvar_RegisterVariable(&violence_hblood);
	Cvar_RegisterVariable(&violence_ablood);
	Cvar_RegisterVariable(&violence_hgibs);
	Cvar_RegisterVariable(&violence_agibs);
	Cvar_RegisterVariable(&sv_newunit);
	Cvar_RegisterVariable(&sv_gravity);
	Cvar_RegisterVariable(&sv_friction);
	Cvar_RegisterVariable(&sv_edgefriction);
	Cvar_RegisterVariable(&sv_stopspeed);
	Cvar_RegisterVariable(&sv_maxspeed);
	Cvar_RegisterVariable(&sv_footsteps);
	Cvar_RegisterVariable(&sv_accelerate);
	Cvar_RegisterVariable(&sv_stepsize);
	Cvar_RegisterVariable(&sv_bounce);
	Cvar_RegisterVariable(&sv_airaccelerate);
	Cvar_RegisterVariable(&sv_wateraccelerate);
	Cvar_RegisterVariable(&sv_waterfriction);
	Cvar_RegisterVariable(&sv_skycolor_r);
	Cvar_RegisterVariable(&sv_skycolor_g);
	Cvar_RegisterVariable(&sv_skycolor_b);
	Cvar_RegisterVariable(&sv_skyvec_x);
	Cvar_RegisterVariable(&sv_skyvec_y);
	Cvar_RegisterVariable(&sv_skyvec_z);
	Cvar_RegisterVariable(&sv_timeout);
	Cvar_RegisterVariable(&sv_clienttrace);
	Cvar_RegisterVariable(&sv_zmax);
	Cvar_RegisterVariable(&sv_wateramp);
	Cvar_RegisterVariable(&sv_skyname);
	Cvar_RegisterVariable(&sv_maxvelocity);
	Cvar_RegisterVariable(&sv_cheats);
	
	if(COM_CheckParm("-dev"))
		Cvar_SetValue("sv_cheats", 1.0);
	
	Cvar_RegisterVariable(&sv_spectatormaxspeed);
	Cvar_RegisterVariable(&sv_allow_download);
	Cvar_RegisterVariable(&sv_allow_upload);
	Cvar_RegisterVariable(&sv_max_upload);
	Cvar_RegisterVariable(&sv_send_logos);
	Cvar_RegisterVariable(&sv_send_resources);
	Cvar_RegisterVariable(&sv_logbans);
	Cvar_RegisterVariable(&hpk_maxsize);
	Cvar_RegisterVariable(&mapcyclefile);
	Cvar_RegisterVariable(&motdfile);
	Cvar_RegisterVariable(&servercfgfile);
	Cvar_RegisterVariable(&mapchangecfgfile);
	Cvar_RegisterVariable(&lservercfgfile);
	Cvar_RegisterVariable(&logsdir);
	Cvar_RegisterVariable(&bannedcfgfile);
	
#ifdef REHLDS_FIXES
	Cvar_RegisterVariable(&listipcfgfile);
	Cvar_RegisterVariable(&syserror_logfile);
#endif
	
	Cvar_RegisterVariable(&sv_rcon_minfailures);
	Cvar_RegisterVariable(&sv_rcon_maxfailures);
	Cvar_RegisterVariable(&sv_rcon_minfailuretime);
	Cvar_RegisterVariable(&sv_rcon_banpenalty);
	Cvar_RegisterVariable(&sv_minrate);
	Cvar_RegisterVariable(&sv_maxrate);
	Cvar_RegisterVariable(&max_queries_sec);
	Cvar_RegisterVariable(&max_queries_sec_global);
	Cvar_RegisterVariable(&max_queries_window);
	Cvar_RegisterVariable(&sv_logblocks);
	Cvar_RegisterVariable(&sv_downloadurl);
	Cvar_RegisterVariable(&sv_version);
	Cvar_RegisterVariable(&sv_allow_dlfile);
	
#ifdef REHLDS_FIXES
	Cvar_RegisterVariable(&sv_force_ent_intersection);
	Cvar_RegisterVariable(&sv_echo_unknown_cmd);
	Cvar_RegisterVariable(&sv_auto_precache_sounds_in_models);
	Cvar_RegisterVariable(&sv_delayed_spray_upload);
	Cvar_RegisterVariable(&sv_rehlds_force_dlmax);
	Cvar_RegisterVariable(&sv_rehlds_hull_centering);
	Cvar_RegisterVariable(&sv_rcon_condebug);
	Cvar_RegisterVariable(&sv_rehlds_userinfo_transmitted_fields);
	Cvar_RegisterVariable(&sv_rehlds_attachedentities_playeranimationspeed_fix);
	Cvar_RegisterVariable(&sv_rehlds_local_gametime);
#endif

	for(int i = 0; i < MAX_MODELS; i++)
		Q_snprintf(localmodels[i], sizeof(localmodels[i]), "*%i", i);

	g_psvs.isSecure = FALSE;

	for(int i = 0; i < g_psvs.maxclientslimit; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		
		SV_ClearFrames(&cl->frames);
		
		Q_memset(cl, 0, sizeof(client_t));
		
		cl->resourcesonhand.pPrev = &cl->resourcesonhand;
		cl->resourcesonhand.pNext = &cl->resourcesonhand;
		cl->resourcesneeded.pPrev = &cl->resourcesneeded;
		cl->resourcesneeded.pNext = &cl->resourcesneeded;
	};

	PM_Init(&g_svmove);
	SV_AllocClientFrames();
	SV_InitDeltas();
};

void CGameServer::Shutdown()
{
	g_DeltaJitRegistry.Cleanup();
	delta_info_t *p = g_sv_delta;
	
	while(p)
	{
		delta_info_t *n = p->next;
		if(p->delta)
			DELTA_FreeDescription(&p->delta);

		Mem_Free(p->name);
		Mem_Free(p->loadfile);
		Mem_Free(p);
		p = n;
	};

	g_sv_delta = NULL;
};

void CGameServer::Frame(float frametime)
{
	if(!g_psv.active)
		return;

	gGlobalVariables.frametime = frametime;
	g_psv.oldtime = g_psv.time;
	
	CheckCmdTimes();
	ReadPackets();
	
	if(SV_IsSimulating())
	{
		SV_Physics();
		g_psv.time += frametime;
	};
	
	SV_QueryMovevarsChanged();
	SV_RequestMissingResourcesFromClients();
	CheckTimeouts();
	SendClientMessages();
	CheckMapDifferences();
	GatherStatistics();
	Steam_RunFrame();
};

void CGameServer::ClearClientStates()
{
	client_t *pcl = g_psvs.clients;
	
	for(int i = 0; i < g_psvs.maxclients; i++, pcl++)
	{
		COM_ClearCustomizationList(&pcl->customdata, FALSE);
		SV_ClearResourceLists(pcl);
	};
};

void CGameServer::SetupMaxClients()
{
	int i;
	client_t *cl;

	for(i = 0, cl = g_psvs.clients; i < g_psvs.maxclientslimit; i++, cl++)
		SV_ClearFrames(&cl->frames);

	g_psvs.maxclients = 1;
	i = COM_CheckParm("-maxplayers");

	if(i)
		g_psvs.maxclients = Q_atoi(com_argv[i + 1]);
	else
	{
		if(gbIsDedicatedServer)
			g_psvs.maxclients = 6;
	}

	cls.state = (cactive_t)(gbIsDedicatedServer == FALSE);

	if(g_psvs.maxclients > 32)
		g_psvs.maxclients = 32;
	if(g_psvs.maxclients < 1)
		g_psvs.maxclients = 6;

	if(gbIsDedicatedServer)
		g_psvs.maxclientslimit = 32;
	else if(host_parms.memsize > 0x1000000)
		g_psvs.maxclientslimit = 4;

	SV_UPDATE_BACKUP = 8;
	SV_UPDATE_MASK = 7;

	if(g_psvs.maxclients != 1)
	{
		SV_UPDATE_BACKUP = 64;
		SV_UPDATE_MASK = 63;
	};

	g_psvs.clients = (client_t *)Hunk_AllocName(
	sizeof(client_t) * g_psvs.maxclientslimit, "clients");
	for(i = 0, cl = g_psvs.clients; i < g_psvs.maxclientslimit; i++, cl++)
	{
		Q_memset(cl, 0, sizeof(client_t));

		cl->resourcesneeded.pPrev = &cl->resourcesneeded;
		cl->resourcesneeded.pNext = &cl->resourcesneeded;
		cl->resourcesonhand.pPrev = &cl->resourcesonhand;
		cl->resourcesonhand.pNext = &cl->resourcesonhand;
	}
	if(g_psvs.maxclients >= 2)
		Cvar_SetValue("deathmatch", 1.0);
	else
		Cvar_SetValue("deathmatch", 0.0);
	
	SV_AllocClientFrames();

	if(g_psvs.maxclientslimit < g_psvs.maxclients)
		g_psvs.maxclients = g_psvs.maxclientslimit;

	Rehlds_Interfaces_InitClients();
};

void CGameServer::CheckForRcon()
{
	if(g_psv.active || cls.state != ca_dedicated || giActive == DLL_CLOSE ||
	   !host_initialized)
		return;

	while(NET_GetPacket(NS_SERVER))
	{
		if(SV_FilterPacket())
			SV_SendBan();
		else
		{
			if(*(uint32 *)net_message.data == 0xFFFFFFFF)
				HandleRconPacket();
		};
	};
};

void CGameServer::HandleConnectionlessPacket(const char *c, const char *args)
{
	if(!Q_strcmp(c, "ping") || (c[0] == A2A_PING && (c[1] == 0 || c[1] == '\n')))
		SVC_Ping();
	else if(c[0] == A2A_ACK && (c[1] == 0 || c[1] == '\n'))
		Con_NetPrintf("A2A_ACK from %s\n", NET_AdrToString(net_from));
	else if(c[0] == A2A_GETCHALLENGE || c[0] == A2S_INFO ||
	        c[0] == A2S_PLAYER || c[0] == A2S_RULES || c[0] == S2A_LOGSTRING ||
	        c[0] == M2S_REQUESTRESTART || c[0] == M2A_CHALLENGE)
		return;

	else if(!Q_stricmp(c, "log"))
	{
		if(sv_logrelay.value != 0.0f && Q_strlen(args) > 4)
		{
			const char *s = &args[Q_strlen("log ")];
			if(s && s[0])
				mpConsole->Printf("%s\n", s);
		};
	}
	else if(!Q_strcmp(c, "connect"))
		ConnectClient();
	else if(!Q_strcmp(c, "pstat"))
	{
		if(g_modfuncs.m_pfnPlayerStatus)
			g_modfuncs.m_pfnPlayerStatus(net_message.data, net_message.cursize);
	}
	else
		SVC_GameDllQuery(args);
};

void CGameServer::ConnectClient()
{
	g_RehldsHookchains.m_SV_ConnectClient.callChain(SV_ConnectClient_internal);
};

void EXT_FUNC CGameServer::ConnectClient_internal()
{
	netadr_t adr;
	Q_memcpy(&adr, &net_from, sizeof(adr));
	
	//mpConnectionHandler
	if(!mpLoginServer->ConnectClient(&adr))
		return;
	
	SV_ClearResourceLists(host_client);
	SV_ClearFrames(&host_client->frames);

	host_client->frames = (client_frame_t *)Mem_ZeroMalloc(sizeof(client_frame_t) * SV_UPDATE_BACKUP);
	
	host_client->resourcesneeded.pPrev = &host_client->resourcesneeded;
	host_client->resourcesneeded.pNext = &host_client->resourcesneeded;
	host_client->resourcesonhand.pPrev = &host_client->resourcesonhand;
	host_client->resourcesonhand.pNext = &host_client->resourcesonhand;
	
	host_client->edict = EDICT_NUM(nClientSlot + 1);
	
	//BroadcastEvent(eNetworkEvent::ClientConnect, nClientSlot);
	if(g_modfuncs.m_pfnConnectClient)
		g_modfuncs.m_pfnConnectClient(nClientSlot);

	Netchan_Setup(NS_SERVER, &host_client->netchan, adr, client - g_psvs.clients, client, SV_GetFragmentSize);
	host_client->next_messageinterval = 5.0;
	host_client->next_messagetime = realtime + 0.05;
	host_client->delta_sequence = -1;
	Q_memset(&host_client->lastcmd, 0, sizeof(usercmd_t));
	host_client->nextping = -1.0;
	
	if(host_client->netchan.remote_address.type == NA_LOOPBACK)
		mpConsole->DPrintf("Local connection.\n");
	else
		mpConsole->DPrintf("Client %s connected\nAdr: %s\n", name, NET_AdrToString(host_client->netchan.remote_address));
	
	Q_strncpy(host_client->hashedcdkey, cdkey, 32);

	host_client->hashedcdkey[32] = 0;
	host_client->active = FALSE;
	host_client->spawned = FALSE;
	host_client->connected = TRUE;
	host_client->uploading = FALSE;
	host_client->fully_connected = FALSE;

#ifdef REHLDS_FIXES
	g_GameClients[host_client - g_psvs.clients]->SetSpawnedOnce(false);
#endif // REHLDS_FIXES

	bIsSecure = Steam_GSBSecure();
	Netchan_OutOfBandPrint(NS_SERVER, adr, "%c %i \"%s\" %i %i", S2C_CONNECTION, host_client->userid, NET_AdrToString(host_client->netchan.remote_address), bIsSecure, build_number());
	Log_Printf("\"%s<%i><%s><>\" connected, address \"%s\"\n", name, host_client->userid, SV_GetClientIDString(host_client), NET_AdrToString(host_client->netchan.remote_address));
#ifdef REHLDS_FIXES
	Q_strncpy(host_client->userinfo, userinfo, MAX_INFO_STRING - 1);
#else
	Q_strncpy(host_client->userinfo, userinfo, MAX_INFO_STRING);
#endif
	host_client->userinfo[MAX_INFO_STRING - 1] = 0;

	SV_ExtractFromUserinfo(host_client);
	host_client->userinfo->SetValueForStarKey("*sid", va("%lld", host_client->network_userid.m_SteamID), MAX_INFO_STRING);

	host_client->datagram.flags = SIZEBUF_ALLOW_OVERFLOW;
	host_client->datagram.data = (byte *)host_client->datagram_buf;
	host_client->datagram.maxsize = sizeof(host_client->datagram_buf);
	host_client->datagram.buffername = host_client->name;
	
	host_client->sendinfo_time = 0.0f;

	// Rehlds Security
	//Rehlds_Security_ClientConnected(host_client - g_psvs.clients);

	g_RehldsHookchains.m_ClientConnected.callChain(NULL, GetRehldsApiClient(host_client));
};

void CGameServer::InactivateClients()
{
	client_t *cl = g_psvs.clients;
	
	for(int i = 0; i < g_psvs.maxclients; i++, cl++)
	{
		if(!cl->active && !cl->connected && !cl->spawned)
			continue;

		if(cl->fakeclient)
		{
			SV_DropClient(cl, FALSE, "Dropping fakeclient on level change");
			continue;
		};
		
		cl->Inactivate();
	};
};

NOXREF void CGameServer::ReconnectAllClients()
{
	NOXREFCHECK;
	
	int i;
	char message[34];
	Q_snprintf(message, sizeof(message), "Server updating Security Module.\n");

	for(i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *client = &g_psvs.clients[i];

		if((client->active || client->connected) && !client->fakeclient)
			client->Reconnect();
	};
};

void CGameServer::CountPlayers(int *clients)
{
	int count = 0;
	client_s *cl = g_psvs.clients;
	
	for(int i = 0; i < g_psvs.maxclients; i++, cl++)
	{
		if(cl->active | cl->spawned | cl->connected)
			count++;
	};

	*clients = count;
};

void CGameServer::CheckCmdTimes()
{
	static double lastreset;

	if(Host_IsSinglePlayerGame())
		return;

	if(realtime - lastreset < 1.0)
		return;

	lastreset = realtime;
	
	for(int i = g_psvs.maxclients - 1; i >= 0; i--)
	{
		client_t *cl = &g_psvs.clients[i];
		if(!cl->connected || !cl->active)
			continue;

		if(cl->connecttime == 0.0)
			cl->connecttime = realtime;

		float dif = cl->connecttime + cl->cmdtime - realtime;
		
		if(dif > clockwindow.value)
		{
			cl->ignorecmdtime = clockwindow.value + realtime;
			cl->cmdtime = realtime - cl->connecttime;
		};

		if(dif < -clockwindow.value)
			cl->cmdtime = realtime - cl->connecttime;
	};
};

void CGameServer::ReadPackets()
{
	while(NET_GetPacket(NS_SERVER))
	{
		if(SV_FilterPacket())
		{
			SV_SendBan();
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
				HandleConnectionlessPacket();
			}
			else if(sv_logblocks.value != 0.0f)
			{
				Log_Printf("Traffic from %s was blocked for exceeding rate limits\n",
				           NET_AdrToString(net_from));
			}
			continue;
		}

		for(int i = 0; i < g_psvs.maxclients; i++)
		{
			client_t *cl = &g_psvs.clients[i];
			if(!cl->connected && !cl->active && !cl->spawned)
				continue;

			if(NET_CompareAdr(net_from, cl->netchan.remote_address) != TRUE)
				continue;

			if(Netchan_Process(&cl->netchan))
			{
				if(g_psvs.maxclients == 1 || !cl->active || !cl->spawned ||
				   !cl->fully_connected)
				{
					cl->send_message = TRUE;
				}

				SV_ExecuteClientMessage(cl);
				gGlobalVariables.frametime = host_frametime;
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

void CGameServer::SendClientMessages()
{
	SV_UpdateToReliableMessages();

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		host_client = cl;

		if((!cl->active && !cl->connected && !cl->spawned) || cl->fakeclient)
			continue;

		if(cl->skip_message)
		{
			cl->skip_message = FALSE;
			continue;
		}

		if(host_limitlocal.value == 0.0f &&
		   cl->netchan.remote_address.type == NA_LOOPBACK)
			cl->send_message = TRUE;

		if(cl->active && cl->spawned && cl->fully_connected &&
		   host_frametime + realtime >= cl->next_messagetime)
			cl->send_message = TRUE;

		if(cl->netchan.message.flags & SIZEBUF_OVERFLOWED)
		{
			SZ_Clear(&cl->netchan.message);
			SZ_Clear(&cl->datagram);
			
			BroadcastPrintf("%s overflowed\n", cl->name);
			
			mpConsole->Printf("WARNING: reliable overflow for %s\n", cl->name);
			
			cl->Drop(false, "Reliable channel overflowed");
			
			cl->send_message = TRUE;
			cl->netchan.cleartime = 0;
		}
		else if(cl->send_message)
		{
			if(sv_failuretime.value < realtime - cl->netchan.last_received)
				cl->send_message = FALSE;
		};

		if(cl->send_message)
		{
			if(!Netchan_CanPacket(&cl->netchan))
			{
				++cl->chokecount;
				continue;
			}

			host_client->send_message = FALSE;
			cl->next_messagetime =
			host_frametime + cl->next_messageinterval + realtime;
			if(cl->active && cl->spawned && cl->fully_connected)
				SV_SendClientDatagram(cl);
			else
				Netchan_Transmit(&cl->netchan, 0, NULL);
		}
	}
	SV_CleanupEnts();
};

void CGameServer::CheckTimeouts()
{
	int i;
	client_t *cl;
	
	float droptime = realtime - sv_timeout.value;

	for(i = 0, cl = g_psvs.clients; i < g_psvs.maxclients; i++, cl++)
	{
		if(cl->fakeclient)
			continue;
		
		if(!cl->connected && !cl->active && !cl->spawned)
			continue;
		
		if(cl->netchan.last_received < droptime)
		{
			BroadcastPrintf("%s timed out\n", cl->name);
			SV_DropClient(cl, FALSE, "Timed out");
		};
	};
};

void CGameServer::CheckMapDifferences()
{
	static double lastcheck;

	if(realtime - lastcheck < 5.0f)
		return;

	lastcheck = realtime;
	
	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		
		if(!cl->active || !cl->crcValue)
			continue;

		if(cl->netchan.remote_address.type == NA_LOOPBACK)
			continue;

		if(cl->crcValue != g_psv.worldmapCRC)
			cl->netchan.message.flags |= SIZEBUF_OVERFLOWED;
	};
};

void CGameServer::GatherStatistics()
{
	int players = 0;

	if(g_psvs.next_cleartime < realtime)
	{
		CountPlayers(&players);
		g_psvs.next_cleartime = realtime + 3600.0;
		Q_memset(&g_psvs.stats, 0, sizeof(g_psvs.stats));
		g_psvs.stats.maxusers = players;
		g_psvs.stats.minusers = players;
		return;
	};

	if(g_psvs.next_sampletime > realtime)
		return;

	CountPlayers(&players);

	g_psvs.stats.num_samples++;
	g_psvs.next_sampletime = realtime + 60.0;
	g_psvs.stats.cumulative_occupancy += (players * 100.0 / g_psvs.maxclients);

	if(g_psvs.stats.num_samples >= 1)
		g_psvs.stats.occupancy =
		g_psvs.stats.cumulative_occupancy / g_psvs.stats.num_samples;

	if(g_psvs.stats.minusers > players)
		g_psvs.stats.minusers = players;
	else if(g_psvs.stats.maxusers < players)
		g_psvs.stats.maxusers = players;

	if((g_psvs.maxclients - 1) <= players)
		g_psvs.stats.at_capacity++;
	if(players <= 1)
		g_psvs.stats.at_empty++;

	if(g_psvs.stats.num_samples >= 1)
	{
		g_psvs.stats.capacity_percent =
		g_psvs.stats.at_capacity * 100.0 / g_psvs.stats.num_samples;
		g_psvs.stats.empty_percent =
		g_psvs.stats.at_empty * 100.0 / g_psvs.stats.num_samples;
	};

	int c = 0;
	float v = 0.0f;
	client_t *cl = g_psvs.clients;

	for(int i = g_psvs.maxclients; i > 0; i--, cl++)
	{
		if(cl->active && !cl->fakeclient)
		{
			c++;
			v += cl->latency;
		};
	};

	if(c)
		v /= c;

	g_psvs.stats.cumulative_latency += v;
	if(g_psvs.stats.num_samples >= 1)
		g_psvs.stats.average_latency =
		g_psvs.stats.cumulative_latency / g_psvs.stats.num_samples;
	if(g_psvs.stats.num_sessions >= 1)
		g_psvs.stats.average_session_len =
		g_psvs.stats.cumulative_sessiontime / g_psvs.stats.num_sessions;
};