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



bool VGuiWrap2_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	/*
	if( staticUIFuncs )
		staticUIFuncs->CallEngineSurfaceAppHandler( event, userData );
	*/

	return false;
};

bool VGuiWrap2_UseVGUI1()
{
	return true;
};

vgui::Panel* VGuiWrap2_GetPanel()
{
	// Nothing
	return nullptr;
};

void VGuiWrap2_ReleaseMouse()
{
	// Nothing
};

void VGuiWrap2_GetMouse()
{
	// Nothing
};

void VGuiWrap2_SetVisible( bool state )
{
	// Nothing
};

bool VGuiWrap2_GameUIKeyPressed()
{
	return false;
	/*
	if( !staticGameUIFuncs )
		return false;

	if( staticGameUIFuncs->IsGameUIActive() )
	{
		if( ei.levelname[ 0 ] )
		{
			staticUIFuncs->HideGameUI();
		}
	}
	else
	{
		staticUIFuncs->ActivateGameUI();
	}

	return true;
	*/
};

bool VGuiWrap2_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	return true;
	/*
	if( !staticUIFuncs )
		return true;

	return staticUIFuncs->Key_Event( down, keynum, pszCurrentBinding ) == 0;
	*/
};

void VGuiWrap2_Paint()
{
};

void VGuiWrap2_NotifyOfServerDisconnect()
{
};

