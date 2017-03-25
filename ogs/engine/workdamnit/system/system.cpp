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
#include "system/dedicatedserverapi.hpp"
#include "system/common.hpp"
#include "system/host.hpp"
#include "system/host_cmd.hpp"
#include "system/traceinit.h"
#include "filesystem/filesystem_internal.hpp"
#include "resources/modinfo.hpp"
#include "console/console.hpp"
#include "world/pr_cmds.hpp"
#include "voice/voice.hpp"
#include "input/keys.hpp"
#include "system/server.hpp"
#include "system/sv_log.hpp"

void (*Launcher_ConsolePrintf)(char *, ...);
char *(*Launcher_GetLocalizedString)(unsigned int);

int (*Launcher_MP3subsys_Suspend_Audio)();
void (*Launcher_MP3subsys_Resume_Audio)();

void (*VID_FlipScreen)();

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

#ifdef REHLDS_FIXES
char g_szFindFirstFileName[MAX_PATH];
#endif

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
	};

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
		CSystem::Error("Protection change failed.");
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


NOBODY void Sys_DebugOutStraight(const char *pStr);
//{
//}







void Sys_Quit()
{
	giActive = DLL_CLOSE;
}

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
		pDispatch = (DISPATCHFUNCTION)GetProcAddress((HMODULE)g_rgextdll[i].lDLLHandle, pname);
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
			return pDll[i].functionTable->pFunctionName;
	};
#else  // _WIN32
	Dl_info addrInfo;
	
	if(dladdr((void *)function, &addrInfo))
		return addrInfo.dli_sname;
#endif // _WIN32

	return NULL;
}

uint32 FindNameInTable(extensiondll_t *pDll, const char *pName)
{
#ifdef _WIN32
	for(int i = 0; i < pDll->functionCount; i++)
	{
		if(!Q_strcmp(pName, pDll->functionTable[i].pFunctionName))
			return pDll[i].functionTable->pFunction;
	};
	
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
};

const char *EXT_FUNC NameForFunction(uint32 function)
{
	const char *pName = "";

	for(int i = 0; i < g_iextdllMac; i++)
	{
		pName = FindAddressInTable(&g_rgextdll[i], function);
		if(pName)
			return pName;
	};

	Con_Printf("Can't find address: %08lx\n", function);
	return NULL;
};

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
	if(!Q_stricmp(com_gamedir, "cstrike") || !Q_stricmp(com_gamedir, "cstrike_beta"))
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
};

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

			if(g_flLastSteamProgressUpdateTime + 2.0f <= currentTime)
			{
				g_flLastSteamProgressUpdateTime = currentTime;
				numTics++;
				
				if(gbIsDedicatedServer)
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