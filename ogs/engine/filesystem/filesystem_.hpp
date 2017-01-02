/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#pragma once

#include "maintypes.h"
#include "public/interface.h"
#include "tier1/utlvector.h"

#ifdef _WIN32
#define FILESYSTEM_DLL_NAME "filesystem_stdio.dll"
#else
#define FILESYSTEM_DLL_NAME "filesystem_stdio.so"
#endif

#ifdef HOOK_ENGINE
#define g_fallbackLocalizationFiles (*pg_fallbackLocalizationFiles)
#define s_pBaseDir (*ps_pBaseDir)
#define bLowViolenceBuild (*pbLowViolenceBuild)

#define g_pFileSystemModule (*pg_pFileSystemModule)
#define g_FileSystemFactory (*pg_FileSystemFactory)
#endif // HOOK_ENGINE

extern CUtlVector<char *> g_fallbackLocalizationFiles;
extern char               s_pBaseDir[512];
extern bool               bLowViolenceBuild;

extern CSysModule *      g_pFileSystemModule;
extern CreateInterfaceFn g_FileSystemFactory;

const char * GetBaseDirectory();
NOXREF void *GetFileSystemFactory();

bool FileSystem_LoadDLL(CreateInterfaceFn filesystemFactory);
void FileSystem_UnloadDLL();

bool BEnabledHDAddon();
bool BEnableAddonsFolder();

void Host_SetHDModels_f();
void Host_SetAddonsFolder_f();
void Host_SetVideoLevel_f();

int Host_GetVideoLevel();

void CheckLiblistForFallbackDir(const char *pGameDir, bool bLanguage, const char *pLanguage, bool bLowViolenceBuild_);

int FileSystem_SetGameDirectory(const char *pDefaultDir, const char *pGameDir);
int FileSystem_AddFallbackGameDir(const char *pGameDir);
int FileSystem_Init(char *basedir, void *voidfilesystemFactory);
void FileSystem_Shutdown();