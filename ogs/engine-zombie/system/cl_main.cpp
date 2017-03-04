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

#include "precompiled.hpp"
#include "client/client.hpp"
#include "system/host.hpp"
#include "system/sizebuf.hpp"
#include "system/system.hpp"
#include "system/buildinfo.hpp"
#include "memory/zone.hpp"
#include "network/net.hpp"
#include "network/net_msg.hpp"
#include "network/protocol.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "console/console.hpp"
#include "sound/sound.hpp"
#include "graphics/screen.hpp"

#ifdef _WIN32
#else
#include <netinet/in.h>
#endif

/*
* Globals initialization
*/

qboolean noclip_anglehack; // remnant from old quake

cvar_t rcon_password = { "rcon_password", "" }; // rcon_client_password
cvar_t rcon_address = { "rcon_address", "" };

cvar_t cl_timeout = { "cl_timeout", "60" }; // 120

cvar_t cl_shownet = { "cl_shownet", "0" }; // can be 0, 1, or 2

cvar_t cl_sbar = { "cl_sbar", "0", FCVAR_ARCHIVE };
cvar_t cl_hudswap = { "cl_hudswap", "0", FCVAR_ARCHIVE };

cvar_t entlatency = { "entlatency", "20" };
cvar_t cl_predict_players = { "cl_predict_players", "1" };
cvar_t cl_predict_players2 = { "cl_predict_players2", "1" };
cvar_t cl_solid_players = { "cl_solid_players", "1" };

cvar_t cl_fixtimerate = { "cl_fixtimerate", "7.5" }; // client clock drift fixer (applied to every received frame from server)

static qboolean allowremotecmd = true;

//
// userinfo mirrors
//
#ifndef HOOK_ENGINE

cvar_t password = { "password", "", FCVAR_USERINFO }; // info_password
cvar_t name = { "name", "unnamed", FCVAR_ARCHIVE | FCVAR_USERINFO }; // cvar_t cl_name = { "name", "unnamed", FCVAR_ARCHIVE | FCVAR_USERINFO, 0.0f, NULL };
cvar_t team = { "team", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t model = { "model", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t skin = { "skin", "", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t topcolor = { "topcolor", "0", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t bottomcolor = { "bottomcolor", "0", FCVAR_ARCHIVE | FCVAR_USERINFO };
cvar_t rate = { "rate", "2500", FCVAR_ARCHIVE | FCVAR_USERINFO }; // cvar_t rate_ = { "rate", "2500", FCVAR_USERINFO, 0.0f, NULL };

cvar_t console = { "console", "1", FCVAR_ARCHIVE }; // "1.0", FCVAR_ARCHIVE, 0.0f, NULL };

#else // HOOK_ENGINE

cvar_t cl_name;
cvar_t rate_;
cvar_t console;

#endif // HOOK_ENGINE

extern cvar_t cl_hightrack;

client_static_t cls;
client_state_t cl;

playermove_t g_clmove;
qboolean cl_inmovie;

// FIXME: put these on hunk?
entity_state_t cl_baselines[MAX_EDICTS];
efrag_t cl_efrags[MAX_EFRAGS];
//cl_entity_t cl_entities[MAX_EDICTS]; // NetQuake is using this instead of cl_baselines
cl_entity_t cl_static_entities[MAX_STATIC_ENTITIES];
lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
dlight_t cl_dlights[MAX_DLIGHTS];

// renderable list
// this is double buffered so the last frame
// can be scanned for oldorigins of trailing objects
int cl_numvisedicts, cl_oldnumvisedicts;
cl_entity_t *cl_visedicts, *cl_oldvisedicts;
cl_entity_t cl_visedicts_list[2][MAX_VISEDICTS];

qboolean nomaster;

byte *host_colormap;

netadr_t master_adr; // address of the master server

cvar_t show_fps = { "cl_showfps", "0" }; // set for running times

int fps_count;

jmp_buf host_abort;

void Master_Connect_f();

float server_version = 0; // version of server we connected to

char emodel_name[] = { 'e' ^ 0xff, 'm' ^ 0xff, 'o' ^ 0xff, 'd' ^ 0xff, 'e' ^ 0xff, 'l' ^ 0xff, 0 };
char pmodel_name[] = { 'p' ^ 0xff, 'm' ^ 0xff, 'o' ^ 0xff, 'd' ^ 0xff, 'e' ^ 0xff, 'l' ^ 0xff, 0 };
char prespawn_name[] = {
	'p' ^ 0xff, 'r' ^ 0xff, 'e' ^ 0xff, 's' ^ 0xff, 'p' ^ 0xff, 'a' ^ 0xff, 'w' ^ 0xff, 'n' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, ' ' ^ 0xff, '0' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, 0
};
char modellist_name[] = { 'm' ^ 0xff, 'o' ^ 0xff, 'd' ^ 0xff, 'e' ^ 0xff, 'l' ^ 0xff, 'l' ^ 0xff, 'i' ^ 0xff, 's' ^ 0xff, 't' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, 0 };
char soundlist_name[] = { 's' ^ 0xff, 'o' ^ 0xff, 'u' ^ 0xff, 'n' ^ 0xff, 'd' ^ 0xff, 'l' ^ 0xff, 'i' ^ 0xff, 's' ^ 0xff, 't' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, ' ' ^ 0xff, '%' ^ 0xff, 'i' ^ 0xff, 0 };

/*
=======================
CL_SendConnectPacket

called by CL_Connect_f and CL_CheckResend
We have gotten a challenge from the server, so try and connect
======================
*/
void CL_SendConnectPacket()
{
#ifndef SWDS
	netadr_t adr;
	char data[2048];

	// JACK: Fixed bug where DNS lookups would cause two connects real fast
	//       Now, adds lookup time to the connect time.
	//		 Should I add it to realtime instead?!?!

	if(cls.state != ca_disconnected)
		return;

	double t1 = Sys_FloatTime();

	if(!NET_StringToAdr(cls.servername, &adr))
	{
		Con_Printf(/*CL_SendConnectPacket:*/ "Bad server address\n");
		cls.connect_time = -1.0f; // 0
		return;
	};

	if(adr.port == 0)
		adr.port = BigShort(PORT_SERVER);
	
	//int port = Cvar_VariableValue("qport"); // net_qport

	double t2 = Sys_FloatTime();

	cls.connect_time = realtime + t2 - t1; // for retransmit requests

	//cls.qport = Cvar_VariableValue("qport");
	//userinfo->modified = false; // userinfo_modified

	Info_SetValueForStarKey(cls.userinfo, "*ip", NET_AdrToString(adr), MAX_INFO_STRING);

	Con_Printf ("Connecting to %s...\n", cls.servername);
	sprintf(data, "%c%c%c%cconnect %i "/*%i*/" %i \"%s\"\n", 255, 255, 255, 255, PROTOCOL_VERSION, /*cls.qport, */cls.challenge, cls.userinfo);

	//NET_SendPacket(strlen(data), data, adr);
#endif // SWDS
};

/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend()
{
#ifndef SWDS
	netadr_t adr;
	char data[2048];

	if(cls.connect_time == -1.0f)
		return;

	if(cls.state != ca_disconnected)
		return;

	if(cls.connect_time && realtime - cls.connect_time < 5.0f)
		return;

	double t1 = Sys_FloatTime();

	if(!NET_StringToAdr(cls.servername, &adr))
	{
		Con_Printf("Bad server address\n");
		cls.connect_time = -1.0f;
		return;
	};

	if(adr.port == 0)
		adr.port = BigShort(PORT_SERVER);

	double t2 = Sys_FloatTime();

	cls.connect_time = realtime + t2 - t1; // for retransmit requests

	Con_Printf("Connecting to %s...\n", cls.servername);
	sprintf(data, "%c%c%c%cgetchallenge\n", 255, 255, 255, 255);
	//NET_SendPacket(strlen(data), data, adr);
#endif // SWDS
};

void CL_BeginServerConnect()
{
	cls.connect_time = 0.0f;
	CL_CheckForResend();
};

/*
================
CL_Connect_f

================
*/
void CL_Connect_f()
{
#ifndef SWDS
	if(Cmd_Argc() != 2)
	{
		Con_Printf("usage: connect <server>\n");
		return;
	};

	char *server = Cmd_Argv(1);

	CL_Disconnect();

	strncpy(cls.servername, server, charsmax(cls.servername));
	CL_BeginServerConnect();
#endif // SWDS
};

/*
=====================
CL_Rcon_f

  Send the rest of the command line over as
  an unconnected command.
=====================
*/
void CL_Rcon_f()
{
	char message[1024];
	int i;
	netadr_t to;

	if(!rcon_password.string) // if (!rcon_client_password->string)
	{
		Con_Printf("You must set 'rcon_password' before issuing an rcon command.\n");
		return;
	};

	message[0] = (char)255;
	message[1] = (char)255;
	message[2] = (char)255;
	message[3] = (char)255;
	message[4] = 0;
	
	//NET_Config(true); // allow remote

	Q_strcat(message, "rcon ");

	Q_strcat(message, rcon_password.string); // rcon_client_password->string
	Q_strcat(message, " ");

	for(i = 1; i < Cmd_Argc(); ++i)
	{
		Q_strcat(message, Cmd_Argv(i));
		Q_strcat(message, " ");
	};

	if(cls.state >= ca_connected)
		to = cls.netchan.remote_address;
	else
	{
		if(!Q_strlen(rcon_address.string))
		{
			Con_Printf("You must either be connected,\n"
			           "or set the 'rcon_address' cvar\n"
			           "to issue rcon commands\n");

			return;
		};

		NET_StringToAdr(rcon_address.string, &to);
		
		if(to.port == 0)
			to.port = BigShort(PORT_SERVER);
	};

	//NET_SendPacket(/*NS_CLIENT,*/ Q_strlen(message) + 1, message, to);
};

/*
=====================
CL_ClearState

=====================
*/
void CL_ClearState()
{
	int i;

	S_StopAllSounds(true);

	Con_DPrintf("Clearing memory\n");
	//D_FlushCaches();
	//Mod_ClearAll();

	if(host_hunklevel) // FIXME: check this...
		Hunk_FreeToLowMark(host_hunklevel);
	
	//CL_ClearEffects();
	//CL_ClearTEnts();

	// wipe the entire cl structure
	memset(&cl, 0, sizeof(cl));

	SZ_Clear(&cls.netchan.message);

	// clear other arrays
	memset(&cl_entities, 0, sizeof(cl_entities));
	memset(cl_efrags, 0, sizeof(cl_efrags));
	memset(cl_dlights, 0, sizeof(cl_dlights));
	memset(cl_lightstyle, 0, sizeof(cl_lightstyle));

	//
	// allocate the efrags and chain together into a free list
	//
	cl.free_efrags = cl_efrags;

	for(i = 0; i < MAX_EFRAGS - 1; ++i)
		cl.free_efrags[i].entnext = &cl.free_efrags[i + 1];

	cl.free_efrags[i].entnext = NULL;
};

/*
=====================
CL_Disconnect

Goes from a connected state to full screen console state
Sends a disconnect message to the server
This is also called on Host_Error, so it shouldn't cause any errors
=====================
*/
void CL_Disconnect()
{
#ifndef SWDS
	if(cls.state == ca_disconnected)
		return;
	
	// Stop sounds (especially looping!)
	S_StopAllSounds(true);

	cls.connect_time = -1.0f; // 0

	// bring the console down and fade the colors back to normal
	//	SCR_BringDownConsole();

	// if running a local server, shut it down
	if(cls.demoplayback) // TODO: demoplayback -> clientstate?
		CL_StopPlayback();
	
	// send a disconnect message to the server
	byte sFinalMsg[32];
	sFinalMsg[0] = clc_stringcmd;
	
	strcpy((char *)sFinalMsg + 1, "disconnect");
	
	Netchan_Transmit(&cls.netchan, strlen(sFinalMsg), sFinalMsg);
	Netchan_Transmit(&cls.netchan, strlen(sFinalMsg), sFinalMsg);
	Netchan_Transmit(&cls.netchan, strlen(sFinalMsg), sFinalMsg);

	//mvStates[state]->OnDisconnect();

	cls.demoplayback = cls.timedemo = false;
	cls.signon = 0;

	//Cam_Reset();
	
	CL_ClearState();
	
	// stop download
	//if(cls.download)
	//{
		//fclose(cls.download);
		//cls.download = NULL;
	//};

	//CL_StopUpload();
	
	cls.state = ca_disconnected;
#endif // SWDS
};

void CL_Disconnect_f()
{
#ifndef SWDS
	CL_Disconnect(); // Host_Error(ERR_DROP, "Disconnected from server");
#endif
};

/*
====================
CL_User_f

user <name or userid>

Dump userdata / masterdata for a user
====================
*/
void CL_User_f()
{
	if(Cmd_Argc() != 2)
	{
		Con_Printf("Usage: user <username / userid>\n");
		return;
	};

	int uid = atoi(Cmd_Argv(1));

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(!cl.players[i].name[0])
			continue;

		if(cl.players[i].userid == uid ||
		   !strcmp(cl.players[i].name, Cmd_Argv(1)))
		{
			Info_Print(cl.players[i].userinfo);
			return;
		};
	};

	Con_Printf("User not on server\n");
};

/*
====================
CL_Users_f

Dump userids for all current players
====================
*/
void CL_Users_f()
{
	int c = 0;

	Con_Printf("userid frags name\n");
	Con_Printf("------ ----- ----\n");

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(cl.players[i].name[0])
		{
			//Con_Printf("%6i %4i %s\n", cl.players[i].userid, cl.players[i].frags, cl.players[i].name);
			c++;
		};
	};

	Con_Printf("%i total users\n", c);
};

void CL_Color_f()
{
	int top, bottom;
	char num[16];

	if(Cmd_Argc() == 1)
	{
		Con_Printf("\"color\" is \"%s %s\"\n",
		           Info_ValueForKey(cls.userinfo, "topcolor"),
		           Info_ValueForKey(cls.userinfo, "bottomcolor"));
		Con_Printf("color <0-13> [0-13]\n");
		return;
	};

	if(Cmd_Argc() == 2)
		top = bottom = atoi(Cmd_Argv(1));
	else
	{
		top = atoi(Cmd_Argv(1));
		bottom = atoi(Cmd_Argv(2));
	};

	top &= 15;
	if(top > 13)
		top = 13;
	bottom &= 15;
	if(bottom > 13)
		bottom = 13;

	sprintf(num, "%i", top);
	Cvar_Set("topcolor", num);
	sprintf(num, "%i", bottom);
	Cvar_Set("bottomcolor", num);
};

/*
==================
CL_FullServerinfo_f

Sent by server when serverinfo changes
==================
*/
void CL_FullServerinfo_f()
{
	char *p;
	float v;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("usage: fullserverinfo <complete info string>\n");
		return;
	};

	strcpy(cl.serverinfo, Cmd_Argv(1));

	if((p = Info_ValueForKey(cl.serverinfo, "*version")) && *p)
	{
		v = Q_atof(p);
		if(v)
		{
			if(!server_version)
				Con_Printf("Version %1.2f Server\n", v);
			server_version = v;
		};
	};
};

/*
==================
CL_FullInfo_f

Allow clients to change userinfo
==================
*/
void CL_FullInfo_f()
{
	char key[512];
	char value[512];
	char *o;
	char *s;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("fullinfo <complete info string>\n");
		return;
	};

	s = Cmd_Argv(1);
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
		};

		o = value;
		s++;
		while(*s && *s != '\\')
			*o++ = *s++;
		*o = 0;

		if(*s)
			s++;

		if(!stricmp(key, pmodel_name) || !stricmp(key, emodel_name))
			continue;

		Info_SetValueForKey(cls.userinfo, key, value, MAX_INFO_STRING);
	};
};

/*
==================
CL_SetInfo_f

Allow clients to change userinfo
==================
*/
void CL_SetInfo_f()
{
	if(Cmd_Argc() == 1)
	{
		Info_Print(cls.userinfo);
		return;
	};

	if(Cmd_Argc() != 3)
	{
		Con_Printf("usage: setinfo [ <key> <value> ]\n");
		return;
	};

	if(!stricmp(Cmd_Argv(1), pmodel_name) || !strcmp(Cmd_Argv(1), emodel_name))
		return;

	Info_SetValueForKey(cls.userinfo, Cmd_Argv(1), Cmd_Argv(2), MAX_INFO_STRING);
	if(cls.state >= ca_connected)
		Cmd_ForwardToServer();
};

/*
====================
CL_Packet_f

packet <destination> <contents>

Contents allows \n escape character
====================
*/
void CL_Packet_f()
{
	char send[2048];
	int i, l;
	char *in, *out;
	netadr_t adr;

	if(Cmd_Argc() != 3)
	{
		Con_Printf("packet <destination> <contents>\n");
		return;
	};
	
	//NET_Config(true); // allow remote

	if(!NET_StringToAdr(Cmd_Argv(1), &adr))
	{
		Con_Printf("Bad address\n");
		return;
	};
	
	if(!adr.port)
		adr.port = BigShort(PORT_SERVER);

	in = Cmd_Argv(2);
	out = send + 4;
	send[0] = send[1] = send[2] = send[3] = (char)0xff;

	l = strlen(in);
	for(i = 0; i < l; ++i)
	{
		if(in[i] == '\\' && in[i + 1] == 'n')
		{
			*out++ = '\n';
			i++;
		}
		else
			*out++ = in[i];
	};

	*out = 0;

	//NET_SendPacket(/*NS_CLIENT,*/ out - send, send, adr);
};

/*
=====================
CL_NextDemo

Called to play the next demo in the demo loop
=====================
*/
void CL_NextDemo()
{
	char str[1024];

	if(cls.demonum == -1)
		return; // don't play demos

	if(!cls.demos[cls.demonum][0] || cls.demonum == MAX_DEMOS)
	{
		cls.demonum = 0;
		if(!cls.demos[cls.demonum][0])
		{
			// Con_Printf ("No demos listed with startdemos\n");
			cls.demonum = -1;
			return;
		}
	}

	sprintf(str, "playdemo %s\n", cls.demos[cls.demonum]);
	Cbuf_InsertText(str);
	cls.demonum++;
};

/*
=================
CL_Changing_f

Just sent as a hint to the client that they should
drop to full console
=================
*/
void CL_Changing_f()
{
	// don't change when downloading
	// so we don't suddenly stop downloading a map
	//if(cls.download)
		//return;

	S_StopAllSounds(true);
	cl.intermission = 0;

	//SCR_BeginLoadingPlaque();

	// not active anymore, but not disconnected
	cls.state = ca_connected;

	Con_Printf("\nChanging map...\n");
};

/*
=================
CL_Reconnect_f

The server is changing levels
=================
*/
void CL_Reconnect_f()
{
	// If we are downloading, we don't change!
	// So we don't suddenly stop downloading a map
	// Don't change when downloading
	//if(cls.download)
		//return;

	S_StopAllSounds(true);

	if(cls.state == ca_connected)
	{
		Con_Printf("reconnecting...\n");
		MSG_WriteChar(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, "new");
		return;
	};

	if(!*cls.servername)
	{
		Con_Printf("No server to reconnect to...\n");
		return;
	};
	
	/*
	if (*cls.servername)
	{
		if (cls.state >= ca_connected)
		{
			CL_Disconnect();
			cls.connect_time = cls.realtime - 1500;
		}
		else
			cls.connect_time = -99999; // fire immediately

		cls.state = ca_connecting;
		Con_Printf ("reconnecting...\n");
	}
	*/
	
	CL_Disconnect();
	CL_BeginServerConnect();
};

/*
=================
CL_ConnectionlessPacket

Responses to broadcasts, etc
=================
*/
void CL_ConnectionlessPacket()
{
	char *s;

	MSG_BeginReading();
	MSG_ReadLong(); // skip the -1

	int c = MSG_ReadByte();
	
	if(!cls.demoplayback)
		Con_Printf("%s: ", NET_AdrToString(net_from));
	
	//	Con_DPrintf ("%s", net_message.data + 5);
	
	// server connection
	if(c == S2C_CONNECTION)
	{
		Con_Printf("connection\n");
		if(cls.state >= ca_connected)
		{
			if(!cls.demoplayback)
				Con_Printf("Dup connect received.  Ignored.\n");
			return;
		};

		//Netchan_Setup(&cls.netchan, net_from, cls.qport);
		MSG_WriteChar(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, "new");
		cls.state = ca_connected;
		Con_Printf("Connected.\n");
		allowremotecmd = false; // localid required now for remote cmds
		return;
	};

	// remote command from gui front end
	if(c == A2C_CLIENT_COMMAND)
	{
		char cmdtext[2048];

		Con_Printf("client command\n");

		if((*(unsigned *)net_from.ip != *(unsigned *)net_local_adr.ip &&
		    *(unsigned *)net_from.ip != htonl(INADDR_LOOPBACK)))
		{
			Con_Printf("Command packet from remote host.  Ignored.\n");
			return;
		};

#ifdef _WIN32
		//ShowWindow(mainwindow, SW_RESTORE);
		//SetForegroundWindow(mainwindow);
#endif

		s = MSG_ReadString();

		strncpy(cmdtext, s, sizeof(cmdtext) - 1);
		cmdtext[sizeof(cmdtext) - 1] = 0;

		s = MSG_ReadString();

		while(*s && isspace(*s))
			s++;
		while(*s && isspace(s[strlen(s) - 1]))
			s[strlen(s) - 1] = 0;
/*
		if(!allowremotecmd && (!*localid.string || strcmp(localid.string, s)))
		{
			if(!*localid.string)
			{
				Con_Printf("===========================\n");
				Con_Printf("Command packet received from local host, but no "
				           "localid has been set.  You may need to upgrade your server "
				           "browser.\n");
				Con_Printf("===========================\n");
				return;
			}
			Con_Printf("===========================\n");
			Con_Printf("Invalid localid on command packet received from local host. "
			           "\n|%s| != |%s|\n"
			           "You may need to reload your server browser and game.\n",
			           s,
			           localid.string);
			Con_Printf("===========================\n");
			Cvar_Set("localid", "");
			return;
		};
		*/

		Cbuf_AddText(cmdtext);
		allowremotecmd = false;
		return;
	};

	// print command from somewhere
	if(c == A2C_PRINT) // if(!Q_strcmp(c, "print"))
	{
		Con_Printf("print\n");

		s = MSG_ReadString();
		Con_Print(s); //Con_Printf("%s", s);
		return;
	};

	// ping from somewhere
	if(c == A2A_PING) // if(!Q_strcmp(c, "ping"))
	{
		//Netchan_OutOfBandPrint(NS_CLIENT, net_from, "ack");
		
		//
		
		char data[6];

		Con_Printf("ping\n");

		data[0] = 0xff;
		data[1] = 0xff;
		data[2] = 0xff;
		data[3] = 0xff;
		data[4] = A2A_ACK;
		data[5] = 0;

		//NET_SendPacket(6, &data, net_from);
		return;
	};
	
	// challenge from the server we are connecting to
	if(c == S2C_CHALLENGE) // if(!Q_strcmp(c, "challenge"))
	{
		Con_Printf("challenge\n");
		
		//
		
		s = MSG_ReadString();

		cls.challenge = Q_atoi(s);
		
		//
		
		//cls.challenge = atoi(Cmd_Argv(1));

		CL_SendConnectPacket();
		return;
	};

#if 0
	if(c == svc_disconnect)
	{
		Con_Printf("disconnect\n");

		Host_EndGame("Server disconnected");
		return;
	};
#endif

	Con_Printf("unknown:  %c\n", c);
};

/*
=================
CL_ReadPackets
=================
*/
void CL_ReadPackets()
{
#ifndef SWDS
	//	while (NET_GetPacket ())
	while(CL_GetMessage())
	{
		//
		// remote command packet
		//
		if(*(int *)net_message.data == -1)
		{
			CL_ConnectionlessPacket();
			continue;
		};

		if(net_message.cursize < 8)
		{
			Con_Printf("%s: Runt packet\n", NET_AdrToString(net_from));
			continue;
		};

		//
		// packet from server
		//
		if(!cls.demoplayback &&
		   !NET_CompareAdr(net_from, cls.netchan.remote_address))
		{
			Con_DPrintf("%s:sequenced packet without connection\n",
			            NET_AdrToString(net_from));
			continue;
		};
		
		// wasn't accepted for some reason
		if(!Netchan_Process(&cls.netchan))
			continue;

		CL_ParseServerMessage();

		//if (cls.demoplayback && cls.state >= ca_active && !CL_DemoBehind())
		//			return;
	};

	//
	// check timeout
	//
	if(cls.state >= ca_connected &&
	   realtime - cls.netchan.last_received > cl_timeout.value)
	{
		Con_Printf("\nServer connection timed out.\n");
		CL_Disconnect();
		return;
	};
#endif // SWDS
};

//=============================================================================

/*
=====================
CL_Download_f
=====================
*/
void CL_Download_f()
{
	char *q;

	if(cls.state == ca_disconnected)
	{
		Con_Printf("Must be connected.\n");
		return;
	};

	if(Cmd_Argc() != 2)
	{
		Con_Printf("Usage: download <datafile>\n");
		return;
	};

	//sprintf(cls.downloadname, "%s/%s", com_gamedir, Cmd_Argv(1));

	char *p = nullptr; //cls.downloadname;
	for(;;)
	{
		if((q = strchr(p, '/')) != NULL)
		{
			*q = 0;
			//Sys_mkdir(cls.downloadname);
			*q = '/';
			p = q + 1;
		}
		else
			break;
	};

	//strcpy(cls.downloadtempname, cls.downloadname);
	//cls.download = fopen(cls.downloadname, "wb");
	//cls.downloadtype = dl_single;

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	SZ_Print(&cls.netchan.message, va("download %s\n", Cmd_Argv(1)));
};

/*
=================
CL_Init
=================
*/
void CL_Init()
{
#ifndef SWDS
	// nothing running on the client
	//if(dedicated->value) // cl.state == ca_dedicated
		//return;
	
	extern cvar_t baseskin;
	extern cvar_t noskins;

	char st[80];

	//sprintf(st, "%4.2f-%04d", VERSION, build_number());
	Info_SetValueForStarKey(cls.userinfo, "*ver", st, MAX_INFO_STRING);

	//
	// register our commands
	//
	Cvar_RegisterVariable(&show_fps);

	//Cvar_RegisterVariable(&cl_warncmd);
	Cvar_RegisterVariable(&cl_sbar);
	Cvar_RegisterVariable(&cl_hudswap);
	//Cvar_RegisterVariable(&cl_maxfps);
	Cvar_RegisterVariable(&cl_timeout);

	Cvar_RegisterVariable(&rcon_password);
	Cvar_RegisterVariable(&rcon_address);

	Cvar_RegisterVariable(&entlatency);
	Cvar_RegisterVariable(&cl_predict_players2);
	Cvar_RegisterVariable(&cl_predict_players);
	Cvar_RegisterVariable(&cl_solid_players);

	//Cvar_RegisterVariable(&localid);

	Cvar_RegisterVariable(&baseskin);
	Cvar_RegisterVariable(&noskins);

	//
	// info mirrors
	//
	Cvar_RegisterVariable(&name);
	Cvar_RegisterVariable(&password);
	//Cvar_RegisterVariable(&spectator);
	Cvar_RegisterVariable(&skin);
	Cvar_RegisterVariable(&team);
	Cvar_RegisterVariable(&topcolor);
	Cvar_RegisterVariable(&bottomcolor);
	Cvar_RegisterVariable(&rate);
	//Cvar_RegisterVariable(&msg);
	//Cvar_RegisterVariable(&noaim);

	//Cmd_AddCommand("cmd", CL_ForwardToServer_f);
	//Cmd_AddCommand("pause", CL_Pause_f);
	//Cmd_AddCommand("pingservers", CL_PingServers_f);

	Cmd_AddCommand("changing", CL_Changing_f);
	//Cmd_AddCommand("rerecord", CL_ReRecord_f);

	//Cmd_AddCommand("skins", Skin_Skins_f);
	//Cmd_AddCommand("allskins", Skin_AllSkins_f);

	Cmd_AddCommand("connect", CL_Connect_f);
	Cmd_AddCommand("reconnect", CL_Reconnect_f);

	Cmd_AddCommand("rcon", CL_Rcon_f);
	// Cmd_AddCommand("packet", CL_Packet_f); // this is dangerous to leave in
	Cmd_AddCommand("user", CL_User_f);
	Cmd_AddCommand("users", CL_Users_f);

	Cmd_AddCommand("setinfo", CL_SetInfo_f);
	Cmd_AddCommand("fullinfo", CL_FullInfo_f);
	Cmd_AddCommand("fullserverinfo", CL_FullServerinfo_f);

	//
	// forward to server commands
	//
	// the only thing this does is allow command completion
	// to work -- all unknown commands are automatically
	// forwarded to the server
	//

	Cmd_AddCommand("dropclient", NULL);
	Cmd_AddCommand("kill", NULL);
	Cmd_AddCommand("pause", NULL);

	Cmd_AddCommand("say", NULL);
	Cmd_AddCommand("say_team", NULL);

	Cmd_AddCommand("serverinfo", NULL);
	Cmd_AddCommand("god", NULL);
	Cmd_AddCommand("notarget", NULL);
	Cmd_AddCommand("noclip", NULL);

	Cmd_AddCommand("invprev", NULL);
	Cmd_AddCommand("invnext", NULL);
#endif // SWDS
};

void CL_InitClosest()
{
#ifndef SWDS
#endif
};

void CL_Shutdown()
{
#ifndef SWDS
#endif
};

void CL_ShutDownUsrMessages()
{
#ifndef SWDS
#endif
};

void CL_ShutDownClientStatic()
{
#ifndef SWDS
	// wipe the entire client static structure
	memset(&cls, 0, sizeof(cls));
#endif
};

void CL_AdjustClock()
{
#ifndef SWDS
#endif
};

void CL_KeepConnectionActive()
{
#ifndef SWDS
#endif
};

void CL_CheckClientState()
{
#ifndef SWDS
#endif
};

void CL_ClearClientState()
{
#ifndef SWDS
#endif
};