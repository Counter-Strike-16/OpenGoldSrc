/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
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

#include "precompiled.hpp"
#include "system/Engine.hpp"
#include "system/Host.hpp"
#include "system/System.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "system/client.hpp"

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

CEngine gEngine;
IOGSEngine *ogseng = &gEngine;

#else // HOOK_ENGINE

IOGSEngine *ogseng;

#endif // HOOK_ENGINE

void ForceReloadProfile()
{
	//Cbuf_AddText("exec config.cfg\n");
	//Cbuf_AddText("+mlook\n");
	//Cbuf_Execute();
	
	//if(COM_CheckParm("-nomousegrab"))
		//Cvar_Set("cl_mousegrab", "0");

	// Key_SetBinding(126, "toggleconsole");
	// Key_SetBinding(96, "toggleconsole");
	// Key_SetBinding(27, "cancelselect");
	
	// SDL_GL_SetSwapInterval((gl_vsync.value <= 0.0) - 1);
	
	//if(cls.state != ca_dedicated)
	{
		//CSystem::Error("Only dedicated mode is supported");
		
		char sRate[32] = {'\0'};
		const char *sRegRate = "";
		
		//sRegRate = GetRateRegistrySetting(rate_.string);
		//Q_strncpy(sRate, sRegRate, sizeof(sRate)); // 0x20u
		
		//Q_strncpy(sRate, sRegRate, charsmax(sRate)); // == sizeof(sRate) - 1
		//sRate[sizeof(sRate) - 1] = '\0';
		
		//Cvar_DirectSet(&rate_, sRate);
	};
};

CEngine::CEngine()
{
	m_fFrameTime = 0.0f;
	m_nSubState = 0;
	m_nDLLState = DLL_INACTIVE;
	m_fOldTime = 0.0f;
	m_bTrapMode = false;
	m_bDoneTrapping = false;
	m_nTrapKey = 0;
	m_nTrapButtons = 0;
	m_nQuitting = QUIT_NOTQUITTING;

#ifdef REHLDS_FIXES
	m_fCurTime = 0.0f;
#endif
};

bool CEngine::Load(bool dedicated, char *basedir, const char *cmdline)
{
	return Load_noVirt(dedicated, basedir, cmdline);
};

bool CEngine::LoadEx(const TEngineLoadParams &aLoadParams)
{
	return LoadEx_noVirt(aLoadParams);
};

void CEngine::Unload()
{
	Unload_noVirt();
};

int CEngine::Frame()
{
	return Frame_noVirt();
};

void CEngine::SetSubState(int iSubState)
{
	SetSubState_noVirt(iSubState);
};

void CEngine::SetState(int iState)
{
	SetState_noVirt(iState);
};

int CEngine::GetState()
{
	return GetState_noVirt();
};

int CEngine::GetSubState()
{
	return GetSubState_noVirt();
};

double CEngine::GetFrameTime()
{
	return GetFrameTime_noVirt();
};

double CEngine::GetCurTime()
{
	return GetCurTime_noVirt();
};

void CEngine::TrapKey_Event(int key, bool down)
{
	TrapKey_Event_noVirt(key, down);
};

void CEngine::TrapMouse_Event(int buttons, bool down)
{
	TrapMouse_Event_noVirt(buttons, down);
};

void CEngine::StartTrapMode()
{
	StartTrapMode_noVirt();
};

bool CEngine::IsTrapping()
{
	return IsTrapping_noVirt();
};

bool CEngine::CheckDoneTrapping(int &buttons, int &key)
{
	return CheckDoneTrapping_noVirt(buttons, key);
};

void CEngine::SetQuitting(int quittype)
{
	SetQuitting_noVirt(quittype);
};

int CEngine::GetQuitting()
{
	return GetQuitting_noVirt();
};

void CEngine::AddCommandText(const char *asText)
{
	AddCommandText_noVirt(asText);
};

void CEngine::GetHostInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
	GetHostInfo_noVirt(fps, nActive, unused, nMaxPlayers, pszMap);
};

bool CEngine::Load_noVirt(bool dedicated, char *basedir, const char *cmdline)
{
	bool success = false;
	
	SetState(DLL_ACTIVE);
	
	mpHost = std::make_unique<CHost>(mpFileSystem);
	
	if(InitGame(cmdline, basedir, nullptr /*game->GetMainWindowAddress()*/, dedicated))
	{
		success = true;
		
#ifdef _WIN32
		ForceReloadProfile();
#endif // _WIN32
	};
	
	return success;
};

bool CEngine::LoadEx_noVirt(const TEngineLoadParams &aLoadParams)
{
	mpFileSystem = aLoadParams.filesystem;

	return Load(aLoadParams.dedicated, aLoadParams.basedir, aLoadParams.cmdline);
};

void CEngine::Unload_noVirt()
{
	ShutdownGame();
	m_nDLLState = DLL_INACTIVE;
};

void CEngine::SetState_noVirt(int iState)
{
	m_nDLLState = iState;
	//GameSetState(iState);
};

void CEngine::SetSubState_noVirt(int iSubState)
{
	//if(iSubState != 1)
		//GameSetSubState(iSubState);
};

int CEngine::Frame_noVirt()
{
#ifndef SWDS
	//(*(void (**)())(*(DWORD *)cdaudio + 24))();
#endif // SWDS

	//if(!game->IsActiveApp())
		//game->SleepUntilInput(m_nDLLState != DLL_PAUSED ? MINIMIZED_SLEEP : NOT_FOCUS_SLEEP);

	m_fCurTime = CSystem::GetFloatTime();
	m_fFrameTime = m_fCurTime - m_fOldTime;
	m_fOldTime = m_fCurTime;

	if(m_fFrameTime < 0.0f)
		m_fFrameTime = 0.001f;

	if(m_nDLLState == DLL_INACTIVE)
		return m_nDLLState;

	static int dummy;
	static int iState;
	
	iState = mpHost->Frame(m_fFrameTime, m_nDLLState, &dummy);
	
	// Compare a new state with the current
	if(iState == m_nDLLState)
		return m_nDLLState;

	SetState(iState);

	if(m_nDLLState == DLL_CLOSE)
		SetQuitting(QUIT_TODESKTOP);
	else if(m_nDLLState == DLL_RESTART)
		SetQuitting(QUIT_RESTART);

	return m_nDLLState;
};

void CEngine::TrapMouse_Event_noVirt(int buttons, bool down)
{
	if(m_bTrapMode && buttons && !down)
	{
		m_bTrapMode = false;
		m_bDoneTrapping = true;
		m_nTrapKey = 0;
		m_nTrapButtons = buttons;
	}
	//else
		//ClientDLL_MouseEvent(buttons);
};

void CEngine::StartTrapMode_noVirt()
{
	if(!m_bTrapMode)
	{
		m_bDoneTrapping = false;
		m_bTrapMode = true;
	};
};

bool CEngine::CheckDoneTrapping_noVirt(int &buttons, int &key)
{
	if(m_bTrapMode)
		return false;
	else if(m_bDoneTrapping)
	{
		m_bDoneTrapping = false;
		key = m_nTrapKey;
		buttons = m_nTrapButtons;
		return true;
	}
	else
		return false;
};

void CEngine::AddCommandText_noVirt(const char *asText)
{
	//mpConsole->AddCommandText(asText); // Cbuf_AddText
};

void CEngine::GetHostInfo_noVirt(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
	mpHost->GetInfo(fps, nActive, unused, nMaxPlayers, pszMap);
};

int CEngine::InitGame(const char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated)
{
	mbDedicated = bIsDedicated;
	//gbIsDedicatedServer = bIsDedicated;
	
#ifndef SWDS
	if(!mbIsDedicated)
	{
		//pmainwindow = (HWND *)pwnd;
		
#ifdef _WIN32
		//videomode->UpdateWindowPosition();
#endif

	};
#endif // SWDS
	
	//Q_memset(&gmodinfo, 0, sizeof(modinfo_t));
	
	//SV_ResetModInfo();
	
	//TraceInit("Sys_Init()", "Sys_Shutdown()", 0);

#ifdef _WIN32
	//CSystem::InitHardwareTimer();
#endif // _WIN32

	//CSystem::CheckCpuInstructionsSupport();

#ifndef SWDS
	//CSystem::InitFloatTime();
#endif // SWDS
	
	mpFileSystem->LogLevelLoadStarted("Launcher");
	
	//SeedRandomNumberGenerator();
	
	//TraceInit("Sys_InitMemory()", "Sys_ShutdownMemory()", 0);
	//CSystem::InitMemory(&host_parms);
	
	//TraceInit("Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0);
	CSystem::InitLauncherInterface();

#ifndef SWDS
	//if(!GL_SetMode(*pmainwindow, &maindc, &baseRC))
		//return 0;
#endif // SWDS
	
	//TraceInit("Host_Init( &host_parms )", "Host_Shutdown()", 0);
	
	if(!mpHost->Init(&host_parms))
		return 0;

	//TraceInit("Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0);
	CSystem::InitAuthentication();
	
	// Since the dedicated mode is oriented on multiplayer (only)
	// We should init the game dll and enable the multiplayer mode for the network here
	if(mbDedicated)
	{
		mpHost->InitializeGameDLL(); // We should immediately init the game dll for dedicated mode
									 // (client is initializing it after the first call to new game)
									 // NOTE: move to host init?
		//mpNetwork->Config(TRUE); // double call (already called inside host_initgamedll)
	};

#ifndef SWDS
	//else
		//ClientDLL_ActivateMouse();

	char en_US[12];

	Q_strcpy(en_US, "en_US.UTF-8");
	en_US[sizeof(en_US) - 1] = 0; // was [16] = 0; dunno why

	char *cat = setlocale(6, NULL);
	
	if(!cat)
		cat = "c";

	if(Q_stricmp(cat, en_US))
	{
		char MessageText[512];
		Q_snprintf(MessageText, sizeof(MessageText), "SetLocale('%s') failed. Using '%s'.\n"
													 "You may have limited glyph support.\n"
													 "Please install '%s' locale.", en_US, cat, en_US);
		//SDL_ShowSimpleMessageBox(0, "Warning", MessageText, *pmainwindow);
	};
	
#endif // SWDS
	return 1;
};

void CEngine::ShutdownGame()
{
	//if(!mbDedicated)
		//ClientDLL_DeactivateMouse();

	//TraceShutdown("Host_Shutdown()", 0);
	mpHost->Shutdown();

	//if(mbDedicated)
		//mpNetwork->Config(FALSE);

	//TraceShutdown("Sys_ShutdownLauncherInterface()", 0);
	CSystem::ShutdownLauncherInterface();

	//TraceShutdown("Sys_ShutdownAuthentication()", 0);
	CSystem::ShutdownAuthentication();

	//TraceShutdown("Sys_ShutdownMemory()", 0);
	//CSystem::ShutdownMemory();
	
	//TraceShutdown("Sys_Shutdown()", 0);
	CSystem::Shutdown();
};