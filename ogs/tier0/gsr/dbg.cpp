//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "dbg.h"
#include "strtools.h"
#include "threadtools.h"
#include "testthread.h"

#ifdef WIN32
#undef ARRAYSIZE
#include "winlite.h"
#endif

//Disable "'typedef ': ignored on left of '' when no variable is declared"
#pragma warning( push )
#pragma warning( disable: 4091 )
#include "minidump.h"
#pragma warning( pop )

AssertFailedNotifyFunc_t s_AssertFailedNotifyFunc = nullptr;

void CallAssertFailedNotifyFunc()
{
	if( s_AssertFailedNotifyFunc )
		s_AssertFailedNotifyFunc();
}

void SetAssertFailedNotifyFunc( AssertFailedNotifyFunc_t func )
{
	s_AssertFailedNotifyFunc = func;
}

FlushLogFunc_t s_FlushLogFunc = nullptr;

void CallFlushLogFunc()
{
	if( s_FlushLogFunc )
		s_FlushLogFunc();
}

void SetFlushLogFunc( FlushLogFunc_t func )
{
	s_FlushLogFunc = func;
}

float CrackSmokingCompiler( float a )
{
	return fabs( a );
}

struct SpewGroup_t
{
	char m_GroupName[ 48 ];
	int m_Level;
	int m_LogLevel;
};

static int s_GroupCount = 0;
static int s_DefaultLevel = 0;
static int s_DefaultLogLevel = 0;

static SpewGroup_t* s_pSpewGroups = nullptr;

static const char* s_pFileName = nullptr;
static int s_Line = 0;
static SpewType_t s_SpewType = SPEW_MESSAGE;

SpewRetval_t DefaultSpewFunc( SpewType_t type, const tchar* pMsg )
{
	_tprintf( _T( "%s" ), pMsg );

	if( type == SPEW_ASSERT )
		return SPEW_DEBUGGER;

	return ( type == SPEW_ERROR ) ? SPEW_ABORT : SPEW_CONTINUE;
}

static SpewOutputFunc_t s_SpewOutputFunc = &DefaultSpewFunc;

SpewOutputFunc_t GetSpewOutputFunc()
{
	if( s_SpewOutputFunc )
		return s_SpewOutputFunc;

	return &DefaultSpewFunc;
}

void SpewOutputFunc( SpewOutputFunc_t func )
{
	if( !func )
		func = &DefaultSpewFunc;

	s_SpewOutputFunc = func;
}

bool FindSpewGroup( const tchar* pGroupName, int* pInd )
{
	if( s_GroupCount > 0 )
	{
		int start = 0, end = s_GroupCount - 1;

		while( true )
		{
			const int index = ( start + end ) / 2;

			const int bias = _tcsicmp( pGroupName, s_pSpewGroups[ index ].m_GroupName );

			if( !bias )
			{
				*pInd = index;
				return true;
			}

			if( bias >= 0 )
			{
				start = index + 1;
			}
			else
			{
				end = index - 1;
			}

			if( end < start )
			{
				*pInd = index;
				break;
			}
		}
	}

	*pInd = 0;

	return false;
}

void SpewAndLogActivate( const tchar *pGroupName, int level, int logLevel )
{
	if( *pGroupName != '*' || pGroupName[ 1 ] )
	{
		int index;

		SpewGroup_t* pGroup;

		if( FindSpewGroup( pGroupName, &index ) )
		{
			pGroup = &s_pSpewGroups[ index ];
		}
		else
		{
			if( s_pSpewGroups )
			{
				s_pSpewGroups = ( SpewGroup_t* ) realloc( s_pSpewGroups, sizeof( SpewGroup_t ) * ( s_GroupCount + 1 ) );
				pGroup = &s_pSpewGroups[ index ];
				memmove( s_pSpewGroups + index + 1, pGroup, sizeof( SpewGroup_t ) * ( s_GroupCount - index ) );
			}
			else
			{
				s_pSpewGroups = ( SpewGroup_t* ) malloc( sizeof( SpewGroup_t ) * ( s_GroupCount + 1 ) );
				pGroup = s_pSpewGroups;
			}

			_tcscpy( pGroup->m_GroupName, pGroupName );
		}

		pGroup->m_Level = level;
		pGroup->m_LogLevel = logLevel;
	}
	else
	{
		s_DefaultLevel = level;
		s_DefaultLogLevel = logLevel;
	}
}

void SpewAndLogChangeIfStillDefault( const tchar *pGroupName, int level, int leveldefault, int logLevel, int logLevelDefault )
{
	int index;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		auto pGroup = &s_pSpewGroups[ index ];

		if( pGroup->m_Level == leveldefault && pGroup->m_LogLevel == logLevelDefault )
			SpewAndLogActivate( pGroupName, level, logLevel );
	}
}

void SpewChangeIfStillDefault( const tchar *pGroupName, int level, int leveldefault )
{
	int index;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		auto pGroup = &s_pSpewGroups[ index ];

		if( pGroup->m_Level == leveldefault )
			SpewAndLogActivate( pGroupName, level, level );
	}
}

void SpewActivate( tchar const* pGroupName, int level )
{
	SpewAndLogActivate( pGroupName, level, level );
}

bool IsLogActive( const tchar *pGroupName, int iLogLevel )
{
	int iLogLevelRequired = s_DefaultLogLevel;

	int index;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		iLogLevelRequired = s_pSpewGroups[ index ].m_LogLevel;
	}

	return iLogLevelRequired >= iLogLevel;
}

bool IsSpewActive( tchar const* pGroupName, int level )
{
	int index;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		return level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		return level <= s_DefaultLevel;
	}
}

void _SpewInfo( SpewType_t type, const tchar* pFile, int line )
{
	auto pszName = pFile;
	auto pszLastSlash = _tcsrchr( pFile, '\\' );
	auto pszLastSlash2 = _tcsrchr( pFile, '/' );

	if( pszLastSlash2 >= pszLastSlash )
		pszLastSlash = pszLastSlash2;

	if( pszLastSlash )
		pszName = pszLastSlash + 1;

	s_pFileName = pszName;
	s_Line = line;
	s_SpewType = type;
}

SpewRetval_t SpewMessageType( SpewType_t spewType, const tchar* pMsgFormat, va_list args )
{
	tchar pTempBuffer[ 5020 ];

	static CThreadMutex autoMutex;

	autoMutex.Lock();

	if( spewType == SPEW_ASSERT )
		Test_SetFailed();

	size_t uiLeft;
	int iWritten;

	if( spewType != SPEW_ASSERT )
	{
		uiLeft = sizeof( pTempBuffer ) - 1;
		iWritten = 0;
	}
	else
	{
		Test_SetFailed();

		iWritten = _sntprintf( pTempBuffer, ARRAYSIZE( pTempBuffer ) - 1, _T( "%s (%d) : " ), s_pFileName, s_Line );

		if( iWritten == -1 )
		{
			autoMutex.Unlock();
			return SPEW_ABORT;
		}

		uiLeft = ( sizeof( pTempBuffer ) - 1 ) - iWritten;
	}

	int iAppendedWritten = vsnprintf( &pTempBuffer[ iWritten ], uiLeft, pMsgFormat, args );

	if( iAppendedWritten == -1 )
	{
		autoMutex.Unlock();
		return SPEW_ABORT;
	}

	if( spewType == SPEW_ASSERT )
		pTempBuffer[ iWritten + iAppendedWritten ] = _T( '\n' );

	SpewRetval_t retval = s_SpewOutputFunc( spewType, pTempBuffer );

	if( retval != SPEW_DEBUGGER )
	{
		if( retval == SPEW_ABORT )
		{
			DMsg( _T( "console" ), 1, _T( "Exiting on SPEW_ABORT\n" ) );
			exit( 0 );
		}
	}
	else if( spewType != SPEW_ASSERT )
	{
		__debugbreak();
	}

	autoMutex.Unlock();

	return retval;
}

SpewRetval_t _SpewMessage( const tchar* pMsgFormat, ... )
{
	va_list va;

	va_start( va, pMsgFormat );
	const auto retval = SpewMessageType( s_SpewType, pMsgFormat, va );
	va_end( va );

	return retval;
}

SpewRetval_t _DSpewMessage( tchar const *pGroupName, int level, tchar const* pMsg, ... )
{
	int index;

	bool bShouldLog;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		bShouldLog = level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		bShouldLog = level <= s_DefaultLevel;
	}

	SpewRetval_t result = SPEW_CONTINUE;

	if( bShouldLog )
	{
		va_list va;

		va_start( va, pMsg );
		result = SpewMessageType( s_SpewType, pMsg, va );
		va_end( va );
	}

	return result;
}

void _ExitOnFatalAssert( tchar const* pFile, int line )
{
	_SpewMessage( "Fatal assert failed: %s, line %d.  Application exiting.\n", pFile, line );
	WriteMiniDump();
	DMsg( "console", 1, "_ExitOnFatalAssert\n" );
	exit( 1 );
}

void Msg( tchar const* pMsg, ... )
{
	va_list va;

	va_start( va, pMsg );
	SpewMessageType( SPEW_MESSAGE, pMsg, va );
	va_end( va );
}

void DMsg( tchar const* pGroupName, int level, tchar const* pMsg, ... )
{
	int index;

	bool bShouldLog;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		bShouldLog = level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		bShouldLog = level <= s_DefaultLevel;
	}

	if( bShouldLog )
	{
		va_list va;

		va_start( va, pMsg );
		SpewMessageType( SPEW_MESSAGE, pMsg, va );
		va_end( va );
	}
}

//TODO: temporary until linker issues with vstdlib are resolved - Solokiller
DBG_INTERFACE void _DMsg( tchar const* pGroupName, int level, tchar const* pMsg, ... )
{
	int index;

	bool bShouldLog;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		bShouldLog = level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		bShouldLog = level <= s_DefaultLevel;
	}

	if( bShouldLog )
	{
		va_list va;

		va_start( va, pMsg );
		SpewMessageType( SPEW_MESSAGE, pMsg, va );
		va_end( va );
	}
}

void Warning( tchar const *pMsg, ... )
{
	va_list va;

	va_start( va, pMsg );
	SpewMessageType( SPEW_WARNING, pMsg, va );
	va_end( va );
}

void DWarning( const tchar* pGroupName, int level, const tchar* pMsgFormat, ... )
{
	int index;

	bool bShouldLog;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		bShouldLog = level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		bShouldLog = level <= s_DefaultLevel;
	}

	if( bShouldLog )
	{
		va_list va;

		va_start( va, pMsgFormat );
		SpewMessageType( SPEW_WARNING, pMsgFormat, va );
		va_end( va );
	}
}

void Log( tchar const *pMsg, ... )
{
	va_list va;

	va_start( va, pMsg );
	SpewMessageType( SPEW_LOG, pMsg, va );
	va_end( va );
}

void DLog( const tchar* pGroupName, int level, const tchar* pMsgFormat, ... )
{
	int index;

	bool bShouldLog;

	if( FindSpewGroup( pGroupName, &index ) )
	{
		bShouldLog = level <= s_pSpewGroups[ index ].m_Level;
	}
	else
	{
		bShouldLog = level <= s_DefaultLevel;
	}

	if( bShouldLog )
	{
		va_list va;

		va_start( va, pMsgFormat );
		SpewMessageType( SPEW_LOG, pMsgFormat, va );
		va_end( va );
	}
}

void Error( tchar const* pMsg, ... )
{
	va_list va;

	va_start( va, pMsg );
	SpewMessageType( SPEW_ERROR, pMsg, va );
	va_end( va );
}

void _AssertValidReadPtr( void* ptr, int count )
{
}

void _AssertValidWritePtr( void* ptr, int count )
{
}

void _AssertValidReadWritePtr( void* ptr, int count )
{
}

void AssertValidStringPtr( const tchar* ptr, int maxchar )
{
#if defined( _DEBUG ) && defined( WIN32 )
	Assert( !IsBadStringPtr( ptr, maxchar ) );
#endif
}

void* Plat_SimpleLog( const tchar* file, int line )
{
	FILE* pFile = fopen( "simple.log", "at+" );
	_ftprintf( pFile, _T( "%s:%i\n" ), file, line );
	fclose( pFile );

	return nullptr;
}
