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

//#include "precompiled.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "client/client.hpp"
#include "common/com_model.h"
#include "console/cmd.hpp"
#include "console/console.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "server/server.hpp"

/*
===============================================================================

CACHE MEMORY

===============================================================================
*/

const int CACHE_NAME_LEN = 64;

const int MAX_SFX = 1024;

typedef struct cache_system_s
{
	int size;

	cache_user_t *user;

	char name[CACHE_NAME_LEN];

	cache_system_t *prev;
	cache_system_t *next;
	cache_system_t *lru_prev;
	cache_system_t *lru_next;
} cache_system_t;

cache_system_t cache_head;

/*
============
Cache_Init

============
*/
void Cache_Init()
{
	cache_head.next = cache_head.prev = &cache_head;
	cache_head.lru_next = cache_head.lru_prev = &cache_head;

	Cmd_AddCommand("flush", Cache_Flush);
};

/*
==============
Cache_Alloc
==============
*/
void *Cache_Alloc(cache_user_t *c, int size, char *name)
{
	cache_system_t *cs;

	if(c->data)
		Sys_Error("%s: already allocated", __FUNCTION__);

	if(size <= 0)
		Sys_Error("%s: size %i", __FUNCTION__, size);
	
	// find memory for it
	while(true)
	{
		cs = Cache_TryAlloc((size + sizeof(cache_system_t) + 15) & ~15, 0);

		if(cs)
		{
			Q_strncpy(cs->name, name, CACHE_NAME_LEN - 1);
			cs->name[CACHE_NAME_LEN - 1] = 0;
			c->data = cs + 1;
			cs->user = c;

			break;
		};
		
		// free the least recently used cahedata
		if(cache_head.lru_prev == &cache_head)
			Sys_Error("%s: out of memory", __FUNCTION__); // not enough memory at all

		Cache_Free(cache_head.lru_prev->user);
	};

	return Cache_Check(c);
};

/*
============
Cache_TryAlloc

Looks for a free block of memory between the high and low hunk marks
Size should already include the header and padding
============
*/
cache_system_t *Cache_TryAlloc(int size, qboolean nobottom)
{
	cache_system_t *cs;
	cache_system_t *newmem;

	if(!nobottom && cache_head.prev == &cache_head)
	{
		if(hunk_size - hunk_low_used - hunk_high_used < size)
			Sys_Error("%s: %i is greater then free hunk", __FUNCTION__, size);

		newmem = (cache_system_t *)(hunk_base + hunk_low_used);
		Q_memset(newmem, 0, sizeof(cache_system_t));
		newmem->size = size;
		cache_head.next = cache_head.prev = newmem;
		newmem->next = newmem->prev = &cache_head;

		Cache_MakeLRU(newmem);
		return newmem;
	};

	cs = cache_head.next;
	newmem = (cache_system_t *)(hunk_base + hunk_low_used);

	do
	{
		if((!nobottom || cs != cache_head.next) &&
		   (signed int)((char *)cs - (char *)newmem) >= size)
		{
			Q_memset(newmem, 0, sizeof(cache_system_t));

			newmem->size = size;
			newmem->next = cs;
			newmem->prev = cs->prev;

			cs->prev->next = newmem;
			cs->prev = newmem;

			Cache_MakeLRU(newmem);
			return newmem;
		};

		newmem = (cache_system_t *)((char *)cs + cs->size);
		cs = cs->next;
	} while(cs != &cache_head);

	if((int)(hunk_size + hunk_base - hunk_high_used - (byte *)newmem) < size)
		return 0;

	Q_memset(newmem, 0, sizeof(cache_system_t));

	newmem->size = size;
	newmem->next = &cache_head;
	newmem->prev = cache_head.prev;

	cache_head.prev->next = newmem;
	cache_head.prev = newmem;

	Cache_MakeLRU(newmem);
	return newmem;
};

/*
==============
Cache_Free

Frees the memory and removes it from the LRU list
==============
*/
void Cache_Free(cache_user_t *c)
{
	if(!c->data)
		Sys_Error("%s: not allocated", __FUNCTION__);

	cache_system_t *cs = ((cache_system_t *)c->data - 1);

	cs->prev->next = cs->next;
	cs->next->prev = cs->prev;
	c->data = cs->prev = cs->next = 0;

	Cache_UnlinkLRU(cs);
};

/*
============
Cache_FreeLow

Throw things out until the hunk can be expanded to the given point
============
*/
void Cache_FreeLow(int new_low_hunk)
{
	cache_system_t *c;

	while(true)
	{
		c = cache_head.next;
		if(c == &cache_head)
			return; // nothing in cache at all
		if((byte *)c >= hunk_base + new_low_hunk)
			return;    // there is space to grow the hunk
		Cache_Move(c); // reclaim the space
	};
};

/*
============
Cache_FreeHigh

Throw things out until the hunk can be expanded to the given point
============
*/
void Cache_FreeHigh(int new_high_hunk)
{
	cache_system_t *c, *prev = NULL;

	while(true)
	{
		c = cache_head.prev;
		if(c == &cache_head)
			return; // nothing in cache at all
		if((byte *)c + c->size <= hunk_base + hunk_size - new_high_hunk)
			return; // there is space to grow the hunk
		if(c == prev)
			Cache_Free(c->user); // didn't move out of the way
		else
		{
			Cache_Move(c); // try to move it
			prev = c;
		};
	};
};

/*
===========
Cache_Move
===========
*/
void Cache_Move(cache_system_t *c)
{
	cache_system_t *newmem = Cache_TryAlloc(c->size, 1);

	if(!newmem)
		Cache_Free(c->user);
	else
	{
		Q_memcpy(newmem + 1, c + 1, c->size - sizeof(cache_system_t));
		newmem->user = c->user;
		Q_memcpy(newmem->name, c->name, sizeof(newmem->name));
		Cache_Free(c->user);
		newmem->user->data = (void *)(newmem + 1);
	};
};

void Cache_MakeLRU(cache_system_t *cs)
{
	if(cs->lru_next || cs->lru_prev)
		Sys_Error("%s: active link", __FUNCTION__);

	cache_head.lru_next->lru_prev = cs;

	cs->lru_next = cache_head.lru_next;
	cs->lru_prev = &cache_head;

	cache_head.lru_next = cs;
};

void Cache_UnlinkLRU(cache_system_t *cs)
{
	if(!cs->lru_next || !cs->lru_prev)
		Sys_Error("%s: NULL link", __FUNCTION__);

	cs->lru_next->lru_prev = cs->lru_prev;
	cs->lru_prev->lru_next = cs->lru_next;
	cs->lru_next = cs->lru_prev = 0;
};

/*
============
Cache_Flush

Throw everything out, so new data will be demand cached
============
*/
void Cache_Flush()
{
	if(cl.maxclients <= 1 || allow_cheats)
		Cache_Force_Flush();
	else
		Con_Printf("Server must enable sv_cheats to activate the flush command in "
		           "multiplayer games.\n");
};

void Cache_Force_Flush()
{
	for(cache_system_t *i = cache_head.next; cache_head.next != &cache_head; i = cache_head.next)
		Cache_Free(i->user);
};

/*
============
ComparePath1

compares the first directory of two paths...
(so  "foo/bar" will match "foo/fred"
============
*/
NOXREF int ComparePath1(char *path1, char *path2)
{
	NOXREFCHECK;
	while(*path1 != '/' && *path1 != '\\' && *path1)
	{
		if(*path1 != *path2)
			return 0;
		else
		{
			path1++;
			path2++;
		};
	};

	return 1;
};

/*
============
CacheSystemCompare

Compares the names of two cache_system_t structs.
Used with qsort()
============
*/
NOXREF int CacheSystemCompare(const void *ppcs1, const void *ppcs2)
{
	NOXREFCHECK;

	cache_system_t *pcs1 = (cache_system_t *)ppcs1;
	cache_system_t *pcs2 = (cache_system_t *)ppcs2;

	return Q_stricmp(pcs1->name, pcs2->name);
};

/*
============
CommatizeNumber

takes a number, and creates a string of that with commas in the
appropriate places.
============
*/
NOXREF char *CommatizeNumber(int num, char *pout)
{
	NOXREFCHECK;

	// this is probably more complex than it needs to be
	int len = 0;
	int i;
	char outbuf[50];

	Q_memset(outbuf, 0, sizeof(outbuf));

	while(num)
	{
		char tempbuf[50];
		int temp = num % 1000;
		num = num / 1000;
		Q_strcpy(tempbuf, outbuf);

		Q_snprintf(outbuf, sizeof(outbuf), ",%03i%s", temp, tempbuf);
	};

	len = Q_strlen(outbuf);

	for(i = 0; i < len; i++) // find first significant digit
		if(outbuf[i] != '0' && outbuf[i] != ',')
			break;

	if(i == len)
		Q_strcpy(pout, "0");
	else
		Q_strcpy(pout, &outbuf[i]); // copy from i to get rid of the first comma and
	                                // leading zeros

	return pout;
};

/*
==============
Cache_Check
==============
*/
void *EXT_FUNC Cache_Check(cache_user_t *c)
{
	if(c->data)
	{
		cache_system_t *cs =
		(cache_system_t *)((char *)c->data - sizeof(cache_system_t));

		Cache_UnlinkLRU((cache_system_t *)c->data - 1);
		Cache_MakeLRU(cs);
	};

	return c->data;
};

/*
============
Cache_Print
============
*/
NOXREF void Cache_Print()
{
	NOXREFCHECK;

	for(cache_system_t *cd = cache_head.next; cd != &cache_head; cd = cd->next)
		Con_Printf("%8i : %s\n", cd->size, cd->name);
};

/*
============
Cache_Report
============
*/
NOXREF void Cache_Report()
{
	NOXREFCHECK;
	Con_DPrintf("%4.1f megabyte data cache\n",
	            (hunk_size - hunk_low_used - hunk_high_used) /
	            (float)(1024 * 1024));
};

/*
============
Cache_Compact
============
*/
NOXREF void Cache_Compact()
{
	NOXREFCHECK;
};

NOXREF int Cache_TotalUsed()
{
	NOXREFCHECK;

	cache_system_t *cd;
	int Total = 0;

	for(cd = cache_head.next; cd != &cache_head; cd = cd->next)
		Total += cd->size;

	return Total;
};

NOXREF void Cache_Print_Models_And_Totals()
{
	char buf[50];
	cache_system_t *cd;
	cache_system_t *sortarray[512];
	int32 i = 0;
	int32 j = 0;
	int32 totalbytes = 0;
	FileHandle_t file;

	file = FS_Open(mem_dbgfile.string, "a");
	if(!file)
		return;

	Q_memset(sortarray, 0, sizeof(cache_system_t *) * 512);

	// pack names into the array
	for(cd = cache_head.next; cd != &cache_head; cd = cd->next)
	{
		if(Q_strstr(cd->name, ".mdl"))
			sortarray[i++] = cd;
	};

	qsort(sortarray, i, sizeof(cache_system_t *), CacheSystemCompare);
	FS_FPrintf(file, "\nCACHED MODELS:\n");

	// now process the sorted list
	for(j = 0; j < i; ++j)
	{
		FS_FPrintf(file, "\t%16.16s : %s\n", CommatizeNumber(sortarray[j]->size, buf), sortarray[j]->name);
		totalbytes += sortarray[j]->size;
	};

	FS_FPrintf(file, "Total bytes in cache used by models:  %s\n", CommatizeNumber(totalbytes, buf));
	FS_Close(file);
};

NOXREF void Cache_Print_Sounds_And_Totals()
{
	char buf[50];
	cache_system_t *cd;
	cache_system_t *sortarray[1024];
	int32 i = 0;
	int32 j = 0;
	int32 totalsndbytes = 0;
	FileHandle_t file;
	int subtot = 0;

	file = FS_Open(mem_dbgfile.string, "a");
	if(!file)
		return;

	Q_memset(sortarray, 0, sizeof(cache_system_t *) * MAX_SFX);

	// pack names into the array.
	for(cd = cache_head.next; cd != &cache_head; cd = cd->next)
	{
		if(Q_strstr(cd->name, ".wav"))
			sortarray[i++] = cd;
	};

	qsort(sortarray, i, sizeof(cache_system_t *), CacheSystemCompare);
	FS_FPrintf(file, "\nCACHED SOUNDS:\n");

	// now process the sorted list.  (totals by directory)
	for(j = 0; j < i; ++j)
	{
		FS_FPrintf(file, "\t%16.16s : %s\n", CommatizeNumber(sortarray[j]->size, buf), sortarray[j]->name);
		totalsndbytes += sortarray[j]->size;

		if((j + 1) == i ||
		   ComparePath1(sortarray[j]->name, sortarray[j + 1]->name) == 0)
		{
			char pathbuf[512];
			Sys_SplitPath(sortarray[j]->name, NULL, pathbuf, NULL, NULL);
			FS_FPrintf(file, "\tTotal Bytes used in \"%s\": %s\n", pathbuf, CommatizeNumber(totalsndbytes - subtot, buf));
			subtot = totalsndbytes;
		};
	};

	FS_FPrintf(file, "Total bytes in cache used by sound:  %s\n", CommatizeNumber(totalsndbytes, buf));
	FS_Close(file);
};