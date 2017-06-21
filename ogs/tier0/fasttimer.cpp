//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "fasttimer.h"

#ifdef WIN32
#undef ARRAYSIZE
#include "winlite.h"

PLATFORM_INTERFACE int64 g_ClockSpeed = 0;
PLATFORM_INTERFACE unsigned long g_dwClockSpeed = 0;

PLATFORM_INTERFACE double g_ClockSpeedMicrosecondsMultiplier = 0;
PLATFORM_INTERFACE double g_ClockSpeedMillisecondsMultiplier = 0;
PLATFORM_INTERFACE double g_ClockSpeedSecondsMultiplier = 0;

PLATFORM_INTERFACE int64 g_ulLastCycleSample = 0;
PLATFORM_INTERFACE int g_cBadCycleCountReceived = 0;

uint64 CalculateCPUFreq()
{
	LARGE_INTEGER PerformanceCount;
	LARGE_INTEGER Frequency;

	HANDLE hThisThread = GetCurrentThread();

	DWORD_PTR dwThreadAffinityMask = SetThreadAffinityMask( hThisThread, 1u );

	QueryPerformanceFrequency( &Frequency );
	const LONGLONG interval = Frequency.QuadPart >> 5;
	QueryPerformanceCounter( &PerformanceCount );

	uint64 cpuTimestamp = __rdtsc();

	if( ( int64 ) cpuTimestamp >= g_ulLastCycleSample || ( ++g_cBadCycleCountReceived, g_cBadCycleCountReceived >= 1000 ) )
	{
		g_ulLastCycleSample = cpuTimestamp;
		g_cBadCycleCountReceived = 0;
	}
	else
	{
		cpuTimestamp = g_ulLastCycleSample;
	}

	LARGE_INTEGER count;

	do
	{
		QueryPerformanceCounter( &count );
	}
	while( ( count.QuadPart - PerformanceCount.QuadPart ) < interval );

	uint64 endCPUTimestamp = __rdtsc();

	if( ( int64 ) endCPUTimestamp >= g_ulLastCycleSample || ( ++g_cBadCycleCountReceived, g_cBadCycleCountReceived >= 1000 ) )
	{
		g_ulLastCycleSample = endCPUTimestamp;
		g_cBadCycleCountReceived = 0;
	}
	else
	{
		endCPUTimestamp = g_ulLastCycleSample;
	}

	SetThreadAffinityMask( hThisThread, dwThreadAffinityMask );

	endCPUTimestamp -= cpuTimestamp;
	PerformanceCount.QuadPart = ( count.QuadPart - PerformanceCount.QuadPart );

	return ( uint64 ) ( ( double ) ( int64 ) endCPUTimestamp / ( ( double ) PerformanceCount.QuadPart / ( double ) Frequency.QuadPart ) );
}
#endif
