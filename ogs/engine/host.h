/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#pragma once
#ifndef OGS_HOST_H
#define OGS_HOST_H

#include "maintypes.h"
#include "filesystem_internal.h"
#include "sys_dll.h"
#include "server.h"
#include "rehlds_api.h"

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

// Note: it is possible to move local client code to host somehow (QW-client style)
// Need to think about it (client_state and client_static -> ClientData or something like that)
// void CHost::Disconnect() // will disconnect local client from server if connected to any

//=============================================================================

#define MAX_COMMAND_LINE_PARAMS 50

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

// Do we need this?

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

//

extern double realtime;
extern double rolling_fps;
extern quakeparms_t host_parms;
extern qboolean host_initialized;
extern double host_frametime;

extern int host_framecount;

extern client_t *host_client;
extern qboolean gfNoMasterServer;
extern double oldrealtime;
extern int host_hunklevel;
extern jmp_buf host_abortserver;
extern jmp_buf host_enddemo;
extern unsigned short *host_basepal;

//

class CEngineClient;
class CGameServer;
class CCmdBuffer;  //
class CCmdSystem;  // -> CCONSOLE!
class CCvarSystem; //
class CNetwork;
class CConsole;
class CSound;
class CInput; // partially implemented in clientdll

/* <271d2> ../engine/host.h:19 */
typedef struct quakeparms_s
{
	char *basedir;
	char *cachedir; // for development over ISDN lines
	
	int argc;
	char **argv;
	
	void *membase;
	int memsize;
} quakeparms_t;

extern quakeparms_t host_parms;

int Host_Init(quakeparms_t *parms);
void Host_InitLocal();
void Host_InitCommands();

void Host_Shutdown();
void Host_ShutdownServer(bool crash);

/*qboolean*/ bool Host_IsSinglePlayerGame();
/*qboolean*/ bool Host_IsServerActive();

void Host_Frame(float time);
void Host_UpdateScreen();
void Host_UpdateSounds();

void Host_ClearMemory(qboolean bQuiet);

bool Host_FilterTime(float time);

void Host_WriteConfig();
void Host_WriteCustomConfig();

NOXREF void Host_EndGame(const char *message, ...);
void __declspec(noreturn) Host_Error(const char *error, ...);

void Host_FindMaxClients();
void Host_GetConsoleCommands();

bool Host_IsInitialized();

void Host_ClientCommands(const char *fmt, ...);

void Host_CheckDynamicStructures();

void Host_ClearClients(qboolean bFramesOnly);
void Host_ClearMemory(qboolean bQuiet);

void Host_ComputeFPS(double frametime);
void Host_GetHostInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap);
void Host_Speeds(double *time);

void Host_CheckConnectionFailure();
void _Host_Frame(float time);
int Host_Frame(float time, int iState, int *stateInfo);
void CheckGore(); // Host_CheckGore?
void Host_Version();

struct host_t
{
	// tHostStateVec mvStates;
	// host_state_t meCurrentState;
	
	CConsole *mpConsole;
	CEngineClient *mpLocalClient;
	CGameServer *mpLocalServer;
	CCmdBuffer *mpCmdBuffer;
	CNetwork *mpNetwork;
	CSound *mpSound;
	CInput *mpInput;
	
	quakeparms_t host_parms; // todo: make ptr
	
	int host_framecount;
	
	bool bInitialized; // True if into command execution
};

#endif // OGS_HOST_H