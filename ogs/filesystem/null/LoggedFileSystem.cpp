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

#include "FileSystemImpl.hpp"
#include <cstdarg>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
static FILE *ghLogFile = NULL;

void TRACE(const char *asMsg, ...)
{
	if(!ghLogFile)
		return;

	va_list ArgList;
	char sMsg[512];

	va_start(ArgList, asMsg);
	vsnprintf(sMsg, sizeof(sMsg), asMsg, ArgList);
	va_end(ArgList);

	sprintf(sMsg, "%s\n", sMsg);
	fputs(sMsg, ghLogFile);
	fflush(ghLogFile);
};
#else
void TRACE(const char *asMsg, ...){};
#endif

static CFileSystemNull gFileSystemNull;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFileSystemNull, IFileSystem, FILESYSTEM_INTERFACE_VERSION, gFileSystemNull);

CFileSystemNull::CFileSystemNull(){};

CFileSystemNull::~CFileSystemNull(){};

void CFileSystemNull::Mount()
{
#ifdef _DEBUG
	ghLogFile = fopen("TRACE.log", "w+");
#endif

	TRACE("CFileSystem::Mount");
	mpImpl->Mount();
};

void CFileSystemNull::Unmount()
{
	TRACE("CFileSystem::Unmount");

#ifdef _DEBUG
	if(ghLogFile)
		fclose(ghLogFile);
#endif
	
	mpImpl->Unmount();
};

void CFileSystemNull::RemoveAllSearchPaths()
{
	TRACE("CFileSystem::RemoveAllSearchPaths");
	mpImpl->RemoveAllSearchPaths();
};

void CFileSystemNull::AddSearchPath(const char *pPath, const char *pathID)
{
	TRACE("CFileSystem::AddSearchPath(%s, %s)", pPath, pathID);
	mpImpl->AddSearchPath(pPath, pathID);
};

bool CFileSystemNull::RemoveSearchPath(const char *pPath)
{
	TRACE("CFileSystem::RemoveSearchPath(%s)", pPath);
	return mpImpl->RemoveSearchPath(pPath);
};

void CFileSystemNull::RemoveFile(const char *pRelativePath,
                                 const char *pathID)
{
	TRACE("CFileSystem::RemoveFile(%s, %s)", pRelativePath, pathID);
	mpImpl->RemoveFile(pRelativePath, pathID);
};

void CFileSystemNull::CreateDirHierarchy(const char *path, const char *pathID)
{
	TRACE("CFileSystem::CreateDirHierarchy(%s, %s)", path, pathID);
	mpImpl->CreateDirHierarchy(path, pathID);
};

bool CFileSystemNull::FileExists(const char *pFileName)
{
	TRACE("CFileSystem::FileExists(%s)", pFileName);
	return mpImpl->FileExists(pFileName);
};

bool CFileSystemNull::IsDirectory(const char *pFileName)
{
	TRACE("CFileSystem::IsDirectory(%s)", pFileName);
	return mpImpl->IsDirectory(pFileName);
};

FileHandle_t CFileSystemNull::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE("CFileSystem::Open(%s, %s, %s)", pFileName, pOptions, pathID);
	return mpImpl->Open(pFileName, pOptions, pathID);
};

void CFileSystemNull::Close(FileHandle_t file)
{
	TRACE("CFileSystem::Close(%d)", file);
	mpImpl->Close(file);
};

void CFileSystemNull::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	TRACE("CFileSystem::Seek(%d, %d, %d)", file, pos, seekType);
	mpImpl->Seek(file, pos, seekType);
};

unsigned int CFileSystemNull::Tell(FileHandle_t file)
{
	TRACE("CFileSystem::Tell(%d)", file);
	return mpImpl->Tell(file);
};

unsigned int CFileSystemNull::Size(FileHandle_t file)
{
	TRACE("CFileSystem::Size(%d)", file);
	return mpImpl->Size(file);
};

unsigned int CFileSystemNull::Size(const char *pFileName)
{
	TRACE("CFileSystem::Size(%s)", pFileName);
	return mpImpl->Size(pFileName);
};

long CFileSystemNull::GetFileTime(const char *pFileName)
{
	TRACE("CFileSystem::GetFileTime(%s)", pFileName);
	return mpImpl->GetFileTime(pFileName);
};

void CFileSystemNull::FileTimeToString(char *pStrip,
                                       int maxCharsIncludingTerminator,
                                       long fileTime)
{
	TRACE("CFileSystem::FileTimeToString(%s, %d, %l", pStrip, maxCharsIncludingTerminator, fileTime);
	mpImpl->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime);
};

bool CFileSystemNull::IsOk(FileHandle_t file)
{
	TRACE("CFileSystem::IsOk(%d)", file);
	return mpImpl->IsOk(file);
};

void CFileSystemNull::Flush(FileHandle_t file)
{
	TRACE("CFileSystem::Flush(%d)", file);
	mpImpl->Flush(file);
};

bool CFileSystemNull::EndOfFile(FileHandle_t file)
{
	TRACE("CFileSystem::EndOfFile(%d)", file);
	return mpImpl->EndOfFile(file);
};

int CFileSystemNull::Read(void *pOutput, int size, FileHandle_t file)
{
	TRACE("CFileSystem::Read(%p, %d, %d)", pOutput, size, file);
	return mpImpl->Read(pOutput, size, file);
};

int CFileSystemNull::Write(void const *pInput, int size, FileHandle_t file)
{
	TRACE("CFileSystem::Write(%p, %d, %d)", pInput, size, file);
	return mpImpl->Write(pInput, size, file);
};

char *CFileSystemNull::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	TRACE("CFileSystem::ReadLine(%s, %d, %d)", pOutput, maxChars, file);
	return mpImpl->ReadLine(pOutput, maxChars, file);
};

int CFileSystemNull::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	TRACE("CFileSystem::FPrintf(%d, %s)", file, pFormat);
	return mpImpl->FPrintf(file, pFormat); // TODO: fix varargs
};

void *CFileSystemNull::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	TRACE("CFileSystem::GetReadBuffer");
	return mpImpl->GetReadBuffer(file, outBufferSize, failIfNotInCache);
};

void CFileSystemNull::ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
	TRACE("CFileSystem::ReleaseReadBuffer(%d, %p)", file, readBuffer);
	mpImpl->ReleaseReadBuffer(file, readBuffer);
};

const char *CFileSystemNull::FindFirst(const char *pWildCard,
                                       FileFindHandle_t *pHandle,
                                       const char *pathID)
{
	TRACE("CFileSystem::FindFirst(%s, %p, %d)", pWildCard, pHandle, pathID);
	return mpImpl->FindFirst(pWildCard, pHandle, pathID);
};

const char *CFileSystemNull::FindNext(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindNext(%d)", handle);
	return mpImpl->FindNext(handle);
};

bool CFileSystemNull::FindIsDirectory(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindIsDirectory(%d)", handle);
	return mpImpl->FindIsDirectory(handle);
};

void CFileSystemNull::FindClose(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindClose(%d)", handle);
	mpImpl->FindClose(handle);
};

void CFileSystemNull::GetLocalCopy(const char *pFileName)
{
	TRACE("CFileSystem::GetLocalCopy(%s)", pFileName);
	mpImpl->GetLocalCopy(pFileName);
};

const char *CFileSystemNull::GetLocalPath(const char *pFileName,
                                          char *pLocalPath,
                                          int localPathBufferSize)
{
	TRACE("CFileSystem::GetLocalPath(%s, %s, %d)", pFileName, pLocalPath, localPathBufferSize);
	return mpImpl->GetLocalPath(pFileName, pLocalPath, localPathBufferSize);
};

char *CFileSystemNull::ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted)
{
	TRACE("CFileSystem::ParseFile(%s, %s, %p", pFileBytes, pToken, pWasQuoted);
	return mpImpl->ParseFile(pFileBytes, pToken, pWasQuoted);
};

bool CFileSystemNull::FullPathToRelativePath(const char *pFullpath,
                                             char *pRelative)
{
	TRACE("CFileSystem::FullPathToRelativePath(%s, %s)", pFullpath, pRelative);
	return mpImpl->FullPathToRelativePath(pFullpath, pRelative);
};

bool CFileSystemNull::GetCurrentDirectory(char *pDirectory, int maxlen)
{
	TRACE("CFileSystem::GetCurrentDirectory(%s, %d)", pDirectory, maxlen);
	return mpImpl->GetCurrentDirectory(pDirectory, maxlen);
};

void CFileSystemNull::PrintOpenedFiles()
{
	TRACE("CFileSystem::PrintOpenedFiles()");
	mpImpl->PrintOpenedFiles();
};

void CFileSystemNull::SetWarningFunc(void (*pfnWarning)(const char *fmt, ...))
{
	TRACE("CFileSystem::SetWarningFunc");
	mpImpl->SetWarningFunc(pfnWarning);
};

void CFileSystemNull::SetWarningLevel(FileWarningLevel_t level)
{
	TRACE("CFileSystem::SetWarningLevel(%d)", level);
	mpImpl->SetWarningLevel(level);
};

void CFileSystemNull::LogLevelLoadStarted(const char *name)
{
	TRACE("CFileSystem::LogLevelLoadStarted(%s)", name);
	mpImpl->LogLevelLoadStarted(name);
};

void CFileSystemNull::LogLevelLoadFinished(const char *name)
{
	TRACE("CFileSystem::LogLevelLoadFinished(%s)", name);
	mpImpl->LogLevelLoadFinished(name);
};

int CFileSystemNull::HintResourceNeed(const char *hintlist,
                                      int forgetEverything)
{
	TRACE("CFileSystem::HintResourceNeed(%s, %d)", hintlist, forgetEverything);
	return mpImpl->HintResourceNeed(hintlist, forgetEverything);
};

int CFileSystemNull::PauseResourcePreloading()
{
	TRACE("CFileSystem::PauseResourcePreloading()");
	return mpImpl->PauseResourcePreloading();
};

int CFileSystemNull::ResumeResourcePreloading()
{
	TRACE("CFileSystem::ResumeResourcePreloading()");
	return mpImpl->ResumeResourcePreloading();
};

int CFileSystemNull::SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	TRACE("CFileSystem::SetVBuf(%d, %s, %d, %l)", stream, buffer, mode, size);
	return mpImpl->SetVBuf(stream, buffer, mode, size);
};

void CFileSystemNull::GetInterfaceVersion(char *p, int maxlen)
{
	TRACE("CFileSystem::GetInterfaceVersion(%s, %d)", p, maxlen);
	mpImpl->GetInterfaceVersion(p, maxlen);
};

bool CFileSystemNull::IsFileImmediatelyAvailable(const char *pFileName)
{
	TRACE("CFileSystem::IsFileImmediatelyAvailable(%s)", pFileName);
	return mpImpl->IsFileImmediatelyAvailable(pFileName);
};

WaitForResourcesHandle_t CFileSystemNull::WaitForResources(const char *resourcelist)
{
	TRACE("CFileSystem::WaitForResources(%s)", resourcelist);
	return mpImpl->WaitForResources(resourcelist);
};

bool CFileSystemNull::GetWaitForResourcesProgress(
WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */)
{
	TRACE("CFileSystem::GetWaitForResourcesProgress");
	return mpImpl->GetWaitForResourcesProgress(handle, progress, complete);
};

void CFileSystemNull::CancelWaitForResources(WaitForResourcesHandle_t handle)
{
	TRACE("CFileSystem::CancelWaitForResources(%d)", handle);
	mpImpl->CancelWaitForResources(handle);
};

bool CFileSystemNull::IsAppReadyForOfflinePlay(int appID)
{
	TRACE("CFileSystem::IsAppReadyForOfflinePlay(%d)", appID);
	return mpImpl->IsAppReadyForOfflinePlay(appID);
};

bool CFileSystemNull::AddPackFile(const char *fullpath, const char *pathID)
{
	TRACE("CFileSystem::AddPackFile(%s, %s)", fullpath, pathID);
	return mpImpl->AddPackFile(fullpath, pathID);
};

FileHandle_t CFileSystemNull::OpenFromCacheForRead(const char *pFileName,
                                                   const char *pOptions,
                                                   const char *pathID)
{
	TRACE("CFileSystem::OpenFromCacheForRead(%s, %s, %s)", pFileName, pOptions, pathID);
	return mpImpl->OpenFromCacheForRead(pFileName, pOptions, pathID);
};

void CFileSystemNull::AddSearchPathNoWrite(const char *pPath,
                                           const char *pathID)
{
	TRACE("CFileSystem::AddSearchPathNoWrite(%s, %s)", pPath, pathID);
	mpImpl->AddSearchPathNoWrite(pPath, pathID);
};