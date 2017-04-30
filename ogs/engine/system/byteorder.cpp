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
#include "system/byteorder.hpp"
#include "common/mathlib_local.hpp"

/*
============================================================================

BYTE ORDER FUNCTIONS

============================================================================
*/

bool bigendien;

short (*BigShort)(short l);
short (*LittleShort)(short l);

int (*BigLong)(int l);
int (*LittleLong)(int l);

float (*BigFloat)(float l);
float (*LittleFloat)(float l);

int LongSwap(int l)
{
	return bswap(l);
};

int LongNoSwap(int l)
{
	return l;
};

short ShortSwap(short l)
{
	return bswap(l);
};

short ShortNoSwap(short l)
{
	return l;
};

float FloatSwap(float f)
{
	/*union
  {
          float f;
          byte b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];

  return dat2.f;*/
	// unsigned long u = bswap(*(unsigned long *)&f);
	// return *(float *)&u;
	return bswap(f);
};

float FloatNoSwap(float f)
{
	return f;
};