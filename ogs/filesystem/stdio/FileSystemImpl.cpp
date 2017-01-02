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

#include <stdio.h>
#include "FileSystemImpl.hpp"

static CFileSystemImpl gFileSystemNull;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFileSystemImpl, IFileSystem, FILESYSTEM_INTERFACE_VERSION, gFileSystemNull);

CFileSystemImpl::CFileSystemImpl(){};

CFileSystemImpl::~CFileSystemImpl(){};

void CFileSystemImpl::Mount(){};

void CFileSystemImpl::Unmount(){};

void CFileSystemImpl::RemoveAllSearchPaths(){};

void CFileSystemImpl::AddSearchPath(const char *pPath, const char *pathID){};

bool CFileSystemImpl::RemoveSearchPath(const char *pPath)
{
	return false;
};

void CFileSystemImpl::RemoveFile(const char *pRelativePath, const char *pathID){};

void CFileSystemImpl::CreateDirHierarchy(const char *path, const char *pathID){};

bool CFileSystemImpl::FileExists(const char *pFileName)
{
	return false;
};

bool CFileSystemImpl::IsDirectory(const char *pFileName)
{
	return false;
};

FileHandle_t CFileSystemImpl::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	return NULL;
};

void CFileSystemImpl::Close(FileHandle_t file)
{
	close(file);
};

void CFileSystemImpl::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	lseek(file, pos, SEEK_SET);
};

unsigned int CFileSystemImpl::Tell(FileHandle_t file)
{
	return 0;
};

unsigned int CFileSystemImpl::Size(FileHandle_t file)
{
	return 0;
};

unsigned int CFileSystemImpl::Size(const char *pFileName)
{
	return 0;
};

/*
============
Sys_FileTime

returns -1 if not present
============
*/
long CFileSystemImpl::GetFileTime(const char *pFileName)
{
	struct stat buf;

	if(stat(pFileName, &buf) == -1)
		return -1;

	return buf.st_mtime;
};

void CFileSystemImpl::FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, long fileTime){};

bool CFileSystemImpl::IsOk(FileHandle_t file)
{
	return false;
};

void CFileSystemImpl::Flush(FileHandle_t file){};

bool CFileSystemImpl::EndOfFile(FileHandle_t file)
{
	return false;
};

int CFileSystemImpl::Read(void *pOutput, int size, FileHandle_t file)
{
	return read(file, pOutput, size);
};

int CFileSystemImpl::Write(void const *pInput, int size, FileHandle_t file)
{
	return write(file, pInput, size);
};

char *CFileSystemImpl::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	return NULL;
};

int CFileSystemImpl::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	return 0;
};

void *CFileSystemImpl::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	return NULL;
};

void CFileSystemImpl::ReleaseReadBuffer(FileHandle_t file, void *readBuffer){};

const char *CFileSystemImpl::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return "";
};

const char *CFileSystemImpl::FindNext(FileFindHandle_t handle)
{
	return "";
};

bool CFileSystemImpl::FindIsDirectory(FileFindHandle_t handle)
{
	return false;
};

void CFileSystemImpl::FindClose(FileFindHandle_t handle){};

void CFileSystemImpl::GetLocalCopy(const char *pFileName){};

const char *CFileSystemImpl::GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return "";
};

char *CFileSystemImpl::ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted)
{
	return NULL;
};

bool CFileSystemImpl::FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	return false;
};

bool CFileSystemImpl::GetCurrentDirectory(char *pDirectory, int maxlen)
{
	return false;
};

void CFileSystemImpl::PrintOpenedFiles(){};

void CFileSystemImpl::SetWarningFunc(void (*pfnWarning)(const char *fmt, ...)){};

void CFileSystemImpl::SetWarningLevel(FileWarningLevel_t level){};

void CFileSystemImpl::LogLevelLoadStarted(const char *name){};

void CFileSystemImpl::LogLevelLoadFinished(const char *name){};

int CFileSystemImpl::HintResourceNeed(const char *hintlist, int forgetEverything)
{
	return 0;
};

int CFileSystemImpl::PauseResourcePreloading()
{
	return 0;
};

int CFileSystemImpl::ResumeResourcePreloading()
{
	return 0;
};

int CFileSystemImpl::SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	return 0;
};

void CFileSystemImpl::GetInterfaceVersion(char *p, int maxlen){};

bool CFileSystemImpl::IsFileImmediatelyAvailable(const char *pFileName)
{
	return false;
};

WaitForResourcesHandle_t CFileSystemImpl::WaitForResources(const char *resourcelist)
{
	return 0;
};

bool CFileSystemImpl::GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */)
{
	return false;
};

void CFileSystemImpl::CancelWaitForResources(WaitForResourcesHandle_t handle){};

bool CFileSystemImpl::IsAppReadyForOfflinePlay(int appID)
{
	return false;
};

bool CFileSystemImpl::AddPackFile(const char *fullpath, const char *pathID)
{
	return false;
};

FileHandle_t CFileSystemImpl::OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID)
{
	return NULL;
};

void CFileSystemImpl::AddSearchPathNoWrite(const char *pPath, const char *pathID){};