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

#pragma once

#include "public/archtypes.h"

typedef struct sizebuf_s sizebuf_t;

typedef struct bf_write_s
{
// For enhanced and safe bits writing functions
#if defined(REHLDS_FIXES)

#pragma pack(push, 1)
	union
	{
		uint64 u64;
		uint32 u32[2];
		uint8 u8[8];
	} pendingData;
	uint64 sse_highbits;
#pragma pack(pop)

	int nCurOutputBit;
	sizebuf_t *pbuf;

#else // defined(REHLDS_FIXES)

	int nCurOutputBit;
	unsigned char *pOutByte;
	sizebuf_t *pbuf;

#endif // defined(REHLDS_FIXES)
} bf_write_t;