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

#include "precompiled.hpp"
#include "system/system.hpp"

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
	registry->WriteString("rate", pchRate);
}

NOXREF const char *GetRateRegistrySetting(const char *pchDef)
{
	return registry->ReadString("rate", pchDef);
}

void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated)
{
	char key[65];
	char hostname[4096];

	if(CRehldsPlatformHolder::get()->gethostname(hostname, sizeof(hostname)))
		Q_snprintf(key, sizeof(key), "%u", RandomLong(0, 0x7FFFFFFF));
	else
	{
		struct hostent *hostinfo;
		hostinfo = CRehldsPlatformHolder::get()->gethostbyname(hostname);
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
}

NOXREF void Legacy_MP3subsys_Resume_Audio()
{
}

void Sys_SetupLegacyAPIs()
{
#ifndef SWDS
	VID_FlipScreen = Sys_VID_FlipScreen;
	D_SurfaceCacheForRes = Sys_GetSurfaceCacheSize;
#endif // SWDS
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
}

NOXREF int Sys_IsWin95()
{
#ifdef _WIN32
	return g_bIsWin95;
#else
	// TODO: no need to check is win
	return 0;
#endif // _WIN32
}

NOXREF int Sys_IsWin98()
{
#ifdef _WIN32
	return g_bIsWin98;
#else
	// TODO: no need to check is win
	return 0;
#endif // _WIN32
}

#ifdef _WIN32
NOXREF void Sys_CheckOSVersion()
{
	struct _OSVERSIONINFOA verInfo;

	Q_memset(&verInfo, 0, sizeof(verInfo));
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	if(!GetVersionExA(&verInfo))
		Sys_Error("Couldn't get OS info");

	g_WinNTOrHigher = verInfo.dwMajorVersion >= 4;
	if(verInfo.dwPlatformId == 1 && verInfo.dwMajorVersion == 4)
	{
		if(verInfo.dwMinorVersion)
		{
			if(verInfo.dwMinorVersion < 90)
				g_bIsWin98 = 1;
		}
		else
		{
			g_bIsWin95 = 1;
		}
	}
}
#endif // _WIN32

void Sys_Init()
{
#ifndef SWDS
	Sys_InitFloatTime();
#endif
}

void Sys_Shutdown()
{
#ifndef SWDS
	Sys_ShutdownFloatTime();
#endif // SWDS
	Steam_ShutdownClient();
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
}

void Sys_InitAuthentication()
{
	Sys_Printf("STEAM Auth Server\r\n");
}

NOXREF void Sys_ShutdownAuthentication()
{
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
					if(g_bMajorMapChange)
					{
						g_bPrintingKeepAliveDots = TRUE;
						Sys_Printf(".");
						recursionGuard = false;
						return;
					}
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
					SetLoadingProgressBarStatusText(msg);
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
		pmainwindow = (HWND *)pwnd;
#ifdef _WIN32
		videomode->UpdateWindowPosition();
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

	Sys_CheckCpuInstructionsSupport();

#ifndef SWDS
	Sys_InitFloatTime();
#endif // SWDS
	FS_LogLevelLoadStarted("Launcher");
	SeedRandomNumberGenerator();
	TraceInit("Sys_InitMemory()", "Sys_ShutdownMemory()", 0);
	Sys_InitMemory();
	TraceInit("Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0);
	Sys_InitLauncherInterface();

#ifndef SWDS
	if(!GL_SetMode(*pmainwindow, &maindc, &baseRC))
		return 0;
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
	const char en_US[12];

	Q_strcpy(en_US, "en_US.UTF-8");
	en_US[16] = 0;

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
		SDL_ShowSimpleMessageBox(0, "Warning", MessageText, *pmainwindow);
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