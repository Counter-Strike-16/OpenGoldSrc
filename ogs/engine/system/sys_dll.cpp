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

//#include "precompiled.hpp"
#include "system/system.hpp"
#include "voice/voice.hpp"

void (*Launcher_ConsolePrintf)(char *, ...);
char *(*Launcher_GetLocalizedString)(unsigned int);

int (*Launcher_MP3subsys_Suspend_Audio)();
void (*Launcher_MP3subsys_Resume_Audio)();

void (*VID_FlipScreen)();

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
modinfo_t gmodinfo;
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

	__asm { fnstenv byte ptr fpuOpts }
	fpuOpts[0] |= 0x3Fu;
	__asm { fldenv  byte ptr fpuOpts }
}

NOINLINE void Sys_InitFPUControlWords()
{
	int fpucw = 0;
	__asm { fnstcw fpucw }

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

	if(!CRehldsPlatformHolder::get()->QueryPerfFreq(&perfFreq))
		Sys_Error("No hardware timer available");

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
	__asm { fnstcw  g_SavedFPUCW1 }
	__asm
	{
		fldcw tmp
	}
}

NOINLINE void Sys_FPUCW_Pop_Prec64()
{
	uint16 tmp = g_SavedFPUCW1;
	__asm { fldcw tmp }
}

#endif // _WIN32

NOXREF void Sys_PageIn(void *ptr, int size)
{
}

// TODO: investigate filesystem_stdio problem (multiple enumeration of files).
const char *Sys_FindFirst(const char *path, char *basename)
{
	if(g_hfind != -1)
		Sys_Error(__FUNCTION__ " without close");

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
#ifdef _WIN32
	Sleep(msec);
#else
	usleep(1000 * msec);
#endif // _WIN32
}

NOBODY void Sys_DebugOutStraight(const char *pStr);
//{
//}

void __declspec(noreturn) Sys_Error(const char *error, ...)
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
		if(pmainwindow)
			hWnd = *pmainwindow;

		Sys_Printf(text);
		SDL_ShowSimpleMessageBox(MB_ICONERROR | MB_OK, "Fatal Error", text, hWnd);
		VideoMode_IsWindowed();
	}
#endif // SWDS

	// exit(-1);
	// Allahu akbar!
	*(int *)NULL = NULL;
}

NOXREF void Sys_Warning(const char *pszWarning, ...)
{
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

	CRehldsPlatformHolder::get()->QueryPerfCounter(&PerformanceCount);
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

#ifdef _WIN32
HMODULE LoadWindowsDLL(LPCSTR lpLibFileName)
{
	if(!lpLibFileName)
		return NULL;

	FS_GetLocalCopy(lpLibFileName);
	return LoadLibraryA(lpLibFileName);
}
#endif //_WIN32

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