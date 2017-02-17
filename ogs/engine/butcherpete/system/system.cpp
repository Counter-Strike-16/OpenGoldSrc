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

//#include "precompiled.hpp"
#include "system/system.hpp"
#include "system/dedicatedserverapi.hpp"
#include "system/common.hpp"
#include "system/host.hpp"
#include "system/host_cmd.hpp"
#include "system/traceinit.h"
#include "filesystem/filesystem_internal.hpp"
#include "resources/modinfo.hpp"
#include "console/console.hpp"
#include "voice/voice.hpp"
#include "input/keys.hpp"
#include "server/server.hpp"
#include "server/sv_log.hpp"
#include "iregistry.h"

void (*Launcher_ConsolePrintf)(char *, ...);
char *(*Launcher_GetLocalizedString)(unsigned int);

int (*Launcher_MP3subsys_Suspend_Audio)();
void (*Launcher_MP3subsys_Resume_Audio)();

void (*VID_FlipScreen)();

qboolean g_bIsWin95;
qboolean g_bIsWin98;

double g_flLastSteamProgressUpdateTime;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

char *szCommonPreloads = "multiplayer_preloads";
char *szReslistsBaseDir = "reslists";
char *szReslistsExt = ".lst";

#else // HOOK_ENGINE

char *szCommonPreloads;
char *szReslistsBaseDir;
char *szReslistsExt;

#endif // HOOK_ENGINE

// double curtime;
// double lastcurtime;
// qboolean sc_return_on_enter;

// char findbase[260];
// char findname[260];
// char findpath[260];
// char findpattern[260];

// const char g_szExtensionDllSubDir;
// const char g_szHalfLifeDllName;
// const char g_szBaseDllName;

char gszDisconnectReason[MAX_DISCONNECT_REASON];
char gszExtendedDisconnectReason[MAX_DISCONNECT_REASON];

qboolean gfExtendedError;

int giSubState;
int giActive;
int giStateInfo;

DLL_FUNCTIONS gEntityInterface;
NEW_DLL_FUNCTIONS gNewDLLFunctions;

extensiondll_t g_rgextdll[50];

int g_iextdllMac;
qboolean gfBackground;
// int starttime;
// qboolean Win32AtLeastV4;
// int lowshift;
// double pfreq;
qboolean g_bPrintingKeepAliveDots;
// DWORD gProcessorSpeed;
#ifndef _WIN32
qboolean gHasMMXTechnology;
#endif // _WIN32
// volatile int sys_checksum;
// char *argv[MAX_NUM_ARGVS];
qboolean con_debuglog;

#ifdef REHLDS_FIXES
char g_szFindFirstFileName[MAX_PATH];
#endif

#ifdef _WIN32
int g_PerfCounterInitialized;
CRITICAL_SECTION g_PerfCounterMutex;

int g_PerfCounterShiftRightAmount;
double g_PerfCounterSlice;
double g_CurrentTime;
double g_StartTime;

int g_WinNTOrHigher;
#endif // _WIN32

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int g_FPUCW_Mask_Prec_64Bit = 0;
int g_FPUCW_Mask_Prec_64Bit_2 = 0;
int g_FPUCW_Mask_Round_Trunc = 0;
int g_FPUCW_Mask_Round_Up = 0;

FileFindHandle_t g_hfind = FILESYSTEM_INVALID_FIND_HANDLE;

#else // HOOK_ENGINE

int g_FPUCW_Mask_Prec_64Bit;
int g_FPUCW_Mask_Prec_64Bit_2;
int g_FPUCW_Mask_Round_Trunc;
int g_FPUCW_Mask_Round_Up;

FileFindHandle_t g_hfind;

#endif // HOOK_ENGINE

#ifdef _WIN32
void Sys_SetupFPUOptions()
{
	static uint8 fpuOpts[32];

	//__asm { fnstenv byte ptr fpuOpts }
	fpuOpts[0] |= 0x3Fu;
	//__asm { fldenv  byte ptr fpuOpts }
}

NOINLINE void Sys_InitFPUControlWords()
{
	int fpucw = 0;
	//__asm { fnstcw fpucw }

	g_FPUCW_Mask_Prec_64Bit = (fpucw & 0xF0FF) | 0x300;
	g_FPUCW_Mask_Prec_64Bit_2 = (fpucw & 0xF0FF) | 0x300;
	g_FPUCW_Mask_Round_Trunc = (fpucw & 0xF0FF) | 0xC00;
	g_FPUCW_Mask_Round_Up = (fpucw & 0xF0FF) | 0x800;
}

void __cdecl Sys_SetStartTime()
{
	int startTimeArg;

	Sys_FloatTime();
	startTimeArg = COM_CheckParm("-starttime");
	if(startTimeArg)
		g_CurrentTime = Q_atof(com_argv[startTimeArg + 1]);
	else
		g_CurrentTime = 0;

	g_StartTime = g_CurrentTime;
}

void __cdecl Sys_InitHardwareTimer()
{
	unsigned int perfHighPart;
	unsigned int perfLowPart;
	LARGE_INTEGER perfFreq;

	if(!g_PerfCounterInitialized)
	{
		InitializeCriticalSection(&g_PerfCounterMutex);
		g_PerfCounterInitialized = 1;
	}
	Sys_SetupFPUOptions();
	Sys_InitFPUControlWords();

	//if(!CRehldsPlatformHolder::get()->QueryPerfFreq(&perfFreq))
		//Sys_Error("No hardware timer available");

	perfHighPart = perfFreq.HighPart;
	perfLowPart = perfFreq.LowPart;
	g_PerfCounterShiftRightAmount = 0;
	while(perfHighPart || perfLowPart > 2000000.0)
	{
		g_PerfCounterShiftRightAmount++;
		perfLowPart = (perfHighPart << 31) | (perfLowPart >> 1);
		perfHighPart >>= 1;
	}
	g_PerfCounterSlice = 1.0 / (double)perfLowPart;

	Sys_CheckOSVersion();
	Sys_SetStartTime();
}

int g_SavedFPUCW1 = 0;
NOINLINE void Sys_FPUCW_Push_Prec64()
{
	uint16 tmp = g_FPUCW_Mask_Prec_64Bit;
	//__asm { fnstcw  g_SavedFPUCW1 }
	//__asm {fldcw tmp}
}

NOINLINE void Sys_FPUCW_Pop_Prec64()
{
	uint16 tmp = g_SavedFPUCW1;
	//__asm { fldcw tmp }
}

#endif // _WIN32

NOXREF void Sys_PageIn(void *ptr, int size)
{
	NOXREFCHECK;
}

// TODO: investigate filesystem_stdio problem (multiple enumeration of files).
const char *Sys_FindFirst(const char *path, char *basename)
{
	if(g_hfind != -1)
		Sys_Error("%s without close", __FUNCTION__);

	const char *psz = FS_FindFirst(path, &g_hfind, 0);

#ifdef REHLDS_FIXES
	// Hack: store first file name to fix multiple enumeration of files in the
	// filesystem module
	if(psz != NULL)
	{
		Q_strncpy(g_szFindFirstFileName, psz, MAX_PATH - 1);
	}
#endif // REHLDS_FIXES

	if(basename && psz)
	{
		COM_FileBase((char *)psz, basename);
	}

	return psz;
}

const char *Sys_FindFirstPathID(const char *path, char *pathid)
{
	// const char *psz;//unused?
	if(g_hfind != -1)
		Sys_Error("Sys_FindFirst without close");
	return FS_FindFirst(path, &g_hfind, pathid);
}

// TODO: investigate filesystem_stdio problem (multiple enumeration of files).
const char *Sys_FindNext(char *basename)
{
	const char *psz = FS_FindNext(g_hfind);

#ifdef REHLDS_FIXES
	// Hack: stop if we are starting over again
	if(psz && !Q_strcmp(g_szFindFirstFileName, psz))
	{
		return NULL;
	}
#endif // REHLDS_FIXES

	if(basename && psz)
	{
		COM_FileBase((char *)psz, basename);
	}

	return psz;
}

void Sys_FindClose()
{
	if(g_hfind != -1)
	{
		FS_FindClose(g_hfind);
		g_hfind = -1;
	}

#ifdef REHLDS_FIXES
	g_szFindFirstFileName[0] = 0;
#endif // REHLDS_FIXES
}

NOBODY int glob_match_after_star(char *pattern, char *text);
//{
//	char *p;                                                     //   282
//	char *t;                                                     //   282
//	char c;                                                       //   283
//	char c1;                                                      //   283
//}

NOBODY int glob_match(char *pattern, char *text);
//{
//	char *p;                                                     //   325
//	char *t;                                                     //   325
//	char c;                                                       //   326
//
// match :                                                                //
// 386;
//	{
//		char c1;                                              //   347
//		int invert;                                           //   348
//		{
//			char cstart;                                  //   359
//			char cend;                                    //   359
//		}
//	}
//	glob_match_after_star(char *pattern,
//				char *text);   //   343
//}

NOXREF void Sys_MakeCodeWriteable(uint32 startaddr, uint32 length)
{
	NOXREFCHECK;
	
#ifdef _WIN32
	if(!VirtualProtect((LPVOID)startaddr, length, PAGE_EXECUTE_READWRITE, (PDWORD)&length))
		Sys_Error("Protection change failed.");
#endif // _WIN32
}

NOBODY void Sys_SetFPCW();
//{
//}

NOBODY void Sys_PushFPCW_SetHigh();
//{
//}

NOBODY void Sys_PopFPCW();
//{
//}

NOBODY void MaskExceptions();
//{
//}

NOBODY void Sys_Init();

NOXREF void Sys_Sleep(int msec)
{
	NOXREFCHECK;
	
#ifdef _WIN32
	Sleep(msec);
#else
	usleep(1000 * msec);
#endif // _WIN32
}

NOBODY void Sys_DebugOutStraight(const char *pStr);
//{
//}

void NORETURN Sys_Error(const char *error, ...)
{
	va_list argptr;
	char text[1024];
	static qboolean bReentry;

	va_start(argptr, error);
	Q_vsnprintf(text, ARRAYSIZE(text), error, argptr);
	va_end(argptr);

#ifdef _WIN32
	MessageBox(GetForegroundWindow(), text, "Fatal error - Dedicated server", MB_ICONERROR | MB_OK);
#endif // _WIN32

	if(bReentry)
	{
		fprintf(stderr, "%s\n", text);
		longjmp(host_abortserver, 2);
	}
	bReentry = true;

	if(g_psvs.dll_initialized && gEntityInterface.pfnSys_Error)
		gEntityInterface.pfnSys_Error(text);

	Log_Printf("FATAL ERROR (shutting down): %s\n", text);

#ifdef REHLDS_FIXES
	if(syserror_logfile.string[0] != '\0')
	{
		auto pFile = FS_Open(syserror_logfile.string, "a");
		if(pFile)
		{
			tm *today;
			time_t ltime;
			char szDate[32];

			time(&ltime);
			today = localtime(&ltime);
			strftime(szDate, ARRAYSIZE(szDate) - 1, "L %d/%m/%Y - %H:%M:%S:", today);

			FS_FPrintf(pFile, "%s (map \"%s\") %s\n", szDate, &pr_strings[gGlobalVariables.mapname], text);
			FS_Close(pFile);
		}
	}
#endif // REHLDS_FIXES

	if(g_bIsDedicatedServer)
	{
		if(Launcher_ConsolePrintf)
			Launcher_ConsolePrintf("FATAL ERROR (shutting down): %s\n", text);
		else
			printf("FATAL ERROR (shutting down): %s\n", text);
	}
#ifndef SWDS
	else
	{
		HWND hWnd = 0;
		//if(pmainwindow)
			//hWnd = *pmainwindow;

		Sys_Printf(text);
		//SDL_ShowSimpleMessageBox(MB_ICONERROR | MB_OK, "Fatal Error", text, hWnd);
		//VideoMode_IsWindowed();
	}
#endif // SWDS

	// exit(-1);
	// Allahu akbar!
	*(int *)NULL = NULL;
}

NOXREF void Sys_Warning(const char *pszWarning, ...)
{
	NOXREFCHECK;
	
	va_list argptr;
	char text[1024];

	va_start(argptr, pszWarning);
	Q_vsnprintf(text, sizeof(text), pszWarning, argptr);
	va_end(argptr);

	Sys_Printf(text);
}

void Sys_Printf(const char *fmt, ...)
{
	char Dest[1024];
	va_list va;

	va_start(va, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, va);
	va_end(va);

	if(g_bIsDedicatedServer && Launcher_ConsolePrintf)
		Launcher_ConsolePrintf("%s", Dest);

#ifdef _WIN32
	OutputDebugStringA(Dest);
#else
	if(!g_bIsDedicatedServer)
		fprintf(stderr, "%s\n", Dest);
#endif // _WIN32
}

void Sys_Quit()
{
	giActive = DLL_CLOSE;
}

#ifdef _WIN32

double EXT_FUNC Sys_FloatTime()
{
	unsigned int currentTime;
	int savedOldTime;
	LARGE_INTEGER PerformanceCount;

	static bool s_NeedInit = true;
	static unsigned int s_oldTime = 0;
	static int s_timeNotChangedCounter = 0;

	if(!g_PerfCounterInitialized)
		return 1.0;

	EnterCriticalSection(&g_PerfCounterMutex);
	Sys_FPUCW_Push_Prec64();

	//CRehldsPlatformHolder::get()->QueryPerfCounter(&PerformanceCount);
	if(g_PerfCounterShiftRightAmount)
		currentTime =
		(PerformanceCount.LowPart >> g_PerfCounterShiftRightAmount) |
		(PerformanceCount.HighPart << (32 - g_PerfCounterShiftRightAmount));
	else
		currentTime = PerformanceCount.LowPart;

	if(!s_NeedInit)
	{
		savedOldTime = s_oldTime;
		if(currentTime <= s_oldTime && s_oldTime - currentTime < 0x10000000)
		{
			s_oldTime = currentTime;
		}
		else
		{
			s_oldTime = currentTime;
			g_CurrentTime = g_CurrentTime +
			(double)(currentTime - savedOldTime) * g_PerfCounterSlice;
			if(g_CurrentTime == g_StartTime)
			{
				if(s_timeNotChangedCounter >= 100000)
				{
					g_CurrentTime = g_CurrentTime + 1.0;
					s_timeNotChangedCounter = 0;
				}
			}
			else
			{
				s_timeNotChangedCounter = 0;
			}
			g_StartTime = g_CurrentTime;
		}
	}
	else
	{
		s_oldTime = currentTime;
		s_NeedInit = false;
	}

	Sys_FPUCW_Pop_Prec64();
	LeaveCriticalSection(&g_PerfCounterMutex);
	return g_CurrentTime;
}

#else // not _WIN32

double Sys_FloatTime()
{
	static struct timespec start_time;
	static bool bInitialized;
	struct timespec now;

	if(!bInitialized)
	{
		bInitialized = 1;
		clock_gettime(1, &start_time);
	}
	clock_gettime(1, &now);
	return (now.tv_sec - start_time.tv_sec) + now.tv_nsec * 0.000000001;
}

#endif //_WIN32

void Dispatch_Substate(int iSubState)
{
	giSubState = iSubState;
}

void GameSetSubState(int iSubState)
{
	if(iSubState & 2)
		Dispatch_Substate(1);
	else if(iSubState != 1)
		Dispatch_Substate(iSubState);
}

void GameSetState(int iState)
{
	giActive = iState;
}

NOBODY void GameSetBackground(qboolean bNewSetting);
//{
//}

DISPATCHFUNCTION GetDispatch(char *pname)
{
	DISPATCHFUNCTION pDispatch;

	for(int i = 0; i < g_iextdllMac; i++)
	{
		pDispatch = (DISPATCHFUNCTION)GetProcAddress(
		(HMODULE)g_rgextdll[i].lDLLHandle, pname);
		if(pDispatch)
			return pDispatch;
	};

	return NULL;
};

const char *FindAddressInTable(extensiondll_t *pDll, uint32 function)
{
#ifdef _WIN32
	for(int i = 0; i < pDll->functionCount; i++)
	{
		if(pDll[i].functionTable->pFunction == function)
		{
			return pDll[i].functionTable->pFunctionName;
		}
	}
#else  // _WIN32
	Dl_info addrInfo;
	if(dladdr((void *)function, &addrInfo))
	{
		return addrInfo.dli_sname;
	}
#endif // _WIN32

	return NULL;
}

uint32 FindNameInTable(extensiondll_t *pDll, const char *pName)
{
#ifdef _WIN32
	for(int i = 0; i < pDll->functionCount; i++)
	{
		if(!Q_strcmp(pName, pDll->functionTable[i].pFunctionName))
		{
			return pDll[i].functionTable->pFunction;
		}
	}
	return NULL;
#else
	return (uint32)dlsym(pDll->lDLLHandle, pName);
#endif // _WIN32
}

NOBODY const char *ConvertNameToLocalPlatform(const char *pchInName);
//{
//	char s_szNewName;                                             //  1409
//	{
//		char szTempName;                                      //  1463
//		char *pchAt;                                         //  1464
//		char *pchClassName;                                  //  1465
//		char *pchFunctionName;                               //  1466
//		FindNameInTable(extensiondll_t *pDll,
//				const char *pName);  / //  1483
//		FindNameInTable(extensiondll_t *pDll,
//				const char *pName);   //  1487
//		FindNameInTable(extensiondll_t *pDll,
//				const char *pName);   //  1491
//	}
//}

uint32 EXT_FUNC FunctionFromName(const char *pName)
{
	return 0; // TODO: do we really need to reverse it?
}

const char *EXT_FUNC NameForFunction(uint32 function)
{
	int i;
	const char *pName;

	for(i = 0; i < g_iextdllMac; i++)
	{
		pName = FindAddressInTable(&g_rgextdll[i], function);
		if(pName)
		{
			return pName;
		}
	}

	Con_Printf("Can't find address: %08lx\n", function);
	return NULL;
}

ENTITYINIT GetEntityInit(char *pClassName)
{
	return (ENTITYINIT)GetDispatch(pClassName);
}

FIELDIOFUNCTION GetIOFunction(char *pName)
{
	return (FIELDIOFUNCTION)GetDispatch(pName);
}

void EXT_FUNC EngineFprintf(void *pfile, const char *szFmt, ...)
{
	AlertMessage(at_console, "EngineFprintf:  Obsolete API\n");
}

void EXT_FUNC AlertMessage(ALERT_TYPE atype, const char *szFmt, ...)
{
	va_list argptr;
	static char szOut[1024];

	va_start(argptr, szFmt);
	if(atype == at_logged && g_psvs.maxclients > 1)
	{
		Q_vsnprintf(szOut, sizeof(szOut), szFmt, argptr);
		Log_Printf("%s", szOut);
	}
	else if(developer.value != 0.0f)
	{
		switch(atype)
		{
		case at_notice:
			Q_strcpy(szOut, "NOTE:  ");
			break;
		case at_console:
			szOut[0] = 0;
			break;
		case at_aiconsole:
			if(developer.value < 2.0f)
				return;
			szOut[0] = 0;
			break;
		case at_warning:
			Q_strcpy(szOut, "WARNING:  ");
			break;
		case at_error:
			Q_strcpy(szOut, "ERROR:  ");
			break;
		case at_logged:
			break;
		default:
			break;
		}
		int iLen = Q_strlen(szOut);
		Q_vsnprintf(&szOut[iLen], sizeof(szOut) - iLen, szFmt, argptr);
		Con_Printf("%s", szOut);
	}
	va_end(argptr);
}

NOXREF void Sys_SplitPath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
	NOXREFCHECK;
	
#ifdef _WIN32
	_splitpath(path, drive, dir, fname, ext);
#else // _WIN32

	char *p;
	char *last_slash = NULL;
	char *dot = NULL;
	unsigned int len;

	if(path[0] && path[1] == ':')
	{
		if(drive)
		{
			Q_strncpy(drive, path, 2);
			drive[2] = 0;
		}
		path += 2;
	}
	else if(drive)
		drive[0] = 0;

	for(p = (char *)path; *p; p++)
	{
		if(*p == '\\' || *p == '/')
			last_slash = ++p;
	}

	if(last_slash)
	{
		if(dir)
		{
			len = (unsigned int)(last_slash - path);
			if(len > 0xFF)
				len = 0xFF;

			Q_strncpy(dir, path, len);
			dir[len] = 0;
		}
		path = last_slash;
	}
	else if(dir)
		dir[0] = 0;

	for(p = (char *)path; *p; p++)
	{
		if(*p == '.')
			dot = p;
	}

	if(!dot)
		dot = p;

	if(fname)
	{
		len = (unsigned int)(dot - path);
		if(len > 0xFF)
			len = 0xFF;

		Q_strncpy(fname, path, len);
		fname[len] = 0;
	}

	if(ext)
	{
		len = (unsigned int)(dot - path);
		if(len > 0xFF)
			len = 0xFF;

		Q_strncpy(ext, dot, len);
		ext[len] = 0;
	}

#endif // _WIN32
}

const char *GetCurrentSteamAppName()
{
	if(!Q_stricmp(com_gamedir, "cstrike") ||
	   !Q_stricmp(com_gamedir, "cstrike_beta"))
		return "Counter-Strike";

	else if(!Q_stricmp(com_gamedir, "valve"))
		return "Half-Life";

	else if(!Q_stricmp(com_gamedir, "ricochet"))
		return "Ricochet";

	else if(!Q_stricmp(com_gamedir, "dod"))
		return "Day of Defeat";

	else if(!Q_stricmp(com_gamedir, "tfc"))
		return "Team Fortress Classic";

	else if(!Q_stricmp(com_gamedir, "dmc"))
		return "Deathmatch Classic";

	else if(!Q_stricmp(com_gamedir, "czero"))
		return "Condition Zero";

	return "Half-Life";
}

NOXREF void SetRateRegistrySetting(const char *pchRate)
{
	NOXREFCHECK;
	
	registry->WriteString("rate", pchRate);
}

NOXREF const char *GetRateRegistrySetting(const char *pchDef)
{
	NOXREFCHECK;
	
	return registry->ReadString("rate", pchDef);
}

void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated)
{
	char key[65];
	char hostname[4096];

	//if(CRehldsPlatformHolder::get()->gethostname(hostname, sizeof(hostname)))
		//Q_snprintf(key, sizeof(key), "%u", RandomLong(0, 0x7FFFFFFF));
	//else
	{
		struct hostent *hostinfo;
		//hostinfo = CRehldsPlatformHolder::get()->gethostbyname(hostname);
		if(hostinfo && hostinfo->h_length == 4 && *hostinfo->h_addr_list != NULL)
		{
			Q_snprintf(key, sizeof(key), "%u.%u.%u.%u", (*hostinfo->h_addr_list)[0], (*hostinfo->h_addr_list)[1], (*hostinfo->h_addr_list)[2], (*hostinfo->h_addr_list)[3]);
		}
		else
		{
			CRC32_t crc;
#ifdef REHLDS_FIXES
			crc = 0;
#endif
			CRC32_ProcessBuffer(&crc, hostname, Q_strlen(hostname));
			Q_snprintf(key, sizeof(key), "%u", crc);
		}
	}
	key[64] = 0;
	Q_strcpy(pszCDKey, key);

	if(nLength)
		*nLength = Q_strlen(key);

	if(bDedicated)
		*bDedicated = 0;
}

NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage)
{
	NOXREFCHECK;
}

void Legacy_Sys_Printf(char *fmt, ...)
{
	va_list argptr;
	char text[1024];

	va_start(argptr, fmt);
	Q_vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	if(dedicated_)
		dedicated_->Sys_Printf(text);
}

NOXREF void Legacy_MP3subsys_Suspend_Audio()
{
	NOXREFCHECK;
}

NOXREF void Legacy_MP3subsys_Resume_Audio()
{
	NOXREFCHECK;
}

void Sys_SetupLegacyAPIs()
{
#ifndef SWDS
	//VID_FlipScreen = Sys_VID_FlipScreen;
	//D_SurfaceCacheForRes = Sys_GetSurfaceCacheSize;
#endif // SWDS
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
}

NOXREF qboolean Sys_IsWin95()
{
	NOXREFCHECK;
	
#ifdef _WIN32
	return g_bIsWin95;
#else
	// TODO: no need to check is win
	return FALSE;
#endif // _WIN32
}

NOXREF qboolean Sys_IsWin98()
{
	NOXREFCHECK;
	
#ifdef _WIN32
	return g_bIsWin98;
#else
	// TODO: no need to check is win
	return FALSE;
#endif // _WIN32
}

#ifdef _WIN32

// Hacky
#pragma warning( push )  
#pragma warning( disable : 4996 )

// It's kinda useless to check for such old versions of Win
// Most servers are running on Linux anyway

void Sys_CheckOSVersion()
{
	OSVERSIONINFOA verInfo;

	Q_memset(&verInfo, 0, sizeof(verInfo));
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	if(!GetVersionEx(&verInfo))
		Sys_Error("Couldn't get OS info");

	g_WinNTOrHigher = verInfo.dwMajorVersion >= 4;
	if(verInfo.dwPlatformId == 1 && verInfo.dwMajorVersion == 4)
	{
		if(verInfo.dwMinorVersion)
		{
			if(verInfo.dwMinorVersion < 90)
				g_bIsWin98 = TRUE;
		}
		else
			g_bIsWin95 = TRUE;
	}
}

#pragma warning ( pop )

#endif // _WIN32

void Sys_Init()
{
#ifndef SWDS
	//Sys_InitFloatTime();
#endif
}

void Sys_Shutdown()
{
#ifndef SWDS
	//Sys_ShutdownFloatTime();
#endif // SWDS
	//Steam_ShutdownClient();
#ifdef _WIN32
	if(g_PerfCounterInitialized)
	{
		DeleteCriticalSection(&g_PerfCounterMutex);
		g_PerfCounterInitialized = 0;
	}
#else
#ifndef SWDS
	GL_Shutdown(*pmainwindow, maindc, baseRC);
#endif // SWDS

#endif // _WIN32
}

void Sys_InitArgv(char *lpCmdLine)
{
	static char *argv[MAX_COMMAND_LINE_PARAMS];
	unsigned char c;
#ifdef REHLDS_FIXES
	bool inQuotes;
#endif

	argv[0] = "";
	c = *lpCmdLine;
	for(host_parms.argc = 1; c && host_parms.argc < MAX_COMMAND_LINE_PARAMS;
	    c = *(++lpCmdLine))
	{
#ifdef REHLDS_FIXES
		// Skip whitespace
		while(c && c <= ' ')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
		if(!c)
		{
			break;
		}

		// TODO: Add MultiByteToWideChar conversion under Windows, to correctly get
		// UTF8, but need to alloc memory to store it
		// Store arg pointer
		argv[host_parms.argc] = lpCmdLine;
		host_parms.argc++;

		// Find end of the argument
		inQuotes = false;
		while(c > ' ' || (c && inQuotes)) // FIXED: Do not break quoted arguments
		{
			if(c == '"')
			{
				inQuotes = !inQuotes;
			}
			lpCmdLine++;
			c = *lpCmdLine;
		}
#else  // REHLDS_FIXES
		// Skip whitespace and UTF8
		while(c && (c <= ' ' || c > '~'))
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
		if(!c)
		{
			break;
		}

		// Store arg pointer
		argv[host_parms.argc] = lpCmdLine;
		host_parms.argc++;

		// Find end of the argument
		while(c > ' ' && c <= '~')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
#endif // REHLDS_FIXES
		if(!c)
		{
			break;
		}
		*lpCmdLine = 0;
	}

	host_parms.argv = argv;
	COM_InitArgv(host_parms.argc, argv);
	host_parms.argc = com_argc;
	host_parms.argv = com_argv;
}

NOXREF void Sys_ShutdownArgv()
{
	NOXREFCHECK;
}

void Sys_InitMemory()
{
	int i;

	i = COM_CheckParm("-heapsize");
	if(i && i < com_argc - 1)
		host_parms.memsize = Q_atoi(com_argv[i + 1]) * 1024;

	if(host_parms.memsize < MINIMUM_WIN_MEMORY)
	{
#ifdef _WIN32
		MEMORYSTATUS lpBuffer;
		lpBuffer.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer);

		if(lpBuffer.dwTotalPhys)
		{
			if(lpBuffer.dwTotalPhys < FIFTEEN_MB)
				Sys_Error("Available memory less than 15MB!!! %i", host_parms.memsize);

			host_parms.memsize = (int)(lpBuffer.dwTotalPhys >> 1);
			if(host_parms.memsize < MINIMUM_WIN_MEMORY)
				host_parms.memsize = MINIMUM_WIN_MEMORY;
		}
		else
			host_parms.memsize = MAXIMUM_WIN_MEMORY;

		if(g_bIsDedicatedServer)
			host_parms.memsize = DEFAULT_MEMORY;
#else
		host_parms.memsize = DEFAULT_MEMORY;
#endif // _WIN32
	}

	if(host_parms.memsize > MAXIMUM_DEDICATED_MEMORY)
		host_parms.memsize = MAXIMUM_DEDICATED_MEMORY;

	if(COM_CheckParm("-minmemory"))
		host_parms.memsize = MINIMUM_WIN_MEMORY;
#ifdef _WIN32
	host_parms.membase = (void *)GlobalAlloc(GMEM_FIXED, host_parms.memsize);
#else
	host_parms.membase = Mem_Malloc(host_parms.memsize);
#endif // _WIN32

	if(!host_parms.membase)
		Sys_Error("Unable to allocate %.2f MB\n",
		          (float)host_parms.memsize / (1024.0f * 1024.0f));
}

void Sys_ShutdownMemory()
{
#ifdef _WIN32
	GlobalFree((HGLOBAL)host_parms.membase);
#else
	Mem_Free(host_parms.membase);
#endif // _WIN32
	host_parms.membase = NULL;
	host_parms.memsize = 0;
}

void Sys_InitLauncherInterface()
{
#ifdef _WIN32
	// TODO: client-side code
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
#else
#ifndef SWDS
	gHasMMXTechnology = TRUE;
#endif

	Sys_SetupLegacyAPIs();
#endif // _WIN32
}

NOXREF void Sys_ShutdownLauncherInterface()
{
	NOXREFCHECK;
}

void Sys_InitAuthentication()
{
	Sys_Printf("STEAM Auth Server\r\n");
}

NOXREF void Sys_ShutdownAuthentication()
{
	NOXREFCHECK;
}

void Sys_ShowProgressTicks(char *specialProgressMsg)
{
	static bool recursionGuard = false;
	static int32 numTics = 0;

	double currentTime;
	if(!recursionGuard)
	{
		recursionGuard = true;
		if(COM_CheckParm("-steam"))
		{
			currentTime = Sys_FloatTime();

			if(g_flLastSteamProgressUpdateTime + 2.0 <= currentTime)
			{
				g_flLastSteamProgressUpdateTime = currentTime;
				numTics++;
				if(g_bIsDedicatedServer)
				{
					//if(g_bMajorMapChange)
					//{
						//g_bPrintingKeepAliveDots = TRUE;
						//Sys_Printf(".");
						//recursionGuard = false;
						//return;
					//}
				}
				else
				{
					int i;
					int numTicsToPrint = (numTics % 5 + 1);
					char msg[128];

					Q_strncpy(msg, "Updating game resources", sizeof(msg) - 1);
					msg[sizeof(msg) - 1] = '\0';

					for(i = 1; i < numTicsToPrint; i++)
					{
						Q_strncat(msg, ".", sizeof(msg) - 1);
						msg[sizeof(msg) - 1] = '\0';
					}
					//SetLoadingProgressBarStatusText(msg);
				}
			}
		}
		recursionGuard = false;
	}
}

int Sys_InitGame(char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated)
{
	host_initialized = FALSE;

#ifndef SWDS
	if(!bIsDedicated)
	{
		//pmainwindow = (HWND *)pwnd;
#ifdef _WIN32
		//videomode->UpdateWindowPosition();
#endif // _WIN32
	}
#endif // SWDS
	g_bIsDedicatedServer = bIsDedicated;
	Q_memset(&gmodinfo, 0, sizeof(modinfo_t));
	SV_ResetModInfo();
	TraceInit("Sys_Init()", "Sys_Shutdown()", 0);

#ifdef _WIN32
	Sys_InitHardwareTimer();
#endif // _WIN32

	//Sys_CheckCpuInstructionsSupport();

#ifndef SWDS
	//Sys_InitFloatTime();
#endif // SWDS
	FS_LogLevelLoadStarted("Launcher");
	//SeedRandomNumberGenerator();
	TraceInit("Sys_InitMemory()", "Sys_ShutdownMemory()", 0);
	Sys_InitMemory();
	TraceInit("Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0);
	Sys_InitLauncherInterface();

#ifndef SWDS
	//if(!GL_SetMode(*pmainwindow, &maindc, &baseRC))
		//return 0;
#endif // SWDS
	TraceInit("Host_Init( &host_parms )", "Host_Shutdown()", 0);
	Host_Init(&host_parms);
	if(!host_initialized)
		return 0;

	TraceInit("Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0);
	Sys_InitAuthentication();
	if(g_bIsDedicatedServer)
	{
		Host_InitializeGameDLL();
		NET_Config(TRUE);
	}

#ifndef SWDS
	else
		ClientDLL_ActivateMouse();

	char MessageText[512];
	char en_US[12];

	Q_strcpy(en_US, "en_US.UTF-8");
	//en_US[16] = 0; // nice try

	char *cat = setlocale(6, NULL);
	if(!cat)
		cat = "c";

	if(Q_stricmp(cat, en_US))
	{
		Q_snprintf(MessageText, sizeof(MessageText), "SetLocale('%s') failed. Using '%s'.\nYou may have limited "
		                                             "glyph support.\nPlease install '%s' locale.",
		           en_US,
		           cat,
		           en_US);
		//SDL_ShowSimpleMessageBox(0, "Warning", MessageText, *pmainwindow);
	}
#endif // SWDS
	return 1;
}

void Sys_ShutdownGame()
{
	if(!g_bIsDedicatedServer)
		ClientDLL_DeactivateMouse();

	TraceShutdown("Host_Shutdown()", 0);
	Host_Shutdown();

	if(g_bIsDedicatedServer)
		NET_Config(FALSE);

	TraceShutdown("Sys_ShutdownLauncherInterface()", 0);
	TraceShutdown("Sys_ShutdownAuthentication()", 0);
	TraceShutdown("Sys_ShutdownMemory()", 0);
	Sys_ShutdownMemory();
	TraceShutdown("Sys_Shutdown()", 0);
	Sys_Shutdown();
}

void ClearIOStates()
{
#ifndef SWDS
	int i;
	for(i = 0; i < 256; i++)
	{
		Key_Event(i, false);
	}
	Key_ClearStates();
	ClientDLL_ClearStates();
#endif // SWDS
}

// TODO: Needs rechecking
/*
NOXREF int BuildMapCycleListHints(char **hints)
{
	NOXREFCHECK;
	
        char szMap[262];
        unsigned int length;
        char *pFileList;
        char szMod[MAX_PATH];
        char cszMapCycleTxtFile[MAX_PATH];
        FileHandle_t pFile;

        COM_FileBase(com_gamedir, szMod);
        Q_sprintf(cszMapCycleTxtFile, "%s/%s", szMod, mapcyclefile.string);
        pFile = FS_Open(cszMapCycleTxtFile, "rb");
        if (!pFile)
        {
                Con_Printf("Unable to open %s\n", cszMapCycleTxtFile);
                return 0;
        }
        Q_sprintf(szMap, "%s\\%s\\%s%s\r\n", szReslistsBaseDir,
GetCurrentSteamAppName(), szCommonPreloads, szReslistsExt);

        *hints = (char *)Mem_Malloc(strlen(szMap) + 1);
        if (*hints == NULL)
        {
                Con_Printf("Unable to allocate memory for map cycle hints
list");
                return 0;
        }
        Q_strcpy(*hints, szMap);
        length = FS_Size(pFile);
        if (length)
        {
                pFileList = (char *)malloc(length);
                if (pFileList && FS_Read(pFileList, length, 1, pFile) != 1)
                {
                        while (1)
                        {
                                pFileList = COM_Parse(pFileList);
                                if (strlen(com_token) <= 0)
                                {
                                        Mem_Free(*hints);
                                        break;
                                }

                                Q_strncpy(szMap, com_token, sizeof(szMap) - 1);
                                szMap[sizeof(szMap) - 1] = '\0';

                                if (COM_TokenWaiting(pFileList))
                                        pFileList = COM_Parse(pFileList);

                                char mapLine[sizeof(szMap)];
                                Q_snprintf(mapLine, sizeof(mapLine),
"%s\\%s\\%s%s\r\n", szReslistsBaseDir, szMod, szMap, szReslistsExt);

                                *hints = (char *)Mem_Realloc(*hints,
strlen(*hints) + 1 + strlen(mapLine) + 1);
                                if (*hints == NULL)
                                {
                                        Con_Printf("Unable to reallocate memory
for map cycle hints list");
                                        return 0;
                                }
                                Q_strcat(*hints, mapLine);
                        }
                }
        }
        FS_Close(pFile);
        Q_sprintf(szMap, "%s\\%s\\mp_maps.txt\r\n", szReslistsBaseDir,
GetCurrentSteamAppName());
        *hints = (char *)Mem_Realloc(*hints, strlen(*hints) + 1 + strlen(szMap)
+ 1);
        Q_strcat(*hints, szMap);
        return 1;
}
*/