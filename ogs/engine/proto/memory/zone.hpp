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
/// @brief zone allocator interface

#pragma once

#include "common/commontypes.h"
#include "maintypes.h"

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

/// @return 0 filled memory
void *Z_Malloc(int size);

void *Z_TagMalloc(int size, int tag);

void Z_Free(void *ptr);

void Z_ClearZone(memzone_t *zone, int size);

void Z_CheckHeap();

NOXREF void Z_Print(memzone_t *zone);

/*
void Z_Init(int zonesize);
void Z_DumpHeap();
int Z_FreeMemory();
*/