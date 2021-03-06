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
/// @brief userinfo functions

#pragma once

#include "common/commontypes.h"
#include "common/com_model.h"
#include "public/FileSystem.h"

/// Max key/value length (with a NULL char)
constexpr auto MAX_KV_LEN = 127; // 512

// Key + value + 2 x slash + NULL
// constexpr auto MAX_INFO_STRING = 256;

constexpr auto INFO_MAX_BUFFER_VALUES = 4;

#ifdef REHLDS_FIXES
constexpr auto MAX_LOCALINFO = 4096;
#else
constexpr auto MAX_LOCALINFO = MAX_INFO_STRING * 128;
#endif // REHLDS_FIXES



void Info_RemovePrefixedKeys(char *s, const char prefix);

qboolean Info_IsKeyImportant(const char *key);

char *Info_FindLargestKey(char *s, int maxsize);

void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize);

void Info_Print(const char *s);

qboolean Info_IsValid(const char *s);

#ifdef REHLDS_FIXES
void Info_CollectFields(char *destInfo, const char *srcInfo, const char *collectedKeysOfFields);
#endif

NOBODY void Info_WriteVars(/*char *s,*/ FileHandle_t fp);