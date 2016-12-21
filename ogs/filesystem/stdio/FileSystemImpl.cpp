#include <stdio.h>
#include "FileSystem.h"

static CFileSystemNull gFileSystemNull;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CFileSystemNull, IFileSystem, FILESYSTEM_INTERFACE_VERSION, gFileSystemNull);

CFileSystemNull::CFileSystemNull()
{
};

CFileSystemNull::~CFileSystemNull()
{
};

void CFileSystemNull::Mount()
{
};

void CFileSystemNull::Unmount()
{
};

void CFileSystemNull::RemoveAllSearchPaths()
{
};

void CFileSystemNull::AddSearchPath(const char *pPath, const char *pathID)
{
};

bool CFileSystemNull::RemoveSearchPath(const char *pPath)
{
	return false;
};

void CFileSystemNull::RemoveFile(const char *pRelativePath, const char *pathID)
{
};

void CFileSystemNull::CreateDirHierarchy(const char *path, const char *pathID)
{
};

bool CFileSystemNull::FileExists(const char *pFileName)
{
	return false;
};

bool CFileSystemNull::IsDirectory(const char *pFileName)
{
	return false;
};

FileHandle_t CFileSystemNull::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	return NULL;
};

void CFileSystemNull::Close(FileHandle_t file)
{
	close(file);
};

void CFileSystemNull::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	lseek(file, pos, SEEK_SET);
};

unsigned int CFileSystemNull::Tell(FileHandle_t file)
{
	return 0;
};

unsigned int CFileSystemNull::Size(FileHandle_t file)
{
	return 0;
};

unsigned int CFileSystemNull::Size(const char *pFileName)
{
	return 0;
};

/*
============
Sys_FileTime

returns -1 if not present
============
*/
long CFileSystemNull::GetFileTime(const char *pFileName)
{
	struct stat buf;
	
	if(stat(pFileName, &buf) == -1)
		return -1;
	
	return buf.st_mtime;
};

void CFileSystemNull::FileTimeToString(char* pStrip, int maxCharsIncludingTerminator, long fileTime)
{
};

bool CFileSystemNull::IsOk(FileHandle_t file)
{
	return false;
};

void CFileSystemNull::Flush(FileHandle_t file)
{
};

bool CFileSystemNull::EndOfFile(FileHandle_t file)
{
	return false;
};

int CFileSystemNull::Read(void *pOutput, int size, FileHandle_t file)
{
	return read(file, pOutput, size);
};

int CFileSystemNull::Write(void const *pInput, int size, FileHandle_t file)
{
	return write(file, pInput, size);
};

char *CFileSystemNull::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	return NULL;
};

int CFileSystemNull::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	return 0;
};

void *CFileSystemNull::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	return NULL;
};

void CFileSystemNull::ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
};

const char *CFileSystemNull::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return "";
};

const char *CFileSystemNull::FindNext(FileFindHandle_t handle)
{
	return "";
};

bool CFileSystemNull::FindIsDirectory(FileFindHandle_t handle)
{
	return false;
};

void CFileSystemNull::FindClose(FileFindHandle_t handle)
{
};

void CFileSystemNull::GetLocalCopy(const char *pFileName)
{
};

const char *CFileSystemNull::GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return "";
};

char *CFileSystemNull::ParseFile(char* pFileBytes, char* pToken, bool* pWasQuoted)
{
	return NULL;
};

bool CFileSystemNull::FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	return false;
};

bool CFileSystemNull::GetCurrentDirectory(char* pDirectory, int maxlen)
{
	return false;
};

void CFileSystemNull::PrintOpenedFiles()
{
};

void CFileSystemNull::SetWarningFunc(void (*pfnWarning)( const char *fmt, ... ))
{
};

void CFileSystemNull::SetWarningLevel(FileWarningLevel_t level)
{
};

void CFileSystemNull::LogLevelLoadStarted(const char *name)
{
};

void CFileSystemNull::LogLevelLoadFinished(const char *name)
{
};

int CFileSystemNull::HintResourceNeed(const char *hintlist, int forgetEverything)
{
	return 0;
};

int CFileSystemNull::PauseResourcePreloading()
{
	return 0;
};

int CFileSystemNull::ResumeResourcePreloading()
{
	return 0;
};

int CFileSystemNull::SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	return 0;
};

void CFileSystemNull::GetInterfaceVersion(char *p, int maxlen)
{
};

bool CFileSystemNull::IsFileImmediatelyAvailable(const char *pFileName)
{
	return false;
};

WaitForResourcesHandle_t CFileSystemNull::WaitForResources(const char *resourcelist)
{
	return 0;
};

bool CFileSystemNull::GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */)
{
	return false;
};

void CFileSystemNull::CancelWaitForResources(WaitForResourcesHandle_t handle)
{
};

bool CFileSystemNull::IsAppReadyForOfflinePlay(int appID)
{
	return false;
};

bool CFileSystemNull::AddPackFile(const char *fullpath, const char *pathID)
{
	return false;
};

FileHandle_t CFileSystemNull::OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID)
{
	return NULL;
};

void CFileSystemNull::AddSearchPathNoWrite(const char *pPath, const char *pathID)
{
};