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

#pragma once

#include "maintypes.h"
#include "common/commontypes.h"
#include "public/archtypes.h"

typedef struct sizebuf_s sizebuf_t;
typedef struct usercmd_s usercmd_t;

void COM_BitOpsInit();

void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteByte(sizebuf_t *sb, int c);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteWord(sizebuf_t *sb, int c);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void MSG_WriteBuf(sizebuf_t *sb, int iSize, void *buf);
void MSG_WriteAngle(sizebuf_t *sb, float f);
void MSG_WriteHiresAngle(sizebuf_t *sb, float f);
void MSG_WriteUsercmd(sizebuf_t *buf, usercmd_t *to, usercmd_t *from);

void MSG_StartBitWriting(sizebuf_t *buf);
void MSG_EndBitWriting(sizebuf_t *buf);

NOXREF qboolean MSG_IsBitWriting();

void MSG_WriteOneBit(int nValue);
void MSG_WriteBits(uint32 data, int numbits);
void MSG_WriteSBits(int data, int numbits);
void MSG_WriteBitString(const char *p);
void MSG_WriteBitData(void *src, int length);
void MSG_WriteBitAngle(float fAngle, int numbits);

////////////////////////////////////////////////////////////

void MSG_StartBitReading(sizebuf_t *buf);
void MSG_EndBitReading(sizebuf_t *buf);

NOXREF qboolean MSG_IsBitReading();

float MSG_ReadBitAngle(int numbits);
int MSG_CurrentBit();

NOXREF uint32 MSG_PeekBits(int numbits);
void MSG_WriteBitCoord(const float f);
void MSG_WriteBitVec3Coord(const vec3_t fa);
void MSG_WriteCoord(sizebuf_t *sb, const float f);
NOXREF void MSG_WriteVec3Coord(sizebuf_t *sb, const vec3_t fa);

void MSG_BeginReading();

int          MSG_ReadChar();
int          MSG_ReadByte();
int          MSG_ReadShort();
NOXREF int   MSG_ReadWord();
int          MSG_ReadLong();
NOXREF float MSG_ReadFloat();
int MSG_ReadBuf(int iSize, void *pbuf);
char *       MSG_ReadString();
char *       MSG_ReadStringLine();
NOXREF float MSG_ReadAngle();
NOXREF float MSG_ReadHiresAngle();
void MSG_ReadUsercmd(usercmd_t *to, usercmd_t *from);
int    MSG_ReadOneBit();
uint32 MSG_ReadBits(int numbits);
int MSG_ReadSBits(int numbits);
NOXREF char *MSG_ReadBitString();
int MSG_ReadBitData(void *dest, int length);
NOXREF float MSG_ReadCoord();
NOXREF float MSG_ReadBitCoord();
NOXREF void MSG_ReadBitVec3Coord(vec3_t fa);
NOXREF void MSG_ReadVec3Coord(sizebuf_t *sb, vec3_t fa);