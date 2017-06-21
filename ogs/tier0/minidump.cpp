//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include <cstring>
#include <ctime>
#include "dbg.h"

#ifdef WIN32
#undef ARRAYSIZE
#include "winlite.h"

//Disable "'typedef ': ignored on left of '' when no variable is declared"
#pragma warning( push )
#pragma warning( disable: 4091 )
#include "minidump.h"
#pragma warning( pop )

void MiniDumpWriter( unsigned int a1, struct _EXCEPTION_POINTERS *a2 );

static bool g_bWritingMiniDump = false;
static FnMiniDump g_MiniDumpFn = &MiniDumpWriter;
static volatile LONG g_CurrentMiniDumpThread = 0;
static int g_MiniDumpRecursionLevel = 0;
static int g_MiniDumpCount = 0;

PLATFORM_INTERFACE void WriteMiniDump()
{
	g_bWritingMiniDump = true;
	RaiseException( 0, EXCEPTION_NONCONTINUABLE, 0, nullptr );
	g_bWritingMiniDump = false;
}

void SETranslator( unsigned int a1, struct _EXCEPTION_POINTERS *a2 )
{
	g_MiniDumpFn( a1, a2 );
}

PLATFORM_INTERFACE void CatchAndWriteMiniDump( FnWMain pfn, int argc, tchar *argv[] )
{
	if( IsDebuggerPresent() )
	{
		pfn( argc, argv );
	}
	else
	{
		_set_se_translator( &SETranslator );
		pfn( argc, argv );
	}
}

bool BGetMiniDumpLock()
{
	bool result;

	DWORD hThisThread = GetCurrentThreadId();

	if( hThisThread != g_CurrentMiniDumpThread && InterlockedCompareExchange( &g_CurrentMiniDumpThread, hThisThread, 0 ) )
	{
		result = false;
	}
	else
	{
		result = true;
		++g_MiniDumpRecursionLevel;
	}

	return result;
}

int MiniDumpUnlock()
{
	if( g_MiniDumpRecursionLevel-- == 1 )
		g_CurrentMiniDumpThread = 0;

	return g_MiniDumpRecursionLevel;
}

void MiniDumpWriter( unsigned int a1, struct _EXCEPTION_POINTERS *a2 )
{
	if( !BGetMiniDumpLock() )
		return;

	HMODULE hDbgHelp = LoadLibraryA( "DbgHelp.dll" );

	if( hDbgHelp == NULL )
	{
		MiniDumpUnlock();
		return;
	}

	auto pWriteDump = reinterpret_cast<BOOL ( WINAPI * )(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
			PMINIDUMP_EXCEPTION_INFORMATION,
			PMINIDUMP_USER_STREAM_INFORMATION,
			PMINIDUMP_CALLBACK_INFORMATION
		)>( GetProcAddress( hDbgHelp, "MiniDumpWriteDump" ) );

	if( pWriteDump )
	{
		__time64_t Time = _time64( 0 );
		struct tm* v6 = _localtime64( &Time );
		++g_MiniDumpCount;
		struct tm * v7 = v6;

		char Filename[ MAX_PATH ];

		Filename[ 0 ] = '\0';

		if( GetModuleFileNameA( NULL, Filename, sizeof( Filename ) ) )
		{
			char* pszExt = strrchr( Filename, '.' );

			if( pszExt )
				*pszExt = '\0';

			char* pszDirEnd = strrchr( Filename, '\\' );

			const char* pszFilename = pszDirEnd ? pszDirEnd + 1 : "unknown";

			const char* pszType = g_bWritingMiniDump ? "assert" : "crash";

			char FileName[ MAX_PATH ];

			_snprintf(
				FileName,
				sizeof( FileName ),
				"%s_%s_%d%.2d%.2d%.2d%.2d%.2d_%d.mdmp",
				pszFilename,
				pszType,
				v7->tm_year + 1900,
				v7->tm_mon + 1,
				v7->tm_mday,
				v7->tm_hour,
				v7->tm_min,
				v7->tm_sec,
				g_MiniDumpCount );

			HANDLE hFile = CreateFileA( FileName, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			bool bSuccess = false;

			if( NULL != hFile )
			{
				MINIDUMP_EXCEPTION_INFORMATION except;

				except.ThreadId = GetCurrentThreadId();
				except.ExceptionPointers = a2;
				except.ClientPointers = TRUE;

				bSuccess = pWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithDataSegs, &except, nullptr, nullptr ) != FALSE;
				CloseHandle( hFile );
			}

			if( !bSuccess )
			{
				char szRenamed[ MAX_PATH ];

				_snprintf( szRenamed, sizeof( szRenamed ), "(failed)%s", FileName );
				rename( FileName, szRenamed );
			}

			CallFlushLogFunc();
		}
	}

	FreeLibrary( hDbgHelp );
	MiniDumpUnlock();
}

#else
PLATFORM_INTERFACE void WriteMiniDump()
{
}
#endif