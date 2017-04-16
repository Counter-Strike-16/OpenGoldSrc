/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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
/// @brief window-specific impl of system funcs

#include "precompiled.hpp"
#include "system/System.hpp"
#include "system/SystemTypes.hpp"
#include "system/common.hpp"

#ifdef _WIN32

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int g_FPUCW_Mask_Prec_64Bit = 0;
int g_FPUCW_Mask_Prec_64Bit_2 = 0;
int g_FPUCW_Mask_Round_Trunc = 0;
int g_FPUCW_Mask_Round_Up = 0;

#else // HOOK_ENGINE

int g_FPUCW_Mask_Prec_64Bit;
int g_FPUCW_Mask_Prec_64Bit_2;
int g_FPUCW_Mask_Round_Trunc;
int g_FPUCW_Mask_Round_Up;

#endif // HOOK_ENGINE

int g_PerfCounterInitialized;
CRITICAL_SECTION g_PerfCounterMutex;

int g_PerfCounterShiftRightAmount;
double g_PerfCounterSlice;
double g_CurrentTime;
double g_StartTime;

int g_WinNTOrHigher;

// Hacky
#pragma warning( push )  
#pragma warning( disable : 4996 )

// It's kinda useless to check for such old versions of Win
// Most servers are running on Linux anyway

void CSystem::CheckOSVersion()
{
	OSVERSIONINFO verInfo;

	Q_memset(&verInfo, 0, sizeof(verInfo));
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	
	if(!GetVersionEx(&verInfo))
		Error("Couldn't get OS info");

	g_WinNTOrHigher = verInfo.dwMajorVersion >= 4;
	
	if(verInfo.dwPlatformId == 1 && verInfo.dwMajorVersion == 4)
	{
		if(verInfo.dwMinorVersion)
		{
			if(verInfo.dwMinorVersion < 90)
				mbIsWin98 = true;
		}
		else
			mbIsWin95 = true;
	};
};

#pragma warning ( pop )

void Sys_SetupFPUOptions()
{
#ifndef __SSE__
	static uint8 fpuOpts[32];

	//__asm { fnstenv byte ptr fpuOpts }
	fpuOpts[0] |= 0x3Fu;
	//__asm { fldenv  byte ptr fpuOpts }
#endif
};

void Sys_InitFPUControlWords()
{
#ifndef __SSE__
	int fpucw = 0;
	//__asm { fnstcw fpucw }

	g_FPUCW_Mask_Prec_64Bit = (fpucw & 0xF0FF) | 0x300;
	g_FPUCW_Mask_Prec_64Bit_2 = (fpucw & 0xF0FF) | 0x300;
	g_FPUCW_Mask_Round_Trunc = (fpucw & 0xF0FF) | 0xC00;
	g_FPUCW_Mask_Round_Up = (fpucw & 0xF0FF) | 0x800;
#endif
};

void Sys_SetStartTime()
{
	int startTimeArg = 0;

	CSystem::FloatTime();

	//startTimeArg = COM_CheckParm("-starttime");

	if(startTimeArg)
		g_CurrentTime = 0; //Q_atof(com_argv[startTimeArg + 1]);
	else
		g_CurrentTime = 0;

	g_StartTime = g_CurrentTime;
};

int g_SavedFPUCW1 = 0;
void Sys_FPUCW_Push_Prec64()
{
#ifndef __SSE__
	uint16 tmp = g_FPUCW_Mask_Prec_64Bit;
	//__asm { fnstcw  g_SavedFPUCW1 }
	//__asm {fldcw tmp}
#endif
};

void Sys_FPUCW_Pop_Prec64()
{
#ifndef __SSE__
	uint16 tmp = g_SavedFPUCW1;
	//__asm { fldcw tmp }
#endif
};

void __cdecl CSystem::InitHardwareTimer()
{
	unsigned int perfHighPart;
	unsigned int perfLowPart;
	LARGE_INTEGER perfFreq;

	if(!g_PerfCounterInitialized)
	{
		InitializeCriticalSection(&g_PerfCounterMutex);
		g_PerfCounterInitialized = 1;
	};

	Sys_SetupFPUOptions();
	Sys_InitFPUControlWords();

	//if(!CRehldsPlatformHolder::get()->QueryPerfFreq(&perfFreq))
	//Sys_Error("No hardware timer available");

	perfHighPart = perfFreq.HighPart;
	perfLowPart = perfFreq.LowPart;
	g_PerfCounterShiftRightAmount = 0;

	while(perfHighPart || perfLowPart > 2000000.0)
	{
		g_PerfCounterShiftRightAmount++;
		perfLowPart = (perfHighPart << 31) | (perfLowPart >> 1);
		perfHighPart >>= 1;
	};

	g_PerfCounterSlice = 1.0 / (double)perfLowPart;

	CheckOSVersion();
	Sys_SetStartTime();
};

double EXT_FUNC CSystem::FloatTime()
{
	unsigned int currentTime;
	int savedOldTime;
	LARGE_INTEGER PerformanceCount;

	static bool s_NeedInit = true;
	static unsigned int s_oldTime = 0;
	static int s_timeNotChangedCounter = 0;

	if(!g_PerfCounterInitialized)
		return 1.0;

	EnterCriticalSection(&g_PerfCounterMutex);
	Sys_FPUCW_Push_Prec64();

	//CRehldsPlatformHolder::get()->QueryPerfCounter(&PerformanceCount);

	if(g_PerfCounterShiftRightAmount)
		currentTime =
		(PerformanceCount.LowPart >> g_PerfCounterShiftRightAmount) |
		(PerformanceCount.HighPart << (32 - g_PerfCounterShiftRightAmount));
	else
		currentTime = PerformanceCount.LowPart;

	if(!s_NeedInit)
	{
		savedOldTime = s_oldTime;

		if(currentTime <= s_oldTime && s_oldTime - currentTime < 0x10000000)
			s_oldTime = currentTime;
		else
		{
			s_oldTime = currentTime;
			g_CurrentTime = g_CurrentTime + (double)(currentTime - savedOldTime) * g_PerfCounterSlice;

			if(g_CurrentTime == g_StartTime)
			{
				if(s_timeNotChangedCounter >= 100000)
				{
					g_CurrentTime = g_CurrentTime + 1.0;
					s_timeNotChangedCounter = 0;
				}
			}
			else
			{
				s_timeNotChangedCounter = 0;
			}
			g_StartTime = g_CurrentTime;
		}
	}
	else
	{
		s_oldTime = currentTime;
		s_NeedInit = false;
	}

	Sys_FPUCW_Pop_Prec64();
	LeaveCriticalSection(&g_PerfCounterMutex);
	return g_CurrentTime;
};

#endif // _WIN32