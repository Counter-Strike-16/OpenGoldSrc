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

#include "public/FileSystem.h"

class CFileSystemImpl : public IFileSystem
{
public:
	CFileSystemImpl();
	~CFileSystemImpl();

	void Mount();
	void Unmount();

	void RemoveAllSearchPaths();

	void AddSearchPath(const char *pPath, const char *pathID);
	bool RemoveSearchPath(const char *pPath);

	void RemoveFile(const char *pRelativePath, const char *pathID);

	void CreateDirHierarchy(const char *path, const char *pathID);

	bool FileExists(const char *pFileName);
	bool IsDirectory(const char *pFileName);

	FileHandle_t Open(const char *pFileName, const char *pOptions, const char *pathID = 0L);
	void Close(FileHandle_t file);

	void Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType);
	unsigned int Tell(FileHandle_t file);

	unsigned int Size(FileHandle_t file);
	unsigned int Size(const char *pFileName);

	long GetFileTime(const char *pFileName);
	void FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime);

	bool IsOk(FileHandle_t file);

	void Flush(FileHandle_t file);
	bool EndOfFile(FileHandle_t file);

	int Read(void *pOutput, int size, FileHandle_t file);
	int Write(void const *pInput, int size, FileHandle_t file); // void const
	char *ReadLine(char *pOutput, int maxChars, FileHandle_t file);
	int FPrintf(FileHandle_t file, char *pFormat, ...);

	void *GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache);
	void ReleaseReadBuffer(FileHandle_t file, void *readBuffer);

	const char *FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID = 0L);
	const char *FindNext(FileFindHandle_t handle);
	bool FindIsDirectory(FileFindHandle_t handle);
	void FindClose(FileFindHandle_t handle);

	void GetLocalCopy(const char *pFileName);

	const char *GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize);

	char *ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted);

	bool FullPathToRelativePath(const char *pFullpath, char *pRelative);

	bool GetCurrentDirectory(char *pDirectory, int maxlen);

	void PrintOpenedFiles();

	void SetWarningFunc(void (*pfnWarning)(const char *fmt, ...));
	void SetWarningLevel(FileWarningLevel_t level);

	void LogLevelLoadStarted(const char *name);
	void LogLevelLoadFinished(const char *name);
	int HintResourceNeed(const char *hintlist, int forgetEverything);
	int PauseResourcePreloading();
	int ResumeResourcePreloading();
	int SetVBuf(FileHandle_t stream, char *buffer, int mode, long size);
	void GetInterfaceVersion(char *p, int maxlen);
	bool IsFileImmediatelyAvailable(const char *pFileName);

	WaitForResourcesHandle_t WaitForResources(const char *resourcelist);
	bool GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */);
	void CancelWaitForResources(WaitForResourcesHandle_t handle);

	bool IsAppReadyForOfflinePlay(int appID);

	bool AddPackFile(const char *fullpath, const char *pathID);

	FileHandle_t OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID = 0L);

	void AddSearchPathNoWrite(const char *pPath, const char *pathID);
};