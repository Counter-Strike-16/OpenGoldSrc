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

#include "network/net_msg.hpp"

void CNetMsg::StartBitWriting()
{
	MSG_StartBitWriting(mpBuffer);
};

void CNetMsg::EndBitWriting()
{
	MSG_EndBitWriting(mpBuffer);
};

qboolean CNetMsg::IsBitWriting()
{
	return MSG_IsBitWriting();
};

void CNetMsg::WriteOneBit(int nValue)
{
	MSG_WriteOneBit(nValue);
};

void CNetMsg::WriteBits(uint32 data, int numbits)
{
	MSG_WriteBits(data, numbits);
};

void CNetMsg::WriteSBits(int data, int numbits)
{
	MSG_WriteSBits(data, numbits);
};

void CNetMsg::WriteBitString(const char *p)
{
	MSG_WriteBitString(p);
};

void CNetMsg::WriteBitData(void *src, int length)
{
	MSG_WriteBitData(src, length);
};

void CNetMsg::WriteBitAngle(float fAngle, int numbits)
{
	MSG_WriteBitAngle(fAngle, numbits);
};

void CNetMsg::WriteChar(int c)
{
	MSG_WriteChar(mpBuffer, c);
};

void CNetMsg::WriteByte(int c)
{
	MSG_WriteByte(mpBuffer, c);
};

void CNetMsg::WriteShort(int c)
{
	MSG_WriteShort(mpBuffer, c);
};

void CNetMsg::WriteWord(int c)
{
	MSG_WriteWord(mpBuffer, c);
};

void CNetMsg::WriteLong(int c)
{
	MSG_WriteLong(mpBuffer, c);
};

void CNetMsg::WriteFloat(float f)
{
	MSG_WriteFloat(mpBuffer, f);
};

void CNetMsg::WriteString(const char *s)
{
	MSG_WriteString(mpBuffer, s);
};

void CNetMsg::WriteBuf(int iSize, void *buf)
{
	MSG_WriteBuf(mpBuffer, iSize, buf);
};

void CNetMsg::WriteAngle(float f)
{
	MSG_WriteAngle(mpBuffer, f);
};

void CNetMsg::WriteHiresAngle(float f)
{
	MSG_WriteHiresAngle(mpBuffer, f);
};

void CNetMsg::WriteUserCmd(usercmd_t *to, usercmd_t *from)
{
	MSG_WriteUsercmd(mpBuffer, to, from);
};

void CNetMsg::StartBitReading()
{
	MSG_StartBitReading(mpBuffer);
};

void CNetMsg::EndBitReading()
{
	MSG_EndBitReading(mpBuffer);
};

qboolean CNetMsg::IsBitReading()
{
	return MSG_IsBitReading();
};

float CNetMsg::ReadBitAngle(int numbits)
{
	return MSG_ReadBitAngle(numbits);
};

int CNetMsg::CurrentBit()
{
	return MSG_CurrentBit();
};

uint32 CNetMsg::PeekBits(int numbits)
{
	return MSG_PeekBits(numbits);
};

void CNetMsg::WriteBitCoord(const float f)
{
	MSG_WriteBitCoord(f);
};

void CNetMsg::WriteBitVec3Coord(const vec3_t fa)
{
	MSG_WriteBitVec3Coord(fa);
};

void CNetMsg::WriteCoord(const float f)
{
	MSG_WriteCoord(mpBuffer, f);
};

void CNetMsg::WriteVec3Coord(const vec3_t fa)
{
	MSG_WriteVec3Coord(mpBuffer, fa);
};

void CNetMsg::BeginReading()
{
	MSG_BeginReading();
};

int CNetMsg::ReadChar()
{
	return MSG_ReadChar();
};

int CNetMsg::ReadByte()
{
};

int CNetMsg::ReadShort()
{
};

int CNetMsg::ReadWord()
{
};

int CNetMsg::ReadLong()
{
};

float CNetMsg::ReadFloat()
{
};

int CNetMsg::ReadBuf(int iSize, void *pbuf)
{
};

char *CNetMsg::ReadString()
{
};

char *CNetMsg::ReadStringLine()
{
};

float CNetMsg::ReadAngle()
{
};

float CNetMsg::ReadHiresAngle()
{
};

void CNetMsg::ReadUsercmd(usercmd_t *to, usercmd_t *from)
{
};

int CNetMsg::ReadOneBit()
{
};

uint32 CNetMsg::ReadBits(int numbits)
{
};

int CNetMsg::ReadSBits(int numbits)
{
};

char *CNetMsg::ReadBitString()
{
};

int CNetMsg::ReadBitData(void *dest, int length)
{
};

float CNetMsg::ReadCoord()
{
};

float CNetMsg::ReadBitCoord()
{
};

void CNetMsg::ReadBitVec3Coord(vec3_t fa)
{
};

void CNetMsg::ReadVec3Coord(vec3_t fa)
{
};