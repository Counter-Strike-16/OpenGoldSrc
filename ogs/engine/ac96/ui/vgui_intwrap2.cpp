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
#include "ui/vgui_int.hpp"

#include <UtlVector.h>

#include <vgui/ILocalize.h>
#include <vgui_controls/Controls.h>

#include "quakedef.h"
#include "client.h"
#include "host_cmd.h"

#include "cdll_int.h"
#include "cl_main.h"
#include "APIProxy.h"
#include "BaseUI/IBaseUI.h"
#include "GameUI/IClientVGUI.h"
#include "GameUI/IGameConsole.h"
#include "GameUI/IGameUI.h"
#include "gl_rmain.h"
#include "gl_screen.h"
#include "host.h"
#include "modinfo.h"
#include "sys_getmodes.h"
#include "vgui_int.h"
#include "vgui2/BaseUI_Interface.h"
#include "vgui2/BaseUISurface.h"

static CUtlVector<char> g_TempConsoleBuffer;

static IBaseUI* staticUIFuncs = nullptr;

static bool staticExclusiveInputShadow = false;

static int s_tutorMessageDecayData[ 256 ] = {};

void VGuiWrap2_Startup()
{
	if( staticUIFuncs )
		return;

	auto factoryFn = Sys_GetFactoryThis();

	CreateInterfaceFn factories[ 2 ] = 
	{
		factoryFn,
		GetFileSystemFactory()
	};

	staticUIFuncs = static_cast<IBaseUI*>( factoryFn( BASEUI_INTERFACE_VERSION, nullptr ) );

	staticUIFuncs->Initialize( factories, ARRAYSIZE( factories ) );
	staticUIFuncs->Start( &cl_enginefuncs, CLDLL_INTERFACE_VERSION );

	//Flush temporary buffer
	g_TempConsoleBuffer.AddToTail( '\0' );
	VGuiWrap2_ConPrintf( g_TempConsoleBuffer.Base() );
	g_TempConsoleBuffer.Purge();
}

void VGuiWrap2_Shutdown()
{
	if( staticUIFuncs )
	{
		staticUIFuncs->Shutdown();
		staticUIFuncs = nullptr;
	}
}

bool VGuiWrap2_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	if( staticUIFuncs )
		staticUIFuncs->CallEngineSurfaceAppHandler( event, userData );

	return false;
}

bool VGuiWrap2_IsGameUIVisible()
{
	if( !staticGameUIFuncs )
		return false;

	return staticGameUIFuncs->IsGameUIActive() != 0;
}

bool VGuiWrap2_UseVGUI1()
{
	if( !staticClient )
	return true;

	return staticClient->UseVGUI1() != 0;
}

vgui::Panel* VGuiWrap2_GetPanel()
{
	// Nothing
	return nullptr;
}

void VGuiWrap2_ReleaseMouse()
{
	// Nothing
}

void VGuiWrap2_GetMouse()
{
	// Nothing
}

void VGuiWrap2_SetVisible( bool state )
{
	// Nothing
}

bool VGuiWrap2_GameUIKeyPressed()
{
	if( !staticGameUIFuncs )
		return false;

	if( staticGameUIFuncs->IsGameUIActive() )
	{
		if( cl.levelname[ 0 ] )
		{
			staticUIFuncs->HideGameUI();
		}
	}
	else
	{
		staticUIFuncs->ActivateGameUI();
	}

	return true;
}

bool VGuiWrap2_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !staticUIFuncs )
		return true;

	return !staticUIFuncs->Key_Event( down, keynum, pszCurrentBinding );
}

void VGuiWrap2_Paint()
{
	if( !staticGameUIFuncs )
	{
		return;
	}

	int x = 0, y = 0;

	Rect_t rect;

	rect.y = 0;

	if( VideoMode_IsWindowed() )
	{
		SDL_GetWindowPosition( pmainwindow, &x, &y );
		SDL_GetWindowSize( pmainwindow, &rect.width, &rect.height );
	}
	else
	{
		VideoMode_GetCurrentVideoMode( &rect.width, &rect.height, nullptr );
	}

	rect.height += y;

	AllowFog( false );

	staticUIFuncs->Paint( x, y, rect.width, rect.height );

	if( !staticClient || staticClient->UseVGUI1() )
	{
		const bool bInputState = staticGameUIFuncs->HasExclusiveInput();

		if( bInputState != staticExclusiveInputShadow )
		{
			if( bInputState )
			{
				VGuiWrap_ReleaseMouse();
			}
			else
			{
				VGuiWrap_GetMouse();
				ClearIOStates();
			}
		}

		staticExclusiveInputShadow = bInputState;
	}

	AllowFog( true );
}

void VGuiWrap2_NotifyOfServerDisconnect()
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->DisconnectFromServer();
}

void VGuiWrap2_HideGameUI()
{
	if( staticUIFuncs )
		staticUIFuncs->HideGameUI();
}

bool VGuiWrap2_IsConsoleVisible()
{
	if( !staticGameConsole )
		return false;

	return staticGameConsole->IsConsoleVisible();
}

void VGuiWrap2_ShowConsole()
{
	if( staticUIFuncs )
	{
		staticUIFuncs->ActivateGameUI();
		staticUIFuncs->ShowConsole();
	}
}

void VGuiWrap2_ShowDemoPlayer()
{
	if( staticUIFuncs )
		staticUIFuncs->ActivateGameUI();

	if( staticGameUIFuncs )
		staticGameUIFuncs->ActivateDemoUI();
}

void VGuiWrap2_HideConsole()
{
	if( staticUIFuncs )
		staticUIFuncs->HideConsole();
}

void VGuiWrap2_ClearConsole()
{
	if( staticGameConsole )
		staticGameConsole->Clear();
}

void VGuiWrap2_ConPrintf( const char* msg )
{
	if( staticGameConsole )
	{
		staticGameConsole->Printf( "%s", msg );
		return;
	}

	const size_t uiLength = strlen( msg );

	g_TempConsoleBuffer.InsertMultipleBefore( g_TempConsoleBuffer.Count(), uiLength, msg );
}

void VGuiWrap2_ConDPrintf( const char* msg )
{
	if( staticGameConsole )
	{
		staticGameConsole->DPrintf( "%s", msg );
		return;
	}

	const size_t uiLength = strlen( msg );

	g_TempConsoleBuffer.InsertMultipleBefore( g_TempConsoleBuffer.Count(), uiLength, msg );
}

void VGuiWrap2_LoadingStarted( const char* resourceType, const char* resourceName )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->LoadingStarted( resourceType, resourceName );
}

void VGuiWrap2_LoadingFinished( const char* resourceType, const char* resourceName )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->LoadingFinished( resourceType, resourceName );
}

void VGuiWrap2_NotifyOfServerConnect( const char* game, int IP, int port )
{
	if( staticGameUIFuncs )
	{
		gfExtendedError = false;
		gszDisconnectReason[ 0 ] = '\0';
		gszExtendedDisconnectReason[ 0 ] = '\0';
		StopLoadingProgressBar();

		staticGameUIFuncs->ConnectToServer( game, IP, port );
	}
}

CareerStateType VGuiWrap2_IsInCareerMatch()
{
	if( !staticCareerUI )
		return CAREER_NONE;

	return g_careerState;
}

ICareerUI* VguiWrap2_GetCareerUI()
{
	return staticCareerUI;
}

size_t VGuiWrap2_GetLocalizedStringLength( const char* label )
{
	if( !label || !vgui2::localize() )
		return 0;

	const wchar_t* pszLocalized = vgui2::localize()->Find( label );

	if( !pszLocalized )
		return 0;

	return wcslen( pszLocalized );
}

void VguiWrap2_GetMouseDelta( int* x, int* y )
{
	g_BaseUISurface.GetMouseDelta( *x, *y );
}

void VGUI2_OnDisconnectFromServer( int eLoginFailure )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->OnDisconnectFromServer( eLoginFailure, "" );
}

void StartLoadingProgressBar( const char* loadingType, int numProgressPoints )
{
	//Display the bar only if we're playing a multiplayer game or are connected to a server
	if( !Host_IsSinglePlayerGame() && ( !UserIsConnectedOnLoopback() || gmodinfo.type != SINGLEPLAYER_ONLY ) )
	{
		if( staticUIFuncs )
			staticUIFuncs->ActivateGameUI();

		if( staticGameUIFuncs )
		{
			staticGameUIFuncs->StartProgressBar( loadingType, numProgressPoints );
			SCR_UpdateScreen();
		}
	}
}

void ContinueLoadingProgressBar( const char* loadingType, int progressPoint, float progressFraction )
{
	if( staticGameUIFuncs )
	{
		if( staticGameUIFuncs->ContinueProgressBar( progressPoint, progressFraction ) )
			SCR_UpdateScreen();
	}
}

void SetLoadingProgressBarStatusText( const char* statusText )
{
	if( staticGameUIFuncs )
	{
		if( staticGameUIFuncs->SetProgressBarStatusText( statusText ) )
			SCR_UpdateScreen();
	}
}

void StopLoadingProgressBar()
{
	if( cls.state == ca_active )
	{
		if( staticUIFuncs )
			staticUIFuncs->HideGameUI();
	}
	else if( staticUIFuncs )
	{
		if( staticClient )
		{
			staticClient->HideAllVGUIMenu();
		}

		staticUIFuncs->ActivateGameUI();
	}
	if( staticGameUIFuncs )
		staticGameUIFuncs->StopProgressBar( gfExtendedError != 0, gszDisconnectReason, gszExtendedDisconnectReason );
	
	gfExtendedError = false;
	gszDisconnectReason[ 0 ] = '\0';
	gszExtendedDisconnectReason[ 0 ] = '\0';
}

void SetSecondaryProgressBar( float progress )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->SetSecondaryProgressBar( progress );
}

void SetSecondaryProgressBarText( const char *statusText )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->SetSecondaryProgressBarText( statusText );
}

void ValidateCDKey( int force, int inConnect )
{
	if( staticGameUIFuncs )
		staticGameUIFuncs->ValidateCDKey( force != 0, inConnect != 0 );
}

void RegisterTutorMessageShown( int mid )
{
	if( mid >= 0 && mid < ARRAYSIZE( s_tutorMessageDecayData ) )
		++s_tutorMessageDecayData[ mid ];
}

int GetTimesTutorMessageShown( int mid )
{
	if( mid >= 0 && mid < ARRAYSIZE( s_tutorMessageDecayData ) )
		return s_tutorMessageDecayData[ mid ];

	return -1;
}

void ProcessTutorMessageDecayBuffer( int* buffer, int bufferLength )
{
	ResetTutorMessageDecayData();

	const auto count = min( static_cast<int>( ARRAYSIZE( s_tutorMessageDecayData ) ), bufferLength );

	if( count > 0 )
	{
		memcpy( s_tutorMessageDecayData, buffer, sizeof( int ) * count );
	}
}

void ConstructTutorMessageDecayBuffer( int* buffer, int bufferLength )
{
	if( !buffer )
		return;

	memset( buffer, 0, sizeof( int ) * bufferLength );

	const auto count = min( static_cast<int>( ARRAYSIZE( s_tutorMessageDecayData ) ), bufferLength );

	if( count > 0 )
	{
		memcpy( buffer, s_tutorMessageDecayData, sizeof( int ) * count );
	}
}

void ResetTutorMessageDecayData()
{
	memset( s_tutorMessageDecayData, 0, sizeof( s_tutorMessageDecayData ) );
}