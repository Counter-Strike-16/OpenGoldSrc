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
#include "math/math.hpp"

void COM_NormalizeAngles(vec_t *angles)
{
	for(int i = 0; i < 3; i++)
	{
		if(angles[i] > 180.0)
			angles[i] = (float)(fmod((double)angles[i], 360.0) - 360.0);
		else if(angles[i] < -180.0)
			angles[i] = (float)(fmod((double)angles[i], 360.0) + 360.0);
	};
};

int32 EXT_FUNC Math_RandomLong(int32 lLow, int32 lHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomLong(&lLow, &lHigh);
#endif

	unsigned long maxAcceptable;
	unsigned long x = lHigh - lLow + 1;
	unsigned long n;
	
	if(x <= 0 || MAX_RANDOM_RANGE < x - 1)
		return lLow;

	// The following maps a uniform distribution on the interval
	// [0,MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0,x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case
	// x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	//
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);
	
	do
	{
		n = ran1();
	}
	while(n > maxAcceptable);

	return lLow + (n % x);
};

float EXT_FUNC Math_RandomFloat(float flLow, float flHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomFloat(&flLow, &flHigh);
#endif

	float fl = fran1();                     // float in [0,1)
	return (fl * (flHigh - flLow)) + flLow; // float in [low,high)
};