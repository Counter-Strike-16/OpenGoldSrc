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
#include "filesystem/filesystem_internal.hpp"
#include "system/common.hpp"
#include "system/system.hpp"

IFileSystem *gpFileSystem = nullptr;

NOXREF void FS_RemoveAllSearchPaths()
{
	// NOXREFCHECK;	// We call it

	gpFileSystem->RemoveAllSearchPaths();
}

void FS_AddSearchPath(const char *pPath, const char *pathID)
{
	gpFileSystem->AddSearchPath(pPath, pathID);
}

NOXREF int FS_RemoveSearchPath(const char *pPath)
{
	NOXREFCHECK;

	return gpFileSystem->RemoveSearchPath(pPath);
}

void FS_RemoveFile(const char *pRelativePath, const char *pathID)
{
	gpFileSystem->RemoveFile(pRelativePath, pathID);
}

void FS_CreateDirHierarchy(const char *path, const char *pathID)
{
	gpFileSystem->CreateDirHierarchy(path, pathID);
}

int FS_FileExists(const char *pFileName)
{
	return gpFileSystem->FileExists(pFileName);
}

NOXREF int FS_IsDirectory(const char *pFileName)
{
	NOXREFCHECK;

	return gpFileSystem->IsDirectory(pFileName);
}

FileHandle_t FS_Open(const char *pFileName, const char *pOptions)
{
	return gpFileSystem->Open(pFileName, pOptions, 0);
}

FileHandle_t FS_OpenPathID(const char *pFileName, const char *pOptions, const char *pathID)
{
	return gpFileSystem->Open(pFileName, pOptions, pathID);
}

void FS_Close(FileHandle_t file)
{
	gpFileSystem->Close(file);
}

void FS_Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	gpFileSystem->Seek(file, pos, seekType);
}

unsigned int FS_Tell(FileHandle_t file)
{
	return gpFileSystem->Tell(file);
}

unsigned int FS_Size(FileHandle_t file)
{
	return gpFileSystem->Size(file);
}

unsigned int FS_FileSize(const char *pFileName)
{
	return gpFileSystem->Size(pFileName);
}

int32 FS_GetFileTime(const char *pFileName)
{
	return gpFileSystem->GetFileTime(pFileName);
}

NOXREF void FS_FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, int32 fileTime)
{
	NOXREFCHECK;

	gpFileSystem->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime);
}

int FS_IsOk(FileHandle_t file)
{
	return gpFileSystem->IsOk(file);
}

void FS_Flush(FileHandle_t file)
{
	gpFileSystem->Flush(file);
}

int FS_EndOfFile(FileHandle_t file)
{
	return gpFileSystem->EndOfFile(file);
}

int FS_Read(void *pOutput, int size, int count, FileHandle_t file)
{
#ifdef REHLDS_FIXES
	return gpFileSystem->Read(pOutput, size * count, file);
#else  // REHLDS_FIXES
	return gpFileSystem->Read(pOutput, size, file);
#endif // REHLDS_FIXES
}

int FS_Write(const void *pInput, int size, int count, FileHandle_t file)
{
#ifdef REHLDS_FIXES
	return gpFileSystem->Write(pInput, size * count, file);
#else  // REHLDS_FIXES
	return gpFileSystem->Write(pInput, size, file);
#endif // REHLDS_FIXES
}

char *FS_ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	return gpFileSystem->ReadLine(pOutput, maxChars, file);
}

int FS_FPrintf(FileHandle_t file, char *pFormat, ...)
{
	char data[8192];
	va_list va;

	va_start(va, pFormat);
	vsprintf(data, pFormat, va);
	va_end(va);

	return gpFileSystem->FPrintf(file, "%s", data);
}

const char *FS_FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return gpFileSystem->FindFirst(pWildCard, pHandle, pathID);
}

const char *FS_FindNext(FileFindHandle_t handle)
{
	return gpFileSystem->FindNext(handle);
}

NOXREF int FS_FindIsDirectory(FileFindHandle_t handle)
{
	NOXREFCHECK;

	return gpFileSystem->FindIsDirectory(handle);
}

void FS_FindClose(FileFindHandle_t handle)
{
	return gpFileSystem->FindClose(handle);
}

void FS_GetLocalCopy(const char *pFileName)
{
	gpFileSystem->GetLocalCopy(pFileName);
}

const char *FS_GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return gpFileSystem->GetLocalPath(pFileName, pLocalPath, localPathBufferSize);
}

NOXREF char *FS_ParseFile(char *pFileBytes, char *pToken, int *pWasQuoted)
{
	NOXREFCHECK;

	bool wasquoted;
	char *result = gpFileSystem->ParseFile(pFileBytes, pToken, &wasquoted);

	if(pWasQuoted)
		*pWasQuoted = wasquoted;

	return result;
}

NOXREF int FS_FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	NOXREFCHECK;

	return gpFileSystem->FullPathToRelativePath(pFullpath, pRelative);
}

NOXREF int FS_GetCurrentDirectory(char *pDirectory, int maxlen)
{
	NOXREFCHECK;

	return gpFileSystem->GetCurrentDirectory(pDirectory, maxlen);
}

NOXREF void FS_PrintOpenedFiles()
{
	NOXREFCHECK;

	gpFileSystem->PrintOpenedFiles();
}

NOXREF void FS_SetWarningFunc(void (*pfnWarning)(const char *, ...))
{
	NOXREFCHECK;

	gpFileSystem->SetWarningFunc(pfnWarning);
}

NOXREF void FS_SetWarningLevel(FileWarningLevel_t level)
{
	NOXREFCHECK;

	gpFileSystem->SetWarningLevel(level);
}

NOXREF unsigned char FS_GetCharacter(FileHandle_t f)
{
	NOXREFCHECK;

	uint8 retval;
	gpFileSystem->Read(&retval, 1, f);
	return retval;
}

void FS_LogLevelLoadStarted(const char *name)
{
	gpFileSystem->LogLevelLoadStarted(name);
}

void FS_LogLevelLoadFinished(const char *name)
{
	gpFileSystem->LogLevelLoadFinished(name);
}

int FS_SetVBuf(FileHandle_t stream, char *buffer, int mode, size_t size)
{
	return gpFileSystem->SetVBuf(stream, buffer, mode, size);
}

void FS_GetInterfaceVersion(char *p, int maxlen)
{
	gpFileSystem->GetInterfaceVersion(p, maxlen);
}

void *FS_GetReadBuffer(FileHandle_t file, int *outBufSize)
{
	return gpFileSystem->GetReadBuffer(file, outBufSize, 1);
}

void FS_ReleaseReadBuffer(FileHandle_t file, void *buffer)
{
	gpFileSystem->ReleaseReadBuffer(file, buffer);
}

void FS_Unlink(const char *filename)
{
	char localPath[512];
	FS_GetLocalPath(filename, localPath, 512);
	_unlink(localPath);
}

void FS_Rename(const char *originalName, const char *newName)
{
	char *cut;
	char localPath[512];
	char newPath[512];

	if(FS_GetLocalPath(originalName, localPath, ARRAYSIZE(localPath)))
	{
		Q_strcpy(newPath, localPath);
		cut = Q_strstr(newPath, originalName);

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
		}
	}
}

void *FS_LoadLibrary(const char *dllName)
{
	void *result = NULL;

	if(dllName)
	{
		FS_GetLocalCopy(dllName);
#ifdef _WIN32
		result = LoadLibraryA(dllName);
#else
		result = dlopen(dllName, 2);
#endif
	}

	return result;
}