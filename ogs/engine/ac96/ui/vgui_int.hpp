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
/// @brief vgui interface

#pragma once

#include <VGUI_Panel.h>

#include "GameUI/CareerDefs.h"

class ICareerUI;

// VGUI

void VGui_ViewportPaintBackground( int* extents );

void VGui_Startup();
void VGui_Shutdown();

void VGui_CallEngineSurfaceAppHandler( void* event, void* userData );

vgui::Panel* VGui_GetPanel();

void VGui_ReleaseMouse();

void VGui_GetMouse();

void VGui_SetVisible( bool state );

void VGui_Paint();

bool VGui_GameUIKeyPressed();

bool VGui_Key_Event( int down, int keynum, const char* pszCurrentBinding );

bool VGui_LoadBMP( FileHandle_t file, byte* buffer, int bufsize, int* width, int* height );

// VGUI1 wrappers

void VGuiWrap_SetRootPanelSize();

void VGuiWrap_Startup();

void VGuiWrap_Shutdown();

bool VGuiWrap_CallEngineSurfaceAppHandler( void* event, void* userData );

vgui::Panel* VGuiWrap_GetPanel();

void VGuiWrap_ReleaseMouse();

void VGuiWrap_GetMouse();

void VGuiWrap_SetVisible( bool state );

void VGuiWrap_Paint( bool paintAll );

// VGUI2 wrappers

void VGuiWrap2_Startup();

void VGuiWrap2_Shutdown();

bool VGuiWrap2_CallEngineSurfaceAppHandler( void* event, void* userData );

bool VGuiWrap2_IsGameUIVisible();

bool VGuiWrap2_UseVGUI1();

vgui::Panel* VGuiWrap2_GetPanel();

void VGuiWrap2_ReleaseMouse();

void VGuiWrap2_GetMouse();

void VGuiWrap2_SetVisible( bool state );

bool VGuiWrap2_GameUIKeyPressed();

bool VGuiWrap2_Key_Event( int down, int keynum, const char* pszCurrentBinding );

void VGuiWrap2_Paint();

void VGuiWrap2_NotifyOfServerDisconnect();

void VGuiWrap2_HideGameUI();

bool VGuiWrap2_IsConsoleVisible();

void VGuiWrap2_ShowConsole();

void VGuiWrap2_ShowDemoPlayer();

void VGuiWrap2_HideConsole();

void VGuiWrap2_ClearConsole();

void VGuiWrap2_ConPrintf( const char* msg );
void VGuiWrap2_ConDPrintf( const char* msg );

void VGuiWrap2_LoadingStarted( const char* resourceType, const char* resourceName );
void VGuiWrap2_LoadingFinished( const char* resourceType, const char* resourceName );

void VGuiWrap2_NotifyOfServerConnect( const char* game, int IP, int port );

CareerStateType VGuiWrap2_IsInCareerMatch();

ICareerUI* VguiWrap2_GetCareerUI();

size_t VGuiWrap2_GetLocalizedStringLength( const char* label );

void VguiWrap2_GetMouseDelta( int* x, int* y );

void ProcessTutorMessageDecayBuffer( int* buffer, int bufferLength );

void ConstructTutorMessageDecayBuffer( int* buffer, int bufferLength );

void ResetTutorMessageDecayData();