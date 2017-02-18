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
/// @brief parse a message received from the server

#include "precompiled.hpp"
#include "client/client.hpp"
#include "system/system.hpp"
#include "system/host.hpp"
#include "network/net_msg.hpp"
#include "console/console.hpp"
#include "console/cvar.hpp"
#include "sound/cdaudio.hpp"

const char *svc_strings[] =
{
	"svc_bad",
	"svc_nop",
	"svc_disconnect",
	"svc_event",
	"svc_version",   // [long] server version
	"svc_setview",   // [short] entity number
	"svc_sound",     // <see code>
	"svc_time",      // [float] server time
	"svc_print",     // [string] null terminated string
	"svc_stufftext", // [string] stuffed into client's console buffer
	                 // the string should be \n terminated
	"svc_setangle",  // [vec3] set the view angle to this absolute value

	"svc_serverinfo",       // [long] version ...
	"svc_lightstyle",       // [byte] [string]
	"svc_updateuserinfo",   // [byte] [string]
	"svc_deltadescription", // [byte] [short]
	"svc_clientdata",       // <shortbits + data>
	"svc_stopsound",        // <see code>
	"svc_pings",            // [byte] [byte]
	"svc_particle",         // [vec3] <variable>
	"svc_damage",           // deprecated

	"svc_spawnstatic",
	"svc_event_reliable",
	"svc_spawnbaseline",

	"svc_temp_entity", // <variable>
	"svc_setpause",
	"svc_signonnum",
	"svc_centerprint",
	"svc_killedmonster",
	"svc_foundsecret",
	"svc_spawnstaticsound",
	"svc_intermission",
	"svc_finale",

	"svc_cdtrack",
	"svc_restore",

	"svc_cutscene",
	"svc_weaponanim",

	"svc_decalname",
	"svc_roomtype",

	"svc_addangle",
	"svc_newusermsg",
	"svc_packetentities",
	"svc_deltapacketentities",
	"svc_choke",
	"svc_resourcelist",
	"svc_newmovevars",
	"svc_resourcerequest",
	"svc_customization",
	"svc_crosshairangle",
	"svc_soundfade",
	"svc_filetxferfailed",
	"svc_hltv",
	"svc_director",

	"svc_voiceinit",
	"svc_voicedata",

	"svc_sendextrainfo",
	"svc_timescale",
	"svc_resourcelocation",

	"svc_sendcvarvalue",
	"svc_sendcvarvalue2",
};

void CL_HandleDisconnect()
{
	char *sReason = MSG_ReadString();

	if(cls.state == ca_connected)
		Host_EndGame("Server disconnected\n"
		             "Server version may be incompatible");
	else
		Host_EndGame("Server disconnected");
};

void CL_ParseEventData(bool bReliable)
{
	/*
  Note: This message can be dropped if the client already has too much content
  in its unreliable buffer.
  Note: Events can be precached using pfnPrecacheEvent routine.
  Note: Events are queued and grouped together every frame, if there's any.
  Note: EventArgs are always inherited from "null" event args.
  Note: Only a max of 31 events can be queued and subsequently sent this way.
  Note: This message has its arguments in bit-packed form.
*/

	if(bReliable)
	{
	};
};

void CL_ParseVersion()
{
	// Seems to be unused

	// long nServerProtocol = MSG_ReadLong();
	// if(PROTOCOL_VERSION != nServerProtocol)
	// Host_Error("CL_Parse_Version: Server is protocol %i instead of %i\n",
	// nServerProtocol, PROTOCOL_VERSION);
};

void CL_ParseView()
{
	cl.refdef.viewentity = MSG_ReadWord();
};

/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket()
{
	vec3_t pos;

	int flags = MSG_ReadWord();

	int volume = DEFAULT_SOUND_PACKET_VOLUME; // VOL_NORM

	if(flags & SND_FL_VOLUME)
		volume = MSG_ReadByte();

	float attenuation = DEFAULT_SOUND_PACKET_ATTENUATION; // ATTN_NONE

	if(flags & SND_FL_ATTENUATION)
		attenuation = MSG_ReadByte() / 64.0f;

	int channel = MSG_ReadShort();
	int ent = MSG_ReadByte();
	int sound_num = MSG_ReadByte();

	for(int i = 0; i < 3; i++)
		pos[i] = MSG_ReadCoord();

	int pitch = DEFAULT_SOUND_PACKET_PITCH; // PITCH_NORM

	if(flags & SND_FL_PITCH)
		pitch = MSG_ReadByte();

	ent = (channel >> 3) & 1023;
	channel &= 7;

	if(ent > MAX_EDICTS)
		Host_EndGame("CL_ParseStartSoundPacket: ent = %i", ent);

	S_StartSound(ent, channel, cl.sound_precache[sound_num], pos, volume / 255.0, attenuation);
};

void CL_ParseTime()
{
/*
  Notifies clients about the current server time

  Note: This message is sent every frame by the server
*/

	// shuffle timestamps
	cl.mtime[1] = cl.mtime[0];
	cl.mtime[0] = MSG_ReadFloat();
};

void CL_ParsePrint()
{
	int i = MSG_ReadByte();

	if(i == PRINT_CHAT)
	{
		S_LocalSound("misc/talk.wav");
		con_ormask = 128;
	};

	Con_Printf("%s", MSG_ReadString());
	con_ormask = 0;
};

void CL_ParseStuffText()
{
	char *s = MSG_ReadString();
	Con_DPrintf("stufftext: %s\n", s);
	Cbuf_AddText(s);
	
	//if(cl_trace_stufftext->value)
		//Msg("^3STUFFTEXT:\n^2%s\n^3END^7\n", s);
};

/*
================
CL_ParseSetAngle

set the view angle to this absolute value
================
*/
void CL_ParseSetAngle()
{
	for(int i = 0; i < 3; ++i)
		cl.viewangles[i] = MSG_ReadAngle();

	// cl.viewangles[PITCH] = cl.viewangles[ROLL] = 0;

	/*
  cl.refdef.cl_viewangles[0] = MSG_ReadBitAngle(16 );
  cl.refdef.cl_viewangles[1] = MSG_ReadBitAngle(16 );
  cl.refdef.cl_viewangles[2] = MSG_ReadBitAngle(16 );
  */
};

/*
==============
CL_ServerInfo
==============
*/
void CL_ParseServerInfo()
{
	int slot;
	player_info_t *player;
	char key[MAX_MSGLEN];
	char value[MAX_MSGLEN];

	Q_strncpy(key, MSG_ReadString(), charsmax(key));
	key[sizeof(key) - 1] = 0;
	Q_strncpy(value, MSG_ReadString(), charsmax(value));
	key[sizeof(value) - 1] = 0;

	Con_DPrintf("SERVERINFO: %s=%s\n", key, value);

	Info_SetValueForKey(cl.serverinfo, key, value, MAX_SERVERINFO_STRING);
};

/*
================
CL_ParseLightStyle
================
*/
void CL_ParseLightStyle()
{
	int nStyle = MSG_ReadByte();
	
	if(nStyle >= MAX_LIGHTSTYLES)
		Sys_Error("svc_lightstyle > MAX_LIGHTSTYLES");
	
	Q_strcpy(cl_lightstyle[nStyle].map, MSG_ReadString());
	cl_lightstyle[nStyle].length = Q_strlen(cl_lightstyle[nStyle].map);
	
	/*
	int style = MSG_ReadByte();
	const char *s = MSG_ReadString();
	float f = MSG_ReadFloat();

	CL_SetLightstyle( style, s, f );
	*/
};

/*
================
CL_UpdateUserinfo

collect userinfo from all players
================
*/
void CL_ParseUserinfo()
{
	int slot = MSG_ReadUBitLong(MAX_CLIENT_BITS);

	if(slot >= MAX_CLIENTS)
		Host_Error("CL_ParseServerMessage: svc_updateuserinfo > MAX_CLIENTS\n");

	player_info_t *player = &cl.players[slot];
	qboolean active = MSG_ReadOneBit() ? true : false;

	if(active)
	{
		Q_strncpy(player->userinfo, MSG_ReadString(), sizeof(player->userinfo));
		Q_strncpy(player->name, Info_ValueForKey(player->userinfo, "name"), sizeof(player->name));
		Q_strncpy(player->model, Info_ValueForKey(player->userinfo, "model"), sizeof(player->model));
		player->topcolor = Q_atoi(Info_ValueForKey(player->userinfo, "topcolor"));
		player->bottomcolor =
		Q_atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));

		if(slot == cl.playernum)
			Q_memcpy(&menu.playerinfo, player, sizeof(player_info_t));
	}
	else
		Q_memset(player, 0, sizeof(*player));
};

void CL_ParseDeltaDescription()
{
};

/*
==================
CL_ParseClientdata

Server information pertaining to this client only, sent every frame
==================
*/
void CL_ParseClientdata()
{
	int i;
	float latency;
	frame_t *frame;

	// calculate simulated time of message
	oldparsecountmod = parsecountmod;

	i = cls.netchan.incoming_acknowledged;
	cl.parsecount = i;
	i &= UPDATE_MASK;
	parsecountmod = i;
	frame = &cl.frames[i];
	parsecounttime = cl.frames[i].senttime;

	frame->receivedtime = realtime;

	// calculate latency
	latency = frame->receivedtime - frame->senttime;

	if(latency < 0 || latency > 1.0f)
	{
		//		Con_Printf ("Odd latency: %5.2f\n", latency);
	}
	else
	{
		// drift the average latency towards the observed latency
		if(latency < cls.latency)
			cls.latency = latency;
		else
			cls.latency += 0.001f; // drift up, so correction are needed
	};
};

void SHOWNET(char *s)
{
	if(cl_shownet.value == 2) // >=
		Con_Printf("%3i:%s\n", msg_readcount - 1, s); // net_message.readcount
};

/*
=====================
CL_ParseServerMessage
=====================
*/
int received_framecount;
void CL_ParseServerMessage()
{
	int cmd;
	char *s;
	int i, j;

	received_framecount = host_framecount;
	cl.last_servermessage = realtime;
	CL_ClearProjectiles();

	//
	// if recording demos, copy the message out
	//
	if(cl_shownet.value == 1)
		Con_Printf("%i ", net_message.cursize);
	else if(cl_shownet.value == 2) // >=
		Con_Printf("------------------\n");

	CL_ParseClientdata();

	//
	// parse the message
	//
	while(1)
	{
		if(msg_badread)
		{
			Host_EndGame("CL_ParseServerMessage: Bad server message");
			break;
		}

		cmd = MSG_ReadByte();

		if(cmd == -1)
		{
			msg_readcount++; // so the EOM showner has the right value
			SHOWNET("END OF MESSAGE");
			break;
		}

		SHOWNET(svc_strings[cmd]);

		// other commands
		switch(cmd)
		{
		// GS protocol entries
		case svc_bad:
			// Host_Error( "svc_bad\n" );
			break;
		case svc_nop:
			// this does nothing
			// Con_Printf("svc_nop\n");
			break;
		case svc_disconnect:
			CL_HandleDisconnect();
			break;
		case svc_event:
			CL_ParseEventData(false);
			break;
		case svc_version:
			CL_ParseVersion();
			break;
		case svc_setview:
			CL_ParseView();
			break;
		case svc_sound:
			CL_ParseStartSoundPacket();
			break;
		case svc_time:
			CL_ParseTime();
			break;
		case svc_print:
			CL_ParsePrint();
			break;
		case svc_stufftext:
			CL_ParseStuffText();
			break;
		case svc_setangle:
			CL_ParseSetAngle();
			break;
		case svc_serverinfo:
			CL_ParseServerInfo();

			// from svc_serverdata
			// Cbuf_Execute ();		// make sure any stuffed commands are
			// done
			// CL_ParseServerData ();
			// vid.recalc_refdef = true;	// leave full screen intermission
			break;
		case svc_lightstyle:
			CL_ParseLightStyle();
			break;
		case svc_updateuserinfo:
			CL_ParseUserinfo();
			break;
		case svc_deltadescription:
			CL_ParseDeltaDescription();
			break;
		case svc_clientdata:
			CL_ParseClientData();
			break;
		case svc_stopsound:
			CL_HandleStopSound();
			break;
		case svc_pings:
			CL_ParsePings();
			break;
		case svc_particle:
			CL_ParseParticle();
			break;
		case svc_damage:
			V_ParseDamage();
			break;
		case svc_spawnstatic:
			CL_ParseStaticEnt();
			break;
		case svc_event_reliable:
			CL_ParseEventData(true);
			break;
		case svc_spawnbaseline:
			i = MSG_ReadShort();
			CL_ParseBaseline(&cl_baselines[i]);
			break;
		case svc_temp_entity:
			CL_ParseTempEntity();
			break;
		case svc_setpause:
			CL_HandlePause();
			break;
		case svc_signonnum:
			CL_ParseSignOnNum();
			break;
		case svc_centerprint:
			CL_ParseCenterPrint();
			break;
		case svc_killedmonster:
			CL_ParseKilledMonster();
			break;
		case svc_foundsecret:
			CL_ParseFoundSecret();
			break;
		case svc_spawnstaticsound:
			CL_ParseStaticSound();
			break;
		case svc_intermission:
			CL_ParseIntermission();
			break;
		case svc_finale:
			CL_ParseFinale();
			break;
		case svc_cdtrack:
			CL_ParseCDTrack();
			break;
		case svc_restore:
			CL_ParseRestore();
			break;
		case svc_cutscene:
			CL_ParseCutscene();
			break;
		case svc_weaponanim:
			CL_ParseWeaponAnim();
			break;
		case svc_decalname:
			CL_ParseDecalName();
			break;
		case svc_roomtype:
			CL_ParseRoomType();
			break;
		case svc_addangle:
			CL_ParseAddAngle();
			break;
		case svc_newusermsg:
			CL_ParseNewUserMsg();
			break;
		case svc_packetentities:
			CL_ParsePacketEntities(false);
			break;
		case svc_deltapacketentities:
			CL_ParsePacketEntities(true);
			break;
		case svc_choke: // some preceding packets were choked
			CL_HandleChoke();
			break;
		case svc_resourcelist:
			CL_ParseResourceList();
			break;
		case svc_newmovevars:
			CL_ParseNewMoveVars();
			break;
		case svc_resourcerequest:
			CL_ParseResourceRequest();
			break;
		case svc_customization:
			CL_ParseCustomization();
			break;
		case svc_crosshairangle:
			CL_ParseCrosshairAngle();
			break;
		case svc_soundfade:
			CL_ParseSoundFade();
			break;
		case svc_filetxferfailed:
			CL_HandleFileTransferFailed();
			break;
		case svc_hltv:
			CL_ParseHLTVMode();
			break;
		case svc_director:
			CL_ParseDirectorMsg();
			break;
		case svc_voiceinit:
			CL_HandleVoiceInit();
			break;
		case svc_voicedata:
			CL_ParseVoiceData();
			break;
		case svc_sendextrainfo:
			CL_ParseExtraInfo();
			break;
		case svc_timescale:
			CL_ParseTimeScale();
			break;
		case svc_resourcelocation:
			CL_ParseResouceLocation();
			break;
		case svc_sendcvarvalue:
			CL_ParseCvarValue();
			break;
		case svc_sendcvarvalue2:
			CL_ParseCvarValue2();
			break;
		default:
			Host_EndGame("CL_ParseServerMessage: Illegible server message");
			break;
			// Old QW protocol
			/*
case svc_updatefrags:
      Sbar_Changed ();
      i = MSG_ReadByte ();
      if (i >= MAX_CLIENTS)
              Host_EndGame ("CL_ParseServerMessage: svc_updatefrags >
MAX_SCOREBOARD");
      cl.players[i].frags = MSG_ReadShort ();
      break;

case svc_updatepl:
      i = MSG_ReadByte ();
      if (i >= MAX_CLIENTS)
              Host_EndGame ("CL_ParseServerMessage: svc_updatepl >
MAX_SCOREBOARD");
      cl.players[i].pl = MSG_ReadByte ();
      break;

case svc_updateentertime:
// time is sent over as seconds ago
      i = MSG_ReadByte ();
      if (i >= MAX_CLIENTS)
              Host_EndGame ("CL_ParseServerMessage: svc_updateentertime >
MAX_SCOREBOARD");
      cl.players[i].entertime = realtime - MSG_ReadFloat ();
      break;

case svc_setinfo:
      CL_SetInfo ();
      break;

case svc_download:
      CL_ParseDownload ();
      break;

case svc_playerinfo:
      CL_ParsePlayerinfo ();
      break;

case svc_nails:
      CL_ParseProjectiles ();
      break;
*/
		}
	}

	CL_SetSolidEntities();
};