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
/// @brief non-portable functions (platform-specific)

#pragma once

#include "common/commontypes.h"
#include "engine/eiface.h"
#include "maintypes.h"
#include "system/systemtypes.hpp"
//#include "public/interface.h"
//#include "tier0/platform.h"
#include "public/FileSystem.h"
#include "world/pr_dlls.hpp"

// vmodes.h must be included before cdll_int.h (wrect_t declaration)
#include "common/enums.h"
#include "common/wrect.h"
#include "engine/cdll_int.h"

//#include "engine_launcher_api.h"

// clang-format off
#ifdef HOOK_ENGINE
	#define g_hfind (*pg_hfind)
	
	#define g_engfuncsExportedToDlls (*pg_engfuncsExportedToDlls)
	
	#define gszDisconnectReason (*pgszDisconnectReason)
	#define gszExtendedDisconnectReason (*pgszExtendedDisconnectReason)
	#define gfExtendedError (*pgfExtendedError)
	#define g_bIsDedicatedServer (*pg_bIsDedicatedServer)
	#define giSubState (*pgiSubState)
	#define giActive (*pgiActive)
	#define giStateInfo (*pgiStateInfo)
	#define gEntityInterface (*pgEntityInterface)
	#define gNewDLLFunctions (*pgNewDLLFunctions)
	#define g_modfuncs (*pg_modfuncs)
	#define g_rgextdll (*pg_rgextdll)
	#define g_iextdllMac (*pg_iextdllMac)
	
	#define gfBackground (*pgfBackground)
	
#ifndef _WIN32
	#define gHasMMXTechnology (*pgHasMMXTechnology)
#endif
	
	#define con_debuglog (*pcon_debuglog)
	#define g_bPrintingKeepAliveDots (*pg_bPrintingKeepAliveDots)
	#define Launcher_ConsolePrintf (*pLauncher_ConsolePrintf)
#endif // HOOK_ENGINE

#ifdef HOOK_ENGINE
	#define dedicated_ (*pdedicated)
	
	#define g_bIsWin95 (*pg_bIsWin95)
	#define g_bIsWin98 (*pg_bIsWin98)
	
	#define g_flLastSteamProgressUpdateTime (*pg_flLastSteamProgressUpdateTime)
	
	#define szCommonPreloads (*pszCommonPreloads)
	
	#define szReslistsBaseDir (*pszReslistsBaseDir)
	#define szReslistsExt (*pszReslistsExt)
#endif
// clang-format on

extern qboolean gbIsDedicatedServer;

extern double g_flLastSteamProgressUpdateTime;

extern char *szCommonPreloads;
extern char *szReslistsBaseDir;
extern char *szReslistsExt;

extern FileFindHandle_t g_hfind;
extern enginefuncs_t g_engfuncsExportedToDlls;

extern char gszDisconnectReason[MAX_DISCONNECT_REASON];
extern char gszExtendedDisconnectReason[MAX_DISCONNECT_REASON];

extern qboolean gfExtendedError;

extern int giSubState;
extern int giActive;
extern int giStateInfo;

extern DLL_FUNCTIONS gEntityInterface;
extern NEW_DLL_FUNCTIONS gNewDLLFunctions;

extern modfuncs_t g_modfuncs;

extern extensiondll_t g_rgextdll[50];

extern int g_iextdllMac;

extern qboolean gfBackground;

#ifndef _WIN32
extern qboolean gHasMMXTechnology;
#endif

extern qboolean g_bPrintingKeepAliveDots;

extern qboolean con_debuglog;

#ifdef _WIN32

#ifdef HOOK_ENGINE
#define g_PerfCounterInitialized (*pg_PerfCounterInitialized)
#define g_PerfCounterMutex (*pg_PerfCounterMutex)

#define g_PerfCounterShiftRightAmount (*pg_PerfCounterShiftRightAmount)
#define g_PerfCounterSlice (*pg_PerfCounterSlice)
#define g_CurrentTime (*pg_CurrentTime)
#define g_StartTime (*pg_StartTime)

#define g_FPUCW_Mask_Prec_64Bit (*pg_FPUCW_Mask_Prec_64Bit)
#define g_FPUCW_Mask_Prec_64Bit_2 (*pg_FPUCW_Mask_Prec_64Bit_2)
#define g_FPUCW_Mask_Round_Trunc (*pg_FPUCW_Mask_Round_Trunc)
#define g_FPUCW_Mask_Round_Up (*pg_FPUCW_Mask_Round_Up)

#define g_WinNTOrHigher (*pg_WinNTOrHigher)
#endif

extern int g_PerfCounterInitialized;
extern CRITICAL_SECTION g_PerfCounterMutex;

extern int g_PerfCounterShiftRightAmount;
extern double g_PerfCounterSlice;
extern double g_CurrentTime;
extern double g_StartTime;

extern int g_FPUCW_Mask_Prec_64Bit;
extern int g_FPUCW_Mask_Prec_64Bit_2;
extern int g_FPUCW_Mask_Round_Trunc;
extern int g_FPUCW_Mask_Round_Up;

extern int g_WinNTOrHigher;
#endif //_WIN32

#ifdef _WIN32
extern void __cdecl Sys_InitHardwareTimer();
#endif //_WIN32

NOXREF void Sys_PageIn(void *ptr, int size);

const char *Sys_FindFirst(const char *path, char *basename);
const char *Sys_FindFirstPathID(const char *path, char *pathid);
const char *Sys_FindNext(char *basename);
void Sys_FindClose();

NOBODY int glob_match_after_star(char *pattern, char *text);
NOBODY int glob_match(char *pattern, char *text);

//
// memory protection
//
NOXREF void Sys_MakeCodeWriteable(uint32 startaddr, uint32 length);

NOBODY void Sys_SetFPCW();
NOBODY void Sys_PushFPCW_SetHigh();
NOBODY void Sys_PopFPCW();

NOBODY void MaskExceptions();

NOBODY void Sys_DebugOutStraight(const char *pStr);

void Sys_Quit();

void Dispatch_Substate(int iSubState);
void GameSetSubState(int iSubState);
void GameSetState(int iState);
NOBODY void GameSetBackground(qboolean bNewSetting);

DISPATCHFUNCTION GetDispatch(char *pname);

const char *FindAddressInTable(extensiondll_t *pDll, uint32 function);
uint32 FindNameInTable(extensiondll_t *pDll, const char *pName);

NOBODY const char *ConvertNameToLocalPlatform(const char *pchInName);

uint32 FunctionFromName(const char *pName);
const char *NameForFunction(uint32 function);

ENTITYINIT GetEntityInit(char *pClassName);

FIELDIOFUNCTION GetIOFunction(char *pName);

void LoadEntityDLLs(const char *szBaseDir);
void LoadThisDll(const char *szDllFilename);
void ReleaseEntityDlls();

void EngineFprintf(void *pfile, const char *szFmt, ...);
void AlertMessage(ALERT_TYPE atype, const char *szFmt, ...);

NOXREF void Sys_SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext);

const char *GetCurrentSteamAppName();

NOXREF void SetRateRegistrySetting(const char *pchRate);

NOXREF const char *GetRateRegistrySetting(const char *pchDef);

void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated);

void Sys_ShowProgressTicks(char *specialProgressMsg);

// TODO: Needs rechecking
// NOXREF int BuildMapCycleListHints(char **hints);