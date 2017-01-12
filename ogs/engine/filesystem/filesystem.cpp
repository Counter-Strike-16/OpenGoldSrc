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
#include "filesystem/filesystem_.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "system/common.hpp"

CUtlVector<char *> g_fallbackLocalizationFiles;
char               s_pBaseDir[512];
bool               bLowViolenceBuild;

CSysModule *      gpFileSystemModule;
CreateInterfaceFn g_FileSystemFactory;

const char *GetBaseDirectory()
{
	return s_pBaseDir;
}

NOXREF void *GetFileSystemFactory()
{
	NOXREFCHECK;

	return (void *)g_FileSystemFactory;
}

bool FileSystem_LoadDLL(CreateInterfaceFn filesystemFactory)
{
	if(!filesystemFactory)
	{
		gpFileSystemModule = Sys_LoadModule(FILESYSTEM_DLL_NAME);

		if(gpFileSystemModule)
			filesystemFactory = Sys_GetFactory(gpFileSystemModule);
	}

	if(filesystemFactory)
	{
		g_FileSystemFactory = filesystemFactory;

		gpFileSystem = (IFileSystem *)filesystemFactory(FILESYSTEM_INTERFACE_VERSION, 0);
		return gpFileSystem != NULL;
	}

	return false;
}

void FileSystem_UnloadDLL()
{
	if(gpFileSystemModule)
	{
		Sys_UnloadModule((CSysModule *)gpFileSystemModule);
		gpFileSystemModule  = NULL;
		g_FileSystemFactory = NULL;
		gpFileSystem        = NULL;
	}
}

bool BEnabledHDAddon()
{
	if(COM_CheckParm("-nohdmodels"))
		return false;

	return (registry->ReadInt("hdmodels", 1) > 0);
}

bool BEnableAddonsFolder()
{
	if(COM_CheckParm("-addons"))
		return false;

	return (registry->ReadInt("addons_folder", 0) > 0);
}

void Host_SetHDModels_f()
{
	if(cls.state && Cmd_Argc() == 2)
	{
		bool bEnabled = (registry->ReadInt("hdmodels", 1) > 0);

		registry->WriteInt("hdmodels", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);

		if(bEnabled != BEnabledHDAddon())
			COM_SetupDirectories();
	}
}

void Host_SetAddonsFolder_f()
{
	if(cls.state && Cmd_Argc() == 2)
	{
		bool bEnabled = (registry->ReadInt("addons_folder", 0) > 0);

		registry->WriteInt("addons_folder", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);

		if(bEnabled != BEnableAddonsFolder())
			COM_SetupDirectories();
	}
}

void Host_SetVideoLevel_f()
{
	if(cls.state && Cmd_Argc() == 2)
		registry->WriteInt("vid_level", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);
}

int Host_GetVideoLevel()
{
	return registry->ReadInt("vid_level", 0);
}

void CheckLiblistForFallbackDir(const char *pGameDir, bool bLanguage, const char *pLanguage, bool bLowViolenceBuild_)
{
	char         szTemp[512];
	FileHandle_t hFile;

	Q_snprintf(szTemp, sizeof(szTemp) - 1, "%s/liblist.gam", pGameDir);
	COM_FixSlashes(szTemp);
	gpFileSystem->GetLocalCopy(szTemp);

	if(Q_stricmp(com_gamedir, pGameDir))
	{
		Q_snprintf(szTemp, 511, "../%s/liblist.gam", pGameDir);
		COM_FixSlashes(szTemp);
		hFile = FS_Open(szTemp, "rt");
	}
	else
		hFile = FS_Open("liblist.gam", "rt");

	if(!hFile)
		return;

	if(FS_EndOfFile(hFile))
	{
		FS_Close(hFile);
		return;
	}

	char szFallback[128];
	char szLine[512];

	char *end;
	char *start;
	int   bytesToCopy;

	while(1)
	{
		szLine[0] = 0;
		FS_ReadLine(szLine, sizeof(szLine) - 1, hFile);
		szLine[511] = 0;
		if(!Q_strnicmp(szLine, "fallback_dir", Q_strlen("fallback_dir")))
		{
			start = Q_strchr(szLine, '"');
			if(!start)
			{
				FS_Close(hFile);
				return;
			}

			end = Q_strchr(start + 1, '"');
			if(!end)
			{
				FS_Close(hFile);
				return;
			}

			bytesToCopy = (int)(end - start) - 1;
			if(bytesToCopy > sizeof(szFallback) - 2)
			{
				FS_Close(hFile);
				return;
			}

			if(bytesToCopy > 0)
				break;
		}
		if(FS_EndOfFile(hFile))
		{
			FS_Close(hFile);
			return;
		}
	}

	Q_strncpy(szFallback, start + 1, bytesToCopy);
	szFallback[bytesToCopy] = 0;

	if(!Q_stricmp(pGameDir, szFallback))
	{
		FS_Close(hFile);
		return;
	}
	if(bLowViolenceBuild)
	{
		Q_snprintf(szTemp, 511, "%s/%s_lv", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		COM_FixSlashes(szTemp);
		gpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	}
	if(BEnableAddonsFolder())
	{
		Q_snprintf(szTemp, 511, "%s/%s_addon", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		COM_FixSlashes(szTemp);
		gpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	}

	if(bLanguage && pLanguage)
	{
		char  baseDir[4096];
		char *tempPtr;

		Q_snprintf(szTemp, 511, "%s/%s_%s", GetBaseDirectory(), szFallback, pLanguage);
		szTemp[511] = 0;
		COM_FixSlashes(szTemp);
		gpFileSystem->AddSearchPath(szTemp, "GAME_FALLBACK");

		if(!COM_CheckParm("-steam"))
		{
			Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
			baseDir[sizeof(baseDir) - 1] = 0;

			tempPtr = Q_strstr(baseDir, "\\game");
			if(tempPtr)
			{
				*tempPtr = 0;

				Q_snprintf(szTemp, 511, "%s\\localization\\%s_%s", baseDir, szFallback, pLanguage);
				szTemp[511] = 0;

				COM_FixSlashes(szTemp);
				gpFileSystem->AddSearchPath(szTemp, "GAME_FALLBACK");
			}
		}
	}

	if(BEnabledHDAddon())
	{
		Q_snprintf(szTemp, 511, "%s/%s_hd", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		COM_FixSlashes(szTemp);
		gpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	}

	Q_snprintf(szTemp, 511, "%s/%s", GetBaseDirectory(), szFallback);
	szTemp[511] = 0;
	COM_FixSlashes(szTemp);
	gpFileSystem->AddSearchPath(szTemp, "GAME_FALLBACK");

	if(Q_stricmp(szFallback, "valve"))
	{
		const int BufLen     = 128;
		char *    szFileName = new char[BufLen];

		Q_snprintf(szFileName, BufLen - 1, "Resource/%s_%%language%%.txt", szFallback);
		szFileName[BufLen - 1] = 0;

		g_fallbackLocalizationFiles.AddToTail(szFileName);
		CheckLiblistForFallbackDir(szFallback, bLanguage, pLanguage, bLowViolenceBuild);
	}
	FS_Close(hFile);
}

int FileSystem_SetGameDirectory(const char *pDefaultDir, const char *pGameDir)
{
	char        temp[512];
	char        language[256];
	const char *pchLang;

	gpFileSystem->RemoveAllSearchPaths();
	language[0] = 0;

	if(!bLowViolenceBuild)
	{
		if(CRehldsPlatformHolder::get()->SteamApps() && GetGameAppID() == 70)
			bLowViolenceBuild = CRehldsPlatformHolder::get()->SteamApps()->BIsLowViolence();
	}

	pchLang = CRehldsPlatformHolder::get()->SteamApps() ? CRehldsPlatformHolder::get()->SteamApps()->GetCurrentGameLanguage() : NULL;
	Q_strncpy(language, pchLang ? pchLang : "english", ARRAYSIZE(language));
#ifdef REHLDS_CHECKS
	language[ARRAYSIZE(language) - 1] = 0;
#endif

	if(!g_bIsDedicatedServer && !IsGameSubscribed(pGameDir))
		return 0;

	CRehldsPlatformHolder::get()->SteamAPI_SetBreakpadAppID(GetGameAppID());

	bool bEnableHDPack = BEnabledHDAddon();
	bool bLanguage     = (Q_strlen(language) != 0 && Q_stricmp(language, "english")) ? true : false;

	if(!pGameDir)
		pGameDir = pDefaultDir;

	if(pGameDir)
	{
		if(bLowViolenceBuild)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_lv", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		}
		if(BEnableAddonsFolder())
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_addon", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		}
		if(bLanguage)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_%s", GetBaseDirectory(), pGameDir, language);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "GAME");

			if(!COM_CheckParm("-steam"))
			{
				char baseDir[MAX_PATH];
				Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
				baseDir[sizeof(baseDir) - 1] = 0;
				char *tempPtr                = Q_strstr(baseDir, "\\game");
				if(tempPtr)
				{
					*tempPtr = 0;
					Q_snprintf(temp, 511, "%s\\localization\\%s_%s", baseDir, pGameDir, language);
					temp[511] = 0;
					COM_FixSlashes(temp);
					gpFileSystem->AddSearchPathNoWrite(temp, "GAME");
				}
			}
		}
		if(bEnableHDPack)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_hd", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		}

		Q_snprintf(temp, 511, "%s/%s", GetBaseDirectory(), pGameDir);
		temp[sizeof(temp) - 1] = 0;
		COM_FixSlashes(temp);
		gpFileSystem->AddSearchPath(temp, "GAME");
		gpFileSystem->AddSearchPath(temp, "GAMECONFIG");

		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_downloads", GetBaseDirectory(), pGameDir);
		temp[sizeof(temp) - 1] = 0;
		COM_FixSlashes(temp);
		gpFileSystem->AddSearchPath(temp, "GAMEDOWNLOAD");

		CheckLiblistForFallbackDir(pGameDir, bLanguage, language, bLowViolenceBuild);
	}
	if(bLanguage)
	{
		if(bLowViolenceBuild)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_lv", GetBaseDirectory(), pDefaultDir);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		}

		if(BEnableAddonsFolder())
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_addon", GetBaseDirectory(), pDefaultDir);
			temp[sizeof(temp) - 1] = 0;
			COM_FixSlashes(temp);
			gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		}

		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_%s", GetBaseDirectory(), pDefaultDir, language);
		temp[sizeof(temp) - 1] = 0;
		COM_FixSlashes(temp);
		gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		if(!COM_CheckParm("-steam"))
		{
			char baseDir[MAX_PATH];

			Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
			baseDir[sizeof(baseDir) - 1] = 0;
			char *tempPtr                = Q_strstr(baseDir, "\\game");
			if(tempPtr)
			{
				*tempPtr = 0;
				Q_snprintf(temp, sizeof(temp) - 1, "%s\\localization\\%s_%s", baseDir, pDefaultDir, language);
				temp[sizeof(temp) - 1] = 0;
				COM_FixSlashes(temp);
				gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
			}
		}
	}
	if(bEnableHDPack)
	{
		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_hd", GetBaseDirectory(), pDefaultDir);
		temp[sizeof(temp) - 1] = 0;
		COM_FixSlashes(temp);
		gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
	}

	Q_snprintf(temp, sizeof(temp) - 1, "%s", GetBaseDirectory());
	temp[sizeof(temp) - 1] = 0;
	COM_FixSlashes(temp);
	gpFileSystem->AddSearchPath(temp, "BASE");

	Q_snprintf(temp, sizeof(temp) - 1, "%s/%s", GetBaseDirectory(), pDefaultDir);
	temp[sizeof(temp) - 1] = 0;
	COM_FixSlashes(temp);
	gpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");

	Q_snprintf(temp, sizeof(temp) - 1, "%s/platform", GetBaseDirectory());
	temp[sizeof(temp) - 1] = 0;
	COM_FixSlashes(temp);
	gpFileSystem->AddSearchPath(temp, "PLATFORM");

	return 1;
}

int FileSystem_AddFallbackGameDir(const char *pGameDir)
{
	char language[128];

	const char *pchLang = CRehldsPlatformHolder::get()->SteamApps() ? CRehldsPlatformHolder::get()->SteamApps()->GetCurrentGameLanguage() : NULL;
	Q_strncpy(language, pchLang ? pchLang : "english", ARRAYSIZE(language));
#ifdef REHLDS_CHECKS
	language[ARRAYSIZE(language) - 1] = 0;
#endif

	if(Q_strlen(language) != 0 && Q_stricmp(language, "english"))
	{
		char temp[MAX_PATH];
		Q_sprintf(temp, "%s/%s_%s", GetBaseDirectory(), pGameDir, language);
		gpFileSystem->AddSearchPath(temp, "GAME");
	}

	gpFileSystem->AddSearchPath(pGameDir, "GAME");
	return 1;
}

int FileSystem_Init(char *basedir, void *voidfilesystemFactory)
{
#ifdef REHLDS_CHECKS
	Q_strncpy(s_pBaseDir, basedir, ARRAYSIZE(s_pBaseDir));
	s_pBaseDir[ARRAYSIZE(s_pBaseDir) - 1] = 0;
#else
	Q_strcpy(s_pBaseDir, basedir);
#endif

	host_parms.basedir = s_pBaseDir;

	if(FileSystem_LoadDLL((CreateInterfaceFn)voidfilesystemFactory))
		return COM_SetupDirectories() != 0;

	return 0;
}

void FileSystem_Shutdown()
{
	FS_RemoveAllSearchPaths();
	FileSystem_UnloadDLL();
}