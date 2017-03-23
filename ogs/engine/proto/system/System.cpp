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

#include "system/System.hpp"

void CSystem::Init()
{
#ifndef SWDS
	//Sys_InitFloatTime();
#endif
};

void CSystem::Shutdown()
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
	};
#else
#ifndef SWDS
	GL_Shutdown(*pmainwindow, maindc, baseRC);
#endif // SWDS

#endif // _WIN32
};

void CSystem::InitArgv(char *lpCmdLine)
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
		};
		
		if(!c)
			break;

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
		argv[host_parms.argc] = lpCmdLine;
		host_parms.argc++;

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

	host_parms.argv = argv;
	COM_InitArgv(host_parms.argc, argv);
	host_parms.argc = com_argc;
	host_parms.argv = com_argv;
};

NOXREF void CSystem::ShutdownArgv()
{
	NOXREFCHECK;
};

void CSystem::InitMemory(quakeparms_t *host_parms)
{
	int i = COM_CheckParm("-heapsize");
	
	if(i && i < com_argc - 1)
		host_parms->memsize = Q_atoi(com_argv[i + 1]) * 1024;

	if(host_parms->memsize < MINIMUM_WIN_MEMORY)
	{
#ifdef _WIN32
		MEMORYSTATUS lpBuffer;
		lpBuffer.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer);

		if(lpBuffer.dwTotalPhys)
		{
			if(lpBuffer.dwTotalPhys < FIFTEEN_MB)
				Error("Available memory less than 15MB!!! %i", host_parms.memsize);

			host_parms.memsize = (int)(lpBuffer.dwTotalPhys >> 1);
			
			if(host_parms->memsize < MINIMUM_WIN_MEMORY)
				host_parms->memsize = MINIMUM_WIN_MEMORY;
		}
		else
			host_parms->memsize = MAXIMUM_WIN_MEMORY;

		if(gbIsDedicatedServer)
			host_parms->memsize = DEFAULT_MEMORY;
#else
		host_parms->memsize = DEFAULT_MEMORY;
#endif // _WIN32
	};

	if(host_parms->memsize > MAXIMUM_DEDICATED_MEMORY)
		host_parms->memsize = MAXIMUM_DEDICATED_MEMORY;

	if(COM_CheckParm("-minmemory"))
		host_parms->memsize = MINIMUM_WIN_MEMORY;
	
#ifdef _WIN32
	host_parms->membase = (void *)GlobalAlloc(GMEM_FIXED, host_parms->memsize);
#else
	host_parms->membase = Mem_Malloc(host_parms->memsize);
#endif // _WIN32

	if(!host_parms->membase)
		Error("Unable to allocate %.2f MB\n", (float)host_parms->memsize / (1024.0f * 1024.0f));
};

void CSystem::ShutdownMemory()
{
#ifdef _WIN32
	GlobalFree((HGLOBAL)host_parms->membase);
#else
	Mem_Free(host_parms->membase);
#endif // _WIN32
	
	host_parms->membase = NULL;
	host_parms->memsize = 0;
};

void CSystem::InitLauncherInterface()
{
#ifdef _WIN32
	// TODO: client-side code
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
	
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

void CSystem::Sleep(int msec)
{
	Sys_Sleep(msec);
};

void CSystem::Printf(const char *fmt, ...)
{
	char Dest[1024];
	va_list va;

	va_start(va, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, va);
	va_end(va);

	if(gbIsDedicatedServer && Launcher_ConsolePrintf)
		Launcher_ConsolePrintf("%s", Dest);

#ifdef _WIN32
	OutputDebugStringA(Dest);
#else
	if(!gbIsDedicatedServer)
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

	Printf(text);
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
		longjmp(host_abortserver, 2);
	};
	
	bReentry = true;

	if(g_psvs.dll_initialized && gEntityInterface.pfnSys_Error)
		gEntityInterface.pfnSys_Error(text);

	Log_Printf("FATAL ERROR (shutting down): %s\n", text);

#ifdef REHLDS_FIXES
	if(syserror_logfile.string[0] != '\0')
	{
		auto pFile = gpFS->Open(syserror_logfile.string, "a");
		
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
		};
	};
#endif // REHLDS_FIXES

	if(gbIsDedicatedServer)
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

		Printf(text);
		//SDL_ShowSimpleMessageBox(MB_ICONERROR | MB_OK, "Fatal Error", text, hWnd);
		//VideoMode_IsWindowed();
	};
#endif // SWDS

	// exit(-1);
	// Allahu akbar!
	*(int *)NULL = NULL;
};

void CSystem::SetupLegacyAPIs()
{
#ifndef SWDS
	//VID_FlipScreen = Sys_VID_FlipScreen;
	//D_SurfaceCacheForRes = Sys_GetSurfaceCacheSize;
#endif // SWDS
	
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
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