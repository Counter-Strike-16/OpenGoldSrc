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
/// @brief non-portable functions (platform-specific)

#pragma once

#include "common/maintypes.h"

typedef struct quakeparms_s quakeparms_t;

extern void (*Launcher_ConsolePrintf)(char *, ...);

void Legacy_Sys_Printf(char *fmt, ...);

NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage);

NOXREF void Legacy_MP3subsys_Suspend_Audio();
NOXREF void Legacy_MP3subsys_Resume_Audio();

double Sys_FloatTime();

class CSystem
{
public:
	static NOBODY void Init();
	static NOXREF void Shutdown();
	
	static void InitFloatTime();
	static void ShutdownFloatTime();
	
	static void InitArgv(char *lpCmdLine);
	static NOXREF void ShutdownArgv();
	
	static void InitMemory();
	static void ShutdownMemory();

	static void InitLauncherInterface();
	static NOXREF void ShutdownLauncherInterface();

	static void InitAuthentication();
	static NOXREF void ShutdownAuthentication();
	
	// called to yield for a little bit so as
	// not to hog cpu when paused or debugging
	static NOXREF void Sleep(int msec);
	
	// send text to the console
	static void Printf(const char *fmt, ...);
	
	static NOXREF void Warning(const char *pszWarning, ...);
	
	// An error will cause the entire program to exit
	static NOBODY void NORETURN Error(const char *error, ...); // mb add int code?
	
#ifdef _WIN32
	static void CheckOSVersion();
#endif
	
	static NOXREF bool IsWin95();
	static NOXREF bool IsWin98();
private:
	static void SetupLegacyAPIs();
	
	static quakeparms_t *mhost_parms;
	
	static bool mbIsWin95{false};
	static bool mbIsWin98{false};
};