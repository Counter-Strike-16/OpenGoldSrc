/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "Engine.h"

CEngine gEngine;
IEngine *eng = &gEngine;

CEngine::CEngine()
{
};

bool CEngine::Load(bool dedicated, char *basedir, char *cmdline)
{
	bool success = false;
	SetState(DLL_ACTIVE);
	
	if(Sys_InitGame(cmdline, basedir, game->GetMainWindowAddress(), dedicated))
	{
		success = true;
#ifdef _WIN32
		ForceReloadProfile();
#endif // _WIN32
	};
	
	return success;
};

void CEngine::Unload()
{
	Sys_ShutdownGame();
	m_nDLLState = DLL_INACTIVE;
};

void CEngine::SetState(int nState)
{
	m_nDLLState = iState;
	GameSetState(iState);
};

void CEngine::SetSubState(int nSubState)
{
	if(iSubState != 1)
		GameSetSubState(iSubState);
};

int CEngine::Frame()
{
#ifndef SWDS
	(*(void(**)(void))(*(_DWORD*)cdaudio + 24))();
#endif // SWDS
	
	if(!game->IsActiveApp())
		game->SleepUntilInput(m_nDLLState != DLL_PAUSED ? MINIMIZED_SLEEP : NOT_FOCUS_SLEEP);
	
	m_fCurTime = Sys_FloatTime();
	m_fFrameTime = m_fCurTime - m_fOldTime;
	m_fOldTime = m_fCurTime;
	
	if(m_fFrameTime < 0.0)
		m_fFrameTime = 0.001;
	
	if(m_nDLLState == DLL_INACTIVE)
		return m_nDLLState;
	
	int dummy;
	int iState = Host_Frame(m_fFrameTime, m_nDLLState, &dummy);
	
	if(iState == m_nDLLState)
		return m_nDLLState;
	
	SetState(iState);
	
	if(m_nDLLState == DLL_CLOSE)
		SetQuitting(QUIT_TODESKTOP);
	else if(m_nDLLState == DLL_RESTART)
		SetQuitting(QUIT_RESTART);
	
	return m_nDLLState;
};

void CEngine::TrapMouse_Event(int buttons, bool down)
{
	if(m_bTrapMode && buttons && !down)
	{
		m_bTrapMode = false;
		m_bDoneTrapping = true;
		m_nTrapKey = 0;
		m_nTrapButtons = buttons;
	}
	else
		ClientDLL_MouseEvent(buttons);
};

void CEngine::StartTrapMode()
{
	if(!m_bTrapMode)
	{
		m_bDoneTrapping = false;
		m_bTrapMode = true;
	};
};

bool CEngine::CheckDoneTrapping(int &buttons, int &key)
{
	if(m_bTrapMode)
		return false;
	
	if(m_bDoneTrapping)
	{
		m_bDoneTrapping = false;
		key = m_nTrapKey;
		buttons = m_nTrapButtons;
		return true;
	};
	
	return false;
};