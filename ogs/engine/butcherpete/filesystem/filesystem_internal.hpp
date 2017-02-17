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

#pragma once

#include "maintypes.h"
#include "public/FileSystem.h"
#include "public/archtypes.h"

#ifdef HOOK_ENGINE
#define g_pFileSystem (*pg_pFileSystem)
#endif

extern IFileSystem *gpFileSystem;

void FS_AddSearchPath(const char *pPath, const char *pathID);
NOXREF int FS_RemoveSearchPath(const char *pPath);
void FS_RemoveAllSearchPaths();

void FS_RemoveFile(const char *pRelativePath, const char *pathID);
void FS_CreateDirHierarchy(const char *path, const char *pathID);

int FS_FileExists(const char *pFileName);
NOXREF int FS_IsDirectory(const char *pFileName);

FileHandle_t FS_Open(const char *pFileName, const char *pOptions);
FileHandle_t FS_OpenPathID(const char *pFileName, const char *pOptions, const char *pathID);

void FS_Close(FileHandle_t file);

void FS_Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType);
unsigned int FS_Tell(FileHandle_t file);
unsigned int FS_Size(FileHandle_t file);
unsigned int FS_FileSize(const char *pFileName);
int32 FS_GetFileTime(const char *pFileName);
NOXREF void FS_FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, int32 fileTime);
int FS_IsOk(FileHandle_t file);
void FS_Flush(FileHandle_t file);
int FS_EndOfFile(FileHandle_t file);
int FS_Read(void *pOutput, int size, int count, FileHandle_t file);
int FS_Write(const void *pInput, int size, int count, FileHandle_t file);
char *FS_ReadLine(char *pOutput, int maxChars, FileHandle_t file);
int FS_FPrintf(FileHandle_t file, char *pFormat, ...);
const char *FS_FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID);
const char *FS_FindNext(FileFindHandle_t handle);
NOXREF int FS_FindIsDirectory(FileFindHandle_t handle);
void FS_FindClose(FileFindHandle_t handle);
void FS_GetLocalCopy(const char *pFileName);
const char *FS_GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize);
NOXREF char *FS_ParseFile(char *pFileBytes, char *pToken, int *pWasQuoted);
NOXREF int FS_FullPathToRelativePath(const char *pFullpath, char *pRelative);
NOXREF int FS_GetCurrentDirectory(char *pDirectory, int maxlen);
NOXREF void FS_PrintOpenedFiles();
NOXREF void FS_SetWarningFunc(void (*pfnWarning)(const char *, ...));
NOXREF void FS_SetWarningLevel(FileWarningLevel_t level);
NOXREF unsigned char FS_GetCharacter(FileHandle_t f);
void FS_LogLevelLoadStarted(const char *name);
void FS_LogLevelLoadFinished(const char *name);
int FS_SetVBuf(FileHandle_t stream, char *buffer, int mode, size_t size);
void FS_GetInterfaceVersion(char *p, int maxlen);
void *FS_GetReadBuffer(FileHandle_t file, int *outBufSize);
void FS_ReleaseReadBuffer(FileHandle_t file, void *buffer);
void FS_Unlink(const char *filename);
void FS_Rename(const char *originalName, const char *newName);
void *FS_LoadLibrary(const char *dllName);