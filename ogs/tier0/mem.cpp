//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
//Allows us to implement the interface
#undef NO_MALLOC_OVERRIDE
#include "memalloc.h"

class CStdMemAlloc : public IMemAlloc
{
public:
	void* Alloc( size_t nSize ) override;
	void* Realloc( void *pMem, size_t nSize ) override;
	void Free( void *pMem, int unknown ) override;
	void* Expand_NoLongerSupported( void* pMem, size_t nSize ) override;

	void* Alloc_Debug( size_t nSize, const char* pFileName, int nLine, int unknown ) override;
	void* Realloc_Debug( void* pMem, size_t nSize, const char* pFileName, int nLine ) override;
	void  Free_Debug( void* pMem, const char* pFileName, int nLine, int unknown ) override;
	void* Expand_NoLongerSupported_Debug( void* pMem, size_t nSize, const char* pFileName, int nLine, int unknown ) override;

	size_t GetSize( void* pMem ) override;

	void PushAllocDbgInfo( const char* pFileName, int nLine ) override;
	void PopAllocDbgInfo() override;

	long CrtSetBreakAlloc( long lNewBreakAlloc ) override;
	int CrtSetReportMode( int nReportType, int nReportMode ) override;
	int CrtIsValidHeapPointer( const void *pMem ) override;
	int CrtIsValidPointer( const void* pMem, unsigned int size, int access ) override;
	int CrtCheckMemory() override;
	int CrtSetDbgFlag( int nNewFlag ) override;
	void CrtMemCheckpoint( _CrtMemState* pState ) override;

	void DumpStats() override;

	void* CrtSetReportFile( int nRptType, void* hFile ) override;
	void* CrtSetReportHook( void* pfnNewHook ) override;
	int CrtDbgReport( int nRptType, const char* szFile,
					  int nLine, const char* szModule, const char* pMsg ) override;

	int heapchk() override;

	bool IsDebugHeap() override;

	void GetActualDbgInfo( const char*& pFileName, int& nLine ) override;
	void RegisterAllocation( const char *pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime ) override;
	void RegisterDeallocation( const char *pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime ) override;

	int GetVersion();

	void CompactHeap();

	MemAllocFailHandler_t SetAllocFailHandler( MemAllocFailHandler_t pfnMemAllocFailHandler );
};

static CStdMemAlloc g_MemAlloc;

IMemAlloc* g_pMemAlloc = &g_MemAlloc;

void* CStdMemAlloc::Alloc( size_t nSize )
{
	return malloc( nSize );
}

void* CStdMemAlloc::Realloc( void* pMem, size_t nSize )
{
	return realloc( pMem, nSize );
}

void CStdMemAlloc::Free( void* pMem, int unknown )
{
	free( pMem );
}

void* CStdMemAlloc::Expand_NoLongerSupported( void* pMem, size_t nSize )
{
	return nullptr;
}

void* CStdMemAlloc::Alloc_Debug( size_t nSize, const char* pFileName, int nLine, int unknown )
{
	return malloc( nSize );
}

void* CStdMemAlloc::Realloc_Debug( void* pMem, size_t nSize, const char* pFileName, int nLine )
{
	return Realloc( pMem, nSize );
}

void CStdMemAlloc::Free_Debug( void* pMem, const char* pFileName, int nLine, int unknown )
{
	Free( pMem, unknown );
}

void* CStdMemAlloc::Expand_NoLongerSupported_Debug( void* pMem, size_t nSize, const char* pFileName, int nLine, int unknown )
{
	return nullptr;
}

size_t CStdMemAlloc::GetSize( void* pMem )
{
	//TODO: implement - Solokiller
	return 0;
}

void CStdMemAlloc::PushAllocDbgInfo( const char* pFileName, int nLine )
{
	//TODO: implement - Solokiller
}

void CStdMemAlloc::PopAllocDbgInfo()
{
	//TODO: implement - Solokiller
}

long CStdMemAlloc::CrtSetBreakAlloc( long lNewBreakAlloc )
{
	//TODO: implement - Solokiller
	return 0;
}

int CStdMemAlloc::CrtSetReportMode( int nReportType, int nReportMode )
{
	//TODO: implement - Solokiller
	return 0;
}

int CStdMemAlloc::CrtIsValidHeapPointer( const void* pMem )
{
	//TODO: implement - Solokiller
	return true;
}

int CStdMemAlloc::CrtIsValidPointer( const void* pMem, unsigned int size, int access )
{
	//TODO: implement - Solokiller
	return true;
}

int CStdMemAlloc::CrtCheckMemory()
{
	//TODO: implement - Solokiller
	return true;
}

int CStdMemAlloc::CrtSetDbgFlag( int nNewFlag )
{
	//TODO: implement - Solokiller
	return 0;
}

void CStdMemAlloc::CrtMemCheckpoint( _CrtMemState* pState )
{
	//TODO: implement - Solokiller
}

void CStdMemAlloc::DumpStats()
{
	//TODO: implement - Solokiller
}

void* CStdMemAlloc::CrtSetReportFile( int nRptType, void* hFile )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void* CStdMemAlloc::CrtSetReportHook( void* pfnNewHook )
{
	//TODO: implement - Solokiller
	return nullptr;
}

int CStdMemAlloc::CrtDbgReport( int nRptType, const char* szFile,
				  int nLine, const char* szModule, const char* pMsg )
{
	//TODO: implement - Solokiller
	return 0;
}

int CStdMemAlloc::heapchk()
{
	//TODO: implement - Solokiller
	return _HEAPOK;
}

bool CStdMemAlloc::IsDebugHeap()
{
	//TODO: implement - Solokiller
	return false;
}

void CStdMemAlloc::GetActualDbgInfo( const char*& pFileName, int& nLine )
{
	//TODO: implement - Solokiller
}

void CStdMemAlloc::RegisterAllocation( const char* pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime )
{
	//TODO: implement - Solokiller
}

void CStdMemAlloc::RegisterDeallocation( const char* pFileName, int nLine, int nLogicalSize, int nActualSize, unsigned nTime )
{
	//TODO: implement - Solokiller
}

int CStdMemAlloc::GetVersion()
{
	return MEMALLOC_VERSION;
}

void CStdMemAlloc::CompactHeap()
{
	//TODO: implement - Solokiller
}

MemAllocFailHandler_t CStdMemAlloc::SetAllocFailHandler( MemAllocFailHandler_t pfnMemAllocFailHandler )
{
	//TODO: implement - Solokiller
	return nullptr;
}