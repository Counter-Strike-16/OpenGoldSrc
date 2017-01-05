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
#include "console/console.hpp"
#include "system/system.hpp"

#ifndef Hunk_Functions_region

const int HUNK_NAME_LEN = 64;
const int HUNK_SENTINEL = 0x1df001ed;

typedef struct hunk_s
{
	int  sentinel;
	int  size;
	
	char name[HUNK_NAME_LEN];
} hunk_t;

byte *hunk_base;
int   hunk_size;

int hunk_low_used;
int hunk_high_used;

qboolean hunk_tempactive;
int      hunk_tempmark;

/*
==============
Hunk_Check

Run consistency and sentinel trashing checks
==============
*/

void Hunk_Check()
{
	for(hunk_t *h = (hunk_t *)hunk_base; (byte *)h != (hunk_base + hunk_low_used); h = (hunk_t *)((byte *)h + h->size))
	{
		if(h->sentinel != HUNK_SENTINEL)
			Sys_Error("%s: trahsed sentinel", __FUNCTION__);

		if(h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
			Sys_Error("%s: bad size", __FUNCTION__);
	};
};

/*
==============
Hunk_Print

If "all" is specified, every single allocation is printed.
Otherwise, allocations with the same name will be totaled up before printing.
==============
*/
NOXREF void Hunk_Print(qboolean all)
{
	NOXREFCHECK;

	hunk_t *h, *next, *endlow, *starthigh, *endhigh;
	int     count, sum;
	int     totalblocks;
	char    name[HUNK_NAME_LEN];

	name[HUNK_NAME_LEN - 1] = 0;
	
	count                   = 0;
	sum                     = 0;
	totalblocks             = 0;

	h         = (hunk_t *)hunk_base;
	endlow    = (hunk_t *)(hunk_base + hunk_low_used);
	starthigh = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
	endhigh   = (hunk_t *)(hunk_base + hunk_size);

	Con_Printf("          :%8i total hunk size\n", hunk_size);
	Con_Printf("-------------------------\n");

	while(true)
	{
		//
		// skip to the high hunk if done with low hunk
		//
		if(h == endlow)
		{
			Con_Printf("-------------------------\n");
			Con_Printf("          :%8i REMAINING\n", hunk_size - hunk_low_used - hunk_high_used);
			Con_Printf("-------------------------\n");
			h = starthigh;
		};

		//
		// if totally done, break
		//
		if(h == endhigh)
			break;

		//
		// run consistancy checks
		//
		if(h->sentinel != HUNK_SENTINEL)
			Sys_Error("%s: trahsed sentinal", __FUNCTION__);
		if(h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
			Sys_Error("%s: bad size", __FUNCTION__);

		next = (hunk_t *)((byte *)h + h->size);
		count++;
		totalblocks++;
		sum += h->size;

		//
		// print the single block
		//
		Q_memcpy(name, h->name, HUNK_NAME_LEN);
		
		if(all)
			Con_Printf("%8p :%8i %8s\n", h, h->size, name);

		//
		// print the total
		//
		if(next == endlow || next == endhigh || Q_strncmp(h->name, next->name, HUNK_NAME_LEN))
		{
			if(!all)
				Con_Printf("          :%8i %8s (TOTAL)\n", sum, name);
			count = 0;
			sum   = 0;
		};

		h = next;
	};

	Con_Printf("-------------------------\n");
	Con_Printf("%8i total blocks\n", totalblocks);
};

/*
===================
Hunk_AllocName
===================
*/
void *Hunk_AllocName(int size, const char *name)
{
	if(size < 0)
		Sys_Error("%s: bad size: %i", __FUNCTION__, size);

	int totalsize = ((size + 15) & ~15) + sizeof(hunk_t);

	if(hunk_size - hunk_high_used - hunk_low_used < totalsize)
		Sys_Error("%s: failed on %i bytes", __FUNCTION__, totalsize);

	hunk_t *h = (hunk_t *)(hunk_base + hunk_low_used);

	hunk_low_used += totalsize;
	Cache_FreeLow(hunk_low_used);

	Q_memset(h, 0, totalsize);
	h->size     = totalsize;
	h->sentinel = HUNK_SENTINEL;
	Q_strncpy(h->name, name, HUNK_NAME_LEN - 1);
	h->name[HUNK_NAME_LEN - 1] = 0;

	return (void *)(h + 1);
};

void *Hunk_Alloc(int size)
{
	return Hunk_AllocName(size, "unknown");
};

int Hunk_LowMark()
{
	return hunk_low_used;
};

void Hunk_FreeToLowMark(int mark)
{
	if(mark < 0 || mark > hunk_low_used)
		Sys_Error("%s: bad mark %i", __FUNCTION__, mark);

	hunk_low_used = mark;
};

int Hunk_HighMark()
{
	if(hunk_tempactive)
	{
		hunk_tempactive = FALSE;
		Hunk_FreeToHighMark(hunk_tempmark);
	};

	return hunk_high_used;
};

void Hunk_FreeToHighMark(int mark)
{
	if(hunk_tempactive)
	{
		hunk_tempactive = FALSE;
		Hunk_FreeToHighMark(hunk_tempmark);
	};

	if(mark < 0 || mark > hunk_high_used)
		Sys_Error("%s: bad mark %i", __FUNCTION__, mark);

	hunk_high_used = mark;
};

/*
===================
Hunk_HighAllocName
===================
*/
void *Hunk_HighAllocName(int size, const char *name)
{
	if(size < 0)
		Sys_Error("%s: bad size: %i", __FUNCTION__, size);

	if(hunk_tempactive)
	{
		Hunk_FreeToHighMark(hunk_tempmark);
		hunk_tempactive = FALSE;
	};

	size = ((size + 15) & ~15) + sizeof(hunk_t);

	if(hunk_size - hunk_high_used - hunk_low_used < size)
	{
		Con_Printf("%s: failed on %i bytes\n", __FUNCTION__, size);
		return 0;
	};

	hunk_high_used += size;
	Cache_FreeHigh(hunk_high_used);

	hunk_t *h = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
	Q_memset(h, 0, size);

	h->size     = size;
	h->sentinel = HUNK_SENTINEL;
	Q_strncpy(h->name, name, HUNK_NAME_LEN - 1);
	h->name[HUNK_NAME_LEN - 1] = 0;

	return (void *)(h + 1);
};

/*
=================
Hunk_TempAlloc

Return space from the top of the hunk
=================
*/
void *Hunk_TempAlloc(int size)
{
	if(hunk_tempactive)
	{
		Hunk_FreeToHighMark(hunk_tempmark);
		hunk_tempactive = 0;
	};

	hunk_tempmark = Hunk_HighMark();
	
	void *buf = Hunk_HighAllocName((size + 15) & ~15, "temp");
	
	hunk_tempactive = 1;
	return buf;
};

#endif // Hunk_Functions_region