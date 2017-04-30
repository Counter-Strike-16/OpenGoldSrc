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

#include "precompiled.hpp"
#include "system/SizeBuffer.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "console/Console.hpp"
#include "rehlds/common_rehlds.h"

CSizeBuffer *AllocSizeBuf(const char *name, sizebuf_t *buf, int startsize)
{
	CSizeBuffer *pSizeBuffer = nullptr; //new CSizeBuffer(name, buf, startsize);
	return pSizeBuffer;
};

CSizeBuffer::CSizeBuffer(IConsole *apConsole, const char *asName, sizebuf_t *apBuffer, int anStartSize) : mpConsole(apConsole)
{
	mpBuffer->buffername = asName;

	if(anStartSize < 256)
		anStartSize = 256;

	mpBuffer->data = (byte *)Hunk_AllocName(anStartSize, asName);
	mpBuffer->maxsize = anStartSize;
	mpBuffer->cursize = 0;
	mpBuffer->flags = SIZEBUF_CHECK_OVERFLOW;
};

void CSizeBuffer::Clear()
{
	mpBuffer->flags &= ~SIZEBUF_OVERFLOWED;
	mpBuffer->cursize = 0;
};

void *CSizeBuffer::GetSpace(int length)
{
	const char *buffername = mpBuffer->buffername ? mpBuffer->buffername : "???";

	if(length < 0)
		CSystem::Error("%s: %i negative length on %s", __FUNCTION__, length, buffername);

	if(mpBuffer->cursize + length > mpBuffer->maxsize)
	{
#ifdef REHLDS_FIXES
		if(!(mpBuffer->flags & SIZEBUF_ALLOW_OVERFLOW))
		{
			if(!mpBuffer->maxsize)
				CSystem::Error("%s: tried to write to an uninitialized sizebuf_t: %s",
				          __FUNCTION__,
				          buffername);
			else if(length > mpBuffer->maxsize)
				CSystem::Error("%s: %i is > full buffer size on %s", __FUNCTION__, length, buffername);
			else
				CSystem::Error("%s: overflow without FSB_ALLOWOVERFLOW set on %s",
				          __FUNCTION__,
				          buffername);
		};

		if(length > mpBuffer->maxsize)
			mpConsole->DevPrintf("%s: %i is > full buffer size on %s, ignoring", __FUNCTION__, length, buffername);
		
#else  // REHLDS_FIXES

		if(!(mpBuffer->flags & SIZEBUF_ALLOW_OVERFLOW))
		{
			if(!mpBuffer->maxsize)
				CSystem::Error("%s: Tried to write to an uninitialized sizebuf_t: %s",
				          __FUNCTION__,
				          buffername);
			else
				CSystem::Error("%s: overflow without FSB_ALLOWOVERFLOW set on %s",
				          __FUNCTION__,
				          buffername);
		};

		if(length > mpBuffer->maxsize)
		{
			if(!(mpBuffer->flags & SIZEBUF_ALLOW_OVERFLOW))
				CSystem::Error("%s: %i is > full buffer size on %s", __FUNCTION__, length, buffername);

			mpConsole->DevPrintf("%s: %i is > full buffer size on %s, ignoring", __FUNCTION__, length, buffername);
		};
#endif // REHLDS_FIXES

		mpConsole->Printf("%s: overflow on %s\n", __FUNCTION__, buffername);

		Clear();
		mpBuffer->flags |= SIZEBUF_OVERFLOWED;
	};

	void *data = &mpBuffer->data[mpBuffer->cursize];
	mpBuffer->cursize = length + mpBuffer->cursize;

	return data;
};

void CSizeBuffer::Write(const void *data, int length)
{
	byte *pData = (byte *)GetSpace(length);

	if(!(mpBuffer->flags & SIZEBUF_OVERFLOWED))
		Q_memcpy(pData, data, length);
};

void CSizeBuffer::Print(const char *data)
{
	int len = Q_strlen(data) + 1;
	byte *pData = (byte *)GetSpace(len - 1) - 1;

	if(mpBuffer->data[mpBuffer->cursize - 1])
		pData = (byte *)GetSpace(len);

	if(!(mpBuffer->flags & SIZEBUF_OVERFLOWED))
		Q_memcpy(pData, data, len);
};