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

// TODO: NEED MAJOR REFACTORING

#include "precompiled.hpp"
#include "system/host_cmd.hpp"
#include "system/host.hpp"
#include "system/system.hpp"
#include "system/info.hpp"
#include "system/sizebuf.hpp"
#include "system/buildinfo.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "filesystem/filesystem_.hpp"
#include "resources/studio_rehlds.hpp"
#include "resources/model_rehlds.hpp"
#include "network/net_msg.hpp"
#include "physics/pmove.hpp"
#include "world/world.hpp"
#include "world/pr_cmds.hpp"
#include "world/pr_edict.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "system/server.hpp"
#include "system/sv_log.hpp"
#include "network/sv_user.hpp"
#include "network/sv_steam3.hpp"
#include "graphics/screen.hpp"
#include "sound/sound.hpp"
#include "input/keys.hpp"
#include "voice/voice.hpp"

typedef int (*SV_BLENDING_INTERFACE_FUNC)(int,
                                          struct sv_blending_interface_s **,
                                          struct server_studio_api_s *,
                                          float *,
                                          float *);

vec3_t r_origin;
double cpuPercent;

int current_skill;
CareerStateType g_careerState;
int gHostSpawnCount;

// qboolean noclip_anglehack;
qboolean g_bMajorMapChange;

int g_iQuitCommandIssued;
char *g_pPostRestartCmdLineArgs;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int r_dointerp = 1;

SV_SAVEGAMECOMMENT_FUNC g_pSaveGameCommentFunc = &Host_SavegameComment;

cvar_t voice_recordtofile = { "voice_recordtofile", "0", 0, 0.0f, NULL };
cvar_t voice_inputfromfile = { "voice_inputfromfile", "0", 0, 0.0f, NULL };
cvar_t gHostMap = { "HostMap", "C1A0", 0, 0.0f, NULL };

TITLECOMMENT gTitleComments[] = { { "T0A0", "#T0A0TITLE" },
	                              { "C0A0", "#C0A0TITLE" },
	                              { "C1A0", "#C0A1TITLE" },
	                              { "C1A1", "#C1A1TITLE" },
	                              { "C1A2", "#C1A2TITLE" },
	                              { "C1A3", "#C1A3TITLE" },
	                              { "C1A4", "#C1A4TITLE" },
	                              { "C2A1", "#C2A1TITLE" },
	                              { "C2A2", "#C2A2TITLE" },
	                              { "C2A3", "#C2A3TITLE" },
	                              { "C2A4D", "#C2A4TITLE2" },
	                              { "C2A4E", "#C2A4TITLE2" },
	                              { "C2A4F", "#C2A4TITLE2" },
	                              { "C2A4G", "#C2A4TITLE2" },
	                              { "C2A4", "#C2A4TITLE1" },
	                              { "C2A5", "#C2A5TITLE" },
	                              { "C3A1", "#C3A1TITLE" },
	                              { "C3A2", "#C3A2TITLE" },
	                              { "C4A1A", "#C4A1ATITLE" },
	                              { "C4A1B", "#C4A1ATITLE" },
	                              { "C4A1C", "#C4A1ATITLE" },
	                              { "C4A1D", "#C4A1ATITLE" },
	                              { "C4A1E", "#C4A1ATITLE" },
	                              { "C4A1", "#C4A1TITLE" },
	                              { "C4A2", "#C4A2TITLE" },
	                              { "C4A3", "#C4A3TITLE" },
	                              { "C5A1", "#C5TITLE" },
	                              { "OFBOOT", "#OF_BOOT0TITLE" },
	                              { "OF0A", "#OF1A1TITLE" },
	                              { "OF1A1", "#OF1A3TITLE" },
	                              { "OF1A2", "#OF1A3TITLE" },
	                              { "OF1A3", "#OF1A3TITLE" },
	                              { "OF1A4", "#OF1A3TITLE" },
	                              { "OF1A", "#OF1A5TITLE" },
	                              { "OF2A1", "#OF2A1TITLE" },
	                              { "OF2A2", "#OF2A1TITLE" },
	                              { "OF2A3", "#OF2A1TITLE" },
	                              { "OF2A", "#OF2A4TITLE" },
	                              { "OF3A1", "#OF3A1TITLE" },
	                              { "OF3A2", "#OF3A1TITLE" },
	                              { "OF3A", "#OF3A3TITLE" },
	                              { "OF4A1", "#OF4A1TITLE" },
	                              { "OF4A2", "#OF4A1TITLE" },
	                              { "OF4A3", "#OF4A1TITLE" },
	                              { "OF4A", "#OF4A4TITLE" },
	                              { "OF5A", "#OF5A1TITLE" },
	                              { "OF6A1", "#OF6A1TITLE" },
	                              { "OF6A2", "#OF6A1TITLE" },
	                              { "OF6A3", "#OF6A1TITLE" },
	                              { "OF6A4b", "#OF6A4TITLE" },
	                              { "OF6A4", "#OF6A1TITLE" },
	                              { "OF6A5", "#OF6A4TITLE" },
	                              { "OF6A", "#OF6A4TITLE" },
	                              { "OF7A", "#OF7A0TITLE" },
	                              { "ba_tram", "#BA_TRAMTITLE" },
	                              { "ba_security", "#BA_SECURITYTITLE" },
	                              { "ba_main", "#BA_SECURITYTITLE" },
	                              { "ba_elevator", "#BA_SECURITYTITLE" },
	                              { "ba_canal", "#BA_CANALSTITLE" },
	                              { "ba_yard", "#BA_YARDTITLE" },
	                              { "ba_xen", "#BA_XENTITLE" },
	                              { "ba_hazard", "#BA_HAZARD" },
	                              { "ba_power", "#BA_POWERTITLE" },
	                              { "ba_teleport1", "#BA_YARDTITLE" },
	                              { "ba_teleport", "#BA_TELEPORTTITLE" },
	                              { "ba_outro", "#BA_OUTRO" } };
TYPEDESCRIPTION gGameHeaderDescription[] = {
	DEFINE_FIELD(GAME_HEADER, mapCount, FIELD_INTEGER),
	DEFINE_ARRAY(GAME_HEADER, mapName, FIELD_CHARACTER, 32),
	DEFINE_ARRAY(GAME_HEADER, comment, FIELD_CHARACTER, 80),
};
TYPEDESCRIPTION gSaveHeaderDescription[] = {
	DEFINE_FIELD(SAVE_HEADER, skillLevel, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, entityCount, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, connectionCount, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, lightStyleCount, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, time, FIELD_TIME),
	DEFINE_ARRAY(SAVE_HEADER, mapName, FIELD_CHARACTER, 32),
	DEFINE_ARRAY(SAVE_HEADER, skyName, FIELD_CHARACTER, 32),
	DEFINE_FIELD(SAVE_HEADER, skyColor_r, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, skyColor_g, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, skyColor_b, FIELD_INTEGER),
	DEFINE_FIELD(SAVE_HEADER, skyVec_x, FIELD_FLOAT),
	DEFINE_FIELD(SAVE_HEADER, skyVec_y, FIELD_FLOAT),
	DEFINE_FIELD(SAVE_HEADER, skyVec_z, FIELD_FLOAT),
};
TYPEDESCRIPTION gAdjacencyDescription[] = {
	DEFINE_ARRAY(LEVELLIST, mapName, FIELD_CHARACTER, 32),
	DEFINE_ARRAY(LEVELLIST, landmarkName, FIELD_CHARACTER, 32),
	DEFINE_FIELD(LEVELLIST, pentLandmark, FIELD_EDICT),
	DEFINE_FIELD(LEVELLIST, vecLandmarkOrigin, FIELD_VECTOR),
};
TYPEDESCRIPTION gEntityTableDescription[] = {
	DEFINE_FIELD(ENTITYTABLE, id, FIELD_INTEGER),
	DEFINE_FIELD(ENTITYTABLE, location, FIELD_INTEGER),
	DEFINE_FIELD(ENTITYTABLE, size, FIELD_INTEGER),
	DEFINE_FIELD(ENTITYTABLE, flags, FIELD_INTEGER),
	DEFINE_FIELD(ENTITYTABLE, classname, FIELD_STRING),
};
TYPEDESCRIPTION gLightstyleDescription[] = {
	DEFINE_FIELD(SAVELIGHTSTYLE, index, FIELD_INTEGER),
	DEFINE_ARRAY(SAVELIGHTSTYLE, style, FIELD_CHARACTER, MAX_LIGHTSTYLES),
};

#else // HOOK_ENGINE

int r_dointerp;

SV_SAVEGAMECOMMENT_FUNC g_pSaveGameCommentFunc;

cvar_t voice_recordtofile;
cvar_t voice_inputfromfile;
cvar_t gHostMap;

TITLECOMMENT gTitleComments[66];
TYPEDESCRIPTION gGameHeaderDescription[3];
TYPEDESCRIPTION gSaveHeaderDescription[13];
TYPEDESCRIPTION gAdjacencyDescription[4];
TYPEDESCRIPTION gEntityTableDescription[5];
TYPEDESCRIPTION gLightstyleDescription[2];

#endif // HOOK_ENGINE

void SV_GetPlayerHulls()
{
	for(int i = 0; i < 4; ++i)
	{
		if(!gEntityInterface.pfnGetHullBounds(i, player_mins[i], player_maxs[i]))
			break;
	}
}



void Host_Motd_f()
{
	int length;
	FileHandle_t pFile;
	char *pFileList;
	char *next;

	pFileList = motdfile.string;
	if(*pFileList == '/' || Q_strstr(pFileList, ":") ||
	   Q_strstr(pFileList, "..") || Q_strstr(pFileList, "\\"))
	{
		Con_Printf("Unable to open %s (contains illegal characters)\n", pFileList);
		return;
	}
	pFile = FS_Open(pFileList, "rb");
	if(!pFile)
	{
		Con_Printf("Unable to open %s\n", pFileList);
		return;
	}
	length = FS_Size(pFile);
	if(length > 0)
	{
		char *buf = (char *)Mem_Malloc(length + 1);
		if(buf)
		{
			FS_Read(buf, length, 1, pFile);
			buf[length] = 0;
			char *now = buf;
			Con_Printf("motd:");
			next = Q_strchr(now, '\n');
			while(next != NULL)
			{
				*next = 0;
				Con_Printf("%s\n", now);
				now = next + 1;
				next = Q_strchr(now, '\n');
			}

			Con_Printf("%s\n", now);

			Mem_Free(buf);
		}
	}
	FS_Close(pFile);
}

void Host_Motd_Write_f()
{
	char newFile[2048] = "";
	unsigned int i;
	FileHandle_t pFile;

	if(!g_psv.active || cmd_source != src_command || cls.state)
		return;

	if(!IsSafeFileToDownload(motdfile.string) ||
	   !Q_strstr(motdfile.string, ".txt"))
	{
		Con_Printf("Invalid motdfile name (%s)\n", motdfile.string);
		return;
	}
	pFile = FS_Open(motdfile.string, "wb+");
	if(!pFile)
	{
		Con_Printf("Unable to open %s\n", motdfile.string);
		return;
	}

#ifdef REHLDS_FIXES
	if(Cmd_Args())
	{
		Q_strncpy(newFile, Cmd_Args(), ARRAYSIZE(newFile) - 1);
		newFile[ARRAYSIZE(newFile) - 1] = '\0';
	}
#else  // REHLDS_FIXES
	Q_strncpy(newFile, Cmd_Args(), ARRAYSIZE(newFile));
#endif // REHLDS_FIXES

	auto len = Q_strlen(newFile);
	for(i = 0; i < len; i++)
	{
		if(newFile[i] == '\\' && newFile[i + 1] == 'n')
		{
			newFile[i] = '\n';
			Q_memmove(&newFile[i + 1], &newFile[i + 2], Q_strlen(&newFile[i + 2]) + 1);
		}
	}
	FS_Write(newFile, Q_strlen(newFile), 1, pFile);
	FS_Close(pFile);
	Con_Printf("Done.\n");
}

void GetStatsString(char *buf, int bufSize)
{
	long double avgOut = 0.0;
	long double avgIn = 0.0;
	int players = 0;

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		host_client = &g_psvs.clients[i];
		if(!host_client->active && !host_client->connected &&
		   !host_client->spawned ||
		   host_client->fakeclient)
			continue;

		players++;
		avgIn = avgIn + host_client->netchan.flow[FLOW_INCOMING].avgkbytespersec;
		avgOut = avgOut + host_client->netchan.flow[FLOW_OUTGOING].avgkbytespersec;
	}

	Q_snprintf(buf, bufSize - 1, "%5.2f %5.2f %5.2f %7i %5i %7.2f %7i", (float)(100.0 * cpuPercent), (float)avgIn, (float)avgOut, (int)floor(Sys_FloatTime() - startTime) / 60, g_userid - 1, (float)(1.0 / host_frametime), players);
	buf[bufSize - 1] = 0;
}

void Host_Stats_f()
{
	char stats[512];
	GetStatsString(stats, sizeof(stats));
	Con_Printf("CPU   In    Out   Uptime  Users   FPS    Players\n%s\n", stats);
}

/*
==================
Host_Quit_f
==================
*/
void Host_Quit_f()
{
	if(Cmd_Argc() == 1)
	{
		giActive = DLL_CLOSE;
		g_iQuitCommandIssued = 1;

		if(1 /* key_dest != key_console */ /* && cls.state != ca_dedicated */)
		{
			//M_Menu_Quit_f();
			return;
		}

		if(cls.state)
			CL_Disconnect();

		Host_ShutdownServer(FALSE);
		Sys_Quit();
	}
	else
	{
		giActive = DLL_PAUSED;
		giStateInfo = 4;
	}
}

void Host_Quit_Restart_f()
{
	giActive = DLL_RESTART;
	giStateInfo = 4;

	if(g_psv.active)
	{
		if(g_psvs.maxclients == 1 && cls.state == ca_active && g_pPostRestartCmdLineArgs)
		{
			Cbuf_AddText("save quick\n");
			Cbuf_Execute();
			Q_strcat(g_pPostRestartCmdLineArgs, " +load quick");
		}
	}
	else if (cls.state == ca_active && cls.trueaddress[0] && g_pPostRestartCmdLineArgs)
	{
		Q_strcat(g_pPostRestartCmdLineArgs, " +connect ");
		Q_strcat(g_pPostRestartCmdLineArgs, cls.servername);
	}
}

void Host_Status_Printf(qboolean conprint, qboolean log, char *fmt, ...)
{
	va_list argptr;
	char string[4096];
	char szfile[260];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	if(conprint)
		Con_Printf("%s", string);
	else
		SV_ClientPrintf("%s", string);

	if(log)
	{
		Q_snprintf(szfile, sizeof(szfile), "%s", "status.log");
		COM_Log(szfile, "%s", string);
	}
}

void Host_Status_f()
{
	client_t *client;
	int seconds;
	int minutes;
	int hltv_slots = 0;
	int hltv_specs = 0;
	int hltv_delay = 0;
	char *val;
	int hours;
	int j;
	int nClients;
	qboolean log = FALSE;
	qboolean conprint = FALSE;
	qboolean bIsSecure;
	qboolean bWantsToBeSecure;
	qboolean bConnectedToSteam3;
	const char *pchConnectionString = "";
	const char *pchSteamUniverse = "";

	if(cmd_source == src_command)
	{
		conprint = TRUE;

		if(!g_psv.active)
		{
			Cmd_ForwardToServer();
			return;
		}
	}

#ifndef REHLDS_FIXES // Remove Useless Stuff
	char szfile[260];
	if(Cmd_Argc() == 2 && !Q_stricmp(Cmd_Argv(1), "log"))
	{
		log = TRUE;
		Q_snprintf(szfile, sizeof(szfile), "%s", "status.log");
		_unlink(szfile);
	}
#endif // REHLDS_FIXES

	Host_Status_Printf(conprint, log, "hostname:  %s\n", Cvar_VariableString("hostname"));

	bIsSecure = Steam_GSBSecure();
	bWantsToBeSecure = Steam_GSBSecurePreference();
	bConnectedToSteam3 = Steam_GSBLoggedOn();

	if(!bIsSecure && bWantsToBeSecure)
	{
		pchConnectionString = "(secure mode enabled, connected to Steam3)";
		
		if(!bConnectedToSteam3)
			pchConnectionString = "(secure mode enabled, disconnected from Steam3)";
	};
	
	if(g_psv.active)
		pchSteamUniverse = Steam_GetGSUniverse();

	val = "insecure";
	if(bIsSecure)
		val = "secure";

	Host_Status_Printf(conprint, log, "version :  %i/%s %d %s %s%s (%d)\n", PROTOCOL_VERSION, gpszVersionString, build_number(), val, pchConnectionString, pchSteamUniverse, GetGameAppID());
	if(!noip)
	{
		Host_Status_Printf(conprint, log, "tcp/ip  :  %s\n", NET_AdrToString(net_local_adr));
	}
#ifdef _WIN32
	if(!noipx)
	{
		Host_Status_Printf(conprint, log, "ipx     :  %s\n", NET_AdrToString(net_local_ipx_adr));
	}
#endif // _WIN32
	Host_Status_Printf(conprint, log, "map     :  %s at: %d x, %d y, %d z\n", g_psv.name, r_origin[0], r_origin[1], r_origin[2]);
	SV_CountPlayers(&nClients);
	Host_Status_Printf(conprint, log, "players :  %i active (%i max)\n\n", nClients, g_psvs.maxclients);
	Host_Status_Printf(conprint, log, "#      name userid uniqueid frag time ping loss adr\n");

	int count = 1;
	client = g_psvs.clients;
	for(j = 0; j < g_psvs.maxclients; j++, client++)
	{
		if(!client->active)
		{
			continue;
		}

		hours = 0;
		seconds = realtime - client->netchan.connect_time;
		minutes = seconds / 60;
		if(minutes)
		{
			seconds %= 60;
			hours = minutes / 60;
			if(hours)
				minutes %= 60;
		}

		if(!client->fakeclient)
			val = SV_GetClientIDString(client);
		else
			val = "BOT";

		Host_Status_Printf(conprint, log, "#%2i %8s %i %s", count++, va("\"%s\"", client->name), client->userid, val);
		if(client->proxy)
		{
			const char *userInfo = Info_ValueForKey(client->userinfo, "hspecs");
			if(Q_strlen(userInfo))
				hltv_specs = Q_atoi(userInfo);

			userInfo = Info_ValueForKey(client->userinfo, "hslots");
			if(Q_strlen(userInfo))
				hltv_slots = Q_atoi(userInfo);

			userInfo = Info_ValueForKey(client->userinfo, "hdelay");
			if(Q_strlen(userInfo))
				hltv_delay = Q_atoi(userInfo);

			Host_Status_Printf(conprint, log, " hltv:%u/%u delay:%u", hltv_specs, hltv_slots, hltv_delay);
		}
		else
			Host_Status_Printf(conprint, log, " %3i", (int)client->edict->v.frags);

		if(hours)
			Host_Status_Printf(conprint, log, " %2i:%02i:%02i", hours, minutes, seconds);
		else
			Host_Status_Printf(conprint, log, " %02i:%02i", minutes, seconds);

		Host_Status_Printf(conprint, log, " %4i  %3i", SV_CalcPing(client), (int)client->packet_loss);
		if((conprint || client->proxy) &&
		   client->netchan.remote_address.type == NA_IP)
		{
			Host_Status_Printf(conprint, log, " %s\n", NET_AdrToString(client->netchan.remote_address));
		}
		else
			Host_Status_Printf(conprint, log, "\n");
	}
	Host_Status_Printf(conprint, log, "%i users\n", nClients);
}

void Host_Status_Formatted_f()
{
	client_t *client;
	int seconds;
	int minutes;
	int hours;
	int j;
	int nClients;
	qboolean log = FALSE;
	qboolean conprint = FALSE;
	qboolean bIsSecure;
	char sz[32];
	char szfile[MAX_PATH];
	char *szIDString;

	if(cmd_source == src_command)
	{
		conprint = TRUE;
		if(!g_psv.active)
		{
			Cmd_ForwardToServer();
			return;
		}
	}
	if(Cmd_Argc() == 2 && !Q_stricmp(Cmd_Argv(1), "log"))
	{
		Q_snprintf(szfile, sizeof(szfile), "%s", "status.log");
		_unlink(szfile);
		log = TRUE;
	}

	bIsSecure = Steam_GSBSecure();
	Host_Status_Printf(conprint, log, "hostname:  %s\n", Cvar_VariableString("hostname"));

	char *szSecure = "insecure";
	if(bIsSecure)
		szSecure = "secure";

	Host_Status_Printf(conprint, log, "version :  %i/%s %d %s\n", PROTOCOL_VERSION, gpszVersionString, build_number(), szSecure);

	if(!noip)
	{
		Host_Status_Printf(conprint, log, "tcp/ip  :  %s\n", NET_AdrToString(net_local_adr));
	}
#ifdef _WIN32
	if(!noipx)
	{
		Host_Status_Printf(conprint, log, "ipx     :  %s\n", NET_AdrToString(net_local_ipx_adr));
	}
#endif // _WIN32

	Host_Status_Printf(conprint, log, "map     :  %s at: %d x, %d y, %d z\n", g_psv.name, r_origin[0], r_origin[1], r_origin[2]);
	SV_CountPlayers(&nClients);
	Host_Status_Printf(conprint, log, "players :  %i active (%i max)\n\n", nClients, g_psvs.maxclients);
	Host_Status_Printf(conprint, log, "%-2.2s\t%-9.9s\t%-7.7s\t%-20.20s\t%-4."
	                                  "4s\t%-8.8s\t%-4.4s\t%-4.4s\t%-21.21s\n",
	                   "# ",
	                   "name",
	                   "userid   ",
	                   "uniqueid ",
	                   "frag",
	                   "time    ",
	                   "ping",
	                   "loss",
	                   "adr");

	int count = 1;
	char *szRemoteAddr;
	client = g_psvs.clients;
	for(j = 0; j < g_psvs.maxclients; j++, client++)
	{
		if(!client->active)
		{
			continue;
		}

		seconds = realtime - client->netchan.connect_time;
		minutes = seconds / 60;
		hours = minutes / 60;

		if(minutes && hours)
			Q_snprintf(sz, sizeof(sz), "%-2i:%02i:%02i", hours, minutes % 60, seconds % 60);
		else
			Q_snprintf(sz, sizeof(sz), "%02i:%02i", minutes, seconds % 60);

		if(conprint)
			szRemoteAddr = NET_AdrToString(client->netchan.remote_address);
		else
			szRemoteAddr = "";

#ifdef REHLDS_FIXES
		// TODO: I think it would be better if do the formatting for fakeclient as
		// in Host_Status_f (in the original it there is no)
		if(client->fakeclient)
			szIDString = "BOT";
		else
#endif // REHLDS_FIXES
			szIDString = SV_GetClientIDString(client);
		Host_Status_Printf(conprint, log, "%-2.2s\t%-9.9s\t%-7.7s\t%-20.20s\t%-4."
		                                  "4s\t%-8.8s\t%-4.4s\t%-4.4s\t%-21.21s\n",
		                   va("%-2i", count++),
		                   va("\"%s\"", client->name),
		                   va("%-7i", client->userid),
		                   szIDString,
		                   va("%-4i", (int)client->edict->v.frags),
		                   sz,
		                   va("%-4i", SV_CalcPing(client)),
		                   va("%-4i", (int)client->packet_loss),
		                   szRemoteAddr);
	}
	Host_Status_Printf(conprint, log, "%i users\n", nClients);
}

/*
==================
Host_Ping_f

==================
*/
void Host_Ping_f()
{
	int i;
	client_t *client;

	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}
	SV_ClientPrintf("Client ping times:\n");

	client = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, client++)
	{
		if(client->active)
			SV_ClientPrintf("%4i %s\n", SV_CalcPing(client), client->name);
	}
}

void Host_Map(qboolean bIsDemo, char *mapstring, char *mapName, qboolean loadGame)
{
	int i;
	UserMsg *pMsg;
	Host_ShutdownServer(FALSE);
	key_dest = key_game;
	SCR_BeginLoadingPlaque(FALSE);
	if(!loadGame)
	{
		Host_ClearGameState();
		SV_InactivateClients();
		g_psvs.serverflags = 0;
	}
	Q_strncpy(cls.mapstring, mapstring, sizeof(cls.mapstring) - 1);
	cls.mapstring[sizeof(cls.mapstring) - 1] = 0;
	if(SV_SpawnServer(bIsDemo, mapName, NULL))
	{
		//ContinueLoadingProgressBar("Server", 7, 0.0);
		if(loadGame)
		{
			if(!LoadGamestate(mapName, 1))
				SV_LoadEntities();
			g_psv.paused = TRUE;
			g_psv.loadgame = TRUE;
			SV_ActivateServer(0);
		}
		else
		{
			SV_LoadEntities();
			SV_ActivateServer(1);
			if(!g_psv.active)
				return;

			if(cls.state != ca_dedicated)
			{
				Q_strcpy(cls.spawnparms, "");
				for(i = 0; i < Cmd_Argc(); i++)
					Q_strncat(cls.spawnparms, Cmd_Argv(i), sizeof(cls.spawnparms) - Q_strlen(cls.spawnparms) - 1);
			}
		}
		if(sv_gpNewUserMsgs)
		{
			pMsg = sv_gpUserMsgs;
			if(pMsg)
			{
				while(pMsg->next)
					pMsg = pMsg->next;
				pMsg->next = sv_gpNewUserMsgs;
			}
			else
				sv_gpUserMsgs = sv_gpNewUserMsgs;

			sv_gpNewUserMsgs = NULL;
		}
		if(cls.state)
			Cmd_ExecuteString("connect local", src_command);
	}
}

void Host_Map_f()
{
	int i;
	char mapstring[64];
	char name[64];
	CareerStateType careerState = g_careerState;
	if(cmd_source != src_command)
	{
		g_careerState = CAREER_NONE;
		return;
	}
	if(Cmd_Argc() > 1 && Q_strlen(Cmd_Args()) > 54)
	{
		g_careerState = CAREER_NONE;
		Con_Printf("map change failed: command string is too long.\n");
		return;
	}
	if(Cmd_Argc() < 2)
	{
		g_careerState = CAREER_NONE;
		Con_Printf("map <levelname> : changes server to specified map\n");
		return;
	}

	CL_Disconnect();
	// TODO: what it? why is this?
	if(careerState == CAREER_LOADING)
		g_careerState = CAREER_LOADING;

	if(COM_CheckParm("-steam") && PF_IsDedicatedServer())
		g_bMajorMapChange = TRUE;

	FS_LogLevelLoadStarted("Map_Common");
	mapstring[0] = 0;
	for(i = 0; i < Cmd_Argc(); i++)
	{
		Q_strncat(mapstring, Cmd_Argv(i), 62 - Q_strlen(mapstring));
		Q_strncat(mapstring, " ", 62 - Q_strlen(mapstring));
	}
	Q_strcat(mapstring, "\n");
	Q_strncpy(name, Cmd_Argv(1), sizeof(name) - 1);
	name[sizeof(name) - 1] = 0;

	if(!g_psvs.dll_initialized)
		Host_InitializeGameDLL();

	int iLen = Q_strlen(name);
	if(iLen > 4 && !Q_stricmp(&name[iLen - 4], ".bsp"))
		name[iLen - 4] = 0;

	FS_LogLevelLoadStarted(name);
	VGuiWrap2_LoadingStarted("level", name);
	SCR_UpdateScreen();
	SCR_UpdateScreen();

	if(!PF_IsMapValid_I(name))
	{
		Con_Printf("map change failed: '%s' not found on server.\n", name);
		if(COM_CheckParm("-steam"))
		{
			if(PF_IsDedicatedServer())
			{
				g_bMajorMapChange = FALSE;
				Sys_Printf("\n");
			}
		}
		return;
	}

	//StartLoadingProgressBar("Server", 24);
	//SetLoadingProgressBarStatusText("#GameUI_StartingServer");
	//ContinueLoadingProgressBar("Server", 1, 0.0);
	Cvar_Set("HostMap", name);
	Host_Map(FALSE, mapstring, name, FALSE);
	if(COM_CheckParm("-steam") && PF_IsDedicatedServer())
	{
		g_bMajorMapChange = FALSE;
		Sys_Printf("\n");
	};
	
	//ContinueLoadingProgressBar("Server", 11, 0.0);
	//NotifyDedicatedServerUI("UpdateMap");

	if(g_careerState == CAREER_LOADING)
	{
		g_careerState = CAREER_PLAYING;
		SetCareerAudioState(1);
	}
	else
		SetCareerAudioState(0);
}

void Host_Career_f()
{
	if(cmd_source == src_command)
	{
		g_careerState = CAREER_LOADING;
		Host_Map_f();
	}
}

void Host_Maps_f()
{
	char *pszSubString;
	if(Cmd_Argc() != 2)
	{
		Con_Printf("Usage:  maps <substring>\nmaps * for full listing\n");
		return;
	}
	pszSubString = (char *)Cmd_Argv(1);
	if(pszSubString && *pszSubString)
	{
		if(*pszSubString == '*')
			pszSubString = NULL;
		COM_ListMaps(pszSubString);
	}
}

void Host_Changelevel_f()
{
	char _level[64];
	char _startspot[64];

	char *level = NULL;
	char *startspot = NULL;

	if(Cmd_Argc() < 2)
	{
		Con_Printf("changelevel <levelname> : continue game on a new level\n");
		return;
	}
	if(!g_psv.active || cls.demoplayback)
	{
		Con_Printf("Only the server may changelevel\n");
		return;
	}
	level = (char *)Cmd_Argv(1);
	if(!PF_IsMapValid_I(level))
	{
		Con_Printf("changelevel failed: '%s' not found on server.\n", level);
		return;
	}

	Q_strncpy(_level, level, 63);
	_level[63] = 0;

	if(Cmd_Argc() != 2)
	{
		startspot = &_startspot[0];
		Q_strncpy(_startspot, Cmd_Argv(2), 63);
		_startspot[63] = 0;
	}
	else
		_startspot[0] = 0;

	SCR_BeginLoadingPlaque(FALSE);
	S_StopAllSounds(1);
	SV_InactivateClients();
	SV_ServerShutdown();
	SV_SpawnServer(FALSE, _level, startspot);
	SV_LoadEntities();
	SV_ActivateServer(1);
}

const char *Host_FindRecentSave(char *pNameBuf)
{
	const char *findfn;
	char basefilename[MAX_PATH];
	int found;
#ifdef REHLDS_FIXES
	int32 newest = 0;
#else
	int32 newest;
#endif
	int32 ft;
	char szPath[MAX_PATH];

	Q_sprintf(pNameBuf, "%s*.sav", Host_SaveGameDirectory());
	Q_snprintf(szPath, sizeof(szPath), "%s", Host_SaveGameDirectory());

	found = 0;
	findfn = Sys_FindFirst(pNameBuf, basefilename);
	while(findfn != NULL)
	{
		if(Q_strlen(findfn) && Q_stricmp(findfn, "HLSave.sav"))
		{
			Q_snprintf(szPath, sizeof(szPath), "%s%s", Host_SaveGameDirectory(), findfn);
			ft = FS_GetFileTime(szPath);
			if(ft > 0 && (!found || newest < ft))
			{
				found = 1;
				newest = ft;
				Q_strcpy(pNameBuf, findfn);
			}
		}
		findfn = Sys_FindNext(basefilename);
	}
	Sys_FindClose();
	return found != 0 ? pNameBuf : NULL;
}

/*
==================
Host_Restart_f

Restarts the current server for a dead player
==================
*/
void Host_Restart_f()
{
	char name[MAX_PATH];
	
	if(cls.demoplayback || !g_psv.active || cmd_source != src_command)
		return;
	
	if(cls.state)
		cls.state = ca_disconnected;

	Host_ClearGameState();
	SV_InactivateClients();
	Q_strncpy(name, g_psv.name, sizeof(name) - 1);
	name[sizeof(name) - 1] = 0;

	SV_ServerShutdown();
	SV_SpawnServer(FALSE, name, NULL);
	SV_LoadEntities();
	SV_ActivateServer(1);
}

void Host_Reload_f()
{
	const char *pSaveName;
	char name[MAX_PATH];
	if(cls.demoplayback || !g_psv.active || cmd_source != src_command)
		return;

	Host_ClearGameState();
	SV_InactivateClients();
	SV_ServerShutdown();

	pSaveName = Host_FindRecentSave(name);
	if(!pSaveName || !Host_Load(pSaveName))
	{
		SV_SpawnServer(FALSE, gHostMap.string, NULL);
		SV_LoadEntities();
		SV_ActivateServer(1);
	}
}

/*
==================
Host_Reconnect_f

This command causes the client to wait for the signon messages again.
This is sent just before a server changes levels
==================
*/
void Host_Reconnect_f()
{
	char cmdString[128];
	
	if(cls.state < ca_connected)
		return;

	if(cls.passive)
	{
		Q_snprintf(cmdString, sizeof(cmdString), "listen %s\n", NET_AdrToString(cls.connect_stream));
		Cbuf_AddText(cmdString);
		return;
	}

	SCR_BeginLoadingPlaque(FALSE);
	cls.signon = 0; // need new connection messages
	cls.state = ca_connected;
	sys_timescale.value = 1.0f;

	Netchan_Clear(&cls.netchan);
	SZ_Clear(&cls.netchan.message);
	MSG_WriteChar(&cls.netchan.message, clc_stringcmd);
	MSG_WriteString(&cls.netchan.message, "new");
}

char *Host_SaveGameDirectory()
{
	static char szDirectory[MAX_PATH];
	Q_memset(szDirectory, 0, sizeof(szDirectory));
	Q_snprintf(szDirectory, sizeof(szDirectory), "SAVE/");
	return szDirectory;
}

void Host_SavegameComment(char *pszBuffer, int iSizeBuffer)
{
	const char *pszName = "";
	const char *pszMapName = (const char *)&pr_strings[gGlobalVariables.mapname];

	for(int i = 0; i < ARRAYSIZE(gTitleComments) && !pszName; ++i)
	{
		if(!Q_strnicmp(pszMapName, gTitleComments[i].pBSPName, Q_strlen(gTitleComments[i].pBSPName)))
			pszName = gTitleComments[i].pTitleName;
	};
	
	if(!pszName)
	{
		if(!pszMapName || !pszMapName[0])
		{
			pszName = pszMapName;
			if(!Q_strlen(cl.levelname))
				pszName = cl.levelname;
		}
	}
	Q_strncpy(pszBuffer, pszName, iSizeBuffer - 1);
	pszBuffer[iSizeBuffer - 1] = 0;
}

void Host_SaveAgeList(const char *pName, int count)
{
	char newName[MAX_PATH];
	char oldName[MAX_PATH];

	Q_snprintf(newName, sizeof(newName), "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count);
	COM_FixSlashes(newName);
	FS_RemoveFile(newName, "GAMECONFIG");

	while(count > 0)
	{
		if(count == 1)
			Q_snprintf(oldName, sizeof(oldName), "%s%s.sav", Host_SaveGameDirectory(), pName);
		else
			Q_snprintf(oldName, sizeof(oldName), "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count - 1);

		COM_FixSlashes(oldName);
		Q_snprintf(newName, sizeof(newName), "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count);
		COM_FixSlashes(newName);
		FS_Rename(oldName, newName);
		count--;
	}
}

int Host_ValidSave()
{
	if(cmd_source != src_command)
		return 0;

	if(!g_psv.active)
	{
		Con_Printf("Not playing a local game.\n");
		return 0;
	}
	if(g_psvs.maxclients != 1)
	{
		Con_Printf("Can't save multiplayer games.\n");
		return 0;
	}
	if(cls.state != ca_active || cls.signon != 2)
	{
		Con_Printf("Can't save during transition.\n");
		return 0;
	}
	if(cl.intermission)
	{
		Con_Printf("Can't save in intermission.\n");
		return 0;
	}
	if(g_psvs.clients->active && g_psvs.clients->edict->v.health <= 0.0)
	{
		Con_Printf("Can't savegame with a dead player\n");
		return 0;
	}
	return 1;
}

SAVERESTOREDATA *SaveInit(int size)
{
	int i;
	edict_t *pEdict;
	SAVERESTOREDATA *pSaveData;

	if(size <= 0)
		size = 0x80000;

	pSaveData = (SAVERESTOREDATA *)Mem_Calloc(
	sizeof(SAVERESTOREDATA) + (sizeof(ENTITYTABLE) * g_psv.num_edicts) + size,
	sizeof(char));
	pSaveData->pTable = (ENTITYTABLE *)(pSaveData + 1);
	pSaveData->tokenSize = 0;
	pSaveData->tokenCount = 0xFFF;
	pSaveData->pTokens = (char **)Mem_Calloc(0xFFF, sizeof(char *));

	for(i = 0; i < g_psv.num_edicts; i++)
	{
		pEdict = &g_psv.edicts[i];

		pSaveData->pTable[i].id = i;
		pSaveData->pTable[i].pent = pEdict;
		pSaveData->pTable[i].flags = 0;
		pSaveData->pTable[i].location = 0;
		pSaveData->pTable[i].size = 0;
		pSaveData->pTable[i].classname = 0;
	}

	pSaveData->tableCount = g_psv.num_edicts;
	pSaveData->connectionCount = 0;
	pSaveData->size = 0;
	pSaveData->bufferSize = size;
	pSaveData->fUseLandmark = 0;

	pSaveData->pBaseData = (char *)(pSaveData->pTable + g_psv.num_edicts);
	pSaveData->pCurrentData = (char *)(pSaveData->pTable + g_psv.num_edicts);

	gGlobalVariables.pSaveData = pSaveData;
	pSaveData->time = gGlobalVariables.time;

	pSaveData->vecLandmarkOffset[0] = vec3_origin[0];
	pSaveData->vecLandmarkOffset[1] = vec3_origin[1];
	pSaveData->vecLandmarkOffset[2] = vec3_origin[2];

	return pSaveData;
}

void SaveExit(SAVERESTOREDATA *save)
{
	if(save->pTokens)
	{
		Mem_Free(save->pTokens);
		save->pTokens = NULL;
		save->tokenCount = 0;
	}
	Mem_Free(save);
	gGlobalVariables.pSaveData = NULL;
}

qboolean SaveGameSlot(const char *pSaveName, const char *pSaveComment)
{
	char hlPath[256];
	char name[256];
	char *pTokenData;
	int tag;
	int i;
	FILE *pFile;
	SAVERESTOREDATA *pSaveData;
	GAME_HEADER gameHeader;

	FS_CreateDirHierarchy(Host_SaveGameDirectory(), "GAMECONFIG");
	pSaveData = SaveGamestate();
	if(!pSaveData)
		return FALSE;

	SaveExit(pSaveData);
	pSaveData = SaveInit(0);

	Q_snprintf(hlPath, sizeof(hlPath), "%s*.HL?", Host_SaveGameDirectory());
	COM_FixSlashes(hlPath);
	gameHeader.mapCount = DirectoryCount(hlPath);
	Q_strncpy(gameHeader.mapName, g_psv.name, 31);
	gameHeader.mapName[31] = 0;
	Q_strncpy(gameHeader.comment, pSaveComment, 79);
	gameHeader.comment[79] = 0;

	gEntityInterface.pfnSaveWriteFields(pSaveData, "GameHeader", &gameHeader, gGameHeaderDescription, ARRAYSIZE(gGameHeaderDescription));
	gEntityInterface.pfnSaveGlobalState(pSaveData);

	pTokenData = NULL;
	for(i = 0; i < pSaveData->tokenCount; i++)
	{
		if(pSaveData->pTokens[i])
		{
			pSaveData->size += Q_strlen(pSaveData->pTokens[i]) + 1;
			if(pSaveData->size > pSaveData->bufferSize)
			{
				Con_Printf("Token Table Save/Restore overflow!");
				pSaveData->size = pSaveData->bufferSize;
				break;
			}
			pTokenData = pSaveData->pCurrentData;
			do
			{
				*pTokenData++ = *pSaveData->pTokens[i]++;
			} while(*pSaveData->pTokens[i]);
			pSaveData->pCurrentData = pTokenData;
		}
		else
		{
			if(pSaveData->size + 1 > pSaveData->bufferSize)
			{
				Con_Printf("Token Table Save/Restore overflow!");
				pSaveData->size = pSaveData->bufferSize;
				break;
			}
			pTokenData = pSaveData->pCurrentData;
			*pTokenData = 0;
			pSaveData->pCurrentData = pTokenData + 1;
		}
	}
	pSaveData->tokenSize = (int)(pSaveData->pCurrentData - pTokenData);

	if(pSaveData->size < pSaveData->bufferSize)
		pSaveData->size -= pSaveData->tokenSize;

	Q_snprintf(name, 252, "%s%s", Host_SaveGameDirectory(), pSaveName);
	COM_DefaultExtension(name, ".sav");
	COM_FixSlashes(name);
	Con_DPrintf("Saving game to %s...\n", name);

	if(Q_stricmp(pSaveName, "quick") || Q_stricmp(pSaveName, "autosave"))
		Host_SaveAgeList(pSaveName, 1);

	pFile = FS_OpenPathID(name, "wb", "GAMECONFIG");

	tag = SAVEGAME_HEADER;
	FS_Write(&tag, sizeof(int), 1, pFile);
	tag = SAVEGAME_VERSION;
	FS_Write(&tag, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->size, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->tokenCount, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->tokenSize, sizeof(int), 1, pFile);
	FS_Write(pSaveData->pCurrentData, pSaveData->tokenSize, 1, pFile);
	FS_Write(pSaveData->pBaseData, pSaveData->size, 1, pFile);
	DirectoryCopy(hlPath, pFile);
	FS_Close(pFile);
	SaveExit(pSaveData);

	return TRUE;
}

void Host_Savegame_f()
{
	char szComment[80];
	char szTemp[80];

	if(!Host_ValidSave())
		return;

	if(Cmd_Argc() != 2)
	{
		Con_DPrintf("save <savename> : save a game\n");
		return;
	}
	if(Q_strstr(Cmd_Argv(1), ".."))
	{
		Con_DPrintf("Relative pathnames are not allowed.\n");
		return;
	}
	g_pSaveGameCommentFunc(szTemp, 80);
	Q_snprintf(szComment, sizeof(szComment) - 1, "%-64.64s %02d:%02d", szTemp, (int)(g_psv.time / 60.0), (int)fmod(g_psv.time, 60.0));
	SaveGameSlot(Cmd_Argv(1), szComment);
	CL_HudMessage("GAMESAVED");
}

void Host_AutoSave_f()
{
	char szComment[80];
	char szTemp[80];

	if(Host_ValidSave())
	{
		g_pSaveGameCommentFunc(szTemp, 80);
		Q_snprintf(szComment, sizeof(szComment) - 1, "%-64.64s %02d:%02d", szTemp, (int)(g_psv.time / 60.0), (int)fmod(g_psv.time, 60.0));
		szComment[sizeof(szComment) - 1] = 0;
		SaveGameSlot("autosave", szComment);
	}
}

qboolean SaveGame(const char *pszSlot, const char *pszComment)
{
	qboolean qret;
	qboolean q;

	q = scr_skipupdate;
	scr_skipupdate = TRUE;
	qret = SaveGameSlot(pszSlot, pszComment);
	scr_skipupdate = q;
	return qret;
}

int SaveReadHeader(FileHandle_t pFile, GAME_HEADER *pHeader, int readGlobalState)
{
	int i;
	int tag;
	int size;
	int tokenCount;
	int tokenSize;
	char *pszTokenList;
	SAVERESTOREDATA *pSaveData;

	FS_Read(&tag, sizeof(int), 1, pFile);
	if(tag != SAVEGAME_HEADER)
	{
		FS_Close(pFile);
		return 0;
	}
	FS_Read(&tag, sizeof(int), 1, pFile);
	if(tag != SAVEGAME_VERSION)
	{
		FS_Close(pFile);
		return 0;
	}
	FS_Read(&size, sizeof(int), 1, pFile);
	FS_Read(&tokenCount, sizeof(int), 1, pFile);
	FS_Read(&tokenSize, sizeof(int), 1, pFile);

	pSaveData = (SAVERESTOREDATA *)Mem_Calloc(
	sizeof(SAVERESTOREDATA) + tokenSize + size, sizeof(char));
	pSaveData->tableCount = 0;
	pSaveData->pTable = NULL;
	pSaveData->connectionCount = 0;

	pszTokenList = (char *)(pSaveData + 1);
	if(tokenSize > 0)
	{
		pSaveData->tokenCount = tokenCount;
		pSaveData->tokenSize = tokenSize;

		FS_Read(pszTokenList, tokenSize, 1, pFile);

		if(!pSaveData->pTokens)
			pSaveData->pTokens = (char **)Mem_Calloc(tokenCount, sizeof(char *));

		for(i = 0; i < tokenCount; i++)
		{
			if(*pszTokenList)
				pSaveData->pTokens[i] = pszTokenList;
			else
				pSaveData->pTokens[i] = NULL;
			while(*pszTokenList++)
				;
		}
	}
	pSaveData->size = 0;
	pSaveData->bufferSize = size;
	pSaveData->fUseLandmark = 0;
	pSaveData->time = 0.0f;

	pSaveData->pCurrentData = pszTokenList;
	pSaveData->pBaseData = pszTokenList;

	FS_Read(pSaveData->pBaseData, size, 1, pFile);
	gEntityInterface.pfnSaveReadFields(pSaveData, "GameHeader", pHeader, gGameHeaderDescription, ARRAYSIZE(gGameHeaderDescription));
	if(readGlobalState)
		gEntityInterface.pfnRestoreGlobalState(pSaveData);
	SaveExit(pSaveData);
	return 1;
}

void SaveReadComment(FileHandle_t f, char *name)
{
	GAME_HEADER gameHeader;
	if(SaveReadHeader(f, &gameHeader, 0))
		Q_strcpy(name, gameHeader.comment);
}

void Host_Loadgame_f()
{
	if(cmd_source != src_command)
		return;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("load <savename> : load a game\n");
		return;
	}
	if(!Host_Load(Cmd_Argv(1)))
		Con_Printf("Error loading saved game\n");
}

int LoadGame(const char *pName)
{
	int iRet;
	qboolean q;

	q = scr_skipupdate;
	scr_skipupdate = TRUE;
	iRet = Host_Load(pName);
	scr_skipupdate = q;
	return iRet;
}

int Host_Load(const char *pName)
{
	FILE *pFile;
	GAME_HEADER gameHeader;
	char name[256];
	int nSlot;

	if(!pName || !pName[0])
		return 0;

	if(Q_strstr(pName, ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return 0;
	}

	if(*pName == '_' && COM_TokenWaiting((char *)&pName[1]))
	{
		nSlot = Q_atoi(pName);
		if(nSlot < 1 || nSlot > 12)
			return 0;

		Q_snprintf(name, 252, "%sHalf-Life-%i", Host_SaveGameDirectory(), nSlot);
	}
	else
		Q_snprintf(name, 252, "%s%s", Host_SaveGameDirectory(), pName);
	name[251] = 0;

	if(!g_psvs.dll_initialized)
		Host_InitializeGameDLL();

	COM_DefaultExtension(name, ".sav");
	COM_FixSlashes(name);
	name[255] = 0;

	Con_Printf("Loading game from %s...\n", name);
	pFile = FS_OpenPathID(name, "rb", "GAMECONFIG");
	if(!pFile)
		return 0;
	Host_ClearGameState();
	if(!SaveReadHeader(pFile, &gameHeader, 1))
	{
		giStateInfo = 1;
		Cbuf_AddText("\ndisconnect\n");
		return 0;
	}

	cls.demonum = -1;
	SV_InactivateClients();
	SCR_BeginLoadingPlaque(FALSE);
	DirectoryExtract(pFile, gameHeader.mapCount);
	FS_Close(pFile);

	Cvar_SetValue("deathmatch", 0.0);
	Cvar_SetValue("coop", 0.0);

	if(!Q_stricmp(com_gamedir, "valve") || !Q_stricmp(com_gamedir, "bshift") ||
	   !Q_stricmp(com_gamedir, "gearbox"))
	{
		g_psvs.maxclients = 1;
		Cvar_SetValue("maxplayers", 1.0);
	}

	Q_snprintf(name, sizeof(name), "map %s\n", gameHeader.mapName);
	CL_Disconnect();
	Host_Map(FALSE, name, gameHeader.mapName, TRUE);
	return 1;
}

SAVERESTOREDATA *SaveGamestate()
{
	char name[256];
	char *pTableData;
	char *pTokenData;
	FILE *pFile;
	int i;
	int dataSize;
	int tableSize;
	edict_t *pent;
	SAVE_HEADER header;
	SAVERESTOREDATA *pSaveData;
	SAVELIGHTSTYLE light;

	if(!gEntityInterface.pfnParmsChangeLevel)
		return NULL;

#ifdef REHLDS_FIXES
	Q_memset(&header, 0, sizeof(SAVE_HEADER));
#endif // REHLDS_FIXES

	FS_CreateDirHierarchy(Host_SaveGameDirectory(), "GAMECONFIG");
	pSaveData = SaveInit(0);
	Q_snprintf(name, sizeof(name), "%s%s.HL1", Host_SaveGameDirectory(), g_psv.name);
	COM_FixSlashes(name);
	gEntityInterface.pfnParmsChangeLevel();
	header.version = build_number();
	header.skillLevel = (int)skill.value;
	header.entityCount = pSaveData->tableCount;
	header.time = g_psv.time;
	header.connectionCount = pSaveData->connectionCount;
	Q_strncpy(header.skyName, sv_skyname.string, sizeof(header.skyName) - 1);
	header.skyName[sizeof(header.skyName) - 1] = 0;
	pSaveData->time = 0.0f;

	header.skyColor_r = (int)sv_skycolor_r.value;
	header.skyColor_g = (int)sv_skycolor_g.value;
	header.skyColor_b = (int)sv_skycolor_b.value;
	header.skyVec_x = sv_skyvec_x.value;
	header.skyVec_y = sv_skyvec_y.value;
	header.skyVec_z = sv_skyvec_z.value;

	Q_strncpy(header.mapName, g_psv.name, sizeof(header.mapName) - 1);
	header.mapName[sizeof(header.mapName) - 1] = 0;

	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		if(g_psv.lightstyles[i])
			header.lightStyleCount++;
	}
	gEntityInterface.pfnSaveWriteFields(pSaveData, "Save Header", &header, gSaveHeaderDescription, ARRAYSIZE(gSaveHeaderDescription));
	pSaveData->time = header.time;

	for(i = 0; i < pSaveData->connectionCount; i++)
		gEntityInterface.pfnSaveWriteFields(
		pSaveData, "ADJACENCY", &pSaveData->levelList[i], gAdjacencyDescription, ARRAYSIZE(gAdjacencyDescription));

	for(i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		if(g_psv.lightstyles[i])
		{
			light.index = i;
			Q_strncpy(light.style, g_psv.lightstyles[i], 63);
			light.style[63] = 0;
			gEntityInterface.pfnSaveWriteFields(pSaveData, "LIGHTSTYLE", &light, gLightstyleDescription, ARRAYSIZE(gLightstyleDescription));
		}
	}

	dataSize = pSaveData->size;
	pTableData = pSaveData->pCurrentData;
	tableSize = 0;

	for(i = 0; i < g_psv.num_edicts; i++)
	{
		pent = &g_psv.edicts[i];
		pSaveData->currentIndex = i;
		pSaveData->pTable[i].location = pSaveData->size;
		if(!pent->free)
		{
			gEntityInterface.pfnSave(pent, pSaveData);
			if(SV_IsPlayerIndex(i))
				pSaveData->pTable[i].flags |= FENTTABLE_PLAYER;
		}
	}
	for(i = 0; i < g_psv.num_edicts; i++)
		gEntityInterface.pfnSaveWriteFields(
		pSaveData, "ETABLE", &pSaveData->pTable[i], gEntityTableDescription, ARRAYSIZE(gEntityTableDescription));

	pTokenData = NULL;
	for(i = 0; i < pSaveData->tokenCount; i++)
	{
		if(pSaveData->pTokens[i])
		{
			pTokenData = pSaveData->pCurrentData;
			do
			{
				*pTokenData++ = *pSaveData->pTokens[i]++;
			} while(*pSaveData->pTokens[i]);
			pSaveData->pCurrentData = pTokenData;
		}
		else
		{
			pTokenData = pSaveData->pCurrentData;
			*pTokenData = 0;
			pSaveData->pCurrentData = pTokenData + 1;
		}
	}
	pSaveData->tokenSize = (int)(pSaveData->pCurrentData - pTokenData);

	COM_CreatePath(name);
	pFile = FS_OpenPathID(name, "wb", "GAMECONFIG");
	if(!pFile)
	{
		Con_Printf("Unable to open save game file %s.", name);
		return NULL;
	}

	i = SAVEFILE_HEADER;
	FS_Write(&i, sizeof(int), 1, pFile);
	i = SAVEGAME_VERSION;
	FS_Write(&i, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->size, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->tableCount, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->tokenCount, sizeof(int), 1, pFile);
	FS_Write(&pSaveData->tokenSize, sizeof(int), 1, pFile);
	FS_Write(pSaveData->pCurrentData, pSaveData->tokenSize, 1, pFile);
	FS_Write(pTableData, tableSize, 1, pFile);
	FS_Write(pSaveData->pBaseData, dataSize, 1, pFile);
	FS_Close(pFile);
	EntityPatchWrite(pSaveData, g_psv.name);

	Q_snprintf(name, sizeof(name), "%s%s.HL2", Host_SaveGameDirectory(), g_psv.name);
	COM_FixSlashes(name);
	CL_Save(name);

	return pSaveData;
}

void CL_Save(const char *name)
{
}

void EntityInit(edict_t *pEdict, int className)
{
	ENTITYINIT pEntityInit;
	if(!className)
		Sys_Error("Bad class!!\n");

	ReleaseEntityDLLFields(pEdict);
	InitEntityDLLFields(pEdict);
	pEdict->v.classname = className;
	pEntityInit = GetEntityInit(&pr_strings[className]);
	if(pEntityInit)
		pEntityInit(&pEdict->v);
}

SAVERESTOREDATA *LoadSaveData(const char *level)
{
	char name[MAX_PATH];
	char *pszTokenList;
	FILE *pFile;
	int i;
	int size;
	int tokenCount;
	int tokenSize;
	int tableCount;
	SAVERESTOREDATA *pSaveData;

	Q_snprintf(name, sizeof(name), "%s%s.HL1", Host_SaveGameDirectory(), level);
	COM_FixSlashes(name);
	Con_Printf("Loading game from %s...\n", name);
	pFile = FS_OpenPathID(name, "rb", "GAMECONFIG");
	if(!pFile)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return NULL;
	}
	FS_Read(&i, sizeof(int), 1, pFile);
	if(i != SAVEFILE_HEADER)
	{
		FS_Close(pFile);
		return NULL;
	}
	FS_Read(&i, sizeof(int), 1, pFile);
	if(i != SAVEGAME_VERSION)
	{
		FS_Close(pFile);
		return NULL;
	}

	FS_Read(&size, sizeof(int), 1, pFile);
	FS_Read(&tableCount, sizeof(int), 1, pFile);
	FS_Read(&tokenCount, sizeof(int), 1, pFile);
	FS_Read(&tokenSize, sizeof(int), 1, pFile);

	pSaveData = (SAVERESTOREDATA *)Mem_Calloc(
	size + tokenSize + sizeof(ENTITYTABLE) * tableCount +
	sizeof(SAVERESTOREDATA),
	1u);
	pSaveData->tableCount = tableCount;
	pSaveData->tokenCount = tokenCount;
	pSaveData->tokenSize = tokenSize;
	Q_strncpy(pSaveData->szCurrentMapName, level, sizeof(pSaveData->szCurrentMapName) - 1);
	pSaveData->szCurrentMapName[sizeof(pSaveData->szCurrentMapName) - 1] = 0;

	pszTokenList = (char *)(pSaveData + 1);
	if(tokenSize > 0)
	{
		FS_Read(pszTokenList, tokenSize, 1, pFile);
		if(!pSaveData->pTokens)
		{
			pSaveData->pTokens = (char **)Mem_Calloc(tokenCount, sizeof(char *));
		}
		for(i = 0; i < tokenCount; i++)
		{
			if(*pszTokenList)
				pSaveData->pTokens[i] = pszTokenList;
			else
				pSaveData->pTokens[i] = NULL;
			while(*pszTokenList++)
				;
		}
	}

	pSaveData->pTable = (ENTITYTABLE *)pszTokenList;
	pSaveData->connectionCount = 0;
	pSaveData->size = 0;

	pSaveData->pBaseData = pszTokenList;
	pSaveData->pCurrentData = pszTokenList;

	pSaveData->fUseLandmark = 1;
	pSaveData->bufferSize = size;
	pSaveData->time = 0.0f;
	pSaveData->vecLandmarkOffset[0] = 0.0f;
	pSaveData->vecLandmarkOffset[1] = 0.0f;
	pSaveData->vecLandmarkOffset[2] = 0.0f;
	gGlobalVariables.pSaveData = pSaveData;

	FS_Read(pSaveData->pBaseData, size, 1, pFile);
	FS_Close(pFile);

	return pSaveData;
}

void ParseSaveTables(SAVERESTOREDATA *pSaveData, SAVE_HEADER *pHeader, int updateGlobals)
{
	int i;
	SAVELIGHTSTYLE light;
	for(i = 0; i < pSaveData->tableCount; i++)
	{
		gEntityInterface.pfnSaveReadFields(
		pSaveData, "ETABLE", &(pSaveData->pTable[i]), gEntityTableDescription, ARRAYSIZE(gEntityTableDescription));
		pSaveData->pTable[i].pent = NULL;
	}

	pSaveData->size = 0;
	pSaveData->pBaseData = pSaveData->pCurrentData;
	gEntityInterface.pfnSaveReadFields(pSaveData, "Save Header", pHeader, gSaveHeaderDescription, ARRAYSIZE(gSaveHeaderDescription));
	pSaveData->connectionCount = pHeader->connectionCount;
	pSaveData->fUseLandmark = 1;
	pSaveData->time = pHeader->time;
	pSaveData->vecLandmarkOffset[0] = 0;
	pSaveData->vecLandmarkOffset[1] = 0;
	pSaveData->vecLandmarkOffset[2] = 0;

	for(i = 0; i < pSaveData->connectionCount; i++)
	{
		gEntityInterface.pfnSaveReadFields(
		pSaveData, "ADJACENCY", &(pSaveData->levelList[i]), gAdjacencyDescription, ARRAYSIZE(gAdjacencyDescription));
	}
	for(i = 0; i < pHeader->lightStyleCount; i++)
	{
		gEntityInterface.pfnSaveReadFields(pSaveData, "LIGHTSTYLE", &light, gLightstyleDescription, ARRAYSIZE(gLightstyleDescription));
		if(updateGlobals)
		{
			g_psv.lightstyles[light.index] =
			(char *)Hunk_Alloc(Q_strlen(light.style) + 1);
			Q_strncpy(g_psv.lightstyles[light.index], light.style, 3);
			g_psv.lightstyles[light.index][3] = 0;
		}
	}
}

void EntityPatchWrite(SAVERESTOREDATA *pSaveData, const char *level)
{
	char name[MAX_PATH];
	FILE *pFile;
	int i;
	int size = 0;

	Q_snprintf(name, sizeof(name), "%s%s.HL3", Host_SaveGameDirectory(), level);
	COM_FixSlashes(name);
	pFile = FS_OpenPathID(name, "wb", "GAMECONFIG");
	if(!pFile)
		return;

	for(i = 0; i < pSaveData->tableCount; i++)
	{
		if(pSaveData->pTable[i].flags & FENTTABLE_REMOVED)
			size++;
	}
	FS_Write(&size, sizeof(int), 1, pFile);
	for(i = 0; i < pSaveData->tableCount; i++)
	{
		if(pSaveData->pTable[i].flags & FENTTABLE_REMOVED)
			FS_Write(&i, sizeof(int), 1, pFile);
	}
	FS_Close(pFile);
}

void EntityPatchRead(SAVERESTOREDATA *pSaveData, const char *level)
{
	char name[MAX_PATH];
	FILE *pFile;
	int i;
	int size;
	int entityId;

	Q_snprintf(name, sizeof(name), "%s%s.HL3", Host_SaveGameDirectory(), level);
	COM_FixSlashes(name);
	pFile = FS_OpenPathID(name, "rb", "GAMECONFIG");
	if(!pFile)
		return;

	FS_Read(&size, 4, 1, pFile);
	for(i = 0; i < size; i++)
	{
		FS_Read(&entityId, 4, 1, pFile);
		pSaveData->pTable[entityId].flags = FENTTABLE_REMOVED;
	}
}

int LoadGamestate(char *level, int createPlayers)
{
	int i;
	edict_t *pent;
	SAVE_HEADER header;
	SAVERESTOREDATA *pSaveData;
	ENTITYTABLE *pEntInfo;

	pSaveData = LoadSaveData(level);
	if(!pSaveData)
		return 0;

	ParseSaveTables(pSaveData, &header, 1);
	EntityPatchRead(pSaveData, level);

	Q_strncpy(g_psv.name, header.mapName, sizeof(g_psv.name) - 1);
	g_psv.name[sizeof(g_psv.name) - 1] = 0;

	Cvar_Set("sv_skyname", header.skyName);
	Cvar_SetValue("skill", header.skillLevel);
	Cvar_SetValue("sv_skycolor_r", header.skyColor_r);
	Cvar_SetValue("sv_skycolor_g", header.skyColor_g);
	Cvar_SetValue("sv_skycolor_b", header.skyColor_b);
	Cvar_SetValue("sv_skyvec_x", header.skyVec_x);
	Cvar_SetValue("sv_skyvec_y", header.skyVec_y);
	Cvar_SetValue("sv_skyvec_z", header.skyVec_z);

	for(i = 0; i < pSaveData->tableCount; i++)
	{
		pent = NULL;
		pEntInfo = &pSaveData->pTable[i];
		if(pEntInfo->classname && pEntInfo->size &&
		   !(pEntInfo->flags & FENTTABLE_REMOVED))
		{
			if(pEntInfo->id)
			{
				if(pEntInfo->id > g_psvs.maxclients)
					pEntInfo->pent = CreateNamedEntity(pEntInfo->classname);
				else
				{
					if(!(pEntInfo->flags & FENTTABLE_PLAYER))
						Sys_Error("ENTITY IS NOT A PLAYER: %d\n", i);

					pent = g_psvs.clients[pEntInfo->id - 1].edict;
					if(createPlayers && pent)
						EntityInit(pent, pEntInfo->classname);
					else
						pent = NULL;
				}
			}
			else
			{
				pent = g_psv.edicts;
				EntityInit(pent, pEntInfo->classname);
			}
		}
		pEntInfo->pent = pent;
	}
	for(i = 0; i < pSaveData->tableCount; i++)
	{
		pEntInfo = &pSaveData->pTable[i];
		pent = pEntInfo->pent;

		pSaveData->size = pEntInfo->location;
		pSaveData->pCurrentData = &pSaveData->pBaseData[pEntInfo->location];

		if(pent)
		{
			if(gEntityInterface.pfnRestore(pent, pSaveData, 0) < 0)
			{
				ED_Free(pent);
				pEntInfo->pent = NULL;
			}
			else
				SV_LinkEdict(pent, FALSE);
		}
	}
	SaveExit(pSaveData);
	g_psv.time = header.time;
	return 1;
}

int EntryInTable(SAVERESTOREDATA *pSaveData, const char *pMapName, int index)
{
	int i;
	for(i = index + 1; i < pSaveData->connectionCount; i++)
	{
		if(!Q_strcmp(pSaveData->levelList[i].mapName, pMapName))
			return i;
	}
	return -1;
}

void LandmarkOrigin(SAVERESTOREDATA *pSaveData, vec_t *output, const char *pLandmarkName)
{
	int i;
	for(i = 0; i < pSaveData->connectionCount; i++)
	{
		if(!Q_strcmp(pSaveData->levelList[i].landmarkName, pLandmarkName))
		{
			output[0] = pSaveData->levelList[i].vecLandmarkOrigin[0];
			output[1] = pSaveData->levelList[i].vecLandmarkOrigin[1];
			output[2] = pSaveData->levelList[i].vecLandmarkOrigin[2];
			return;
		}
	}
	output[0] = 0.0f;
	output[1] = 0.0f;
	output[2] = 0.0f;
}

int EntityInSolid(edict_t *pent)
{
	vec3_t point;
	if(pent->v.movetype == MOVETYPE_FOLLOW)
	{
		edict_t *pAimEnt = pent->v.aiment;
		if(pAimEnt && pAimEnt->v.flags & FL_CLIENT)
			return 0;
	}
	g_groupmask = pent->v.groupinfo;
	point[0] = (pent->v.absmin[0] + pent->v.absmax[0]) * 0.5f;
	point[1] = (pent->v.absmin[1] + pent->v.absmax[1]) * 0.5f;
	point[2] = (pent->v.absmin[2] + pent->v.absmax[2]) * 0.5f;
	return (SV_PointContents(point) == CONTENTS_SOLID);
}

int CreateEntityList(SAVERESTOREDATA *pSaveData, int levelMask)
{
	int i;
	int movedCount = 0;
	int active;
	edict_t *pent;
	ENTITYTABLE *pEntInfo;

	if(pSaveData->tableCount < 1)
		return 0;

	for(i = 0; i < pSaveData->tableCount; i++)
	{
		pent = NULL;
		pEntInfo = &pSaveData->pTable[i];
		if(pEntInfo->classname && pEntInfo->size && pEntInfo->id)
		{
			active = !!(pEntInfo->flags & levelMask);
			if(SV_IsPlayerIndex(pEntInfo->id))
			{
				client_t *cl = &g_psvs.clients[pEntInfo->id - 1];
				if(active && cl)
				{
					pent = cl->edict;
					if(pent && !pent->free)
					{
						if(!(pEntInfo->flags & FENTTABLE_PLAYER))
							Sys_Error("ENTITY IS NOT A PLAYER: %d\n", i);

						if(cl->active)
							EntityInit(pent, pEntInfo->classname);
					}
				}
			}
			else if(active)
				pent = CreateNamedEntity(pEntInfo->classname);
		}
		pEntInfo->pent = pent;
	}
	for(i = 0; i < pSaveData->tableCount; i++)
	{
		pEntInfo = &pSaveData->pTable[i];
		pent = pEntInfo->pent;
		pSaveData->currentIndex = i;
		pSaveData->size = pEntInfo->location;
		pSaveData->pCurrentData = &pSaveData->pBaseData[pEntInfo->location];

		if(pent && (pEntInfo->flags & levelMask))
		{
			if(pEntInfo->flags & FENTTABLE_GLOBAL)
			{
				Con_DPrintf("Merging changes for global: %s\n",
				            &pr_strings[pEntInfo->classname]);
				gEntityInterface.pfnRestore(pent, pSaveData, 1);
				ED_Free(pent);
			}
			else
			{
				Con_DPrintf("Transferring %s (%d)\n", &pr_strings[pEntInfo->classname], NUM_FOR_EDICT(pent));
				if(gEntityInterface.pfnRestore(pent, pSaveData, 0) < 0)
					ED_Free(pent);
				else
				{
					SV_LinkEdict(pent, FALSE);
					if(!(pEntInfo->flags & FENTTABLE_PLAYER) && EntityInSolid(pent))
					{
						Con_DPrintf("Suppressing %s\n", &pr_strings[pEntInfo->classname]);
						ED_Free(pent);
					}
					else
					{
						movedCount++;
						pEntInfo->flags = FENTTABLE_REMOVED;
					}
				}
			}
		}
	}
	return movedCount;
}

void LoadAdjacentEntities(const char *pOldLevel, const char *pLandmarkName)
{
	int i;
	int test;
	int flags;
	int index;
	int movedCount = 0;
	SAVE_HEADER header;
	vec3_t landmarkOrigin;
	SAVERESTOREDATA *pSaveData;
	SAVERESTOREDATA currentLevelData;

	Q_memset(&currentLevelData, 0, sizeof(SAVERESTOREDATA));
	gGlobalVariables.pSaveData = &currentLevelData;
	gEntityInterface.pfnParmsChangeLevel();

	for(i = 0; i < currentLevelData.connectionCount; i++)
	{
		for(test = 0; test < i; test++)
		{
			// Only do maps once
			if(!Q_strcmp(currentLevelData.levelList[i].mapName,
			             currentLevelData.levelList[test].mapName))
				break;
		}
		// Map was already in the list
		if(test < i)
			continue;

		pSaveData = LoadSaveData(currentLevelData.levelList[i].mapName);
		if(pSaveData)
		{
			ParseSaveTables(pSaveData, &header, 0);
			EntityPatchRead(pSaveData, currentLevelData.levelList[i].mapName);
			pSaveData->time = g_psv.time;
			pSaveData->fUseLandmark = 1;
			flags = 0;

			LandmarkOrigin(&currentLevelData, landmarkOrigin, pLandmarkName);
			LandmarkOrigin(pSaveData, pSaveData->vecLandmarkOffset, pLandmarkName);

			pSaveData->vecLandmarkOffset[0] -= landmarkOrigin[0];
			pSaveData->vecLandmarkOffset[1] -= landmarkOrigin[1];
			pSaveData->vecLandmarkOffset[2] -= landmarkOrigin[2];

			if(!Q_strcmp(currentLevelData.levelList[i].mapName, pOldLevel))
				flags |= FENTTABLE_PLAYER;

			index = -1;
			while(1)
			{
				index = EntryInTable(pSaveData, g_psv.name, index);
				if(index < 0)
					break;

				flags |= (1 << index);
			}

			if(flags)
				movedCount = CreateEntityList(pSaveData, flags);
			// If ents were moved, rewrite entity table to save file
			if(movedCount)
				EntityPatchWrite(pSaveData, currentLevelData.levelList[i].mapName);
			SaveExit(pSaveData);
		}
	}
	gGlobalVariables.pSaveData = NULL;
}

void FileCopy(FileHandle_t pOutput, FileHandle_t pInput, int fileSize)
{
	char buf[1024];
	int size;
	while(fileSize > 0)
	{
		if(fileSize > sizeof(buf))
			size = sizeof(buf);
		else
			size = fileSize;

		FS_Read(buf, size, 1, pInput);
		FS_Write(buf, size, 1, pOutput);
		fileSize -= size;
	}
}

void DirectoryCopy(const char *pPath, FileHandle_t pFile)
{
	const char *findfn;
	char basefindfn[MAX_PATH];
	int fileSize;
	CFile *pCopy;
	char szName[MAX_PATH];

	findfn = Sys_FindFirst(pPath, basefindfn);
	while(findfn != NULL)
	{
		Q_snprintf(szName, sizeof(szName), "%s%s", Host_SaveGameDirectory(), findfn);
		COM_FixSlashes(szName);
		pCopy = gpFileSystem->OpenPathID(szName, "rb", "GAMECONFIG");
		fileSize = pCopy->GetSize();
		pFile->Write(findfn, MAX_PATH, 1);
		pFile->Write(&fileSize, sizeof(int), 1);
		FileCopy(pFile, pCopy, fileSize);
		gpFileSystem->Close(pCopy);
		findfn = Sys_FindNext(basefindfn);
	}
	Sys_FindClose();
}

void DirectoryExtract(FileHandle_t pFile, int fileCount)
{
	int i;
	int fileSize;
	FILE *pCopy;
	char szName[MAX_PATH];
	char fileName[MAX_PATH];

	for(i = 0; i < fileCount; i++)
	{
		FS_Read(fileName, sizeof(fileName), 1, pFile);
		FS_Read(&fileSize, sizeof(int), 1, pFile);
		Q_snprintf(szName, sizeof(szName), "%s%s", Host_SaveGameDirectory(), fileName);
		COM_FixSlashes(szName);
		pCopy = FS_OpenPathID(szName, "wb", "GAMECONFIG");
		FileCopy(pCopy, pFile, fileCount);
		FS_Close(pCopy);
	}
}

int DirectoryCount(const char *pPath)
{
	int count;
	const char *findfn;

	count = 0;
	findfn = Sys_FindFirstPathID(pPath, "GAMECONFIG");

	while(findfn != NULL)
	{
		findfn = Sys_FindNext(NULL);
		count++;
	}
	Sys_FindClose();
	return count;
}

void Host_ClearSaveDirectory()
{
	char szName[MAX_PATH];
	const char *pfn;

	Q_snprintf(szName, sizeof(szName), "%s", Host_SaveGameDirectory());
	Q_strncat(szName, "*.HL?", sizeof(szName) - Q_strlen(szName) - 1);
	COM_FixSlashes(szName);

	if(Sys_FindFirstPathID(szName, "GAMECONFIG") != NULL)
	{
		Sys_FindClose();
		Q_snprintf(szName, sizeof(szName), "%s", Host_SaveGameDirectory());
		COM_FixSlashes(szName);
		FS_CreateDirHierarchy(szName, "GAMECONFIG");
		Q_strncat(szName, "*.HL?", sizeof(szName) - Q_strlen(szName) - 1);

		for(pfn = Sys_FindFirstPathID(szName, "GAMECONFIG"); pfn;
		    pfn = Sys_FindNext(NULL))
		{
			Q_snprintf(szName, sizeof(szName), "%s%s", Host_SaveGameDirectory(), pfn);
			FS_RemoveFile(szName, "GAMECONFIG");
		}
	}
	Sys_FindClose();
}

void Host_ClearGameState()
{
	S_StopAllSounds(TRUE);
	Host_ClearSaveDirectory();
	if(gEntityInterface.pfnResetGlobalState)
		gEntityInterface.pfnResetGlobalState();
}

void Host_Changelevel2_f()
{
	char level[64];
	char oldlevel[64];
	char _startspot[64];
	char *startspot;

	SAVERESTOREDATA *pSaveData;
	qboolean newUnit;

	giActive = DLL_TRANS;

	if(Cmd_Argc() < 2)
	{
		Con_Printf("changelevel2 <levelname> : continue game on a new level in the "
		           "unit\n");
		return;
	}
	if(!g_psv.active || cls.demoplayback || g_psv.paused)
	{
		Con_Printf("Only the server may changelevel\n");
		return;
	}
	if(g_psvs.maxclients > 1)
	{
		Con_Printf(
		"changelevel2 <levelname> : not for use with multiplayer games\n");
		return;
	}

	SCR_BeginLoadingPlaque(FALSE);
	S_StopAllSounds(TRUE);

	Q_strncpy(level, Cmd_Argv(1), sizeof(level) - 1);
	level[sizeof(level) - 1] = 0;

	if(Cmd_Argc() != 2)
	{
		startspot = &_startspot[0];
		Q_strncpy(_startspot, Cmd_Argv(2), sizeof(_startspot) - 1);
		_startspot[sizeof(_startspot) - 1] = 0;
	}
	else
		startspot = NULL;

	Q_strncpy(oldlevel, g_psv.name, sizeof(oldlevel) - 1);
	oldlevel[sizeof(oldlevel) - 1] = 0;

	pSaveData = SaveGamestate();
	SV_ServerShutdown();
	FS_LogLevelLoadStarted(level);

	if(!SV_SpawnServer(FALSE, level, startspot))
		Sys_Error("Host_Changelevel2: Couldn't load map %s\n", level);

	if(pSaveData)
		SaveExit(pSaveData);

	newUnit = FALSE;
	if(!LoadGamestate(level, 0))
	{
		newUnit = TRUE;
		SV_LoadEntities();
	}

	LoadAdjacentEntities(oldlevel, startspot);
	gGlobalVariables.time = g_psv.time;

	g_psv.paused = TRUE;
	g_psv.loadgame = TRUE;

	if(newUnit && sv_newunit.value != 0.0f)
		Host_ClearSaveDirectory();

	SV_ActivateServer(0);
}

void Host_Version_f()
{
	Con_Printf("Protocol version %i\nExe version %s (%s)\n", PROTOCOL_VERSION, gpszVersionString, gpszProductString);

#ifdef REHLDS_FIXES
	Con_Printf("ReHLDS version: " APP_VERSION "\n");
	Con_Printf("Build date: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n",
	           build_number());
	Con_Printf("Build from: " APP_COMMIT_URL APP_COMMIT_SHA "\n");
#else
	Con_Printf("Exe build: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n",
	           build_number());
#endif
}

void Host_FullInfo_f()
{
	char key[512];
	char value[512];
	char *o;
	char *s;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("fullinfo <complete info string>\n");
		return;
	}

	s = (char *)Cmd_Argv(1);
	if(*s == '\\')
		s++;

	while(*s)
	{
		o = key;
		while(*s && *s != '\\')
			*o++ = *s++;
		*o = 0;

		if(!*s)
		{
			Con_Printf("MISSING VALUE\n");
			return;
		}

		o = value;
		s++;
		while(*s && *s != '\\')
			*o++ = *s++;
		*o = 0;
		if(*s)
			s++;

		if(cmd_source == src_command)
		{
			Info_SetValueForKey(cls.userinfo, key, value, MAX_INFO_STRING);
			Cmd_ForwardToServer();
			return;
		}
		Info_SetValueForKey(host_client->userinfo, key, value, MAX_INFO_STRING);
		host_client->sendinfo = TRUE;
	}
}

/*
==================
Host_God_f

Sets client to godmode
==================
*/
void Host_God_f()
{
	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	};

	if (gGlobalVariables.deathmatch && !host_client->privileged)
		return;

	sv_player->v.flags = (int)sv_player->v.flags ^ FL_GODMODE;
	if (!((int)sv_player->v.flags & FL_GODMODE) )
		SV_ClientPrintf ("godmode OFF\n");
	else
		SV_ClientPrintf ("godmode ON\n");
};

void Host_Notarget_f ()
{
	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer ();
		return;
	}

	if (gGlobalVariables.deathmatch && !host_client->privileged)
		return;

	sv_player->v.flags = (int)sv_player->v.flags ^ FL_NOTARGET;
	if (!((int)sv_player->v.flags & FL_NOTARGET) )
		SV_ClientPrintf ("notarget OFF\n");
	else
		SV_ClientPrintf ("notarget ON\n");
};

/*
==================
Host_Fly_f

Sets client to flymode
==================
*/
void Host_Fly_f()
{
	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer ();
		return;
	}

	if (gGlobalVariables.deathmatch && !host_client->privileged)
		return;

	if (sv_player->v.movetype != MOVETYPE_FLY)
	{
		sv_player->v.movetype = MOVETYPE_FLY;
		SV_ClientPrintf ("flymode ON\n");
	}
	else
	{
		sv_player->v.movetype = MOVETYPE_WALK;
		SV_ClientPrintf ("flymode OFF\n");
	}
}

qboolean noclip_anglehack;

void Host_Noclip_f ()
{
	if (cmd_source == src_command)
	{
		Cmd_ForwardToServer ();
		return;
	}

	if (gGlobalVariables.deathmatch && !host_client->privileged)
		return;

	if (sv_player->v.movetype != MOVETYPE_NOCLIP)
	{
		noclip_anglehack = true;
		sv_player->v.movetype = MOVETYPE_NOCLIP;
		SV_ClientPrintf ("noclip ON\n");
	}
	else
	{
		noclip_anglehack = false;
		sv_player->v.movetype = MOVETYPE_WALK;
		SV_ClientPrintf ("noclip OFF\n");
	}
}

edict_t	*FindViewthing (void)
{
	int		i;
	edict_t	*e;
	
	for (i=0 ; i<sv.num_edicts ; i++)
	{
		e = EDICT_NUM(i);
		if ( !Q_strcmp (PR_GetString(e->v.classname), "viewthing") )
			return e;
	}
	Con_Printf ("No viewthing on map\n");
	return NULL;
}

/*
==================
Host_Viewmodel_f
==================
*/
void Host_Viewmodel_f (void)
{
	edict_t	*e;
	model_t	*m;

	e = FindViewthing ();
	if (!e)
		return;

	m = Mod_ForName (Cmd_Argv(1), false);
	if (!m)
	{
		Con_Printf ("Can't load %s\n", Cmd_Argv(1));
		return;
	}
	
	e->v.frame = 0;
	cl.model_precache[(int)e->v.modelindex] = m;
}

/*
==================
Host_Viewframe_f
==================
*/
void Host_Viewframe_f (void)
{
	edict_t	*e;
	int		f;
	model_t	*m;

	e = FindViewthing ();
	if (!e)
		return;
	m = cl.model_precache[(int)e->v.modelindex];

	f = atoi(Cmd_Argv(1));
	if (f >= m->numframes)
		f = m->numframes-1;

	e->v.frame = f;		
}


void PrintFrameName (model_t *m, int frame)
{
	aliashdr_t 			*hdr;
	maliasframedesc_t	*pframedesc;

	hdr = (aliashdr_t *)Mod_Extradata (m);
	if (!hdr)
		return;
	pframedesc = &hdr->frames[frame];
	
	Con_Printf ("frame %i: %s\n", frame, pframedesc->name);
}

/*
==================
Host_Viewnext_f
==================
*/
void Host_Viewnext_f (void)
{
	edict_t	*e;
	model_t	*m;
	
	e = FindViewthing ();
	if (!e)
		return;
	m = cl.model_precache[(int)e->v.modelindex];

	e->v.frame = e->v.frame + 1;
	if (e->v.frame >= m->numframes)
		e->v.frame = m->numframes - 1;

	PrintFrameName (m, e->v.frame);		
}

/*
==================
Host_Viewprev_f
==================
*/
void Host_Viewprev_f (void)
{
	edict_t	*e;
	model_t	*m;

	e = FindViewthing ();
	if (!e)
		return;

	m = cl.model_precache[(int)e->v.modelindex];

	e->v.frame = e->v.frame - 1;
	if (e->v.frame < 0)
		e->v.frame = 0;

	PrintFrameName (m, e->v.frame);		
};

NOXREF void Host_KillVoice_f()
{
	NOXREFCHECK;
	
	Voice_Deinit();
}

void Host_SetInfo_f()
{
	if(Cmd_Argc() == 1)
	{
		Info_Print(cls.userinfo);
		return;
	}
	if(Cmd_Argc() != 3)
	{
		Con_Printf("usage: setinfo [ <key> <value> ]\n");
		return;
	}
	if(cmd_source == src_command)
	{
		Info_SetValueForKey(cls.userinfo, Cmd_Argv(1), Cmd_Argv(2), MAX_INFO_STRING);
		Cmd_ForwardToServer();
		return;
	}
	Info_SetValueForKey(host_client->userinfo, Cmd_Argv(1), Cmd_Argv(2), MAX_INFO_STRING);
	host_client->sendinfo = TRUE;
}

void Host_Say(qboolean teamonly)
{
	client_t *client;
	client_t *save;
	int j;
	char *p;
	char text[128];
	// qboolean fromServer;//unsued?

	if(cls.state != ca_dedicated)
	{
		if(cmd_source != src_command)
			return;

		Cmd_ForwardToServer();
		return;
	}

	if(Cmd_Argc() < 2)
		return;

	p = (char *)Cmd_Args();
	if(!p)
		return;

	save = host_client;
	if(*p == '"')
	{
		p++;
		p[Q_strlen(p) - 1] = 0;
	}

#ifdef REHLDS_FIXES
	// I think '\x01' don't need in TextMsg
	Q_snprintf(text, sizeof(text), "<%s> ", Cvar_VariableString("hostname"));
#else  // REHLDS_FIXES
	Q_snprintf(text, sizeof(text), "%c<%s> ", 1, Cvar_VariableString("hostname"));
#endif // REHLDS_FIXES

	if(Q_strlen(p) > 63)
		p[63] = 0;

	j = sizeof(text) - 2 - Q_strlen(text);
	if(Q_strlen(p) > (unsigned int)j)
		p[j] = 0;

	Q_strcat(text, p);
	Q_strcat(text, "\n");

	for(j = 0, client = g_psvs.clients; j < g_psvs.maxclients; j++, client++)
	{
		if(!client->active || !client->spawned || client->fakeclient)
			continue;

		host_client = client;

#ifdef REHLDS_FIXES
		// Text can be unsafe (format %, localize #) therefore need to send text as
		// argument. TextMsg is used here instead of SayText, because SayText in
		// Half-Life doesn't support arguments.
		PF_MessageBegin_I(MSG_ONE, RegUserMsg("TextMsg", -1), NULL, &g_psv.edicts[j + 1]);
		PF_WriteByte_I(HUD_PRINTTALK);
		PF_WriteString_I("%s");
		PF_WriteString_I(text);
		PF_MessageEnd_I();
#else  // REHLDS_FIXES
		PF_MessageBegin_I(MSG_ONE, RegUserMsg("SayText", -1), NULL, &g_psv.edicts[j + 1]);
		PF_WriteByte_I(0);
		PF_WriteString_I(text);
		PF_MessageEnd_I();
#endif // REHLDS_FIXES
	}

	host_client = save;
	Sys_Printf("%s", &text[1]);
	Log_Printf("Server say \"%s\"\n", p);
}

void Host_Say_f()
{
	Host_Say(FALSE);
}

void Host_Say_Team_f()
{
	Host_Say(TRUE);
}

void Host_Tell_f()
{
	client_t *client;
	client_t *save;
	int j;
	char *p;
	char text[64];
	char *tellmsg;

	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}
	if(Cmd_Argc() < 3)
		return;

	p = (char *)Cmd_Args();
	if(!p)
		return;

	Q_snprintf(text, sizeof(text), "%s TELL: ", host_client->name);
	
	// remove quotes if present
	if(*p == '"')
	{
		p++;
		p[Q_strlen(p) - 1] = 0;
	};
	
	// check length & truncate if necessary
	j = ARRAYSIZE(text) - 2 - Q_strlen(text);
	if(Q_strlen(p) > (unsigned int)j)
		p[j] = 0;

	tellmsg = Q_strstr(p, Cmd_Argv(1));
	if(tellmsg != NULL)
		Q_strcat(text, &tellmsg[Q_strlen(Cmd_Argv(1))]);
	else
		Q_strcat(text, p);

	Q_strcat(text, "\n");
	save = host_client;

	for(j = 0, client = g_psvs.clients; j < g_psvs.maxclients; j++, client++)
	{
		if(!client->active || !client->spawned || client->fakeclient)
			continue;
		if(Q_stricmp(client->name, Cmd_Argv(1)))
			continue;

		host_client = client;

		PF_MessageBegin_I(MSG_ONE, RegUserMsg("SayText", -1), NULL, &g_psv.edicts[j + 1]);
		PF_WriteByte_I(0);
		PF_WriteString_I(text);
		PF_MessageEnd_I();
		break;
	}
	host_client = save;
}

/*
==================
Host_Kill_f
==================
*/
void Host_Kill_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	};
	
	if(sv_player->v.health <= 0.0f
#ifdef REHLDS_FIXES
	    || sv_player->v.deadflag != DEAD_NO
#endif
	)
	{
		SV_ClientPrintf("Can't suicide -- already dead!\n");
		return;
	};
	
	gGlobalVariables.time = g_psv.time;
	gEntityInterface.pfnClientKill(sv_player);
}

/*
==================
CL_Pause_f
==================
*/
void Host_TogglePause_f()
{
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}
	if(!pausable.value)
	{
		SV_ClientPrintf("Pause not allowed.\n");
		return;
	}
	g_psv.paused ^= TRUE;
	if(g_psv.paused)
		SV_BroadcastPrintf("%s paused the game\n",
		                   &pr_strings[sv_player->v.netname]);
	else
		SV_BroadcastPrintf("%s unpaused the game\n",
		                   &pr_strings[sv_player->v.netname]);

// TODO: send svc_setpause to client on spawn command
#ifdef REHLDS_FIXES
	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		if(!g_psvs.clients[i].connected || g_psvs.clients[i].fakeclient)
			continue;

		MSG_WriteByte(&g_psvs.clients[i].netchan.message, svc_setpause);
		MSG_WriteByte(&g_psvs.clients[i].netchan.message, g_psv.paused);
	}
#else  // REHLDS_FIXES
	MSG_WriteByte(&g_psv.reliable_datagram, svc_setpause);
	MSG_WriteByte(&g_psv.reliable_datagram, g_psv.paused);
#endif // REHLDS_FIXES
}

void Host_Pause_f()
{
	// pause only singleplayer when console or main menu opens
	if(!cl.levelname[0])
		return;
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}
	if(!pausable.value)
		return;

	g_psv.paused = TRUE;
#ifdef REHLDS_FIXES
	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		if(!g_psvs.clients[i].connected || g_psvs.clients[i].fakeclient)
			continue;

		MSG_WriteByte(&g_psvs.clients[i].netchan.message, svc_setpause);
		MSG_WriteByte(&g_psvs.clients[i].netchan.message, g_psv.paused);
	}
#else  // REHLDS_FIXES
	MSG_WriteByte(&g_psv.reliable_datagram, svc_setpause);
	MSG_WriteByte(&g_psv.reliable_datagram, g_psv.paused);
#endif // REHLDS_FIXES
}

void Host_Unpause_f()
{
	// unpause only singleplayer when console or main menu opens
	if(!cl.levelname[0])
		return;
	if(cmd_source == src_command)
	{
		Cmd_ForwardToServer();
		return;
	}
	if(!pausable.value)
		return;

	g_psv.paused = FALSE;
#ifdef REHLDS_FIXES
	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		if(!g_psvs.clients[i].connected || g_psvs.clients[i].fakeclient)
			continue;

		MSG_WriteByte(&g_psvs.clients[i].netchan.message, svc_setpause);
		MSG_WriteByte(&g_psvs.clients[i].netchan.message, g_psv.paused);
	}
#else  // REHLDS_FIXES
	MSG_WriteByte(&g_psv.reliable_datagram, svc_setpause);
	MSG_WriteByte(&g_psv.reliable_datagram, g_psv.paused);
#endif // REHLDS_FIXES
}

void Host_Interp_f()
{
	r_dointerp ^= 1;
	if(!r_dointerp)
		Con_Printf("Frame Interpolation OFF\n");
	else
		Con_Printf("Frame Interpolation ON\n");
}

/*
=====================
CL_NextDemo

Called to play the next demo in the demo loop
=====================
*/
void Host_NextDemo()
{
	char str[1024];
	
	// don't play demos
	if(cls.demonum == -1)
		return;

	SCR_BeginLoadingPlaque(FALSE);
	
	if(cls.demos[cls.demonum][0])
	{
#ifdef REHLDS_FIXES
		if(cls.demonum >= MAX_DEMOS)
#else
		if(cls.demonum == MAX_DEMOS)
#endif // REHLDS_FIXES
			cls.demonum = 0;

		Q_snprintf(str, sizeof(str), "playdemo %s\n", cls.demos[cls.demonum]);
		Cbuf_InsertText(str);
		++cls.demonum;
	};
	
	Con_Printf("No demos listed with startdemos\n");
	cls.demonum = -1;
};

void Host_Startdemos_f()
{
	int i;
	int c;

	if(cls.state == ca_dedicated)
	{
		if(!g_psv.active)
			Con_Printf("Cannot play demos on a dedicated server.\n");
		return;
	}
	c = Cmd_Argc() - 1;
	if(c > MAX_DEMOS)
	{
		c = MAX_DEMOS;
		Con_Printf("Max %i demos in demoloop\n", MAX_DEMOS);
		Con_Printf("%i demo(s) in loop\n", MAX_DEMOS);
	}
	Con_Printf("%i demo(s) in loop\n", c);
	for(i = 1; i < c + 1; i++)
	{
		Q_strncpy(cls.demos[i - 1], Cmd_Argv(i), 15);
		cls.demos[i - 1][15] = 0;
	}
	if(g_psv.active || cls.demonum == -1 || cls.demoplayback)
		cls.demonum = -1;
	else
	{
		cls.demonum = 0;
		Host_NextDemo();
	}
}

/*
==================
Host_Demos_f

Return to looping demos
==================
*/
void Host_Demos_f()
{
	if(cls.state != ca_dedicated)
	{
		if(cls.demonum == -1)
			cls.demonum = 0;
		
		CL_Disconnect_f();
		Host_NextDemo();
	}
}

/*
==================
Host_Stopdemo_f

Return to looping demos
==================
*/
void Host_Stopdemo_f()
{
	if(cls.state != ca_dedicated)
	{
		if(cls.demoplayback)
		{
			CL_StopPlayback();
			CL_Disconnect();
		}
	}
}

NOXREF void Host_EndSection(const char *pszSection)
{
	NOXREFCHECK;
	
	giActive = DLL_PAUSED;
	giSubState = 1;
	giStateInfo = 1;

	if(!pszSection || !*pszSection)
		Con_Printf(" endsection with no arguments\n");
	else
	{
		if(!Q_stricmp(pszSection, "_oem_end_training"))
			giStateInfo = 1;
		else if(!Q_stricmp(pszSection, "_oem_end_logo"))
			giStateInfo = 2;
		else if(!Q_stricmp(pszSection, "_oem_end_demo"))
			giStateInfo = 3;
		else
			Con_DPrintf(" endsection with unknown Section keyvalue\n");
	}
	Cbuf_AddText("\ndisconnect\n");
}

void Host_Soundfade_f()
{
	int percent;
	int inTime;
	int holdTime;
	int outTime;

	if(Cmd_Argc() != 3 && Cmd_Argc() != 5)
	{
		Con_Printf("soundfade <percent> <hold> [<out> <int>]\n");
		return;
	}

	percent = Q_atoi(Cmd_Argv(1));

	if(percent > 100)
		percent = 100;
	if(percent < 0)
		percent = 0;

	holdTime = Q_atoi(Cmd_Argv(2));
	if(holdTime > 255)
		holdTime = 255;

	if(Cmd_Argc() == 5)
	{
		outTime = Q_atoi(Cmd_Argv(3));
		if(outTime > 255)
			outTime = 255;

		inTime = Q_atoi(Cmd_Argv(4));
		if(inTime > 255)
			inTime = 255;
	}
	else
	{
		outTime = 0;
		inTime = 0;
	}

	cls.soundfade.nStartPercent = percent;
	cls.soundfade.soundFadeStartTime = realtime;
	cls.soundfade.soundFadeOutTime = outTime;
	cls.soundfade.soundFadeHoldTime = holdTime;
	cls.soundfade.soundFadeInTime = inTime;
}

void Host_KillServer_f()
{
	if(cls.state != ca_dedicated)
		CL_Disconnect_f();

	else if(g_psv.active)
	{
		Host_ShutdownServer(FALSE);

		if(cls.state != ca_dedicated)
			NET_Config(FALSE);
	}
}

void Host_VoiceRecordStart_f()
{
	const char *pUncompressedFile = NULL;
	const char *pDecompressedFile = NULL;
	const char *pInputFile = NULL;

	if(cls.state != ca_active)
		return;

	if(voice_recordtofile.value)
	{
		pDecompressedFile = "voice_decompressed.wav";
		pUncompressedFile = "voice_micdata.wav";
	}
	if(voice_inputfromfile.value)
		pInputFile = "voice_input.wav";
	Voice_RecordStart(pUncompressedFile, pDecompressedFile, pInputFile);
}

void Host_VoiceRecordStop_f()
{
	if(cls.state != ca_active)
		return;

	if(Voice_IsRecording())
	{
		CL_AddVoiceToDatagram(TRUE);
		Voice_RecordStop();
	}
}

void Host_Crash_f()
{
	int *p = NULL;
	*p = 0xffffffff;
}

//=============================================================================

void SV_CheckBlendingInterface()
{
	int i;
	SV_BLENDING_INTERFACE_FUNC studio_interface;

	R_ResetSvBlending();
	for(i = 0; i < g_iextdllMac; i++)
	{
#ifdef _WIN32
		studio_interface = (SV_BLENDING_INTERFACE_FUNC)GetProcAddress(
		(HMODULE)g_rgextdll[i].lDLLHandle, "Server_GetBlendingInterface");
#else
		studio_interface = (SV_BLENDING_INTERFACE_FUNC)dlsym(
		g_rgextdll[i].lDLLHandle, "Server_GetBlendingInterface");
#endif // _WIN32
		if(studio_interface)
		{
			if(studio_interface(SV_BLENDING_INTERFACE_VERSION, &g_pSvBlendingAPI, &server_studio_api, (float *)rotationmatrix, (float *)bonetransform))
				return;

			Con_DPrintf("Couldn't get server .dll studio model blending interface. "
			            "Version mismatch?\n");
			R_ResetSvBlending();
		}
	}
}

void SV_CheckSaveGameCommentInterface()
{
	int i;
	SV_SAVEGAMECOMMENT_FUNC pTemp = NULL;
	for(i = 0; i < g_iextdllMac; i++)
	{
#ifdef _WIN32
		pTemp = (SV_SAVEGAMECOMMENT_FUNC)GetProcAddress(
		(HMODULE)g_rgextdll[i].lDLLHandle, "SV_SaveGameComment");
#else
		pTemp = (SV_SAVEGAMECOMMENT_FUNC)dlsym(g_rgextdll[i].lDLLHandle,
		                                       "SV_SaveGameComment");
#endif // _WIN32
		if(pTemp)
			break;
	}
	g_pSaveGameCommentFunc = pTemp;
}
