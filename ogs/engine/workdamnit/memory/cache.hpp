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
/// @brief cache allocator interface

#pragma once

////////////////////////////////////////

void Cache_Init();

/// Returns NULL if all purgable data was tossed and there still wasn't enough
/// room
void *Cache_Alloc(cache_user_t *c, int size, char *name);

cache_system_t *Cache_TryAlloc(int size, qboolean nobottom);

void Cache_Free(cache_user_t *c);

void Cache_FreeLow(int new_low_hunk);
void Cache_FreeHigh(int new_high_hunk);

void Cache_Move(cache_system_t *c);

void Cache_MakeLRU(cache_system_t *cs);
void Cache_UnlinkLRU(cache_system_t *cs);

void Cache_Flush();
void Cache_Force_Flush();

NOXREF int ComparePath1(char *path1, char *path2);
NOXREF int CacheSystemCompare(const void *ppcs1, const void *ppcs2);

NOXREF char *CommatizeNumber(int num, char *pout);

/// Returns the cached data, and moves to the head of the LRU list if present,
/// otherwise returns NULL
void *Cache_Check(cache_user_t *c);

NOXREF void Cache_Print();
NOXREF void Cache_Report();
NOXREF void Cache_Compact();

NOXREF int Cache_TotalUsed();

NOXREF NOBODY void Cache_Print_Models_And_Totals();
NOXREF NOBODY void Cache_Print_Sounds_And_Totals();