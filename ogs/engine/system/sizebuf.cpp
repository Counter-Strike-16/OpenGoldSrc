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
#include "system/sizebuf.hpp"
#include "console/console.hpp"
#include "maintypes.h"
#include "memory/hunk.hpp"
#include "rehlds/common_rehlds.h"
#include "system/common.hpp"
#include "system/system.hpp"

void SZ_Alloc(const char *name, sizebuf_t *buf, int startsize)
{
	buf->buffername = name;

	if(startsize < 256)
		startsize = 256;

	buf->data = (byte *)Hunk_AllocName(startsize, name);
	buf->maxsize = startsize;
	buf->cursize = 0;
	buf->flags = SIZEBUF_CHECK_OVERFLOW;
}

void SZ_Clear(sizebuf_t *buf)
{
	buf->flags &= ~SIZEBUF_OVERFLOWED;
	buf->cursize = 0;
}

void *EXT_FUNC SZ_GetSpace(sizebuf_t *buf, int length)
{
	void *data;
	const char *buffername = buf->buffername ? buf->buffername : "???";

	if(length < 0)
		Sys_Error("%s: %i negative length on %s", __FUNCTION__, length, buffername);

	if(buf->cursize + length > buf->maxsize)
	{
#ifdef REHLDS_FIXES
		if(!(buf->flags & SIZEBUF_ALLOW_OVERFLOW))
		{
			if(!buf->maxsize)
				Sys_Error("%s: tried to write to an uninitialized sizebuf_t: %s",
				          __FUNCTION__,
				          buffername);
			else if(length > buf->maxsize)
				Sys_Error("%s: %i is > full buffer size on %s", __FUNCTION__, length, buffername);
			else
				Sys_Error("%s: overflow without FSB_ALLOWOVERFLOW set on %s",
				          __FUNCTION__,
				          buffername);
		};

		if(length > buf->maxsize)
			Con_DPrintf("%s: %i is > full buffer size on %s, ignoring", __FUNCTION__, length, buffername);
#else  // REHLDS_FIXES

		if(!(buf->flags & SIZEBUF_ALLOW_OVERFLOW))
		{
			if(!buf->maxsize)
				Sys_Error("%s: Tried to write to an uninitialized sizebuf_t: %s",
				          __FUNCTION__,
				          buffername);
			else
				Sys_Error("%s: overflow without FSB_ALLOWOVERFLOW set on %s",
				          __FUNCTION__,
				          buffername);
		};

		if(length > buf->maxsize)
		{
			if(!(buf->flags & SIZEBUF_ALLOW_OVERFLOW))
				Sys_Error("%s: %i is > full buffer size on %s", __FUNCTION__, length, buffername);

			Con_DPrintf("%s: %i is > full buffer size on %s, ignoring", __FUNCTION__, length, buffername);
		};
#endif // REHLDS_FIXES

		Con_Printf("%s: overflow on %s\n", __FUNCTION__, buffername);

		SZ_Clear(buf);
		buf->flags |= SIZEBUF_OVERFLOWED;
	};

	data = &buf->data[buf->cursize];
	buf->cursize = length + buf->cursize;

	return data;
};

void SZ_Write(sizebuf_t *buf, const void *data, int length)
{
	byte *pData = (byte *)SZ_GetSpace(buf, length);

	if(!(buf->flags & SIZEBUF_OVERFLOWED))
		Q_memcpy(pData, data, length);
};

void SZ_Print(sizebuf_t *buf, const char *data)
{
	int len = Q_strlen(data) + 1;
	byte *pData = (byte *)SZ_GetSpace(buf, len - 1) - 1;

	if(buf->data[buf->cursize - 1])
		pData = (byte *)SZ_GetSpace(buf, len);

	if(!(buf->flags & SIZEBUF_OVERFLOWED))
		Q_memcpy(pData, data, len);
};