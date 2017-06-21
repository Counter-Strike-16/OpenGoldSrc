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

#include "system/System.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "system/client.hpp"




void ForceReloadProfile()
{
	//mpCmdBuffer->AddText("exec config.cfg\n");
	//mpCmdBuffer->AddText("+mlook\n");
	//mpCmdBuffer->Execute();
	
	//if(mpCmdLine->CheckParm("-nomousegrab"))
		//mpConVarHandler->SetVarString("cl_mousegrab", "0");

	// Key_SetBinding(126, "toggleconsole");
	// Key_SetBinding(96, "toggleconsole");
	// Key_SetBinding(27, "cancelselect");
	
	// SDL_GL_SetSwapInterval((gl_vsync.value <= 0.0) - 1);
	
	//if(cls.state != ca_dedicated)
	{
#ifdef SWDS
		//CSystem::Error("Only dedicated mode is supported");
#else
		char sRate[32] = {'\0'};
		const char *sRegRate = "";
		
		//sRegRate = CSystem::GetRateRegistrySetting(rate_.string);
		//Q_strncpy(sRate, sRegRate, sizeof(sRate)); // 0x20u
		
		//Q_strncpy(sRate, sRegRate, charsmax(sRate)); // == sizeof(sRate) - 1
		sRate[sizeof(sRate) - 1] = '\0';
		
		//Cvar_DirectSet(&rate_, sRate);
#endif // SWDS
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
	
	host_parms = new quakeparms_t;
};

CEngine::~CEngine() //= default;
{
	// temp until the usage of quakeparms_t won't be removed
	
	if(host_parms)
		delete host_parms;
	
	host_parms = nullptr;
};

bool CEngine::Load(bool dedicated, char *basedir, const char *cmdline)
{
	return Load_noVirt(dedicated, basedir, cmdline);
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
	
	
	
	
	host_parms->basedir = basedir;
	
	if(InitGame(cmdline, basedir, nullptr /*game->GetMainWindowAddress()*/, dedicated))
	{
		success = true;
		
#ifdef _WIN32
		ForceReloadProfile();
#endif // _WIN32
	};
	
	return success;
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

void CEngine::GetHostInfo_noVirt(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
	mpHost->GetInfo(fps, nActive, unused, nMaxPlayers, pszMap);
};

int CEngine::InitGame(const char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated)
{
	mbDedicated = bIsDedicated;
	//gbIsDedicatedServer = bIsDedicated;
	
#ifndef SWDS
	if(!mbDedicated)
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
	CSystem::Init(host_parms, mpFileSystem.get(), nullptr /*apDedicatedExports*/, mbDedicated);
	
	mpFileSystem->LogLevelLoadStarted("Launcher");
	
	//SeedRandomNumberGenerator();

#ifndef SWDS
	//if(!GL_SetMode(*pmainwindow, &maindc, &baseRC))
		//return 0;
#endif // SWDS
	
	//TraceInit("Host_Init( &host_parms )", "Host_Shutdown()", 0);
	
	if(!mpHost->Init(host_parms))
		return 0;
	
#ifndef SWDS
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
	//mpEventManager->BroadcastEvent(GenerateGameShutdownEvent());
	
	//TraceShutdown("Host_Shutdown()", 0);
	mpHost->Shutdown();

	//if(mbDedicated)
		//mpNetwork->Config(FALSE);
	
	//TraceShutdown("FileSystem_Shutdown()", 0);
	mpFileSystem->Shutdown();
	
	//TraceShutdown("Sys_Shutdown()", 0);
	CSystem::Shutdown();
};