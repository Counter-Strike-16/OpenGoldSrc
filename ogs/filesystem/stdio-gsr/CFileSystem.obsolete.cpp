/**
*	@file
*	Method definitions for obsolete API features.
*/

#include "CFileSystem.h"

void CFileSystem::Mount()
{
	//Nothing
}

void CFileSystem::Unmount()
{
	//Nothing
}

void *CFileSystem::GetReadBuffer( FileHandle_t file, int *outBufferSize, bool failIfNotInCache )
{
	*outBufferSize = 0;

	return nullptr;
}

void CFileSystem::ReleaseReadBuffer( FileHandle_t file, void *readBuffer )
{
	//Nothing
}

void CFileSystem::GetLocalCopy( const char *pFileName )
{
	//Nothing
}

void CFileSystem::LogLevelLoadStarted( const char *name )
{
	//Nothing
}

void CFileSystem::LogLevelLoadFinished( const char *name )
{
	//Nothing
}

int CFileSystem::HintResourceNeed( const char *hintlist, int forgetEverything )
{
	//Nothing
	return 0;
}

int CFileSystem::PauseResourcePreloading()
{
	//Nothing
	return 0;
}

int CFileSystem::ResumeResourcePreloading()
{
	//Nothing
	return 0;
}

bool CFileSystem::IsFileImmediatelyAvailable( const char *pFileName )
{
	return true;
}

WaitForResourcesHandle_t CFileSystem::WaitForResources( const char *resourcelist )
{
	return 0;
}

bool CFileSystem::GetWaitForResourcesProgress( WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */ )
{
	if( progress )
		*progress = 0;

	if( complete )
		*complete = true;

	return false;
}

void CFileSystem::CancelWaitForResources( WaitForResourcesHandle_t handle )
{
	//Nothing
}

bool CFileSystem::IsAppReadyForOfflinePlay( int appID )
{
	return true;
}
