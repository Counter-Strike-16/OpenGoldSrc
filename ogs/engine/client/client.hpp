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

#include "common/commontypes.h"
#include "client/clienttypes.hpp"
#include "common/cvardef.h"
#include "common/entity_state.h"
#include "common/com_model.h"
#include "common/cl_entity.h"
#include "common/usercmd.h"
#include "common/netadr.h"
#include "filesystem/filesystem_internal.hpp"
#include "network/net_chan.hpp"
#include "input/keys.hpp"
#include "system/common.hpp"
#include "resources/consistency.hpp"
#include "world/event.hpp"
#include "server/server.hpp"
#include "common/screenfade.h"
#include "sound/soundfade.hpp"
#include "client/frame.hpp"

#ifdef HOOK_ENGINE
#define g_pcls (*pcls)
#define g_pcl (*pcl)
#define key_dest (*pkey_dest)

#define g_clmove (*pg_clmove)
#define cl_inmovie (*pcl_inmovie)

#define cl_name (*pcl_name)
#define rate_ (*prate)
#define console (*pconsole)
#endif // HOOK_ENGINE

extern keydest_t key_dest;

extern playermove_t g_clmove;
extern qboolean     cl_inmovie;

extern cvar_t cl_name;
extern cvar_t rate_;
extern cvar_t console;

typedef struct dlight_s     dlight_t;
typedef struct playermove_s playermove_t;
typedef struct sfx_s        sfx_t;

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
	cactive_t state;

	netchan_t netchan;

	sizebuf_t datagram;
	byte      datagram_buf[MAX_DATAGRAM];

	double connect_time;

	int connect_retry;
	int challenge;

	byte authprotocol;

	int userid;

	char trueaddress[32];

	float slist_time;

	int signon;

	char servername[MAX_PATH];
	char mapstring[64];
	char spawnparms[2048];
	char userinfo[256];

	float nextcmdtime;

	int lastoutgoingcommand;
	int demonum;

	char demos[MAX_DEMOS][16];

	qboolean demorecording;
	qboolean demoplayback;
	qboolean timedemo;

	float demostarttime;
	int   demostartframe;

	int forcetrack;

	FileHandle_t demofile;
	FileHandle_t demoheader;

	qboolean demowaiting;
	qboolean demoappending;

	char demofilename[MAX_PATH];
	int  demoframecount;

	int   td_lastframe;
	int   td_startframe;
	float td_starttime;

	incomingtransfer_t dl;

	float  packet_loss;
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
	int validsequence;

	int parsecount; ///< server message counter
	int parsecountmod;

	int stats[32];
	int weapons;

	usercmd_t cmd;

	vec3_t viewangles;
	vec3_t punchangle;
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

	float maxspeed;

	int pushmsec;
	int light_level;
	int intermission;

	double mtime[2];
	double time;
	double oldtime;

	frame_t       frames[64];
	cmd_t         commands[64];
	local_state_t predicted_frames[64];

	int delta_sequence;
	int playernum;

	event_t event_precache[HL_EVENT_MAX];

	model_t *model_precache[HL_MODEL_MAX];
	int      model_precache_count;

	sfx_s *sound_precache[HL_SOUND_MAX];

	consistency_t consistency_list[MAX_CONSISTENCY_LIST];
	int           num_consistency;

	int highentity;

	char levelname[40];

	int maxclients;
	int gametype;
	int viewentity;

	model_t *worldmodel;
	efrag_t *free_efrags;

	int num_entities;
	int num_statics;

	cl_entity_t viewent;

	int cdtrack;
	int looptrack;

	CRC32_t serverCRC;

	unsigned char clientdllmd5[16];

	float weaponstarttime;
	int   weaponsequence;

	int fPrecaching;

	dlight_t *pLight;

	player_info_t players[32];

	entity_state_t instanced_baseline[64];
	int            instanced_baseline_number;

	CRC32_t mapCRC;

	event_state_t events;

	char downloadUrl[128];
} client_state_t;

extern client_state_t cl;

void CL_RecordHUDCommand(char *cmdname);

void R_DecalRemoveAll(int textureIndex);

void     CL_CheckForResend();

qboolean CL_CheckFile(sizebuf_t *msg, char *filename);

void CL_ClearClientState();
void CL_Connect_f();
void CL_DecayLights();
void CL_Disconnect();
void CL_Disconnect_f();
void CL_EmitEntities();
void CL_InitClosest();
void CL_Init();
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel);

void CL_PredictMove(qboolean repredicting);

void     CL_PrintLogos();
void     CL_ReadPackets();

qboolean CL_RequestMissingResources();

void     CL_Move();
void     CL_SendConnectPacket();
void     CL_StopPlayback();
void     CL_UpdateSoundFade();
void     CL_AdjustClock();
void CL_Save(const char *name);
void CL_HudMessage(const char *pMessage);

extern "C" void ClientDLL_UpdateClientData();
extern "C" void ClientDLL_HudVidInit();

void Chase_Init();

extern "C" void ClientDLL_Init();

int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf);

void CL_ShutDownUsrMessages();
void CL_ShutDownClientStatic();

extern "C" void ClientDLL_MoveClient(struct playermove_s *ppmove);

void CL_Shutdown();

extern "C" void ClientDLL_Frame(double time);
extern "C" void ClientDLL_CAM_Think();

void CL_InitEventSystem();
void CL_CheckClientState();

void CL_RedoPrediction();

void CL_SetLastUpdate();

void CL_WriteMessageHistory(int starting_count, int cmd);
void CL_MoveSpectatorCamera();
void CL_AddVoiceToDatagram(qboolean bFinal);
void CL_VoiceIdle();

void PollDInputDevices();

void CL_KeepConnectionActive();
void CL_UpdateModuleC();

int  VGuiWrap2_IsInCareerMatch();
void VguiWrap2_GetCareerUI();
int VGuiWrap2_GetLocalizedStringLength(const char *label);
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName);

void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength);
void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength);
int GetTimesTutorMessageShown(int id);
void RegisterTutorMessageShown(int mid);
void ResetTutorMessageDecayData();

void SetCareerAudioState(int state);