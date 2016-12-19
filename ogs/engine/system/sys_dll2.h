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

#ifndef SYS_DLL2_H
#define SYS_DLL2_H
#ifdef _WIN32
#pragma once
#endif

#include "rehlds/maintypes.h"
#include "public/interface.h"
#include "tier0/platform.h"

#include "engine_launcher_api.h"

#define FIFTEEN_MB			(15 * 1024 * 1024)
#define MINIMUM_WIN_MEMORY		0x0e00000
#define WARNING_MEMORY			0x0200000
#define MAXIMUM_WIN_MEMORY		0x8000000 // Ask for 128 MB max
#define MAXIMUM_DEDICATED_MEMORY	0x8000000 // Ask for 128 MB max
#define DEFAULT_MEMORY			0x2800000

#ifdef HOOK_ENGINE
#define dedicated_ (*pdedicated)
#define g_bIsWin95 (*pg_bIsWin95)
#define g_bIsWin98 (*pg_bIsWin98)
#define g_flLastSteamProgressUpdateTime (*pg_flLastSteamProgressUpdateTime)
#define szCommonPreloads (*pszCommonPreloads)
#define szReslistsBaseDir (*pszReslistsBaseDir)
#define szReslistsExt (*pszReslistsExt)
#endif

extern qboolean g_bIsWin95;
extern qboolean g_bIsWin98;
extern double g_flLastSteamProgressUpdateTime;
extern char *szCommonPreloads;
extern char *szReslistsBaseDir;
extern char *szReslistsExt;

const char *GetCurrentSteamAppName();
NOXREF void SetRateRegistrySetting(const char *pchRate);
NOXREF const char *GetRateRegistrySetting(const char *pchDef);
void EXPORT F(IEngineAPI **api);
void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated);
NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage);
void Legacy_Sys_Printf(char *fmt, ...);
NOXREF void Legacy_MP3subsys_Suspend_Audio();
NOXREF void Legacy_MP3subsys_Resume_Audio();
void Sys_SetupLegacyAPIs();
NOXREF int Sys_IsWin95();
NOXREF int Sys_IsWin98();

#ifdef _WIN32
NOXREF void Sys_CheckOSVersion();
#endif

NOXREF void Sys_Init();
NOXREF void Sys_Shutdown();
void Sys_InitArgv(char *lpCmdLine);
NOXREF void Sys_ShutdownArgv();
void Sys_InitMemory();
void Sys_ShutdownMemory();
void Sys_InitLauncherInterface();
NOXREF void Sys_ShutdownLauncherInterface();
void Sys_InitAuthentication();
NOXREF void Sys_ShutdownAuthentication();
void Sys_ShowProgressTicks(char *specialProgressMsg);
int Sys_InitGame(char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated);
void Sys_ShutdownGame();
void ClearIOStates();

// TODO: Needs rechecking
//NOXREF int BuildMapCycleListHints(char **hints);

#endif // SYS_DLL2_H