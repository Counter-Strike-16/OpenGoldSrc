/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#include <thread>
#include "precompiled.hpp"
#include "system/System.hpp"
#include "system/Host.hpp"
#include "system/common.hpp"
#include "system/SystemTypes.hpp"
#include "system/DedicatedServerAPI.hpp"
#include "public/idedicatedexports.h"

char *(*Launcher_GetLocalizedString)(unsigned int);

int (*Launcher_MP3subsys_Suspend_Audio)();
void (*Launcher_MP3subsys_Resume_Audio)();

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

FileFindHandle_t g_hfind = FILESYSTEM_INVALID_FIND_HANDLE;

#else // HOOK_ENGINE

FileFindHandle_t g_hfind;

#endif // HOOK_ENGINE

const int MAX_COMMAND_LINE_PARAMS = 50;

quakeparms_t *CSystem::mhost_parms = nullptr;

IDedicatedServerExports *CSystem::mpRemoteConsole = nullptr;
CFileSystem *CSystem::mpFileSystem = nullptr;

bool CSystem::mbDedicatedServer = false;

bool CSystem::mbIsWin95 = false;
bool CSystem::mbIsWin98 = false;

void CSystem::Init(quakeparms_t *host_parms, CFileSystem *apFileSystem, IDedicatedExports *apRemoteConsole, bool abDedicatedServer)
	: mhost_parms(host_parms), mpFileSystem(apFileSystem), mpRemoteConsole(apRemoteConsole), mbDedicatedServer(abDedicatedServer)
{
#ifdef _WIN32
	//InitHardwareTimer();
#endif // _WIN32

	//CheckCpuInstructionsSupport();

#ifndef SWDS
	InitFloatTime();
#endif
	
	//TraceInit("Sys_InitMemory()", "Sys_ShutdownMemory()", 0);
	//InitMemory(&host_parms);
	
	//TraceInit("Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0);
	InitLauncherInterface();
};

void CSystem::Shutdown()
{
	//TraceShutdown("Sys_ShutdownLauncherInterface()", 0);
	ShutdownLauncherInterface();

	//TraceShutdown("Sys_ShutdownAuthentication()", 0);
	ShutdownAuthentication();

	//TraceShutdown("Sys_ShutdownMemory()", 0);
	//ShutdownMemory();
	
#ifndef SWDS
	ShutdownFloatTime();
#endif // SWDS
	
	//Steam_ShutdownClient(); // why here?
	
#ifdef _WIN32
	//if(g_PerfCounterInitialized)
	{
		//DeleteCriticalSection(&g_PerfCounterMutex);
		//g_PerfCounterInitialized = 0;
	};
	
#else // if not _WIN32
	
#ifndef SWDS
	GL_Shutdown(*pmainwindow, maindc, baseRC); // why here?
#endif // SWDS

#endif // _WIN32
};

#ifndef SWDS

/*
================
Sys_InitFloatTime
================
*/
void CSystem::InitFloatTime()
{
	// Win-only?
#ifdef _WIN32
	
	GetFloatTime();

	int j = gpCmdLine->CheckArg("-starttime");

	//if(j)
		//curtime = (double)(Q_atof(com_argv[j + 1]));
	//else
		//curtime = 0.0f;

	//lastcurtime = curtime;
#endif
};

void CSystem::ShutdownFloatTime()
{
	// Nothing?
};
#endif // SWDS

void CSystem::InitArgv(char *lpCmdLine)
{
	static char *argv[MAX_COMMAND_LINE_PARAMS];
	unsigned char c;
#ifdef REHLDS_FIXES
	bool inQuotes;
#endif

	argv[0] = "";
	c = *lpCmdLine;
	
	for(mhost_parms->argc = 1; c && mhost_parms->argc < MAX_COMMAND_LINE_PARAMS; c = *(++lpCmdLine))
	{
#ifdef REHLDS_FIXES
		// Skip whitespace
		while(c && c <= ' ')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		};
		
		if(!c)
			break;

		// TODO: Add MultiByteToWideChar conversion under Windows, to correctly get
		// UTF8, but need to alloc memory to store it
		// Store arg pointer
		argv[mhost_parms->argc] = lpCmdLine;
		mhost_parms->argc++;

		// Find end of the argument
		inQuotes = false;
		
		while(c > ' ' || (c && inQuotes)) // FIXED: Do not break quoted arguments
		{
			if(c == '"')
				inQuotes = !inQuotes;
			
			lpCmdLine++;
			c = *lpCmdLine;
		};
#else  // REHLDS_FIXES
		
		// Skip whitespace and UTF8
		while(c && (c <= ' ' || c > '~'))
		{
			lpCmdLine++;
			c = *lpCmdLine;
		};
		
		if(!c)
			break;

		// Store arg pointer
		argv[mhost_parms->argc] = lpCmdLine;
		mhost_parms->argc++;

		// Find end of the argument
		while(c > ' ' && c <= '~')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		};
#endif // REHLDS_FIXES
		
		if(!c)
			break;
		
		*lpCmdLine = 0;
	};

	mhost_parms->argv = argv;
	//COM_InitArgv(mhost_parms->argc, argv);
	//mhost_parms->argc = com_argc;
	//mhost_parms->argv = com_argv;
};

NOXREF void CSystem::ShutdownArgv()
{
	NOXREFCHECK;
};

void CSystem::InitMemory()
{
	int i = 0; //COM_CheckParm("-heapsize");
	
	//if(i && i < com_argc - 1)
		//mhost_parms->memsize = Q_atoi(com_argv[i + 1]) * 1024;

	if(mhost_parms->memsize < MINIMUM_WIN_MEMORY)
	{
#ifdef _WIN32
		MEMORYSTATUS lpBuffer;
		lpBuffer.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer);

		if(lpBuffer.dwTotalPhys)
		{
			if(lpBuffer.dwTotalPhys < FIFTEEN_MB)
				Error("Available memory less than 15MB!!! %i", mhost_parms->memsize);

			mhost_parms->memsize = (int)(lpBuffer.dwTotalPhys >> 1);
			
			if(mhost_parms->memsize < MINIMUM_WIN_MEMORY)
				mhost_parms->memsize = MINIMUM_WIN_MEMORY;
		}
		else
			mhost_parms->memsize = MAXIMUM_WIN_MEMORY;

		if(mbDedicatedServer)
			mhost_parms->memsize = DEFAULT_MEMORY;
#else
		mhost_parms->memsize = DEFAULT_MEMORY;
#endif // _WIN32
	};

	if(mhost_parms->memsize > MAXIMUM_DEDICATED_MEMORY)
		mhost_parms->memsize = MAXIMUM_DEDICATED_MEMORY;

	//if(COM_CheckParm("-minmemory"))
		//mhost_parms->memsize = MINIMUM_WIN_MEMORY;
	
#ifdef _WIN32
	mhost_parms->membase = (void *)GlobalAlloc(GMEM_FIXED, mhost_parms->memsize);
#else
	mhost_parms->membase = Mem_Malloc(mhost_parms->memsize);
#endif // _WIN32

	if(!mhost_parms->membase)
		Error("Unable to allocate %.2f MB\n", (float)mhost_parms->memsize / (1024.0f * 1024.0f));
};

void CSystem::ShutdownMemory()
{
#ifdef _WIN32
	GlobalFree((HGLOBAL)mhost_parms->membase);
#else
	Mem_Free(mhost_parms->membase); // CMemory::Free
#endif // _WIN32
	
	mhost_parms->membase = NULL;
	mhost_parms->memsize = 0;
};

void CSystem::InitLauncherInterface()
{
#ifdef _WIN32
	
	// TODO: client-side code
	
#else // if not _WIN32
	
#ifndef SWDS
	gHasMMXTechnology = TRUE;
#endif

	SetupLegacyAPIs();
#endif // _WIN32
};

NOXREF void CSystem::ShutdownLauncherInterface()
{
	NOXREFCHECK;
};

void CSystem::InitAuthentication()
{
	Printf("STEAM Auth Server\r\n");
};

NOXREF void CSystem::ShutdownAuthentication()
{
	NOXREFCHECK;
};

NOXREF void CSystem::Sleep(int msec)
{
	NOXREFCHECK;

	std::this_thread::sleep_for(std::chrono::milliseconds(msec));
};

void CSystem::Printf(const char *fmt, ...)
{
	char Dest[1024];
	va_list va;

	va_start(va, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, va);
	va_end(va);

	if(mbDedicatedServer)
		LegacySysPrintf("%s", Dest);

#ifdef _WIN32
	OutputDebugStringA(Dest);
#else
	if(!mbDedicatedServer)
		fprintf(stderr, "%s\n", Dest);
#endif // _WIN32
};

NOXREF void CSystem::Warning(const char *pszWarning, ...)
{
	NOXREFCHECK;
	
	va_list argptr;
	char text[1024];

	va_start(argptr, pszWarning);
	Q_vsnprintf(text, sizeof(text), pszWarning, argptr);
	va_end(argptr);

	Printf(text); // Printf("[WARNING] %s", text);
};

void NORETURN CSystem::Error(const char *error, ...)
{
	va_list argptr;
	char text[1024];
	static bool bReentry;

	va_start(argptr, error);
	Q_vsnprintf(text, ARRAYSIZE(text), error, argptr);
	va_end(argptr);

#ifdef _WIN32
	MessageBox(GetForegroundWindow(), text, "Fatal error - Dedicated server", MB_ICONERROR | MB_OK);
#endif // _WIN32

	if(bReentry)
	{
		fprintf(stderr, "%s\n", text);
		//longjmp(host_abortserver, 2);
	};

	bReentry = true;

	//eventmanager->BroadcastEvent(GenerateSysErrorEvent())
	
	//if(g_psvs.dll_initialized && gEntityInterface.pfnSys_Error)
		//gEntityInterface.pfnSys_Error(text);

	//Log_Printf("FATAL ERROR (shutting down): %s\n", text);

#ifdef REHLDS_FIXES
	if(syserror_logfile.string[0] != '\0')
	{
		auto pLogFile = mpFileSystem->Open(syserror_logfile.string, "a");

		if(pLogFile)
		{
			tm *today;
			time_t ltime;
			char szDate[32];

			time(&ltime);
			today = localtime(&ltime);
			strftime(szDate, ARRAYSIZE(szDate) - 1, "L %d/%m/%Y - %H:%M:%S:", today);

			pLogFile->Printf("%s (map \"%s\") %s\n", szDate, &pr_strings[gGlobalVariables.mapname], text);
			
			mpFileSystem->Close(pLogFile);
		};
	};
#endif // REHLDS_FIXES

	if(mbDedicatedServer)
	{
		if(!LegacySysPrintf("FATAL ERROR (shutting down): %s\n", text))
			printf("FATAL ERROR (shutting down): %s\n", text);
	}
#ifndef SWDS
	else
	{
		HWND hWnd = 0;

		//if(pmainwindow)
			//hWnd = *pmainwindow;

		Printf(text);
		//SDL_ShowSimpleMessageBox(MB_ICONERROR | MB_OK, "Fatal Error", text, hWnd);
		//VideoMode_IsWindowed();
	};
#endif // SWDS

	// Allahu akbar!
	*(int *)NULL = NULL; // exit(-1);
};

NOXREF void CSystem::SetRateRegistrySetting(const char *pchRate)
{
	NOXREFCHECK;
	
	registry->WriteString("rate", pchRate);
};

NOXREF const char *CSystem::GetRateRegistrySetting(const char *pchDef)
{
	NOXREFCHECK;
	
	return registry->ReadString("rate", pchDef);
};

void CSystem::SetupLegacyAPIs()
{
#ifndef SWDS
	//VID_FlipScreen = Sys_VID_FlipScreen;
	//D_SurfaceCacheForRes = Sys_GetSurfaceCacheSize;
#endif // SWDS
};

bool CSystem::LegacySysPrintf(char *fmt, ...)
{
	if(mpRemoteConsole)
	{
		va_list argptr;
		char text[1024];

		va_start(argptr, fmt);
		Q_vsnprintf(text, sizeof(text), fmt, argptr);
		va_end(argptr);
		
		mpRemoteConsole->Sys_Printf(text);
		return true;
	};
	
	return false;
};

NOXREF bool CSystem::IsWin95()
{
	NOXREFCHECK;
	
#ifdef _WIN32
	return mbIsWin95;
#else
	// NOTE: no need to check for Win
	return false;
#endif // _WIN32
};

NOXREF bool CSystem::IsWin98()
{
	NOXREFCHECK;
	
#ifdef _WIN32
	return mbIsWin98;
#else
	// NOTE: no need to check for Win
	return false;
#endif // _WIN32
};

NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage) // looks like this thing came out from q2 (Com_Error)
{
	NOXREFCHECK;
};

NOXREF void Legacy_MP3subsys_Suspend_Audio()
{
	NOXREFCHECK;
};

NOXREF void Legacy_MP3subsys_Resume_Audio()
{
	NOXREFCHECK;
};