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

#include "precompiled.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
//#include <cstdlib>
//#include <cstring>

/*
========================
Memory_Init
========================
*/
void Memory_Init(void *buf, int size) // moved here from zone.cpp
{
	int zonesize = ZONE_DYNAMIC_SIZE;

	hunk_base = (byte *)buf;
	hunk_size = size;
	hunk_low_used = 0;
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

void *Mem_Malloc(size_t size)
{
	return malloc(size);
};

void *Mem_ZeroMalloc(size_t size)
{
	void *p = malloc(size);
	Q_memset(p, 0, size);
	return p;
};

void *Mem_Realloc(void *memblock, size_t size)
{
	return realloc(memblock, size);
};

void *EXT_FUNC Mem_Calloc(int num, size_t size) // (size_t nmemb, size_t size)
{
	return calloc(num, size);
};

char *Mem_Strdup(const char *strSource)
{
	return _strdup(strSource);
};

void Mem_Free(void *p)
{
	free(p);
};