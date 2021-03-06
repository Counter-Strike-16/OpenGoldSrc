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

#include <memory>
#include "common/maintypes.h"
#include "public/FileSystem.h"

// clang-format off
#ifdef HOOK_ENGINE
	#define g_hfind (*pg_hfind)
	
	#define gbIsDedicatedServer (*pg_bIsDedicatedServer)
	
#ifndef _WIN32
	#define gHasMMXTechnology (*pgHasMMXTechnology)
#endif
	
	#define Launcher_ConsolePrintf (*pLauncher_ConsolePrintf)
#endif // HOOK_ENGINE
// clang-format on

typedef struct quakeparms_s quakeparms_t;

extern bool gbIsDedicatedServer;

extern FileFindHandle_t g_hfind;

#ifndef _WIN32
extern qboolean gHasMMXTechnology;
#endif

NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage);

NOXREF void Legacy_MP3subsys_Suspend_Audio();
NOXREF void Legacy_MP3subsys_Resume_Audio();

// exports

double Sys_FloatTime();

void EngineFprintf(void *pfile, const char *szFmt, ...);
void AlertMessage(ALERT_TYPE atype, const char *szFmt, ...);

//

class IDedicatedExports;
class CFileSystem;

class CSystem
{
public:
	static NOBODY void Init(quakeparms_t *host_parms, CFileSystem *apFileSystem, IDedicatedExports *apRemoteConsole, bool abDedicatedServer);
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
	
	static double GetFloatTime(); // platform-specific

	/// Called to yield for a little bit so as
	/// not to hog cpu when paused or debugging
	static NOXREF void Sleep(int msec);
	
	/// Send text to the remote console
	static void Printf(const char *fmt, ...);
	
	static NOXREF void Warning(const char *pszWarning, ...);
	
	/// An error will cause the entire program to exit
	static NOBODY void NORETURN Error(const char *error, ...); // mb add int code?
	
	static NOXREF void SetRateRegistrySetting(const char *pchRate);
	static NOXREF const char *GetRateRegistrySetting(const char *pchDef);
	
#ifdef _WIN32
	static void __cdecl InitHardwareTimer();

	static void CheckOSVersion();
#endif
	
	static NOXREF bool IsWin95();
	static NOXREF bool IsWin98();
private:
	static void SetupLegacyAPIs();
	static bool LegacySysPrintf(char *fmt, ...); // RemoteConsolePrintf
	
	static quakeparms_t *mhost_parms;
	
	static IDedicatedExports *mpRemoteConsole; // mpDedicatedExports
	static CFileSystem *mpFileSystem;
	
	static bool mbDedicatedServer;

	static bool mbIsWin95;
	static bool mbIsWin98;
};

///////////////////////////////////////////////////////

/// @brief high-level system module

// This is the high-level system module
// Here the portable core system components are placed

struct ISystem;

class CSystemNew
{
public:
	CSystem(ISystem *apImpl) : mpImpl(apImpl){}
	~CSystem() = default;
	
	bool Init();
	void Shutdown();
	
	void GenerateEvents();
	
	/// Allow game to yield CPU time
	void Sleep(std::chrono::milliseconds anMilli); // using that you can pass something like 1000ms
	
	// return const refs so it's possible to code write like
	// mpSystem->GetCmdLine()->HasArg("blah")
	// storing is also possible
	const CCmdLine &GetCmdLine() const; // return IConCmdArgs?
	const IConsole &GetConsole() const;
	//const CFileSystem &GetFileSystem() const;
	//const IRegistry &GetRegistry() const;
	
	// GetCmdProcessor()->Exec()->pCmdBuffer->Exec() hell yeah baby!
	//const CCmdProcessor &GetCmdProcessor() const;
private:
	std::unique_ptr<CCmdLine> mpCmdLine;
	
	ISystem *mpImpl{nullptr}; // low-level pimpl
	IConsole *mpConsole{nullptr};
};