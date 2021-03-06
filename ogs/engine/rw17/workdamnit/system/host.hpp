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
/// @brief primary header for host

#pragma once

#include <csetjmp>
#include "common/commontypes.h"
#include "common/maintypes.h"
#include "public/FileSystem.h"

// clang-format off
#ifdef HOOK_ENGINE
	#define host_name (*phost_name)
	#define host_speeds (*phost_speeds)
	#define host_profile (*phost_profile)
	#define developer (*pdeveloper)
	#define host_limitlocal (*phost_limitlocal)
	#define skill (*pskill)
	#define deathmatch (*pdeathmatch)
	#define coop (*pcoop)
	#define sys_ticrate (*psys_ticrate)
	#define sys_timescale (*psys_timescale)
	#define fps_max (*pfps_max)
	#define host_killtime (*phost_killtime)
	#define sv_stats (*psv_stats)
	#define fps_override (*pfps_override)
	#define host_framerate (*phost_framerate)
	#define pausable (*ppausable)
	#define suitvolume (*psuitvolume)

	#define realtime (*prealtime)
	#define rolling_fps (*prolling_fps)
	#define host_parms (*phost_parms)
	#define host_initialized (*phost_initialized)
	#define host_frametime (*phost_frametime)
	#define host_framecount (*phost_framecount)
	#define host_client (*phost_client)
	#define gfNoMasterServer (*pgfNoMasterServer)
	#define oldrealtime (*poldrealtime)
	#define host_hunklevel (*phost_hunklevel)
	#define host_abortserver (*phost_abortserver)
	#define host_enddemo (*phost_enddemo)
	#define host_basepal (*phost_basepal)
#endif // HOOK_ENGINE
// clang-format on

//=============================================================================

typedef struct cvar_s cvar_t;
typedef struct client_s client_t;

//=============================================================================

extern cvar_t host_name;
extern cvar_t host_speeds;
extern cvar_t host_profile;
extern cvar_t developer;
extern cvar_t host_limitlocal;
extern cvar_t skill;
extern cvar_t deathmatch;
extern cvar_t coop;
extern cvar_t sys_ticrate;
extern cvar_t sys_timescale;
extern cvar_t fps_max;
extern cvar_t host_killtime;
extern cvar_t sv_stats;
extern cvar_t fps_override;
extern cvar_t host_framerate;
extern cvar_t pausable;
extern cvar_t suitvolume;

//
// host
//
extern quakeparms_t host_parms;

extern client_t *host_client;
extern qboolean gfNoMasterServer;

extern unsigned short *host_basepal; // 16 bit color palette

NOXREF void Host_EndGame(const char *message, ...);

void NORETURN Host_Error(const char *error, ...);

void Host_ClientCommands(const char *fmt, ...);
void Host_ClearClients(qboolean bFramesOnly);
void Host_ShutdownServer(qboolean crash);

void Host_CheckDyanmicStructures();
void Host_ClearMemory(qboolean bQuiet);
qboolean Host_FilterTime(float time);

void Host_ComputeFPS(double frametime);
void Host_GetHostInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap);
void Host_Speeds(double *time);

void Host_CheckConnectionFailure();

int Host_Frame(float time, int iState, int *stateInfo);

void CheckGore();

qboolean Host_IsSinglePlayerGame();
qboolean Host_IsServerActive();

void Host_Version();

int Host_Init(quakeparms_t *parms);
void Host_Shutdown();

int GetGameAppID();

bool IsGameSubscribed(const char *gameName);
NOXREF bool BIsValveGame();