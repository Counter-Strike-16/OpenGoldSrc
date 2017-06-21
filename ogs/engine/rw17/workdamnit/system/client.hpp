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
/// @brief primary client module header

#pragma once

#include "system/clienttypes.hpp"
#include "network/frame.hpp"
#include "common/cl_entity.h"
#include "common/com_model.h"
#include "common/commontypes.h"
#include "common/cvardef.h"
#include "common/beamdef.h"
#include "common/entity_state.h"
#include "common/netadr.h"
#include "common/screenfade.h"
#include "common/usercmd.h"
#include "filesystem/FileSystem.hpp"
#include "network/net_chan.hpp"
#include "resources/consistency.hpp"
#include "network/protocol.hpp"
#include "sound/SoundFadeData.hpp"
#include "system/common.hpp"
#include "world/event.hpp"
//#include "engine/custom.h"
#include "common/qlimits.h"

#ifdef HOOK_ENGINE
#define cls (*pcls)
#define cl (*pcl)

#define g_clmove (*pg_clmove)
#define cl_inmovie (*pcl_inmovie)

#define cl_name (*pcl_name)
#define rate_ (*prate)
#define console (*pconsole)
#endif // HOOK_ENGINE

typedef struct dlight_s dlight_t;
typedef struct playermove_s playermove_t;
typedef struct sfx_s sfx_t;

extern playermove_t g_clmove;
extern qboolean cl_inmovie;

//
// cvars
//
extern cvar_t cl_name;
extern cvar_t password;
extern cvar_t rate_;
extern cvar_t console;

typedef struct cmd_s
{
	usercmd_t cmd;

	float senttime;
	float receivedtime;
	float frame_lerp;

	qboolean processedfuncs;
	qboolean heldback;

	int sendsize;
} cmd_t;

/**
==================================================================

the client_static_t structure is persistant through an arbitrary number
of server connections

==================================================================
*/
typedef struct client_static_s
{
	// local state
	cactive_t state;
	
	// network stuff
	netchan_t netchan;

	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];
	
	// connection information
	
	double connect_time; ///< for connection retransmits {-1.0f}

	int connect_retry;
	int challenge;

	byte authprotocol;

	int userid;

	char trueaddress[32];

	float slist_time;

	int signon;

	char servername[MAX_PATH]; // MAX_OSPATH; name of server from original connect
	char mapstring[64];
	char spawnparms[2048];
	
	// private userinfo for sending to masterless servers
	char userinfo[256]; // MAX_INFO_STRING

	float nextcmdtime;

	int lastoutgoingcommand;
	
	// demo loop control
	int demonum; // -1 = don't play demos
	char demos[MAX_DEMOS][MAX_DEMONAME]; // when not playing
	
	// demo recording info must be here, so it isn't cleared on level change
	
	qboolean demorecording;
	qboolean demoplayback;
	qboolean timedemo;

	float demostarttime;
	int demostartframe;

	int forcetrack;

	FileHandle_t demofile;
	FileHandle_t demoheader;

	qboolean demowaiting; // don't record until a non-delta message is received
	qboolean demoappending;

	char demofilename[MAX_PATH];
	int demoframecount;

	int td_lastframe; // to meter out one message a frame
	int td_startframe; // host_framecount at start
	float td_starttime; // realtime at second frame of timedemo

	incomingtransfer_t dl;

	float packet_loss;
	double packet_loss_recalc_time;

	int playerbits;

	soundfade_t soundfade;

	char physinfo[MAX_PHYSINFO_STRING];

	unsigned char md5_clientdll[16];

	netadr_t game_stream;
	netadr_t connect_stream;

	qboolean passive;
	qboolean spectator;
	qboolean director;
	qboolean fSecureClient;
	qboolean isVAC2Secure;

	uint64 GameServerSteamID;

	int build_num;
} client_static_t;

extern client_static_t cls;

/**
* the client_state_t structure is wiped completely at every
* server signon
*/
typedef struct client_state_s
{
	int max_edicts;

	resource_t resourcesonhand;
	resource_t resourcesneeded;
	resource_t resourcelist[MAX_RESOURCE_LIST];

	int num_resources;

	qboolean need_force_consistency_response;

	char serverinfo[512]; // MAX_SERVERINFO_STRING

	int servercount; ///< server identification for prespawns
	int validsequence;	///< this is the sequence number of the last good
						///< packetentity_t we got.  If this is 0, we can't
						///< render a frame yet

	int parsecount; ///< server message counter
	int parsecountmod;
	
	// information for local display
	int stats[32]; ///< MAX_CL_STATS; health, etc (unused quake legacy)
	
	int weapons;

	usercmd_t cmd;
	
	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  And only reset at level change
	// and teleport times
	vec3_t viewangles;
	
	vec3_t punchangle; // temporary view kick from weapon firing
	vec3_t crosshairangle;

	vec3_t simorg;
	vec3_t simvel;
	vec3_t simangles;

	vec3_t predicted_origins[64];
	vec3_t prediction_error;

	float idealpitch;

	vec3_t viewheight;

	screenfade_t sf;

	qboolean paused; ///< send over by server

	int onground;
	int moving;
	int waterlevel;
	int usehull;

	float maxspeed; // local value for maximum movement speed (for prediction)

	int pushmsec;
	int light_level;
	int intermission; // don't change view angle, full screen, etc

	double mtime[2];
	
	// the client simulates or interpolates movement to get these values
	double time; ///< this is the time value that the client
				 ///< is rendering at.  allways <= realtime
	
	double oldtime;
	
	// sentcmds[cl.netchan.outgoing_sequence & UPDATE_MASK] = cmd
	frame_t frames[64]; // UPDATE_BACKUP
	
	cmd_t commands[64]; // UPDATE_BACKUP
	local_state_t predicted_frames[64]; // UPDATE_BACKUP

	int delta_sequence;
	int playernum;

	event_t event_precache[MAX_EVENTS];

	model_t *model_precache[MAX_MODELS];
	int model_precache_count;

	sfx_s *sound_precache[MAX_SOUNDS];

	consistency_t consistency_list[MAX_CONSISTENCY_LIST];
	int num_consistency;

	int highentity;

	char levelname[40]; ///< for display on solo scoreboard

	int maxclients;
	int gametype;
	int viewentity; // playernum + 1
	
	// refresh related state
	model_t *worldmodel; ///< cl_entitites[0].model
	efrag_t *free_efrags;

	int num_entities; ///< stored bottom up in cl_entities array
	int num_statics; ///< stored top down in cl_entitiers

	cl_entity_t viewent; ///< weapon viewmodel

	int cdtrack;
	int looptrack;

	CRC32_t serverCRC;

	unsigned char clientdllmd5[16];

	float weaponstarttime;
	int weaponsequence;

	int fPrecaching;

	dlight_t *pLight;
	
	// all player information
	player_info_t players[32]; // MAX_PLAYERS

	entity_state_t instanced_baseline[64];
	int instanced_baseline_number;

	CRC32_t mapCRC;

	event_state_t events;

	char downloadUrl[128];
} client_state_t;

extern client_state_t cl;

typedef struct
{
	int length;
	char map[MAX_STYLESTRING];
} lightstyle_t;

// FIXME, allocate dynamically
extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	cl_entity_t		cl_entities[MAX_EDICTS];
extern	cl_entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	dlight_t		cl_dlights[MAX_DLIGHTS];
extern	cl_entity_t		cl_temp_entities[MAX_TEMP_ENTITIES];
extern	BEAM			cl_beams[MAX_BEAMS];

const int MAX_VISEDICTS = 256;

// Visible entities double-buffer
extern	int				cl_numvisedicts, cl_oldnumvisedicts;
extern	cl_entity_t		*cl_visedicts, *cl_oldvisedicts;
extern	cl_entity_t		cl_visedicts_list[2][MAX_VISEDICTS];

//
// main
//

void CL_Init(); //+
void CL_InitClosest();

void CL_Shutdown(); //+
void CL_ShutDownUsrMessages();
void CL_ShutDownClientStatic();

void CL_CheckClientState();
void CL_ClearClientState();

void CL_Connect_f(); //+

void CL_CheckForResend(); //+
void CL_KeepConnectionActive();

void CL_Disconnect();   //+
void CL_Disconnect_f(); //+

void CL_RecordHUDCommand(char *cmdname);

qboolean CL_CheckFile(sizebuf_t *msg, char *filename);

model_s *CL_GetModelByIndex(int index);

void CL_AddToResourceList(resource_t *pResource, resource_t *pList);

//event_hook_s *CL_FindEventHook(char *name);

int CL_IsThirdPerson();
void CL_CameraOffset(float *ofs);
void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
void CL_QueueHTTPDownload(const char *filename);
int CL_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);

void CL_PrintLogos();
void CL_ReadPackets(); //+

qboolean CL_RequestMissingResources();

void CL_Move();
void CL_SendConnectPacket(); //+

void CL_AdjustClock();
void CL_Save(const char *name);
void CL_HudMessage(const char *pMessage);

int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf);

void CL_SetLastUpdate();

void CL_WriteMessageHistory(int starting_count, int cmd);
void CL_MoveSpectatorCamera();

void CL_UpdateModuleC();

//
// sound
//
void CL_UpdateSoundFade();

//
// scene
//
void CL_DecayLights(); //+
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel);

//
// render
//
void R_DecalRemoveAll(int textureIndex);

//
// chase camera
//
void Chase_Init();

//
// message parsing
//
void CL_ParseServerMessage(); //+

//
// entities
//
void CL_SetSolidPlayers(int playernum); //+
void CL_SetSolidEntities();
void CL_EmitEntities(); //+

//
// events
//
void CL_InitEventSystem();
void CL_FireEvents(); //+

//
// demo
//
void CL_StopPlayback(); //+
qboolean CL_GetMessage(); //+

//
// client side prediction
//
//void CL_InitPrediction();
//void CL_PredictMovement();
void CL_PredictMove(qboolean repredicting); //+
//void CL_PredictUsercmd(player_state_t *from, player_state_t *to, usercmd_t *u, qboolean spectator);
void CL_RedoPrediction(); //+
//void CL_CheckPredictionError();

//
// voice
//
void CL_AddVoiceToDatagram(qboolean bFinal);
void CL_VoiceIdle();

//
//
//
void SetCareerAudioState(int state);