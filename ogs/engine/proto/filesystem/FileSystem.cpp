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
/// @brief high-level filesystem module

#include "precompiled.hpp"
#include "system/client.hpp"
#include "console/cmd.hpp"
#include "filesystem/FileSystem.hpp"
#include "system/common.hpp"
#include "system/Host.hpp"
#include "system/System.hpp"
#include "system/SystemTypes.hpp"
#include "system/StringHandler.hpp"
#include "steam/isteamclient.h"
#include "steam/isteamapps.h"

CUtlVector<char *> g_fallbackLocalizationFiles;
bool bLowViolenceBuild;

int CFileSystem::Init(char *basedir)
{
#ifdef REHLDS_CHECKS
	Q_strncpy(msBaseDir, basedir, ARRAYSIZE(msBaseDir));
	msBaseDir[ARRAYSIZE(msBaseDir) - 1] = 0;
#else
	Q_strcpy(msBaseDir, basedir);
#endif

	return 0; // COM_SetupDirectories() != 0
};

void CFileSystem::Shutdown()
{
	RemoveAllSearchPaths();
};

void CFileSystem::CreatePath(char *path)
{
	if(*path == 0)
		return;
	
	char old;
	
	for(char *ofs = path + 1; *ofs; ofs++)
	{
		if(*ofs == '/' || *ofs == '\\')
		{
			old = *ofs;
			*ofs = 0;
			CreateDirHierarchy(path, 0);
			*ofs = old;
		};
	};
};

NOXREF void CFileSystem::WriteFile(char *filename, void *data, int len)
{
	NOXREFCHECK;

	char path[MAX_PATH];
	Q_snprintf(path, MAX_PATH - 1, "%s", filename);
	path[MAX_PATH - 1] = 0;

	CStringHandler::FixSlashes(path);
	CreatePath(path);

	CFile *fp = Open(path, "wb");

	if(fp)
	{
		CSystem::Printf("%s: %s\n", __FUNCTION__, path);
		Write(data, len, 1, fp);
		Close(fp);
	}
	else
		CSystem::Printf("%s: failed on %s\n", __FUNCTION__, path);
};

NOXREF void CFileSystem::CopyFile(char *netpath, char *cachepath)
{
	NOXREFCHECK;

	int count;
	int remaining;
	char buf[4096];

	FileHandle_t out;
	FileHandle_t in = Open(netpath, "rb");

	if(!in)
		return;

	count = Size(in);
	CreatePath(cachepath);

	for(out = Open(cachepath, "wb"); count; count -= remaining)
	{
		remaining = count;

		if(remaining > 4096)
			remaining = 4096;

		Read(buf, remaining, 1, in);
		Write(buf, remaining, 1, out);
	};

	Close(in);
	Close(out);
};

const char *CFileSystem::GetBaseDirectory()
{
	return msBaseDir;
};

int CFileSystem::SetGameDirectory(const char *pDefaultDir, const char *pGameDir)
{
	char temp[512];
	char language[256];
	const char *pchLang;

	mpFileSystem->RemoveAllSearchPaths();
	language[0] = 0;

	if(!bLowViolenceBuild)
	{
		//if(CRehldsPlatformHolder::get()->SteamApps() && GetGameAppID() == 70)
			//bLowViolenceBuild = CRehldsPlatformHolder::get()->SteamApps()->BIsLowViolence();
	};

	//pchLang = CRehldsPlatformHolder::get()->SteamApps() ? CRehldsPlatformHolder::get()->SteamApps()->GetCurrentGameLanguage() : NULL;
	Q_strncpy(language, pchLang ? pchLang : "english", ARRAYSIZE(language));
#ifdef REHLDS_CHECKS
	language[ARRAYSIZE(language) - 1] = 0;
#endif

	//if(!gbIsDedicatedServer && !IsGameSubscribed(pGameDir))
		//return 0;

	//CRehldsPlatformHolder::get()->SteamAPI_SetBreakpadAppID(GetGameAppID());

	bool bEnableHDPack = BEnabledHDAddon();
	bool bLanguage = (Q_strlen(language) != 0 && Q_stricmp(language, "english")) ? true : false;

	if(!pGameDir)
		pGameDir = pDefaultDir;

	if(pGameDir)
	{
		if(bLowViolenceBuild)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_lv", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		}
		if(BEnableAddonsFolder())
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_addon", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		}
		if(bLanguage)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_%s", GetBaseDirectory(), pGameDir, language);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "GAME");

			//if(!mpCmdLine->CheckArg("-steam"))
			{
				char baseDir[MAX_PATH];
				Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
				baseDir[sizeof(baseDir) - 1] = 0;
				char *tempPtr = Q_strstr(baseDir, "\\game");
				if(tempPtr)
				{
					*tempPtr = 0;
					Q_snprintf(temp, 511, "%s\\localization\\%s_%s", baseDir, pGameDir, language);
					temp[511] = 0;
					CStringHandler::FixSlashes(temp);
					mpFileSystem->AddSearchPathNoWrite(temp, "GAME");
				};
			};
		};

		if(bEnableHDPack)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_hd", GetBaseDirectory(), pGameDir);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "GAME");
		};

		Q_snprintf(temp, 511, "%s/%s", GetBaseDirectory(), pGameDir);
		temp[sizeof(temp) - 1] = 0;
		CStringHandler::FixSlashes(temp);
		mpFileSystem->AddSearchPath(temp, "GAME");
		mpFileSystem->AddSearchPath(temp, "GAMECONFIG");

		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_downloads", GetBaseDirectory(), pGameDir);
		temp[sizeof(temp) - 1] = 0;
		CStringHandler::FixSlashes(temp);
		mpFileSystem->AddSearchPath(temp, "GAMEDOWNLOAD");

		CheckLiblistForFallbackDir(pGameDir, bLanguage, language, bLowViolenceBuild);
	};

	if(bLanguage)
	{
		if(bLowViolenceBuild)
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_lv", GetBaseDirectory(), pDefaultDir);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		}

		if(BEnableAddonsFolder())
		{
			Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_addon", GetBaseDirectory(), pDefaultDir);
			temp[sizeof(temp) - 1] = 0;
			CStringHandler::FixSlashes(temp);
			mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		}

		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_%s", GetBaseDirectory(), pDefaultDir, language);
		temp[sizeof(temp) - 1] = 0;
		CStringHandler::FixSlashes(temp);
		mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
		
		//if(!mpCmdLine->CheckArg("-steam"))
		{
			char baseDir[MAX_PATH];

			Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
			baseDir[sizeof(baseDir) - 1] = 0;
			char *tempPtr = Q_strstr(baseDir, "\\game");
			if(tempPtr)
			{
				*tempPtr = 0;
				Q_snprintf(temp, sizeof(temp) - 1, "%s\\localization\\%s_%s", baseDir, pDefaultDir, language);
				temp[sizeof(temp) - 1] = 0;
				CStringHandler::FixSlashes(temp);
				mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
			};
		};
	};

	if(bEnableHDPack)
	{
		Q_snprintf(temp, sizeof(temp) - 1, "%s/%s_hd", GetBaseDirectory(), pDefaultDir);
		temp[sizeof(temp) - 1] = 0;
		CStringHandler::FixSlashes(temp);
		mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");
	};

	Q_snprintf(temp, sizeof(temp) - 1, "%s", GetBaseDirectory());
	temp[sizeof(temp) - 1] = 0;
	CStringHandler::FixSlashes(temp);
	mpFileSystem->AddSearchPath(temp, "BASE");

	Q_snprintf(temp, sizeof(temp) - 1, "%s/%s", GetBaseDirectory(), pDefaultDir);
	temp[sizeof(temp) - 1] = 0;
	CStringHandler::FixSlashes(temp);
	mpFileSystem->AddSearchPathNoWrite(temp, "DEFAULTGAME");

	Q_snprintf(temp, sizeof(temp) - 1, "%s/platform", GetBaseDirectory());
	temp[sizeof(temp) - 1] = 0;
	CStringHandler::FixSlashes(temp);
	mpFileSystem->AddSearchPath(temp, "PLATFORM");

	return 1;
};

void CFileSystem::AddDefaultDir(char *pszDir)
{
	if(pszDir && *pszDir)
		AddFallbackGameDir(pszDir);
};

int CFileSystem::AddFallbackGameDir(const char *pGameDir)
{
	char language[128];

	const char *pchLang = ""; //CRehldsPlatformHolder::get()->SteamApps() ? CRehldsPlatformHolder::get()->SteamApps()->GetCurrentGameLanguage() : NULL;
	
	Q_strncpy(language, pchLang ? pchLang : "english", ARRAYSIZE(language));

#ifdef REHLDS_CHECKS
	language[ARRAYSIZE(language) - 1] = 0;
#endif

	if(Q_strlen(language) != 0 && Q_stricmp(language, "english"))
	{
		char temp[MAX_PATH];
		Q_sprintf(temp, "%s/%s_%s", GetBaseDirectory(), pGameDir, language);
		mpFileSystem->AddSearchPath(temp, "GAME");
	};

	mpFileSystem->AddSearchPath(pGameDir, "GAME");
	return 1;
};

// NOTE: Pointer to filesystem could be null!

void CFileSystem::RemoveAllSearchPaths()
{
	mpFileSystem->RemoveAllSearchPaths();
};

void CFileSystem::AddSearchPath(const char *pPath, const char *pathID)
{
	mpFileSystem->AddSearchPath(pPath, pathID);
};

NOXREF int CFileSystem::RemoveSearchPath(const char *pPath)
{
	NOXREFCHECK;

	return mpFileSystem->RemoveSearchPath(pPath);
};

void CFileSystem::RemoveFile(const char *pRelativePath, const char *pathID)
{
	mpFileSystem->RemoveFile(pRelativePath, pathID);
};

void CFileSystem::CreateDirHierarchy(const char *path, const char *pathID)
{
	mpFileSystem->CreateDirHierarchy(path, pathID);
};

int CFileSystem::FileExists(const char *pFileName)
{
	return mpFileSystem->FileExists(pFileName);
};

NOXREF int CFileSystem::IsDirectory(const char *pFileName)
{
	NOXREFCHECK;

	return mpFileSystem->IsDirectory(pFileName);
};

FileHandle_t CFileSystem::Open(const char *pFileName, const char *pOptions)
{
	return mpFileSystem->Open(pFileName, pOptions, 0);
};

FileHandle_t CFileSystem::OpenPathID(const char *pFileName, const char *pOptions, const char *pathID)
{
	return mpFileSystem->Open(pFileName, pOptions, pathID);
};

void CFileSystem::Close(CFile *file)
{
	mpFileSystem->Close(file->GetHandle());
};

// merged with int EXT_FUNC COM_FileSize(char *filename)
unsigned int CFileSystem::FileSize(const char *pFileName)
{
	int iSize = -1;
	
	// I think it's already handled by IFileSystem...
	// I even think that this is the actual impl inside the IFileSystem...
	
	FileHandle_t fp = Open(pFileName, "rb");
	
	if(fp)
	{
		iSize = Size(fp);
		Close(fp);
	};
	
	return iSize; // ouch...
	
	//return mpFileSystem->Size(pFileName);
};

NOXREF int CFileSystem::ExpandFileName(char *filename)
{
	NOXREFCHECK;

	char netpath[MAX_PATH];

	GetLocalPath(filename, netpath, ARRAYSIZE(netpath));
	Q_strcpy(filename, netpath);
	return *filename != 0;
};

int32 CFileSystem::GetFileTime(const char *pFileName)
{
	return mpFileSystem->GetFileTime(pFileName);
};

NOXREF void CFileSystem::FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, int32 fileTime)
{
	NOXREFCHECK;

	mpFileSystem->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime);
};

const char *CFileSystem::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return mpFileSystem->FindFirst(pWildCard, pHandle, pathID);
};

const char *CFileSystem::FindNext(FileFindHandle_t handle)
{
	return mpFileSystem->FindNext(handle);
};

NOXREF int CFileSystem::FindIsDirectory(FileFindHandle_t handle)
{
	NOXREFCHECK;

	return mpFileSystem->FindIsDirectory(handle);
};

void CFileSystem::FindClose(FileFindHandle_t handle)
{
	return mpFileSystem->FindClose(handle);
};

void CFileSystem::GetLocalCopy(const char *pFileName)
{
	mpFileSystem->GetLocalCopy(pFileName);
};

const char *CFileSystem::GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return mpFileSystem->GetLocalPath(pFileName, pLocalPath, localPathBufferSize);
};

NOXREF char *CFileSystem::ParseFile(char *pFileBytes, char *pToken, int *pWasQuoted)
{
	NOXREFCHECK;

	bool wasquoted;
	char *result = mpFileSystem->ParseFile(pFileBytes, pToken, &wasquoted);

	if(pWasQuoted)
		*pWasQuoted = wasquoted;

	return result;
};

NOXREF int CFileSystem::FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	NOXREFCHECK;

	return mpFileSystem->FullPathToRelativePath(pFullpath, pRelative);
};

NOXREF int CFileSystem::GetCurrentDirectory(char *pDirectory, int maxlen)
{
	NOXREFCHECK;

	return mpFileSystem->GetCurrentDirectory(pDirectory, maxlen);
};

NOXREF void CFileSystem::PrintOpenedFiles()
{
	NOXREFCHECK;

	mpFileSystem->PrintOpenedFiles();
};

NOXREF void CFileSystem::SetWarningFunc(void (*pfnWarning)(const char *, ...))
{
	NOXREFCHECK;

	mpFileSystem->SetWarningFunc(pfnWarning);
};

NOXREF void CFileSystem::SetWarningLevel(FileWarningLevel_t level)
{
	NOXREFCHECK;

	mpFileSystem->SetWarningLevel(level);
};

void CFileSystem::LogLevelLoadStarted(const char *name)
{
	mpFileSystem->LogLevelLoadStarted(name);
};

void CFileSystem::LogLevelLoadFinished(const char *name)
{
	mpFileSystem->LogLevelLoadFinished(name);
};

int CFileSystem::SetVBuf(FileHandle_t stream, char *buffer, int mode, size_t size)
{
	return mpFileSystem->SetVBuf(stream, buffer, mode, size);
};

void CFileSystem::GetInterfaceVersion(char *p, int maxlen)
{
	mpFileSystem->GetInterfaceVersion(p, maxlen);
};

void *CFileSystem::GetReadBuffer(FileHandle_t file, int *outBufSize)
{
	return mpFileSystem->GetReadBuffer(file, outBufSize, 1);
};

void CFileSystem::ReleaseReadBuffer(FileHandle_t file, void *buffer)
{
	mpFileSystem->ReleaseReadBuffer(file, buffer);
};

void CFileSystem::Unlink(const char *filename)
{
	char localPath[512];
	GetLocalPath(filename, localPath, 512);
	_unlink(localPath);
};

void CFileSystem::Rename(const char *originalName, const char *newName)
{
	char localPath[512];
	char newPath[512];

	if(GetLocalPath(originalName, localPath, ARRAYSIZE(localPath)))
	{
		Q_strcpy(newPath, localPath);
		char *cut = Q_strstr(newPath, originalName);

		if(cut)
		{
			*cut = 0;
#ifdef REHLDS_CHECKS
			Q_strncat(newPath, newName, ARRAYSIZE(newPath) - Q_strlen(newPath));
			newPath[ARRAYSIZE(newPath) - 1] = 0;
#else
			Q_strcat(newPath, newName);
#endif

			rename(localPath, newPath);
		};
	};
};

void *FS_LoadLibrary(const char *dllName)
{
	void *result = NULL;

	if(dllName)
	{
		//FS_GetLocalCopy(dllName);
#ifdef _WIN32
		result = LoadLibraryA(dllName);
#else
		result = dlopen(dllName, 2);
#endif
	};

	return result;
};

bool BEnabledHDAddon()
{
	//if(mpCmdLine->CheckArg("-nohdmodels"))
		//return false;

	return false; //(registry->ReadInt("hdmodels", 1) > 0);
}

bool BEnableAddonsFolder()
{
	//if(mpCmdLine->CheckArg("-addons"))
		//return false;

	return false; //(registry->ReadInt("addons_folder", 0) > 0);
};

void Host_SetHDModels_f()
{
	//if(cls.state && Cmd_Argc() == 2)
	{
		//bool bEnabled = (registry->ReadInt("hdmodels", 1) > 0);

		//registry->WriteInt("hdmodels", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);

		//if(bEnabled != BEnabledHDAddon())
			//COM_SetupDirectories();
	};
};

void Host_SetAddonsFolder_f()
{
	//if(cls.state && Cmd_Argc() == 2)
	{
		//bool bEnabled = (registry->ReadInt("addons_folder", 0) > 0);

		//registry->WriteInt("addons_folder", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);

		//if(bEnabled != BEnableAddonsFolder())
			//COM_SetupDirectories();
	};
}

void Host_SetVideoLevel_f()
{
	//if(cls.state && Cmd_Argc() == 2)
		//registry->WriteInt("vid_level", !Q_stricmp(Cmd_Argv(1), "1") ? 1 : 0);
}

int Host_GetVideoLevel()
{
	return 0; //registry->ReadInt("vid_level", 0);
}

void CFileSystem::CheckLiblistForFallbackDir(const char *pGameDir, bool bLanguage, const char *pLanguage, bool bLowViolenceBuild_)
{
	char szTemp[512];
	FileHandle_t hFile;

	Q_snprintf(szTemp, sizeof(szTemp) - 1, "%s/liblist.gam", pGameDir);
	
	CStringHandler::FixSlashes(szTemp);
	
	GetLocalCopy(szTemp);

	//if(Q_stricmp(com_gamedir, pGameDir))
	{
		Q_snprintf(szTemp, 511, "../%s/liblist.gam", pGameDir);
		CStringHandler::FixSlashes(szTemp);
		hFile = Open(szTemp, "rt");
	}
	//else
		//hFile = Open("liblist.gam", "rt");

	if(!hFile)
		return;

	if(EndOfFile(hFile))
	{
		Close(hFile);
		return;
	};

	char szFallback[128];
	char szLine[512];

	char *end;
	char *start;
	int bytesToCopy;

	while(1)
	{
		szLine[0] = 0;
		ReadLine(szLine, sizeof(szLine) - 1, hFile);
		szLine[511] = 0;

		if(!Q_strnicmp(szLine, "fallback_dir", Q_strlen("fallback_dir")))
		{
			start = Q_strchr(szLine, '"');

			if(!start)
			{
				Close(hFile);
				return;
			};

			end = Q_strchr(start + 1, '"');

			if(!end)
			{
				Close(hFile);
				return;
			};

			bytesToCopy = (int)(end - start) - 1;

			if(bytesToCopy > sizeof(szFallback) - 2)
			{
				Close(hFile);
				return;
			};

			if(bytesToCopy > 0)
				break;
		};

		if(EndOfFile(hFile))
		{
			Close(hFile);
			return;
		};
	};

	Q_strncpy(szFallback, start + 1, bytesToCopy);
	szFallback[bytesToCopy] = 0;

	if(!Q_stricmp(pGameDir, szFallback))
	{
		Close(hFile);
		return;
	};

	if(bLowViolenceBuild)
	{
		Q_snprintf(szTemp, 511, "%s/%s_lv", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		CStringHandler::FixSlashes(szTemp);
		mpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	};

	if(BEnableAddonsFolder())
	{
		Q_snprintf(szTemp, 511, "%s/%s_addon", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		CStringHandler::FixSlashes(szTemp);
		mpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	};

	if(bLanguage && pLanguage)
	{
		char baseDir[4096];
		char *tempPtr;

		Q_snprintf(szTemp, 511, "%s/%s_%s", GetBaseDirectory(), szFallback, pLanguage);
		szTemp[511] = 0;
		CStringHandler::FixSlashes(szTemp);
		mpFileSystem->AddSearchPath(szTemp, "GAME_FALLBACK");

		//if(!mpCmdLine->CheckArg("-steam"))
		{
			Q_strncpy(baseDir, GetBaseDirectory(), sizeof(baseDir) - 1);
			baseDir[sizeof(baseDir) - 1] = 0;

			tempPtr = Q_strstr(baseDir, "\\game");
			if(tempPtr)
			{
				*tempPtr = 0;

				Q_snprintf(szTemp, 511, "%s\\localization\\%s_%s", baseDir, szFallback, pLanguage);
				szTemp[511] = 0;

				CStringHandler::FixSlashes(szTemp);
				AddSearchPath(szTemp, "GAME_FALLBACK");
			};
		};
	};

	if(BEnabledHDAddon())
	{
		Q_snprintf(szTemp, 511, "%s/%s_hd", GetBaseDirectory(), szFallback);
		szTemp[511] = 0;
		CStringHandler::FixSlashes(szTemp);
		mpFileSystem->AddSearchPathNoWrite(szTemp, "GAME_FALLBACK");
	};

	Q_snprintf(szTemp, 511, "%s/%s", GetBaseDirectory(), szFallback);
	szTemp[511] = 0;
	CStringHandler::FixSlashes(szTemp);
	AddSearchPath(szTemp, "GAME_FALLBACK");

	if(Q_stricmp(szFallback, "valve"))
	{
		const int BufLen = 128;
		char *szFileName = new char[BufLen];

		Q_snprintf(szFileName, BufLen - 1, "Resource/%s_%%language%%.txt", szFallback);
		szFileName[BufLen - 1] = 0;

		g_fallbackLocalizationFiles.AddToTail(szFileName);
		CheckLiblistForFallbackDir(szFallback, bLanguage, pLanguage, bLowViolenceBuild);
	};
	
	Close(hFile);
};

// TODO: finish me!
bool CFileSystem::SetupDirectories()
{
	char pDirName[512] = {0};

	//com_clientfallback[0] = 0;
	com_gamedir[0] = 0;

	ParseDirectoryFromCmd("-basedir", pDirName, "valve");
	ParseDirectoryFromCmd("-game", com_gamedir, pDirName);

	if(SetGameDirectory(pDirName, (const char *)(com_gamedir[0] != 0 ? com_gamedir : 0)))
	{
		Info_SetValueForStarKey(Cvar_Serverinfo(), "*gamedir", com_gamedir, MAX_INFO_STRING);
		return true;
	};

	return false;
};

void CFileSystem::ParseDirectoryFromCmd(const char *pCmdName, char *pDirName, const char *pDefault)
{
	const char *pParameter = "";
	int cmdParameterIndex = gpCmdLine->FindArg((char *)pCmdName);

	if(cmdParameterIndex && cmdParameterIndex < gpCmdLine->GetArgCount() - 1)
	{
		pParameter = gpCmdLine->GetArgVal(cmdParameterIndex + 1); // com_argv[cmdParameterIndex + 1]

		if(*pParameter == '-' || *pParameter == '+')
			pParameter = "";
	};

	// Found a valid parameter on the cmd line?
	if(pParameter)
	{
		// Grab it
		Q_strcpy(pDirName, pParameter);
	}
	else if(pDefault)
	{
		// Ok, then use the default
		Q_strcpy(pDirName, pDefault);
	}
	else
	{
		// If no default either, then just terminate the string
		pDirName[0] = 0;
	};

	CStringHandler::StripTrailingSlash(pDirName);
};