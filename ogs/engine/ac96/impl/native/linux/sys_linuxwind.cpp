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
#include "system/game.hpp"
#include "video/IWindow.hpp"
#include "graphics/vid.hpp"

IWindow *gpMainWindow = nullptr;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

CGame g_Game;
IGame *game = &g_Game;

#else // HOOK_ENGINE

IGame *game;

#endif // HOOK_ENGINE

bool CGame::Init(void *pvInstance)
{
	return Init_noVirt(pvInstance);
};

bool CGame::Init_noVirt(void *pvInstance)
{
	return true;
};

bool CGame::Shutdown()
{
	return Shutdown_noVirt();
};

bool CGame::Shutdown_noVirt()
{
	return true;
};

bool CGame::CreateGameWindow()
{
	return CreateGameWindow_noVirt();
};

bool CGame::CreateGameWindow_noVirt()
{
	gpMainWindow = VID_OpenWindow();
	return true;
};

void CGame::SleepUntilInput(int time)
{
	SleepUntilInput_noVirt(time);
};

void CGame::SleepUntilInput_noVirt(int time)
{
#ifdef _WIN32
	Sleep(time * 1000);
#else  // _WIN32
	sleep(time);
#endif // _WIN32
};

HWND CGame::GetMainWindow()
{
	return GetMainWindow_noVirt();
};

HWND CGame::GetMainWindow_noVirt()
{
	return mainwindow;
};

HWND *CGame::GetMainWindowAddress()
{
	return GetMainWindowAddress_noVirt();
};

HWND *CGame::GetMainWindowAddress_noVirt()
{
	//return *mainwindow;
	return nullptr;
};

void CGame::SetWindowXY(int x, int y)
{
	SetWindowXY_noVirt(x, y);
};

void CGame::SetWindowXY_noVirt(int x, int y)
{
	gpMainWindow->SetPos(x, y);
};

void CGame::SetWindowSize(int w, int h)
{
	SetWindowSize_noVirt(w, h);
};

void CGame::SetWindowSize_noVirt(int w, int h)
{
	gpMainWindow->SetSize(w, h);
};

void CGame::GetWindowRect(int *x, int *y, int *w, int *h)
{
	GetWindowRect_noVirt(x, y, w, h);
};

void CGame::GetWindowRect_noVirt(int *x, int *y, int *w, int *h)
{
	if(x)
		*x = 0;
	if(y)
		*y = 0;
	if(w)
		*w = 0;
	if(h)
		*h = 0;
};

bool CGame::IsActiveApp()
{
	return IsActiveApp_noVirt();
};

bool CGame::IsActiveApp_noVirt()
{
	return m_bActiveApp;
};

bool CGame::IsMultiplayer()
{
	return IsMultiplayer_noVirt();
};

bool CGame::IsMultiplayer_noVirt()
{
	return true;
};

void CGame::PlayStartupVideos()
{
	return PlayStartupVideos_noVirt();
};

void CGame::PlayStartupVideos_noVirt()
{
};

void CGame::PlayAVIAndWait(const char *aviFile)
{
	PlayAVIAndWait_noVirt(aviFile);
};

void CGame::PlayAVIAndWait_noVirt(const char *aviFile)
{
};

void CGame::SetCursorVisible(bool bState)
{
	SetCursorVisible_noVirt(bState);
};

void CGame::SetCursorVisible_noVirt(bool bState)
{
};