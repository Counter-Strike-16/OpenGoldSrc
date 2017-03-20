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
/// @brief sound caching

#include "sound/sound.hpp"
#include "system/common.hpp"

int cache_full_cycle;

byte *S_Alloc(int size);

/*
================
ResampleSfx
================
*/
void ResampleSfx(sfx_t *sfx, int inrate, int inwidth, byte *data)
{
	int outcount;
	int srcsample;
	float stepscale;
	int i;
	int sample, samplefrac, fracstep;
	sfxcache_t *sc = (sfxcache_t*)Cache_Check(&sfx->cache);
	
	if(!sc)
		return;

	//stepscale = (float)inrate / shm->speed; // this is usually 0.5, 1, or 2

	outcount = sc->length / stepscale;
	sc->length = outcount;
	if(sc->loopstart != -1)
		sc->loopstart = sc->loopstart / stepscale;

	//sc->speed = shm->speed;
	//if(loadas8bit.value)
		//sc->width = 1;
	//else
		sc->width = inwidth;
	sc->stereo = 0;

	// resample / decimate to the current source rate

	if(stepscale == 1 && inwidth == 1 && sc->width == 1)
	{
		// fast special case
		for(i = 0; i < outcount; i++)
			((signed char *)sc->data)[i] = (int)((unsigned char)(data[i]) - 128);
	}
	else
	{
		// general case
		samplefrac = 0;
		fracstep = stepscale * 256;
		for(i = 0; i < outcount; i++)
		{
			srcsample = samplefrac >> 8;
			samplefrac += fracstep;
			if(inwidth == 2)
				sample = LittleShort(((short *)data)[srcsample]);
			else
				sample = (int)((unsigned char)(data[srcsample]) - 128) << 8;
			if(sc->width == 2)
				((short *)sc->data)[i] = sample;
			else
				((signed char *)sc->data)[i] = sample >> 8;
		}
	}
}

//=============================================================================

