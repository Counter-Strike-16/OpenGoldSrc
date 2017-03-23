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

#pragma once

#include <memory>
#include "common/maintypes.h"
#include "system/IEngine.hpp"
#include "system/IGame.hpp"
#include "system/Host.hpp"

// sleep time when not focus
constexpr auto NOT_FOCUS_SLEEP = 50;
constexpr auto MINIMIZED_SLEEP = 20;

// clang-format off
#ifdef HOOK_ENGINE
	#define game (*pgame)
	#define eng (*peng)
#endif // HOOK_ENGINE
// clang-format on

extern IGame *game;
extern IEngine *eng;

class CEngine : public IEngine
{
public:
	CEngine();
	virtual ~CEngine(){}

	virtual bool Load(bool dedicated, char *rootDir, const char *cmdLine);
	virtual void Unload();
	
	virtual void SetState(int iState);
	virtual int GetState();
	
	virtual void SetSubState(int iSubstate);
	virtual int GetSubState();
	
	virtual int Frame();
	
	virtual double GetFrameTime();
	virtual double GetCurTime();
	
	virtual void TrapKey_Event(int key, bool down);
	virtual void TrapMouse_Event(int buttons, bool down);
	
	virtual void StartTrapMode();
	virtual bool IsTrapping();
	virtual bool CheckDoneTrapping(int &buttons, int &keys);
	
	virtual int GetQuitting();
	virtual void SetQuitting(int quittype);

	// non-virtual function's of wrap for hooks a virtual
	// Only needed for HOOK_ENGINE
	bool Load_noVirt(bool dedicated, char *rootDir, const char *cmdLine);
	void Unload_noVirt();
	
	void SetState_noVirt(int iState);
	int GetState_noVirt(){return m_nDLLState;}
	
	void SetSubState_noVirt(int iSubstate);
	int GetSubState_noVirt(){return m_nSubState;}
	
	int Frame_noVirt();
	
	double GetFrameTime_noVirt(){return m_fFrameTime;}
	double GetCurTime_noVirt(){return m_fCurTime;}
	
	void TrapKey_Event_noVirt(int key, bool down){}
	void TrapMouse_Event_noVirt(int buttons, bool down);
	
	void StartTrapMode_noVirt();
	bool IsTrapping_noVirt(){return m_bTrapMode;}
	
	bool CheckDoneTrapping_noVirt(int &buttons, int &keys);
	
	int GetQuitting_noVirt(){return m_nQuitting;}
	void SetQuitting_noVirt(int quittype){m_nQuitting = quittype;}
private:
	int InitGame(char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated);
	void ShutdownGame();
	
	quakeparms_t host_parms;
	
	std::unique_ptr<CHost> mpHost;
	
	double m_fCurTime{0.0f};
	double m_fFrameTime{0.0f};
	double m_fOldTime{0.0f};
	
	int m_nQuitting{0};
	int m_nDLLState{0};
	int m_nSubState{0};
	
	int m_nTrapKey{0};
	int m_nTrapButtons{0};
	
	bool m_bTrapMode{false};
	bool m_bDoneTrapping{false};
	
	bool mbDedicated{false};
};