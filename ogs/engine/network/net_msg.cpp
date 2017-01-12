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

//#include "precompiled/hpp"
#include "network/net_msg.hpp"
#include "system/common.hpp"
#include "system/sizebuf.hpp"
#include "system/systemtypes.hpp"
#include "common/usercmd.h"

// MESSAGE IO FUNCTIONS
// Handles byte ordering and avoids alignment errors

int msg_badread;
int msg_readcount;

// Some bit tables...
const uint32 BITTABLE[] =
    {
        0x00000001, 0x00000002, 0x00000004, 0x00000008,
        0x00000010, 0x00000020, 0x00000040, 0x00000080,
        0x00000100, 0x00000200, 0x00000400, 0x00000800,
        0x00001000, 0x00002000, 0x00004000, 0x00008000,
        0x00010000, 0x00020000, 0x00040000, 0x00080000,
        0x00100000, 0x00200000, 0x00400000, 0x00800000,
        0x01000000, 0x02000000, 0x04000000, 0x08000000,
        0x10000000, 0x20000000, 0x40000000, 0x80000000,
        0x00000000,
};

const uint32 ROWBITTABLE[] =
    {
        0x00000000, 0x00000001, 0x00000003, 0x00000007,
        0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
        0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
        0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
        0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
        0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
        0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
        0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
        0xFFFFFFFF,
};

const uint32 INVBITTABLE[] =
    {
        0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7,
        0xFFFFFFEF, 0xFFFFFFDF, 0xFFFFFFBF, 0xFFFFFF7F,
        0xFFFFFEFF, 0xFFFFFDFF, 0xFFFFFBFF, 0xFFFFF7FF,
        0xFFFFEFFF, 0xFFFFDFFF, 0xFFFFBFFF, 0xFFFF7FFF,
        0xFFFEFFFF, 0xFFFDFFFF, 0xFFFBFFFF, 0xFFF7FFFF,
        0xFFEFFFFF, 0xFFDFFFFF, 0xFFBFFFFF, 0xFF7FFFFF,
        0xFEFFFFFF, 0xFDFFFFFF, 0xFBFFFFFF, 0xF7FFFFFF,
        0xEFFFFFFF, 0xDFFFFFFF, 0xBFFFFFFF, 0x7FFFFFFF,
        0xFFFFFFFF,
};

void MSG_WriteChar(sizebuf_t *sb, int c)
{
	unsigned char *buf = (unsigned char *)SZ_GetSpace(sb, 1);
	*(char *)buf       = (char)c;
}

void MSG_WriteByte(sizebuf_t *sb, int c)
{
	unsigned char *buf = (unsigned char *)SZ_GetSpace(sb, 1);
	*(byte *)buf       = (byte)c;
}

void MSG_WriteShort(sizebuf_t *sb, int c)
{
	unsigned char *buf = (unsigned char *)SZ_GetSpace(sb, 2);
	*(int16 *)buf      = (int16)c;
}

void MSG_WriteWord(sizebuf_t *sb, int c)
{
	unsigned char *buf = (unsigned char *)SZ_GetSpace(sb, 2);
	*(uint16 *)buf     = (uint16)c;
}

void MSG_WriteLong(sizebuf_t *sb, int c)
{
	unsigned char *buf = (unsigned char *)SZ_GetSpace(sb, 4);
	*(uint32 *)buf     = (uint32)c;
}

void MSG_WriteFloat(sizebuf_t *sb, float f)
{
	int i = LittleLong(*(int *)&f);
	SZ_Write(sb, &i, 4);
}

void MSG_WriteString(sizebuf_t *sb, const char *s)
{
	if(s)
	{
		SZ_Write(sb, s, Q_strlen(s) + 1);
	}
	else
	{
		SZ_Write(sb, "", 1);
	}
}

void MSG_WriteBuf(sizebuf_t *sb, int iSize, void *buf)
{
	if(buf)
	{
		SZ_Write(sb, buf, iSize);
	}
}

void MSG_WriteAngle(sizebuf_t *sb, float f)
{
	MSG_WriteByte(sb, (int64)(fmod((double)f, 360.0) * 256.0 / 360.0) & 0xFF);
}

void MSG_WriteHiresAngle(sizebuf_t *sb, float f)
{
	MSG_WriteShort(sb, (int64)(fmod((double)f, 360.0) * 65536.0 / 360.0) & 0xFFFF);
}

void MSG_WriteUsercmd(sizebuf_t *buf, usercmd_t *to, usercmd_t *from)
{
	delta_t **ppdesc;

	ppdesc = DELTA_LookupRegistration("usercmd_t");
	MSG_StartBitWriting(buf);
	DELTA_WriteDelta((byte *)from, (byte *)to, 1, *ppdesc, 0);
	MSG_EndBitWriting(buf);
}

typedef struct bf_write_s
{
// For enhanced and safe bits writing functions
#if defined(REHLDS_FIXES)

#pragma pack(push, 1)
	union
	{
		uint64 u64;
		uint32 u32[2];
		uint8  u8[8];
	} pendingData;
	uint64 sse_highbits;
#pragma pack(pop)

	int        nCurOutputBit;
	sizebuf_t *pbuf;

#else // defined(REHLDS_FIXES)

	int            nCurOutputBit;
	unsigned char *pOutByte;
	sizebuf_t *    pbuf;

#endif // defined(REHLDS_FIXES)
} bf_write_t;

typedef struct bf_read_s
{
	int            nMsgReadCount; // was msg_readcount
	sizebuf_t *    pbuf;
	int            nBitFieldReadStartByte;
	int            nBytesRead;
	int            nCurInputBit;
	unsigned char *pInByte;
} bf_read_t;

// Bit field reading/writing storage.
bf_read_t bfread;
ALIGN16 bf_write_t bfwrite;

void COM_BitOpsInit()
{
	Q_memset(&bfwrite, 0, sizeof(bf_write_t));
	Q_memset(&bfread, 0, sizeof(bf_read_t));
}

// Enhanced and safe bits writing functions
#if defined(REHLDS_FIXES)

void MSG_WBits_MaybeFlush()
{
	if(bfwrite.nCurOutputBit < 32)
		return;

	uint32 *pDest = (uint32 *)SZ_GetSpace(bfwrite.pbuf, 4);
	if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
		*pDest = bfwrite.pendingData.u32[0];

	bfwrite.pendingData.u32[0] = bfwrite.pendingData.u32[1];
	bfwrite.pendingData.u32[1] = 0;
	bfwrite.nCurOutputBit -= 32;
}

void MSG_WriteBits(uint32 data, int numbits)
{
	uint32 maxval = _mm_cvtsi128_si32(_mm_slli_epi64(_mm_cvtsi32_si128(1), numbits)) - 1; //maxval = (1 << numbits) - 1
	if(data > maxval)
		data = maxval;

	MSG_WBits_MaybeFlush();

	__m128i pending = _mm_load_si128((__m128i *)&bfwrite.pendingData.u64);

	__m128i mmdata = _mm_slli_epi64(_mm_cvtsi32_si128(data), bfwrite.nCurOutputBit); //mmdata = data << bfwrite.nCurOutputBit
	pending        = _mm_or_si128(pending, mmdata);

	_mm_store_si128((__m128i *)&bfwrite.pendingData.u64, pending);
	bfwrite.nCurOutputBit += numbits;
}

void MSG_WriteOneBit(int nValue)
{
	MSG_WriteBits(nValue, 1);
}

void MSG_StartBitWriting(sizebuf_t *buf)
{
	bfwrite.nCurOutputBit   = 0;
	bfwrite.pbuf            = buf;
	bfwrite.pendingData.u64 = 0;
}

void MSG_EndBitWriting(sizebuf_t *buf)
{
	int bytesNeed = bfwrite.nCurOutputBit / 8;
	if((bfwrite.nCurOutputBit % 8) || bytesNeed == 0)
	{
		bytesNeed++;
	}

	uint8 *pData = (uint8 *)SZ_GetSpace(bfwrite.pbuf, bytesNeed);
	if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
	{
		for(int i = 0; i < bytesNeed; i++)
		{
			pData[i] = bfwrite.pendingData.u8[i];
		}
	}
}

#else // defined(REHLDS_FIXES)

void MSG_WriteOneBit(int nValue)
{
	if(bfwrite.nCurOutputBit >= 8)
	{
		SZ_GetSpace(bfwrite.pbuf, 1);
		bfwrite.nCurOutputBit = 0;
		++bfwrite.pOutByte;
	}

	if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
	{
		if(nValue)
		{
			*bfwrite.pOutByte |= BITTABLE[bfwrite.nCurOutputBit];
		}
		else
		{
			*bfwrite.pOutByte &= INVBITTABLE[bfwrite.nCurOutputBit * 4];
		}

		bfwrite.nCurOutputBit++;
	}
}

void MSG_StartBitWriting(sizebuf_t *buf)
{
	bfwrite.nCurOutputBit = 0;
	bfwrite.pbuf          = buf;
	bfwrite.pOutByte      = &buf->data[buf->cursize];
}

void MSG_EndBitWriting(sizebuf_t *buf)
{
	if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
	{
		*bfwrite.pOutByte &= 255 >> (8 - bfwrite.nCurOutputBit);
		SZ_GetSpace(bfwrite.pbuf, 1);
		bfwrite.nCurOutputBit = 0;
		bfwrite.pOutByte      = 0;
		bfwrite.pbuf          = 0;
	}
}

void MSG_WriteBits(uint32 data, int numbits)
{
	if(numbits < 32)
	{
		if(data >= (uint32)(1 << numbits))
			data = ROWBITTABLE[numbits];
	}

	int surplusBytes = 0;
	if((uint32)bfwrite.nCurOutputBit >= 8)
	{
		surplusBytes          = 1;
		bfwrite.nCurOutputBit = 0;
		++bfwrite.pOutByte;
	}

	int bits = numbits + bfwrite.nCurOutputBit;
	if(bits <= 32)
	{
		int bytesToWrite = bits >> 3;
		int bitsLeft     = bits & 7;
		if(!bitsLeft)
			--bytesToWrite;
		SZ_GetSpace(bfwrite.pbuf, surplusBytes + bytesToWrite);
		if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
		{
			*(uint32 *)bfwrite.pOutByte = (data << bfwrite.nCurOutputBit) | *(uint32 *)bfwrite.pOutByte & ROWBITTABLE[bfwrite.nCurOutputBit];
			bfwrite.nCurOutputBit       = 8;
			if(bitsLeft)
				bfwrite.nCurOutputBit = bitsLeft;
			bfwrite.pOutByte          = &bfwrite.pOutByte[bytesToWrite];
		}
	}
	else
	{
		SZ_GetSpace(bfwrite.pbuf, surplusBytes + 4);
		if(!(bfwrite.pbuf->flags & SIZEBUF_OVERFLOWED))
		{
			*(uint32 *)bfwrite.pOutByte = (data << bfwrite.nCurOutputBit) | *(uint32 *)bfwrite.pOutByte & ROWBITTABLE[bfwrite.nCurOutputBit];
			int leftBits                = 32 - bfwrite.nCurOutputBit;
			bfwrite.nCurOutputBit       = bits & 7;
			bfwrite.pOutByte += 4;
			*(uint32 *)bfwrite.pOutByte = data >> leftBits;
		}
	}
}

#endif //defined(REHLDS_FIXES)

NOXREF qboolean MSG_IsBitWriting()
{
	NOXREFCHECK;

	return bfwrite.pbuf != 0;
}

void MSG_WriteSBits(int data, int numbits)
{
	int idata = data;

	if(numbits < 32)
	{
		int maxnum = (1 << (numbits - 1)) - 1;

		if(data > maxnum || (maxnum = -maxnum, data < maxnum))
		{
			idata = maxnum;
		}
	}

	int sigbits = idata < 0;

	MSG_WriteOneBit(sigbits);
	MSG_WriteBits(abs(idata), numbits - 1);
}

void MSG_WriteBitString(const char *p)
{
#ifdef REHLDS_FIXES
	const uint8_t *pch = (uint8_t *)p;
#else  // REHLDS_FIXES
	char *pch = (char *)p;
#endif // REHLDS_FIXES

	while(*pch)
	{
		MSG_WriteBits(*pch, 8);
		++pch;
	}

	MSG_WriteBits(0, 8);
}

void MSG_WriteBitData(void *src, int length)
{
	byte *p = (byte *)src;

	for(int i = 0; i < length; i++, p++)
		MSG_WriteBits(*p, 8);
}

void MSG_WriteBitAngle(float fAngle, int numbits)
{
	if(numbits >= 32)
		Sys_Error("%s: Can't write bit angle with 32 bits precision\n", __FUNCTION__);

	uint32 shift = (1 << numbits);
	uint32 mask  = shift - 1;

	int d = (int)(shift * fmod((double)fAngle, 360.0)) / 360;
	d &= mask;

	MSG_WriteBits(d, numbits);
}

float MSG_ReadBitAngle(int numbits)
{
	return (float)(MSG_ReadBits(numbits) * (360.0 / (1 << numbits)));
}

int MSG_CurrentBit()
{
	int nbits = 8 * msg_readcount;

	if(bfread.pbuf)
		nbits = bfread.nCurInputBit + 8 * bfread.nBytesRead;

	return nbits;
};

NOXREF qboolean MSG_IsBitReading()
{
	NOXREFCHECK;

	return bfread.pbuf != 0;
};

void MSG_StartBitReading(sizebuf_t *buf)
{
	bfread.nCurInputBit           = 0;
	bfread.nBytesRead             = 0;
	bfread.nBitFieldReadStartByte = msg_readcount;
	bfread.pbuf                   = buf;
	bfread.pInByte                = &buf->data[msg_readcount];
	bfread.nMsgReadCount          = msg_readcount + 1;

	if(msg_readcount + 1 > buf->cursize)
		msg_badread = 1;
};

void MSG_EndBitReading(sizebuf_t *buf)
{
	if(bfread.nMsgReadCount > buf->cursize)
		msg_badread = 1;

	msg_readcount                 = bfread.nMsgReadCount;
	bfread.nBitFieldReadStartByte = 0;
	bfread.nCurInputBit           = 0;
	bfread.nBytesRead             = 0;
	bfread.pInByte                = 0;
	bfread.pbuf                   = 0;
}

int MSG_ReadOneBit()
{
	int nValue;

	if(msg_badread)
	{
		nValue = 1;
	}
	else
	{
		if(bfread.nCurInputBit >= 8)
		{
			++bfread.nMsgReadCount;
			bfread.nCurInputBit = 0;
			++bfread.nBytesRead;
			++bfread.pInByte;
		}

		if(bfread.nMsgReadCount <= bfread.pbuf->cursize)
		{
			nValue = (*bfread.pInByte & BITTABLE[bfread.nCurInputBit]) != 0;
			++bfread.nCurInputBit;
		}
		else
		{
			nValue      = 1;
			msg_badread = 1;
		}
	}

	return nValue;
}

uint32 MSG_ReadBits(int numbits)
{
	uint32 result;

#ifdef REHLDS_FIXES
	if(numbits > 32)
		Sys_Error("%s: invalid numbits %d\n", __FUNCTION__, numbits);
#endif // REHLDS_FIXES

	if(msg_badread)
		result = 1;
	else
	{
		if(bfread.nCurInputBit >= 8)
		{
			++bfread.nMsgReadCount;
			++bfread.nBytesRead;
			++bfread.pInByte;

			bfread.nCurInputBit = 0;
		}

		uint32 bits = (bfread.nCurInputBit + numbits) & 7;

		if((unsigned int)(bfread.nCurInputBit + numbits) <= 32)
		{
			result = (*(unsigned int *)bfread.pInByte >> bfread.nCurInputBit) & ROWBITTABLE[numbits];

			uint32 bytes = (bfread.nCurInputBit + numbits) >> 3;

			if(bits)
			{
				bfread.nCurInputBit = bits;
			}
			else
			{
				bfread.nCurInputBit = 8;
				bytes--;
			}

			bfread.pInByte += bytes;
			bfread.nMsgReadCount += bytes;
			bfread.nBytesRead += bytes;
		}
		else
		{
			result              = ((*(unsigned int *)(bfread.pInByte + 4) & ROWBITTABLE[bits]) << (32 - bfread.nCurInputBit)) | (*(unsigned int *)bfread.pInByte >> bfread.nCurInputBit);
			bfread.nCurInputBit = bits;
			bfread.pInByte += 4;
			bfread.nMsgReadCount += 4;
			bfread.nBytesRead += 4;
		}

		if(bfread.nMsgReadCount > bfread.pbuf->cursize)
		{
			result      = 1;
			msg_badread = 1;
		}
	}

	return result;
}

NOXREF uint32 MSG_PeekBits(int numbits)
{
	NOXREFCHECK;

	bf_read_t savebf = bfread;
	uint32    r      = MSG_ReadBits(numbits);
	bfread           = savebf;

	return r;
}

int MSG_ReadSBits(int numbits)
{
	int nSignBit = MSG_ReadOneBit();
	int result   = MSG_ReadBits(numbits - 1);

	if(nSignBit)
		result = -result;

	return result;
}

NOXREF char *MSG_ReadBitString()
{
	NOXREFCHECK;

	static char buf[8192];

	char *p = &buf[0];

	for(char c = MSG_ReadBits(8); c; c = MSG_ReadBits(8))
	{
#ifdef REHLDS_FIXES
		if(msg_badread) // Prevent infinite cycle if msg_badread
		{
			break;
		}
#endif
		*p++ = c;
	}

	*p = 0;

	return buf;
}

int MSG_ReadBitData(void *dest, int length)
{
	if(length > 0)
	{
		int            i = length;
		unsigned char *p = (unsigned char *)dest;

		do
		{
			*p = (unsigned char)MSG_ReadBits(8);
			p++;
			--i;
		} while(i);
	}

	return length;
}

NOXREF float MSG_ReadBitCoord()
{
	NOXREFCHECK;

	float value = 0;

	int intval   = MSG_ReadOneBit();
	int fractval = MSG_ReadOneBit();

	if(intval || fractval)
	{
		int signbit = MSG_ReadOneBit();

		if(intval)
			intval = MSG_ReadBits(12);

		if(fractval)
			fractval = MSG_ReadBits(3);

		value = (float)(fractval / 8.0 + intval);

		if(signbit)
			value = -value;
	}

	return value;
}

void MSG_WriteBitCoord(const float f)
{
	int signbit  = f <= -0.125;
	int intval   = abs((int32)f);
	int fractval = abs((int32)f * 8) & 7;

	MSG_WriteOneBit(intval);
	MSG_WriteOneBit(fractval);

	if(intval || fractval)
	{
		MSG_WriteOneBit(signbit);
		if(intval)
			MSG_WriteBits(intval, 12);
		if(fractval)
			MSG_WriteBits(fractval, 3);
	}
}

NOXREF void MSG_ReadBitVec3Coord(vec3_t fa)
{
	NOXREFCHECK;

	int xflag = MSG_ReadOneBit();
	int yflag = MSG_ReadOneBit();
	int zflag = MSG_ReadOneBit();

	if(xflag)
		fa[0] = MSG_ReadBitCoord();
	if(yflag)
		fa[1] = MSG_ReadBitCoord();
	if(zflag)
		fa[2] = MSG_ReadBitCoord();
}

void MSG_WriteBitVec3Coord(const vec3_t fa)
{
	bool xflag = fa[0] <= -0.125 || fa[0] >= 0.125;
	bool yflag = fa[1] <= -0.125 || fa[1] >= 0.125;
	bool zflag = fa[2] <= -0.125 || fa[2] >= 0.125;

	MSG_WriteOneBit(xflag);
	MSG_WriteOneBit(yflag);
	MSG_WriteOneBit(zflag);

	if(xflag)
		MSG_WriteBitCoord(fa[0]);
	if(yflag)
		MSG_WriteBitCoord(fa[1]);
	if(zflag)
		MSG_WriteBitCoord(fa[2]);
}

NOXREF float MSG_ReadCoord()
{
	NOXREFCHECK;

	return (float)(MSG_ReadShort() * (1.0 / 8));
}

void MSG_WriteCoord(sizebuf_t *sb, const float f)
{
	MSG_WriteShort(sb, (int)(f * 8.0));
}

NOXREF void MSG_ReadVec3Coord(sizebuf_t *sb, vec3_t fa)
{
	NOXREFCHECK;

	if(MSG_IsBitReading())
	{
		MSG_ReadBitVec3Coord(fa);
	}
	else
	{
		MSG_StartBitReading(sb);
		MSG_ReadBitVec3Coord(fa);
		MSG_EndBitReading(sb);
	}
}

NOXREF void MSG_WriteVec3Coord(sizebuf_t *sb, const vec3_t fa)
{
	NOXREFCHECK;

	MSG_StartBitWriting(sb);
	MSG_WriteBitVec3Coord(fa);
	MSG_EndBitWriting(sb);
}

void MSG_BeginReading()
{
	msg_readcount = 0;
	msg_badread   = 0;
}

int MSG_ReadChar()
{
	int c;

	if(msg_readcount < net_message.cursize)
	{
		c = net_message.data[msg_readcount];
		msg_readcount++;
	}
	else
	{
		msg_badread = 1;
		c           = -1;
	}

	return c;
}

int MSG_ReadByte()
{
	int c;

	if(msg_readcount < net_message.cursize)
	{
		c = net_message.data[msg_readcount];
		msg_readcount++;
	}
	else
	{
		msg_badread = 1;
		c           = -1;
	}

	return c;
}

int MSG_ReadShort()
{
	int c;

	if(msg_readcount + 2 <= net_message.cursize)
	{
		c = *(int16 *)&net_message.data[msg_readcount];
		msg_readcount += 2;
	}
	else
	{
		msg_badread = 1;
		c           = -1;
	}

	return c;
}

NOXREF int MSG_ReadWord()
{
	NOXREFCHECK;

	int c;

	if(msg_readcount + 2 <= net_message.cursize)
	{
		c = *(uint16 *)&net_message.data[msg_readcount];
		msg_readcount += 2;
	}
	else
	{
		msg_badread = 1;
		c           = -1;
	}

	return c;
}

int MSG_ReadLong()
{
	int c;

	if(msg_readcount + 4 <= net_message.cursize)
	{
		c = *(uint32 *)&net_message.data[msg_readcount];
		msg_readcount += 4;
	}
	else
	{
		msg_badread = 1;
		c           = -1;
	}

	return c;
}

NOXREF float MSG_ReadFloat()
{
	NOXREFCHECK;

	float f;

	if(msg_readcount + 4 <= net_message.cursize)
	{
		f = *((float *)LittleLong(*(int *)&net_message.data[msg_readcount]));
		msg_readcount += 4;
	}
	else
	{
		msg_badread = 1;
		f           = -1.0;
	}

	return f;
}

int MSG_ReadBuf(int iSize, void *pbuf)
{
	if(msg_readcount + iSize <= net_message.cursize)
	{
		Q_memcpy(pbuf, &net_message.data[msg_readcount], iSize);
		msg_readcount += iSize;

		return 1;
	}

	msg_badread = 1;
	return -1;
}

char *MSG_ReadString()
{
	int         c = 0, l = 0;
	static char string[8192];

	while((c = MSG_ReadChar(), c) && c != -1 && l < ARRAYSIZE(string) - 1)
	{
		string[l++] = c;
	}
	string[l] = 0;

	return string;
}

char *MSG_ReadStringLine()
{
	int         c = 0, l = 0;
	static char string[2048];

	while((c        = MSG_ReadChar(), c) && c != '\n' && c != -1 && l < ARRAYSIZE(string) - 1)
		string[l++] = c;

	string[l] = 0;

	return string;
};

NOXREF float MSG_ReadAngle()
{
	NOXREFCHECK;

	int c = MSG_ReadChar();
#ifdef REHLDS_FIXES
	if(c == -1) // FIXED: Added check for wrong value, but just return 0 instead of -1 * (360.0 / 256)
	{
		return 0;
	}
#endif
	return (float)(c * (360.0 / 256));
};

NOXREF float MSG_ReadHiresAngle()
{
	NOXREFCHECK;

	int c = MSG_ReadShort();

#ifdef REHLDS_FIXES
	if(c == -1) // FIXED: Added check for wrong value, but just return 0 instead of -1 * (360.0 / 65536)
		return 0;
#endif

	return (float)(MSG_ReadShort() * (360.0 / 65536));
};

void MSG_ReadUsercmd(usercmd_t *to, usercmd_t *from)
{
	MSG_StartBitReading(&net_message);
#ifdef REHLDS_OPT_PEDANTIC
	DELTA_ParseDelta((byte *)from, (byte *)to, g_pusercmddelta);
#else
	DELTA_ParseDelta((byte *)from, (byte *)to, SV_LookupDelta("usercmd_t"));
#endif
	MSG_EndBitReading(&net_message);
	COM_NormalizeAngles(to->viewangles);
};