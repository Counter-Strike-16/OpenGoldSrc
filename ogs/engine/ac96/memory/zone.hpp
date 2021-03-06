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
/// @brief zone, hunk and cache allocators

/*
memory allocation


H_??? The hunk manages the entire memory block given to quake.  It must be
contiguous.  Memory can be allocated from either the low or high end in a
stack fashion.  The only way memory is released is by resetting one of the
pointers.

Hunk allocations should be given a name, so the Hunk_Print () function
can display usage.

Hunk allocations are guaranteed to be 16 byte aligned.

The video buffers are allocated high to avoid leaving a hole underneath
server allocations when changing to a higher video mode.


Z_??? Zone memory functions used for small, dynamic allocations like text
strings from command input.  There is only about 48K for it, allocated at
the very bottom of the hunk.

Cache_??? Cache memory is for objects that can be dynamically loaded and
can usefully stay persistant between levels.  The size of the cache
fluctuates from level to level.

To allocate a cachable object


Temp_??? Temp memory is used for file loading and surface caching.  The size
of the cache memory is adjusted so that there is a minimum of 512k remaining
for temp memory.


------ Top of Memory -------

high hunk allocations

<--- high hunk reset point held by vid

video buffer

z buffer

surface cache

<--- high hunk used

cachable memory

<--- low hunk used

client and server low hunk allocations

<-- low hunk reset point held by host

startup hunk allocations

Zone block

----- Bottom of Memory -----



*/

#pragma once

#include "common/commontypes.h"
#include "common/maintypes.h"

////////////////////////////////////////

// clang-format off
#ifdef HOOK_ENGINE
	#define mem_dbgfile (*pmem_dbgfile)
	#define mainzone (*pmainzone)
	
	#define hunk_base (*phunk_base)
	#define hunk_size (*phunk_size)
	#define hunk_low_used (*phunk_low_used)
	#define hunk_high_used (*phunk_high_used)
	#define hunk_tempmark (*phunk_tempmark)
	#define hunk_tempactive (*phunk_tempactive)
	
	#define cache_head (*pcache_head)
#endif // HOOK_ENGINE
// clang-format on

////////////////////////////////////////

const int ZONE_DYNAMIC_SIZE = 0x20000;

////////////////////////////////////////

typedef struct cvar_s cvar_t;
typedef struct memzone_s memzone_t;

typedef struct cache_user_s cache_user_t;
typedef struct cache_system_s cache_system_t;

////////////////////////////////////////

extern cvar_t mem_dbgfile;
extern memzone_t *mainzone;

extern byte *hunk_base;

extern int hunk_size;
extern int hunk_low_used;
extern int hunk_high_used;
extern int hunk_tempmark;

extern qboolean hunk_tempactive;

extern cache_system_t cache_head;

////////////////////////////////////////

//void Z_Init(int zonesize);

/// @return 0 filled memory
void *Z_Malloc(int size);

void *Z_TagMalloc(int size, int tag);

void Z_Free(void *ptr);

void Z_ClearZone(memzone_t *zone, int size);

//void Z_DumpHeap();
void Z_CheckHeap();

NOXREF void Z_Print(memzone_t *zone);

//int Z_FreeMemory();

////////////////////////////////////////

/// @return 0 filled memory
void *Hunk_Alloc(int size);

void *Hunk_TempAlloc(int size);

void *Hunk_AllocName(int size, const char *name);
void *Hunk_HighAllocName(int size, const char *name);

int Hunk_LowMark();
int Hunk_HighMark();

void Hunk_FreeToLowMark(int mark);
void Hunk_FreeToHighMark(int mark);

void Hunk_Check();

NOXREF void Hunk_Print(qboolean all);

/*
void *Hunk_Begin(int maxsize);

int Hunk_End();
void Hunk_Free(void *base);
*/

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