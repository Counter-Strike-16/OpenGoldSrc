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
//#include "commondef.hpp"
#include "system/client.hpp"
#include "system/system.hpp"
#include "system/host.hpp"
#include "network/net_msg.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "sound/sound.hpp"
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

	"svc_serverinfo",       // [long] version
							// [string] signon string
							// [string]..[0]model cache [string]...[0]sounds cache
							// [string]..[0]item cache
	"svc_lightstyle",       // [byte] [string]
	"svc_updateuserinfo",   // [byte] [string]
	"svc_deltadescription", // [byte] [short]
	"svc_clientdata",       // <shortbits + data>
	"svc_stopsound",        // <see code>
	"svc_pings",            // [byte] [byte]
	"svc_particle",         // [vec3] <variable>
	"svc_damage",           // deprecated; [byte] impact [byte] blood [vec3] from

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
	"svc_finale",		// [string] music [string] text

	"svc_cdtrack",		// [byte] track [byte] looptrack
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

//=============================================================================

/*
=====================================================================

ACTION MESSAGES

=====================================================================
*/

void CL_HandleDisconnect()
{
	char *sReason = MSG_ReadString();

	if(cls.state == ca_connected)
		Host_EndGame("Server disconnected\n"
		             "Server version may be incompatible");
	else
		Host_EndGame("Server disconnected");
};

/*
	Note: This message can be dropped if the client already has too much content
	in its unreliable buffer.
	Note: Events can be precached using pfnPrecacheEvent routine.
	Note: Events are queued and grouped together every frame, if there's any.
	Note: EventArgs are always inherited from "null" event args.
	Note: Only a max of 31 events can be queued and subsequently sent this way.
	Note: This message has its arguments in bit-packed form.
*/
void CL_ParseEventData(bool bReliable)
{
	if(bReliable)
	{
	};
};

// Seems to be unused
void CL_ParseVersion()
{
	// long nServerProtocol = MSG_ReadLong();
	// if(PROTOCOL_VERSION != nServerProtocol)
	// Host_Error("CL_Parse_Version: Server is protocol %i instead of %i\n",
	// nServerProtocol, PROTOCOL_VERSION);
};

void CL_ParseView()
{
	//cl.refdef.viewentity = MSG_ReadWord();
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

	//if(flags & SND_FL_VOLUME)
		//volume = MSG_ReadByte();

	float attenuation = DEFAULT_SOUND_PACKET_ATTENUATION; // ATTN_NONE

	//if(flags & SND_FL_ATTENUATION)
		//attenuation = MSG_ReadByte() / 64.0f;

	int channel = MSG_ReadShort();
	int ent = MSG_ReadByte();
	int sound_num = MSG_ReadByte();

	for(int i = 0; i < 3; i++)
		pos[i] = MSG_ReadCoord();

	int pitch = DEFAULT_SOUND_PACKET_PITCH; // PITCH_NORM

	//if(flags & SND_FL_PITCH)
		//pitch = MSG_ReadByte();

	ent = (channel >> 3) & 1023;
	channel &= 7;

	if(ent > MAX_EDICTS)
		Host_EndGame("CL_ParseStartSoundPacket: ent = %i", ent);

	//S_StartSound(ent, channel, cl.sound_precache[sound_num], pos, volume / 255.0, attenuation);
};

/*
	Notifies clients about the current server time
	Note: This message is sent every frame by the server
*/
void CL_ParseTime()
{
	// shuffle timestamps
	cl.mtime[1] = cl.mtime[0];
	cl.mtime[0] = MSG_ReadFloat();
};

void CL_ParsePrint()
{
	int i = MSG_ReadByte();

	//if(i == PRINT_CHAT)
	//{
		//S_LocalSound("misc/talk.wav");
		//con_ormask = 128;
	//};

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

	//Info_SetValueForKey(cl.serverinfo, key, value, MAX_SERVERINFO_STRING);
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
	
	//Q_strcpy(cl_lightstyle[nStyle].map, MSG_ReadString());
	//cl_lightstyle[nStyle].length = Q_strlen(cl_lightstyle[nStyle].map);
	
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
	int slot = 0; //MSG_ReadUBitLong(MAX_CLIENT_BITS);

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

		//if(slot == cl.playernum)
			//Q_memcpy(&menu.playerinfo, player, sizeof(player_info_t));
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
	//oldparsecountmod = parsecountmod;

	i = cls.netchan.incoming_acknowledged;
	cl.parsecount = i;
	//i &= UPDATE_MASK;
	//parsecountmod = i;
	frame = &cl.frames[i];
	//parsecounttime = cl.frames[i].senttime;

	frame->receivedtime = realtime;

	// calculate latency
	//latency = frame->receivedtime - frame->senttime;

	if(latency < 0 || latency > 1.0f)
	{
		//		Con_Printf ("Odd latency: %5.2f\n", latency);
	}
	else
	{
		// drift the average latency towards the observed latency
		//if(latency < cls.latency)
			//cls.latency = latency;
		//else
			//cls.latency += 0.001f; // drift up, so correction are needed
	};
};

void CL_HandleStopSound()
{
	short i = MSG_ReadShort();
	//S_StopSound(i >> 3, i & 7);
};

/*
================
CL_UpdateUserPings

collect pings and packet lossage from clients
================
*/
void CL_ParsePings()
{
	int i = MSG_ReadByte();

	if(i >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_updateping > MAX_SCOREBOARD");

	cl.players[i].ping = MSG_ReadShort();
};

void CL_ParseParticle()
{
};

/*
=====================
CL_ParseStaticEnt

Static entities are non-interactive world objects like torches
=====================
*/
void CL_ParseStaticEnt()
{
	cl_entity_t *ent;
	int i;
	entity_state_t es; // state
	
	Q_memset(&es, 0, sizeof(es));

	//CL_ParseBaseline(&es);

	i = cl.num_statics;

	//if(i >= MAX_STATIC_ENTITIES)
		//Host_EndGame("Too many static entities");
	
	/*
	if(i >= MAX_STATIC_ENTITIES)
	{
		MsgDev(D_ERROR, "CL_ParseStaticEntity: static entities limit exceeded!\n");
		return;
	};
	*/

	//ent = &cl_static_entities[i];
	cl.num_statics++;
	
	/*
	ent->index = 0; // ???
	ent->baseline = state;
	ent->curstate = state;
	ent->prevstate = state;

	// statics may be respawned in game e.g. for demo recording
	if( cls.state == ca_connected )
		ent->trivial_accept = INVALID_HANDLE;

	// setup the new static entity
	CL_UpdateEntityFields( ent );

	if( Mod_GetType( state.modelindex ) == mod_studio )
	{
		CL_UpdateStudioVars( ent, &state, true );

		// animate studio model
		ent->curstate.animtime = cl.time;
		ent->curstate.framerate = 1.0f;
		ent->latched.prevframe = 0.0f;
	}
	*/

	// copy it to the current state
	//ent->model = cl.model_precache[es.modelindex];
	//ent->frame = es.frame;
	//ent->colormap = vid.colormap;
	//ent->skinnum = es.skin;

	VectorCopy(es.origin, ent->origin);
	VectorCopy(es.angles, ent->angles);

	//R_AddEfrags(ent); // add link
};

/*
==================
CL_ParseBaseline
==================
*/
void CL_ParseBaseline(entity_state_t *es)
{
	es->modelindex = MSG_ReadByte(); // MSG_ReadShort();
	//state.sequence = MSG_ReadByte(); // ?
	es->frame = MSG_ReadByte();
	es->colormap = MSG_ReadByte(); // MSG_ReadWord();
	es->skin = MSG_ReadByte();

	for(int i = 0; i < 3; ++i)
	{
		es->origin[i] = MSG_ReadCoord();
		es->angles[i] = MSG_ReadAngle(); // MSG_ReadBitAngle(16);
	};
	
	es->rendermode = MSG_ReadByte();

	if(es->rendermode != kRenderNormal)
	{
		es->renderamt = MSG_ReadByte();
		
		es->rendercolor.r = MSG_ReadByte();
		es->rendercolor.g = MSG_ReadByte();
		es->rendercolor.b = MSG_ReadByte();
		
		es->renderfx = MSG_ReadByte();
	};
};

void CL_HandlePause()
{
	//cl.refdef.paused = (MSG_ReadOneBit() != 0);
	//Cvar_SetValue ("paused", !cl_paused->value);
	
	//cl.paused = MSG_ReadByte();
	//if(cl.paused)
		//CDAudio_Pause();
	//else
		//CDAudio_Resume();
};

void CL_ParseSignOnNum()
{
	// Called just after client_putinserver
	// Signals the client that the server has marked it as "active"

	int nSignOn = MSG_ReadByte();
};

void CL_ParseCenterPrint()
{
	//SCR_CenterPrint(MSG_ReadString());
};

void CL_ParseKilledMonster()
{
	// Deprecated

	//cl.stats[STAT_MONSTERS]++;
};

void CL_ParseFoundSecret()
{
	// Deprecated

	//cl.stats[STAT_SECRETS]++;
};

/*
===================
CL_ParseStaticSound
===================
*/
void CL_ParseStaticSound()
{
	vec3_t org;

	for(int i = 0; i < 3; ++i)
		org[i] = MSG_ReadCoord();

	int sound_num = MSG_ReadByte();
	int vol = MSG_ReadByte();
	int atten = MSG_ReadByte();

	//S_StaticSound(cl.sound_precache[sound_num], org, vol, atten);
};

void CL_ParseIntermission()
{
	cl.intermission = 1;
	//cl.completed_time = realtime;
	//vid.recalc_refdef = true; // go to full screen

	for(int i = 0; i < 3; ++i)
		cl.simorg[i] = MSG_ReadCoord();

	for(int i = 0; i < 3; ++i)
		cl.simangles[i] = MSG_ReadAngle();

	VectorCopy(vec3_origin, cl.simvel);
};

void CL_ParseFinale()
{
	cl.intermission = 2;
	//cl.completed_time = realtime;
	//vid.recalc_refdef = true; // go to full screen
	//SCR_CenterPrint(MSG_ReadString());
};

void CL_ParseCDTrack()
{
	cl.cdtrack = MSG_ReadByte();
	cl.looptrack = MSG_ReadByte();

	CDAudio_Play((byte)cl.cdtrack, true);
};

void CL_ParseRestore()
{
	
};

/*
  Shows the intermission camera view, and writes-out text passed in first
  parameter.

  Note: Intermission mode 3
  Note: This text will keep showing on clients in future intermissions
*/
void CL_ParseCutscene()
{
	char *sText = MSG_ReadString();
};

/*
  Sended only if local client weapon simulation (prediction) is disabled
*/
void CL_ParseWeaponAnim()
{
	int nSequence = MSG_ReadByte(); // sequence number
	int nBodygroup = MSG_ReadByte(); // weapon model bodygroup (i.e. for different hands models)
	//CL_WeaponAnim(param1, param2);
};

/*
  Allows to set, into the client's decals array and at specific position index
  (0->511), a decal name.
  E.g: let's say you send a message to set a decal "{break" at index 200.
  As result, when a message TE_ will be used to show a decal at index 200, we
  will see "{break".

  Note: If there is already an existing decal at the provided index, it will be
  overwritten.
  Note: It appears we can play only with decals from decals.wad.
*/
void CL_ParseDecalName()
{
	int nPosIndex = MSG_ReadByte();

	char *sDecalName = MSG_ReadString();
};

void CL_ParseRoomType()
{
	short nRoomType = MSG_ReadShort();
	//Cvar_SetFloat("room_type", nRoomType);
};

/*
================
CL_ParseAddAngle

add the view angle yaw
================
*/
void CL_ParseAddAngle()
{
/*
  Note: When pev->fixangle is set to 2, this message is called with
  pev->avelocity[1] as a value.
  Note: The value needs to be scaled by (65536 / 360).
*/
	//float add_angle = MSG_ReadBitAngle(16 );
	//cl.refdef.cl_viewangles[1] += add_angle;
	
	short nAngle = MSG_ReadShort();
};

/*
================
CL_RegisterUserMessage

register new user message or update existing
================
*/
void CL_ParseNewUserMsg()
{
/*
  Note: Sent every time a new message is registered on the server, but most
  games do this only once on the map change or server startup.
  Note: Name can be represented as an array of 4 "longs".
*/

	int nIndex = MSG_ReadByte(); // svc_num
	int nSize = MSG_ReadByte();
	
	char *sName = MSG_ReadString(); // 16 bits or bytes? (4 longs == 16 bytes on x86)
	                  // MSG_ReadLong(*(int *)&pMsg->szName[0]);
	                  // MSG_ReadLong(*(int *)&pMsg->szName[4]);
	                  // MSG_ReadLong(*(int *)&pMsg->szName[8]);
	                  // MSG_ReadLong(*(int *)&pMsg->szName[12]);
	
	// Important stuff
	//if(nSize == 0xFF)
		//nSize = -1;
	
	//nIndex = bound(0, nIndex, 255);
	
	//CL_LinkUserMessage(sName, nIndex, nSize);
};

void CL_ParsePacketEntities(bool bDelta)
{
	if(bDelta)
	{
	};
};

void CL_HandleChoke()
{
	// Notify the client that some outgoing datagrams were not transmitted due
	// to exceeding bandwidth rate limits

	// i = MSG_ReadByte ();
	// for (j=0 ; j<i ; j++)
	//	cl.frames[ (cls.netchan.incoming_acknowledged-1-j)&UPDATE_MASK
	//].receivedtime = -2;
};

void CL_ParseResourceList()
{
	//CL_ParseModellist();
	//CL_ParseSoundlist();
	
	int	i = 0;

	//Q_memset( &reslist, 0, sizeof( resourcelist_t ));

	//reslist.rescount = MSG_ReadWord() - 1;

	//for( i = 0; i < reslist.rescount; i++ )
	//{
		//reslist.restype[i] = MSG_ReadWord();
		//Q_strncpy( reslist.resnames[i], MSG_ReadString(), CS_SIZE );
	//};

	//cls.downloadcount = 0;

	//HTTP_ResetProcessState();

	//for( i = 0; i < reslist.rescount; i++ )
	//{
		// skip some types
#if 0
		if( reslist.restype[i] == t_model && !Q_strchr( download_types->latched_string, 'm' ) )
			continue;
		if( reslist.restype[i] == t_sound && !Q_strchr( download_types->latched_string, 's' ) )
			continue;
		if( reslist.restype[i] == t_eventscript && !Q_strchr( download_types->latched_string, 'e' ) )
			continue;
		if( reslist.restype[i] == t_generic && !Q_strchr( download_types->latched_string, 'c' ) )
			continue;
#endif
		//if( reslist.restype[i] == t_sound )
			//CL_CheckingSoundResFile( reslist.resnames[i] );
		//else
			//CL_CheckingResFile( reslist.resnames[i] );
	//};

	//if( !cls.downloadcount )
	//{
		//MSG_WriteByte( &cls.netchan.message, clc_stringcmd );
		//MSG_WriteString( &cls.netchan.message, "continueloading" );
	//};
};

void CL_ParseNewMoveVars()
{
	//Delta_InitClient(); // finalize client delta's

	//MSG_ReadDeltaMovevars( msg, &clgame.oldmovevars, &clgame.movevars );

	// update sky if changed
	//if( Q_strcmp( clgame.oldmovevars.skyName, clgame.movevars.skyName ) && cl.video_prepped )
		//R_SetupSky( clgame.movevars.skyName );

	//Q_memcpy( &clgame.oldmovevars, &clgame.movevars, sizeof( movevars_t ));
	
	// keep features an actual!
	//clgame.oldmovevars.features = clgame.movevars.features = host.features;
};

void CL_ParseResourceRequest()
{

};

/*
==================
CL_ParseCustomization
==================
*/
void CL_ParseCustomization()
{
	// TODO
};

/*
================
CL_ParseCrosshairAngle

offset crosshair angles
================
*/
void CL_ParseCrosshairAngle()
{
	//cl.refdef.crosshairangle[0] = MSG_ReadChar() * 0.2f;
	//cl.refdef.crosshairangle[1] = MSG_ReadChar() * 0.2f;
	//cl.refdef.crosshairangle[2] = 0.0f; // not used for screen space
};

void CL_ParseSoundFade()
{
	float fadePercent = (float)MSG_ReadByte();
	float holdTime = (float)MSG_ReadByte();
	
	float fadeOutSeconds = (float)MSG_ReadByte();
	float fadeInSeconds = (float)MSG_ReadByte();

	//S_FadeClientVolume( fadePercent, fadeOutSeconds, holdTime, fadeInSeconds );
};

void CL_HandleFileTransferFailed(){};

void CL_ParseHLTVMode()
{
	int nMode = MSG_ReadByte();
};

/*
==============
CL_ParseDirectorMsg

spectator message (hltv)
==============
*/
void CL_ParseDirectorMsg()
{
	byte pbuf[256];
	int iSize = MSG_ReadByte();

	// parse user message into buffer
	//MSG_ReadBytes(msg, pbuf, iSize);
	//gpClientDLL->pfnDirectorMessage(iSize, pbuf);
};

void CL_HandleVoiceInit()
{
	
};

void CL_ParseVoiceData()
{
	
};

void CL_ParseExtraInfo()
{
	
};

void CL_ParseTimeScale()
{
	float fTimeScale = MSG_ReadFloat();
};

void CL_ParseResourceLocation()
{
	// sv_downloadurl
	char *sURL = MSG_ReadString();
};

/*
==============
CL_ParseCvarValue

Find the client cvar value
and sent it back to the server
==============
*/
void CL_ParseCvarValue()
{
	const char *cvarName = MSG_ReadString();
	cvar_t *cvar = Cvar_FindVar(cvarName);

	// build the answer
	//MSG_WriteByte(&cls.netchan.message, clc_requestcvarvalue);
	MSG_WriteString(&cls.netchan.message, cvar ? cvar->string : "Not Found");
};

/*
==============
CL_ParseCvarValue2

Find the client cvar value
and sent it back to the server
==============
*/
void CL_ParseCvarValue2()
{
	int requestID = MSG_ReadLong();
	const char *cvarName = MSG_ReadString();
	cvar_t *cvar = Cvar_FindVar(cvarName);

	// build the answer
	//MSG_WriteByte(&cls.netchan.message, clc_requestcvarvalue2);
	MSG_WriteLong(&cls.netchan.message, requestID);
	MSG_WriteString(&cls.netchan.message, cvarName);
	MSG_WriteString(&cls.netchan.message, cvar ? cvar->string : "Not Found");
};

void SHOWNET(const char *s)
{
	//if(cl_shownet.value == 2) // >=
		//Con_Printf("%3i:%s\n", msg_readcount - 1, s); // net_message.readcount
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
	//cl.last_servermessage = realtime;
	//CL_ClearProjectiles();

	//
	// if recording demos, copy the message out
	//
	//if(cl_shownet.value == 1)
		//Con_Printf("%i ", net_message.cursize);
	//else if(cl_shownet.value == 2) // >=
		//Con_Printf("------------------\n");

	//CL_ParseClientdata();

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
			//CL_ParseClientData();
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
			//V_ParseDamage();
			break;
		case svc_spawnstatic:
			CL_ParseStaticEnt();
			break;
		case svc_event_reliable:
			CL_ParseEventData(true);
			break;
		case svc_spawnbaseline:
			i = MSG_ReadShort();
			//CL_ParseBaseline(&cl_baselines[i]);
			break;
		case svc_temp_entity:
			//CL_ParseTempEntity();
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
			CL_ParseResourceLocation();
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