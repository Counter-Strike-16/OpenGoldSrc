//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"

#undef ARRAYSIZE
#include "winlite.h"

using VTuneFunc = int ( * )();

static VTuneFunc VTResumeFn = nullptr;
static VTuneFunc VTPauseFn = nullptr;

bool vtune( bool resume )
{
#ifdef WIN32
	static bool bLoaded = false;

	if( !bLoaded )
	{
		bLoaded = true;

		auto hLib = LoadLibraryA( "vtuneapi.dll" );

		if( hLib )
		{
			VTResumeFn = reinterpret_cast<decltype( VTResumeFn )>( GetProcAddress( hLib, "VTResume" ) );
			VTPauseFn = reinterpret_cast<decltype( VTPauseFn )>( GetProcAddress( hLib, "VTPause" ) );
		}
	}

	auto fn = resume ? VTResumeFn : VTPauseFn;

	if( !fn )
		return false;

	fn();

	return true;
#else
	return true;
#endif
}

PLATFORM_INTERFACE const tchar *Plat_GetCommandLine()
{
#ifdef TCHAR_IS_CHAR
	return GetCommandLineA();
#else
	return GetCommandLineW();
#endif
}

#ifdef WIN32
bool Plat_IsInDebugSession()
{
	return IsDebuggerPresent() != 0;
}
#endif

#ifdef WIN32
static LARGE_INTEGER g_Frequency = {};
static int64 g_Freq = 0;
static LARGE_INTEGER g_PerfCount = {};
#endif

double Plat_FloatTime()
{
#ifdef WIN32
	LARGE_INTEGER PerformanceCount;

	if( !g_Frequency.QuadPart )
	{
		QueryPerformanceFrequency( &g_Frequency );
		g_Freq = g_Frequency.QuadPart / 1000;
		QueryPerformanceCounter( &g_PerfCount );
	}

	QueryPerformanceCounter( &PerformanceCount );

	return ( double ) ( PerformanceCount.QuadPart - g_PerfCount.QuadPart )
		/ ( double ) g_Frequency.QuadPart;
#else
	static int secbase = 0;

	long double result;
	timeval tp;

	gettimeofday( &tp, 0 );
	if( secbase )
	{
		result = ( long double ) ( tp.tv_sec - secbase ) + ( long double ) tp.tv_usec / 1000000.0;
		if( g_VCRMode )
			g_pVCR->Hook_Sys_FloatTime( result );
	}
	else
	{
		result = ( long double ) tp.tv_usec / 1000000.0;
		secbase = tp.tv_sec;
	}
	return result;
#endif
}