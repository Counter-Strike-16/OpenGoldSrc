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

#include "sound/SoundChannel.hpp"

/*
=================
SND_Spatialize
=================
*/
void CSoundChannel::Spatialize()
{
	vec_t dot;
	vec_t dist;
	vec_t lscale, rscale, scale;
	vec3_t source_vec;
	sfx_t *snd;

	// anything coming from the view entity will allways be full volume
	if(entnum == cl.viewentity)
	{
		leftvol = master_vol;
		rightvol = master_vol;
		return;
	}

	// calculate stereo seperation and distance attenuation

	snd = sfx;
	VectorSubtract(origin, listener_origin, source_vec);

	dist = VectorNormalize(source_vec) * dist_mult;

	dot = DotProduct(listener_right, source_vec);

	if(shm->channels == 1)
	{
		rscale = 1.0;
		lscale = 1.0;
	}
	else
	{
		rscale = 1.0 + dot;
		lscale = 1.0 - dot;
	};

	// add in distance effect
	scale = (1.0 - dist) * rscale;
	rightvol = (int)(master_vol * scale);
	
	if(rightvol < 0)
		rightvol = 0;

	scale = (1.0 - dist) * lscale;
	leftvol = (int)(master_vol * scale);
	
	if(leftvol < 0)
		leftvol = 0;
};