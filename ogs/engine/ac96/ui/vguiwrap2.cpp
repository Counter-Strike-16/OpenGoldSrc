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

#include "precompiled.hpp"
#include "ui/vguiwrap2.hpp"

void VGuiWrap2_Startup()
{
};

void VGuiWrap2_Shutdown()
{
};

int EXT_FUNC VGuiWrap2_IsInCareerMatch() // return CareerStateType
{
#ifndef SWDS
#endif
	
	/*
	if( !staticCareerUI )
		return CAREER_NONE;

	return g_careerState;
	*/
	
	return 0; // CAREER_NONE;
};

void *VguiWrap2_GetCareerUI() // return ICareerUI*
{
#ifndef SWDS
#endif
	
	return nullptr;
};

// return size_t?
int EXT_FUNC VGuiWrap2_GetLocalizedStringLength(const char *label)
{
#ifndef SWDS
#endif
	
	return 0;
};

void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName)
{
#ifndef SWDS
#endif
};

void VGuiWrap2_LoadingFinished(const char *resourceType, const char *resourceName)
{
};

void VguiWrap2_GetMouseDelta(int *x, int *y)
{
	g_BaseUISurface.GetMouseDelta(*x, *y);
};

void VGuiWrap2_NotifyOfServerConnect( const char* game, int IP, int port )
{
};

void VGuiWrap2_HideGameUI()
{
	/*
	if( staticUIFuncs )
		staticUIFuncs->HideGameUI();
	*/
};

bool VGuiWrap2_IsGameUIVisible()
{
	return false;
	/*
	if( !staticGameUIFuncs )
		return false;

	return staticGameUIFuncs->IsGameUIActive();
	*/
};

void VGuiWrap2_ShowDemoPlayer()
{
};

// GameUI.dll -> IGameConsole interaction

void VGuiWrap2_ShowConsole()
{
};

void VGuiWrap2_HideConsole()
{
};

bool VGuiWrap2_IsConsoleVisible()
{
	return false;
};

void VGuiWrap2_ClearConsole()
{
};

void VGuiWrap2_ConPrintf( const char* msg )
{
};

void VGuiWrap2_ConDPrintf( const char* msg )
{
};