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

#include "rehlds/maintypes.h"
#include "modinfo.h"
#include "public/FileSystem.h"
#include "pr_dlls.h"

//vmodes.h must be included before cdll_int.h (wrect_t declaration)
#include "vmodes.h"
#include "engine/cdll_int.h"

#ifdef _WIN32
#define NOXREFCHECK		__asm { push [ebp + 4] } Sys_Error(__FUNCTION__ " NOXREF, but called from 0x%.08x")
#else
#define NOXREFCHECK		int *a = 0; *a = 0
#endif

#define MAX_DISCONNECT_REASON 256

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
#define gmodinfo (*pgmodinfo)
#define gfBackground (*pgfBackground)

#ifndef _WIN32
#define gHasMMXTechnology (*pgHasMMXTechnology)
#endif

#define con_debuglog (*pcon_debuglog)
#define g_bPrintingKeepAliveDots (*pg_bPrintingKeepAliveDots)
#define Launcher_ConsolePrintf (*pLauncher_ConsolePrintf)
#endif // HOOK_ENGINE


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
extern modinfo_t gmodinfo;
extern qboolean gfBackground;

#ifndef _WIN32
extern qboolean gHasMMXTechnology;
#endif
extern qboolean g_bPrintingKeepAliveDots;

extern qboolean con_debuglog;
extern void(*Launcher_ConsolePrintf)(char *, ...);

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
void Sys_FindClose(void);
NOBODY int glob_match_after_star(char *pattern, char *text);
NOBODY int glob_match(char *pattern, char *text);
NOXREF void Sys_MakeCodeWriteable(uint32 startaddr, uint32 length);
NOBODY void Sys_SetFPCW(void);
NOBODY void Sys_PushFPCW_SetHigh(void);
NOBODY void Sys_PopFPCW(void);
NOBODY void MaskExceptions(void);
NOBODY void Sys_Init(void);
NOXREF void Sys_Sleep(int msec);
NOBODY void Sys_DebugOutStraight(const char *pStr);
NOBODY void __declspec(noreturn) Sys_Error(const char *error, ...);
NOXREF void Sys_Warning(const char *pszWarning, ...);
void Sys_Printf(const char *fmt, ...);
void Sys_Quit(void);
double Sys_FloatTime(void);
void Dispatch_Substate(int iSubState);
void GameSetSubState(int iSubState);
void GameSetState(int iState);
NOBODY void GameSetBackground(qboolean bNewSetting);
qboolean Voice_GetClientListening(int iReceiver, int iSender);
qboolean Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen);
DISPATCHFUNCTION GetDispatch(char *pname);
const char *FindAddressInTable(extensiondll_t *pDll, uint32 function);
uint32 FindNameInTable(extensiondll_t *pDll, const char *pName);
NOBODY const char *ConvertNameToLocalPlatform(const char *pchInName);
uint32 FunctionFromName(const char *pName);
const char *NameForFunction(uint32 function);
ENTITYINIT GetEntityInit(char *pClassName);
FIELDIOFUNCTION GetIOFunction(char *pName);
NOBODY void DLL_SetModKey(modinfo_t *pinfo, char *pkey, char *pvalue);
void LoadEntityDLLs(const char *szBaseDir);
void LoadThisDll(const char *szDllFilename);
void ReleaseEntityDlls(void);
void EngineFprintf(void *pfile, const char *szFmt, ...);
void AlertMessage(ALERT_TYPE atype, const char *szFmt, ...);
NOXREF void Sys_SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext);