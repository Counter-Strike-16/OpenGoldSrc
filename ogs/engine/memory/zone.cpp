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

//#include "precompiled.hpp"
#include "memory/zone.hpp"
#include "system/system.hpp"
#include "console/console.hpp"
#include "system/common.hpp"

/*
==============================================================================

ZONE MEMORY ALLOCATION

There is never any space between memblocks, and there will never be two
contiguous free memblocks.

The rover can be left pointing at a non-empty block

The zone calls are pretty much only used for small strings and structures,
all big things are allocated on the hunk.
==============================================================================
*/

const int ZONEID      = 0x001d4a11;
const int MINFRAGMENT = 64;

typedef struct memblock_s
{
	int size;
	int tag;
	int id;

	struct memblock_s *next;
	struct memblock_s *prev;

	int pad;
} memblock_t;

typedef struct memzone_s
{
	int         size;
	memblock_t  blocklist;
	memblock_t *rover;
} memzone_t;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

cvar_t mem_dbgfile = {"mem_dbgfile", ".\\mem.txt", 0, 0.0f, NULL};

#else // HOOK_ENGINE

cvar_t mem_dbgfile;

#endif // HOOK_ENGINE

memzone_t *mainzone;

/*
========================
Memory_Init
========================
*/
void Memory_Init(void *buf, int size)
{
	int zonesize = ZONE_DYNAMIC_SIZE;

	hunk_base      = (byte *)buf;
	hunk_size      = size;
	hunk_low_used  = 0;
	hunk_high_used = 0;

	Cache_Init();

	int p = COM_CheckParm("-zone");

	if(p)
	{
		if(p < com_argc - 1)
			zonesize = Q_atoi(com_argv[p + 1]) * 1024;
		else
			Sys_Error("%s: you must specify a size in KB after -zone", __FUNCTION__);
	};

	mainzone = ((memzone_t *)Hunk_AllocName(zonesize, "zone"));
	Z_ClearZone(mainzone, zonesize);
};

void *Z_Malloc(int size)
{
	Z_CheckHeap();

	void *buf = Z_TagMalloc(size, 1);

	if(!buf)
		Sys_Error("%s: failed on allocation of %i bytes", __FUNCTION__, size);

	Q_memset(buf, 0, size);
	return buf;
};

void *Z_TagMalloc(int size, int tag)
{
	int         extra;
	memblock_t *start, *rover, *newz, *base;

	if(tag == 0)
		Sys_Error("%s: tried to use a 0 tag", __FUNCTION__);

	size += sizeof(memblock_t);
	size += 4;
	size = (size + 7) & ~7;

	base = rover = mainzone->rover;

	start = base->prev;

	do
	{
		if(rover == start)
			return (NULL);

		if(rover->tag)
			base = rover = rover->next;
		else
			rover = rover->next;
	} while(base->tag || base->size < size);

	extra = base->size - size;

	if(extra > MINFRAGMENT)
	{
		newz             = (memblock_t *)((byte *)base + size);
		newz->size       = extra;
		newz->tag        = 0;
		newz->prev       = base;
		newz->id         = ZONEID;
		newz->next       = base->next;
		newz->next->prev = newz;
		base->next       = newz;
		base->size       = size;
	};

	base->tag       = tag;
	mainzone->rover = base->next;
	base->id        = ZONEID;

	// marker for memory trash testing
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *)((byte *)base + sizeof(memblock_t));
};

void Z_Free(void *ptr)
{
	if(!ptr)
		Sys_Error("%s: NULL pointer", __FUNCTION__);

	memblock_t *block = (memblock_t *)((char *)ptr - sizeof(memblock_t));

	if(block->id != ZONEID)
		Sys_Error("%s: freed a pointer without ZONEID", __FUNCTION__);

	if(!block->tag)
		Sys_Error("%s: freed a freed pointer", __FUNCTION__);

	block->tag = 0;

	memblock_t *otherblock = block->prev;

	if(!otherblock->tag)
	{
		otherblock->size += block->size;
		otherblock->next  = block->next;
		block->next->prev = otherblock;

		if(block == mainzone->rover)
			mainzone->rover = otherblock;

		block = otherblock;
	};

	otherblock = block->next;

	if(!otherblock->tag)
	{
		block->size += otherblock->size;
		block->next            = otherblock->next;
		otherblock->next->prev = block;

		if(otherblock == mainzone->rover)
			mainzone->rover = block;
	};
};

void Z_ClearZone(memzone_t *zone, int size)
{
	memblock_t *block = (memblock_t *)&zone[1];

	zone->blocklist.prev = zone->blocklist.next = zone->rover = block;
	zone->blocklist.size = zone->blocklist.id = 0;
	zone->blocklist.tag                       = 1;

	block->prev = block->next = &zone->blocklist;

	block->tag  = 0;
	block->id   = ZONEID;
	block->size = size - sizeof(memzone_t);
};

void Z_CheckHeap()
{
	for(memblock_t *block = mainzone->blocklist.next;; block = block->next)
	{
		if(block->next == &mainzone->blocklist)
			break;

		if((byte *)block + block->size != (byte *)block->next)
			Sys_Error("%s: block size does not touch the next block\n", __FUNCTION__);

		if(block->next->prev != block)
			Sys_Error("%s: next block doesn't have proper back link\n", __FUNCTION__);

		if(!block->tag && !block->next->tag)
			Sys_Error("%s: two consecutive free blocks\n", __FUNCTION__);
	};
};

NOXREF void Z_Print(memzone_t *zone)
{
	NOXREFCHECK;

	Con_Printf("zone size: %i  location: %p\n", mainzone->size, mainzone);

	for(memblock_t *block = zone->blocklist.next;; block = block->next)
	{
		Con_Printf("block:%p    size:%7i    tag:%3i\n", block, block->size, block->tag);

		// all blocks have been hit
		if(block->next == &zone->blocklist)
			break;

		if((byte *)block + block->size != (byte *)block->next)
			Con_Printf("ERROR: block size does not touch the next block\n");

		if(block->next->prev != block)
			Con_Printf("ERROR: next block doesn't have proper back link\n");

		if(!block->tag && !block->next->tag)
			Con_Printf("ERROR: two consecutive free blocks\n");
	};
};