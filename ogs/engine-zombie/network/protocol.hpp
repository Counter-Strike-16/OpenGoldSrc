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
/// @brief communications protocols

#pragma once

/*
==============================================================

NETWORK PROTOCOL

==============================================================
*/

const int PROTOCOL_VERSION = 48;

const char VALVE_MASTER_ADDRESS[] = "127.0.0.1:27010";

//=========================================

const int PORT_CLIENT = 27005;
const int PORT_MASTER = 27010; // 27011
const int PORT_SERVER = 27015;
const int PORT_RCON = 27015;
const int PORT_HLTV = 27020;
const int PORT_MATCHMAKING = 27025;
const int PORT_SYSTEMLINK = 27030;
const int PORT_RPT = 27035;
const int PORT_RPT_LISTEN = 27036;

//=========================================

/*
==========================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

==========================================================
*/

// Max # of clients allowed in a server
//const int MAX_CLIENTS = 32;

// How many data slots to use when in multiplayer (must be power of 2)
const int MULTIPLAYER_BACKUP = 64;
// Same for single player
const int SINGLEPLAYER_BACKUP = 8;

const int RESOURCE_INDEX_BITS = 12;

#ifdef REHLDS_FIXES
	const int RESOURCE_MAX_COUNT = (1 << RESOURCE_INDEX_BITS);
#endif // REHLDS_FIXES

const int MAX_SOUNDS = 512;
const int MAX_SOUND_HASHLOOKUP_SIZE = (MAX_SOUNDS * 2 - 1);

// How many bits to use to encode an edict
const int MAX_EDICT_BITS = 11; // # of bits needed to represent max edicts

// Max # of edicts in a level (2048)
//const int MAX_EDICTS = (1 << MAX_EDICT_BITS);

const int MAX_MODEL = 512;
const int MAX_GENERIC = 512;
const int MAX_EVENTS = 256;

const int MAX_RESOURCE_LIST = 1280;

const int MAX_PACKET_ENTITIES = 256; // 256 visible entities per frame

const int DEFAULT_SOUND_PACKET_VOLUME = 255;

const float DEFAULT_SOUND_PACKET_ATTENUATION = 1.0f;

const int DEFAULT_SOUND_PACKET_PITCH = 100;

//=========================================

// out of band message id bytes

// M = master, S = server, C = client, A = any
// the second character will allways be \n if the message isn't a single
// byte long (?? not true anymore?)

// Client connection is initiated by requesting a challenge value
//  the server sends this value back
const char S2C_CHALLENGE = 'A'; // + challenge value

// Send a userid, client remote address, is this server secure and engine build
// number
const char S2C_CONNECTION = 'B';

const char A2A_NACK				= 'm'; // [+ comment] general failure
const char A2A_ECHO				= 'e'; // for echoing
const char A2C_PRINT			= 'n'; // print a message on client
const char A2C_CLIENT_COMMAND	= 'B'; // + command line

// HLMaster rejected a server's connection because the server needs to be
// updated
const char M2S_REQUESTRESTART = 'O';

// send a log event as key value
const char S2A_LOGSTRING = 'R';

// Send a log string
const char S2A_LOGKEY = 'S';

// Basic information about the server
const char A2S_INFO = 'T';

// Details about each player on the server
const char A2S_PLAYER = 'U';

// The rules the server is using
const char A2S_RULES = 'V';

// Another user is requesting a challenge value from this machine
const char A2A_GETCHALLENGE = 'W'; // Request challenge # from another machine

// Generic Ping Request
const char A2A_PING = 'i'; // respond with an A2A_ACK

// Generic Ack
const char A2A_ACK = 'j'; // general acknowledgement without info

// Challenge response from master
const char M2A_CHALLENGE = 's'; // + challenge value

const char S2M_HEARTBEAT	= 'a'; // + serverinfo + userlist + fraglist
const char S2M_SHUTDOWN		= 'C';

//=========================================

//==================
// the svc_strings[] array in cl_parse.c should mirror this
//==================

//
// server to client
//
typedef enum svc_commands_e
{
	svc_bad,
	svc_nop,
	svc_disconnect,
	svc_event,
	svc_version,
	svc_setview,
	svc_sound,
	svc_time,
	svc_print,
	svc_stufftext,
	svc_setangle,
	svc_serverinfo,
	svc_lightstyle,
	svc_updateuserinfo,
	svc_deltadescription,
	svc_clientdata,
	svc_stopsound,
	svc_pings,
	svc_particle,
	svc_damage,
	svc_spawnstatic,
	svc_event_reliable,
	svc_spawnbaseline,
	svc_temp_entity,
	svc_setpause,
	svc_signonnum,
	svc_centerprint,
	svc_killedmonster,
	svc_foundsecret,
	svc_spawnstaticsound,
	svc_intermission,
	svc_finale,
	svc_cdtrack,
	svc_restore,
	svc_cutscene,
	svc_weaponanim,
	svc_decalname,
	svc_roomtype,
	svc_addangle,
	svc_newusermsg,
	svc_packetentities,
	svc_deltapacketentities,
	svc_choke,
	svc_resourcelist,
	svc_newmovevars,
	svc_resourcerequest,
	svc_customization,
	svc_crosshairangle,
	svc_soundfade,
	svc_filetxferfailed,
	svc_hltv,
	svc_director,
	svc_voiceinit,
	svc_voicedata,
	svc_sendextrainfo,
	svc_timescale,
	svc_resourcelocation,
	svc_sendcvarvalue,
	svc_sendcvarvalue2,

	svc_startofusermessages = svc_sendcvarvalue2, // + 1?

	svc_endoflist = 255,
} svc_commands_t;

//==============================================

//
// client to server
//
typedef enum clc_commands_e
{
	clc_bad,
	clc_nop,
	clc_move,			// [[usercmd_t]
	clc_stringcmd,		// [string] message
	clc_delta,			// [byte] sequence number, requests delta compression of message
	clc_resourcelist,
	clc_tmove,			// teleport request, spectator only
	clc_fileconsistency,
	clc_voicedata,
	clc_hltv,
	clc_cvarvalue,
	clc_cvarvalue2,

	clc_endoflist = 255,
} clc_commands_t;