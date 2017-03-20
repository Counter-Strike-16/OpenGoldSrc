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
#include "sound/SoundChannel.hpp"

void CSoundChannel::Update()
{
	// don't adjust volume too fast
	if(chan->master_vol < vol)
	{
		chan->master_vol += host_frametime * ambient_fade.value;
		
		if(chan->master_vol > vol)
			chan->master_vol = vol;
	}
	else if(chan->master_vol > vol)
	{
		chan->master_vol -= host_frametime * ambient_fade.value;
		
		if(chan->master_vol < vol)
			chan->master_vol = vol;
	}

	chan->leftvol = chan->rightvol = chan->master_vol;
};

/*
=================
SND_Spatialize
=================
*/
void CSoundChannel::Spatialize()
{
	vec_t lscale, rscale, scale;
	vec3_t source_vec;

	// anything coming from the view entity will always be full volume
	if(entnum == cl.viewentity)
	{
		leftvol = master_vol;
		rightvol = master_vol;
		return;
	};

	// calculate stereo separation and distance attenuation

	sfx_t *snd = sfx;
	VectorSubtract(origin, listener_origin, source_vec);

	vec_t dist = VectorNormalize(source_vec) * dist_mult;

	vec_t dot = DotProduct(listener_right, source_vec);

	if(shm->channels == 1)
	{
		rscale = 1.0f;
		lscale = 1.0f;
	}
	else
	{
		rscale = 1.0f + dot;
		lscale = 1.0f - dot;
	};

	// add in distance effect
	scale = (1.0f - dist) * rscale;
	rightvol = (int)(master_vol * scale);
	
	if(rightvol < 0)
		rightvol = 0;

	scale = (1.0f - dist) * lscale;
	leftvol = (int)(master_vol * scale);
	
	if(leftvol < 0)
		leftvol = 0;
};

// =======================================================================
// Start a sound effect
// =======================================================================
void CSoundChannel::StartSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation)
{
	channel_t *check;
	int ch_idx;
	int skip;

	if(!sfx)
		return;

	int vol = fvol * 255;

	// spatialize
	//memset(this, 0, sizeof(*this));
	
	VectorCopy(origin, this->origin);
	dist_mult = attenuation / sound_nominal_clip_dist;
	master_vol = vol;
	this->entnum = entnum;
	this->entchannel = entchannel;
	
	Spatialize();
	
	// not audible at all
	if(!leftvol && !rightvol)
		return;

	// new channel
	sfxcache_t *sc = S_LoadSound(sfx);
	
	if(!sc)
	{
		this->sfx = NULL;
		return; // couldn't load the sound's data
	};

	this->sfx = sfx;
	pos = 0.0f;
	end = paintedtime + sc->length;

	// if an identical sound has also been started this frame, offset the pos
	// a bit to keep it from just making the first one louder
	check = &channels[NUM_AMBIENTS];
	for(ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS;
	    ch_idx++, check++)
	{
		if(check == target_chan)
			continue;
		
		if(check->sfx == sfx && !check->pos)
		{
			skip = rand() % (int)(0.1 * shm->speed);
			if(skip >= target_chan->end)
				skip = target_chan->end - 1;
			target_chan->pos += skip;
			target_chan->end -= skip;
			break;
		};
	};
};

/*
=================
S_StaticSound
=================
*/
void CSoundChannel::StartStaticSound(sfx_t *sfx, vec3_t origin, float vol, float attenuation)
{
	if(!sfx)
		return;
	
	sfxcache_t *sc;
	
	//sc = S_LoadSound(sfx);
	
	if(!sc)
		return;

	if(sc->loopstart == -1)
	{
		Con_Printf("Sound %s not looped\n", sfx->name);
		return;
	};

	this->sfx = sfx;
	VectorCopy(origin, this->origin);
	master_vol = vol;
	dist_mult = (attenuation / 64) / sound_nominal_clip_dist;
	end = paintedtime + sc->length;

	Spatialize();
};

void CSoundChannel::Stop()
{
	end = 0;
	sfx = NULL;
};