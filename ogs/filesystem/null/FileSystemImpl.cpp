#include <stdio.h>
#include <cstdarg>
#include <string.h>
#include "FileSystemImpl.h"

#ifdef _DEBUG
static FILE *ghLogFile = NULL;

void TRACE(const char *asMsg, ...)
{
	if(!ghLogFile)
		return;

	va_list ArgList;
	char sMsg[512];

	va_start(ArgList, asMsg, 2);
	vsnprintf(sMsg, sizeof(sMsg), asMsg, ArgList);
	va_end(ArgList);

	sprintf(sMsg, "%s\n", sMsg);
	fputs(sMsg, ghLogFile);
};
#else
void TRACE(const char *asMsg, ...){};
#endif

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
#ifdef _DEBUG
	ghLogFile = fopen("TRACE.log", "a+");
#endif

	TRACE("CFileSystem::Mount");
};

void CFileSystemNull::Unmount()
{
	TRACE("CFileSystem::Unmount");

#ifdef _DEBUG
	if(ghLogFile)
		fclose(ghLogFile);
#endif
};

void CFileSystemNull::RemoveAllSearchPaths()
{
	TRACE("CFileSystem::RemoveAllSearchPaths");
};

void CFileSystemNull::AddSearchPath(const char *pPath, const char *pathID)
{
	TRACE("CFileSystem::AddSearchPath");
};

bool CFileSystemNull::RemoveSearchPath(const char *pPath)
{
	TRACE("CFileSystem::RemoveSearchPath(%s)", pPath);
	return false;
};

void CFileSystemNull::RemoveFile(const char *pRelativePath, const char *pathID)
{
	TRACE("CFileSystem::RemoveFile");
};

void CFileSystemNull::CreateDirHierarchy(const char *path, const char *pathID)
{
	TRACE("CFileSystem::CreateDirHierarchy");
};

bool CFileSystemNull::FileExists(const char *pFileName)
{
	TRACE("CFileSystem::FileExists(%s)", pFileName);
	return false;
};

bool CFileSystemNull::IsDirectory(const char *pFileName)
{
	TRACE("CFileSystem::IsDirectory(%s)", pFileName);
	return false;
};

FileHandle_t CFileSystemNull::Open(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE("CFileSystem::Open");
	return NULL;
};

void CFileSystemNull::Close(FileHandle_t file)
{
	TRACE("CFileSystem::Close");
};

void CFileSystemNull::Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	TRACE("CFileSystem::Seek");
};

unsigned int CFileSystemNull::Tell(FileHandle_t file)
{
	TRACE("CFileSystem::Tell");
	return 0;
};

unsigned int CFileSystemNull::Size(FileHandle_t file)
{
	TRACE("CFileSystem::");
	return 0;
};

unsigned int CFileSystemNull::Size(const char *pFileName)
{
	TRACE("CFileSystem::Size");
	return 0;
};

long CFileSystemNull::GetFileTime(const char *pFileName)
{
	TRACE("CFileSystem::GetFileTime");
	return 0;
};

void CFileSystemNull::FileTimeToString(char* pStrip, int maxCharsIncludingTerminator, long fileTime)
{
	TRACE("CFileSystem::FileTimeToString");
};

bool CFileSystemNull::IsOk(FileHandle_t file)
{
	TRACE("CFileSystem::IsOk");
	return false;
};

void CFileSystemNull::Flush(FileHandle_t file)
{
	TRACE("CFileSystem::Flush");
};

bool CFileSystemNull::EndOfFile(FileHandle_t file)
{
	TRACE("CFileSystem::EndOfFile");
	return false;
};

int CFileSystemNull::Read(void* pOutput, int size, FileHandle_t file)
{
	TRACE("CFileSystem::Read");
	return 0;
};

int CFileSystemNull::Write(void const* pInput, int size, FileHandle_t file)
{
	TRACE("CFileSystem::Write");
	return 0;
};

char *CFileSystemNull::ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	TRACE("CFileSystem::ReadLine");
	return NULL;
};

int CFileSystemNull::FPrintf(FileHandle_t file, char *pFormat, ...)
{
	TRACE("CFileSystem::FPrintf");
	return 0;
};

void *CFileSystemNull::GetReadBuffer(FileHandle_t file, int *outBufferSize, bool failIfNotInCache)
{
	TRACE("CFileSystem::GetReadBuffer");
	return NULL;
};

void CFileSystemNull::ReleaseReadBuffer(FileHandle_t file, void *readBuffer)
{
	TRACE("CFileSystem::ReleaseReadBuffer");
};

const char *CFileSystemNull::FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	TRACE("CFileSystem::FindFirst");
	return "";
};

const char *CFileSystemNull::FindNext(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindNext");
	return "";
};

bool CFileSystemNull::FindIsDirectory(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindIsDirectory");
	return false;
};

void CFileSystemNull::FindClose(FileFindHandle_t handle)
{
	TRACE("CFileSystem::FindClose");
};

void CFileSystemNull::GetLocalCopy(const char *pFileName)
{
	TRACE("CFileSystem::GetLocalCopy");
};

const char *CFileSystemNull::GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	TRACE("CFileSystem::GetLocalPath");
	return "";
};

char *CFileSystemNull::ParseFile(char* pFileBytes, char* pToken, bool* pWasQuoted)
{
	TRACE("CFileSystem::ParseFile");
	return NULL;
};

bool CFileSystemNull::FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	TRACE("CFileSystem::FullPathToRelativePath");
	return false;
};

bool CFileSystemNull::GetCurrentDirectory(char *pDirectory, int maxlen)
{
	TRACE("CFileSystem::GetCurrentDirectory");
	return false;
};

void CFileSystemNull::PrintOpenedFiles()
{
	TRACE("CFileSystem::PrintOpenedFiles");
};

void CFileSystemNull::SetWarningFunc(void (*pfnWarning)( const char *fmt, ... ))
{
	TRACE("CFileSystem::SetWarningFunc");
};

void CFileSystemNull::SetWarningLevel(FileWarningLevel_t level)
{
	TRACE("CFileSystem::SetWarningLevel");
};

void CFileSystemNull::LogLevelLoadStarted(const char *name)
{
	TRACE("CFileSystem::LogLevelLoadStarted");
};

void CFileSystemNull::LogLevelLoadFinished(const char *name)
{
	TRACE("CFileSystem::LogLevelLoadFinished");
};

int CFileSystemNull::HintResourceNeed(const char *hintlist, int forgetEverything)
{
	TRACE("CFileSystem::HintResourceNeed");
	return 0;
};

int CFileSystemNull::PauseResourcePreloading()
{
	TRACE("CFileSystem::PauseResourcePreloading");
	return 0;
};

int CFileSystemNull::ResumeResourcePreloading()
{
	TRACE("CFileSystem::ResumeResourcePreloading");
	return 0;
};

int CFileSystemNull::SetVBuf(FileHandle_t stream, char *buffer, int mode, long size)
{
	TRACE("CFileSystem::SetVBuf");
	return 0;
};

void CFileSystemNull::GetInterfaceVersion(char *p, int maxlen)
{
	TRACE("CFileSystem::GetInterfaceVersion");
};

bool CFileSystemNull::IsFileImmediatelyAvailable(const char *pFileName)
{
	TRACE("CFileSystem::IsFileImmediatelyAvailable");
	return false;
};

WaitForResourcesHandle_t CFileSystemNull::WaitForResources(const char *resourcelist)
{
	TRACE("CFileSystem::WaitForResources");
	return 0;
};

bool CFileSystemNull::GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */)
{
	TRACE("CFileSystem::GetWaitForResourcesProgress");
	return false;
};

void CFileSystemNull::CancelWaitForResources(WaitForResourcesHandle_t handle)
{
	TRACE("CFileSystem::CancelWaitForResources");
};

bool CFileSystemNull::IsAppReadyForOfflinePlay(int appID)
{
	TRACE("CFileSystem::IsAppReadyForOfflinePlay");
	return false;
};

bool CFileSystemNull::AddPackFile(const char *fullpath, const char *pathID)
{
	TRACE("CFileSystem::AddPackFile");
	return false;
};

FileHandle_t CFileSystemNull::OpenFromCacheForRead(const char *pFileName, const char *pOptions, const char *pathID)
{
	TRACE("CFileSystem::OpenFromCacheForRead");
	return NULL;
};

void CFileSystemNull::AddSearchPathNoWrite(const char *pPath, const char *pathID)
{
	TRACE("CFileSystem::AddSearchPathNoWrite");
};