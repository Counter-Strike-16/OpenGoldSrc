/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

#include "public/FileSystem.h"

#ifdef HOOK_ENGINE
	#define key_dest (*pkey_dest)
#endif

/// Keyboard input destination/state
typedef enum {
	key_game = 0, ///< player is in game
	// key_console was here
	key_message,  ///< player is writing a chat message
	key_menu      ///< player is in menu
} keydest_t;

extern keydest_t key_dest;

//extern int anykeydown;

void Key_Init();
void Key_Shutdown();

void Key_ClearStates();

void Key_Event(int key, int down); // void Key_Event(int key, qboolean down, unsigned time);

int Key_CountBindings();
void Key_WriteBindings(FileHandle_t f);

void Key_SetBinding(int keynum, const char *binding);
const char *Key_LookupBinding(const char *pBinding);

const char *Key_KeynumToString(int nKey);
//int Key_StringToKeynum(const char *str);

int Key_GetKey(const char *sBinding); // int Key_GetKey();