

#ifndef SYS_DLL2_H
#define SYS_DLL2_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"
#include "interface.h"
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

const char *GetCurrentSteamAppName(void);
NOXREF void SetRateRegistrySetting(const char *pchRate);
NOXREF const char *GetRateRegistrySetting(const char *pchDef);
void EXPORT F(IEngineAPI **api);
void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated);
NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage);
void Legacy_Sys_Printf(char *fmt, ...);
NOXREF void Legacy_MP3subsys_Suspend_Audio(void);
NOXREF void Legacy_MP3subsys_Resume_Audio(void);
void Sys_SetupLegacyAPIs(void);
NOXREF int Sys_IsWin95(void);
NOXREF int Sys_IsWin98(void);

#ifdef _WIN32
NOXREF void Sys_CheckOSVersion(void);
#endif

NOXREF void Sys_Init(void);
NOXREF void Sys_Shutdown(void);
void Sys_InitArgv(char *lpCmdLine);
NOXREF void Sys_ShutdownArgv(void);
void Sys_InitMemory(void);
void Sys_ShutdownMemory(void);
void Sys_InitLauncherInterface(void);
NOXREF void Sys_ShutdownLauncherInterface(void);
void Sys_InitAuthentication(void);
NOXREF void Sys_ShutdownAuthentication(void);
void Sys_ShowProgressTicks(char *specialProgressMsg);
int Sys_InitGame(char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated);
void Sys_ShutdownGame(void);
void ClearIOStates(void);

NOBODY class IBaseInterface *__CreateCEngineAPIIEngineAPI_interface(void);

// Needs rechecking
//NOXREF int BuildMapCycleListHints(char **hints);

NOBODY class IBaseInterface *__CreateCDedicatedServerAPIIDedicatedServerAPI_interface(void);
NOBODY void _GLOBAL__sub_I_D_SurfaceCacheForRes(void);

#endif // SYS_DLL2_H
