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
/// @brief main control for any streaming sound output device

#include "precompiled.hpp"
//#include "commondef.hpp"
#include "common/commontypes.h"
#include "sound/sound.hpp"
#include "memory/zone.hpp"
#include "system/system.hpp"
#include "system/common.hpp"
#include "system/host.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "client/client.hpp"
#include "common/cvardef.h"

#ifdef _WIN32
//#include "winquake.hpp"
#endif

void S_Play();
void S_PlayVol();
void S_SoundList();
void S_Update_();
void S_StopAllSounds(qboolean clear);
void S_StopAllSoundsC();

// QuakeWorld hack...
#define viewentity playernum + 1

// =======================================================================
// Internal sound data & structures
// =======================================================================

channel_t channels[MAX_CHANNELS];
int total_channels;

// pointer should go away
volatile dma_t *shm = 0;
volatile dma_t sn;

vec_t sound_nominal_clip_dist = 1000.0f;

int soundtime;   // sample PAIRS
int paintedtime; // sample PAIRS

const int MAX_SFX = 512;

sfx_t *ambient_sfx[NUM_AMBIENTS];

int desired_speed = 11025;
int desired_bits = 16;

cvar_t bgmvolume = { "bgmvolume", "1", FCVAR_ARCHIVE };
cvar_t volume = { "volume", "0.7", FCVAR_ARCHIVE };

cvar_t nosound = { "nosound", "0" };
cvar_t precache = { "precache", "1" };
cvar_t loadas8bit = { "loadas8bit", "0" };
cvar_t bgmbuffer = { "bgmbuffer", "4096" };
cvar_t ambient_level = { "ambient_level", "0.3" };
cvar_t ambient_fade = { "ambient_fade", "100" };
cvar_t snd_noextraupdate = { "snd_noextraupdate", "0" };
cvar_t snd_show = { "snd_show", "0" };
cvar_t _snd_mixahead = { "_snd_mixahead", "0.1", FCVAR_ARCHIVE };

// ====================================================================
// User-setable variables
// ====================================================================

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

qboolean fakedma = false;
int fakedma_updates = 15;

void S_SoundInfo_f()
{
	if(!sound_started || !shm)
	{
		Con_Printf("sound system not started\n");
		return;
	}

	Con_Printf("%5d stereo\n", shm->channels - 1);
	Con_Printf("%5d samples\n", shm->samples);
	Con_Printf("%5d samplepos\n", shm->samplepos);
	Con_Printf("%5d samplebits\n", shm->samplebits);
	Con_Printf("%5d submission_chunk\n", shm->submission_chunk);
	Con_Printf("%5d speed\n", shm->speed);
	Con_Printf("0x%x dma buffer\n", shm->buffer);
	Con_Printf("%5d total_channels\n", total_channels);
}

// =======================================================================
// Load a sound
// =======================================================================

/*
==================
S_TouchSound

==================
*/
void S_TouchSound(char *name)
{
	sfx_t *sfx;

	if(!sound_started)
		return;

	sfx = S_FindName(name);
	Cache_Check(&sfx->cache);
}



//=============================================================================

/*
=================
SND_PickChannel
=================
*/
channel_t *SND_PickChannel(int entnum, int entchannel)
{
	int ch_idx;
	int first_to_die;
	int life_left;

	// Check for replacement sound, or find the best one to replace
	first_to_die = -1;
	life_left = 0x7fffffff;
	for(ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS;
	    ch_idx++)
	{
		if(entchannel != 0 // channel 0 never overrides
		   &&
		   channels[ch_idx].entnum == entnum &&
		   (channels[ch_idx].entchannel == entchannel ||
		    entchannel == -1))
		{ // allways override sound from same entity
			first_to_die = ch_idx;
			break;
		}

		// don't let monster sounds override player sounds
		if(channels[ch_idx].entnum == cl.viewentity && entnum != cl.viewentity &&
		   channels[ch_idx].sfx)
			continue;

		if(channels[ch_idx].end - paintedtime < life_left)
		{
			life_left = channels[ch_idx].end - paintedtime;
			first_to_die = ch_idx;
		}
	}

	if(first_to_die == -1)
		return NULL;

	if(channels[first_to_die].sfx)
		channels[first_to_die].sfx = NULL;

	return &channels[first_to_die];
}

// =======================================================================
// Start a sound effect
// =======================================================================

void S_StartSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation)
{
	channel_t *target_chan, *check;
	sfxcache_t *sc;
	int vol;
	int ch_idx;
	int skip;

	if(!sound_started)
		return;

	if(!sfx)
		return;

	if(nosound.value)
		return;

	vol = fvol * 255;

	// pick a channel to play on
	target_chan = SND_PickChannel(entnum, entchannel);
	if(!target_chan)
		return;

	// spatialize
	memset(target_chan, 0, sizeof(*target_chan));
	VectorCopy(origin, target_chan->origin);
	target_chan->dist_mult = attenuation / sound_nominal_clip_dist;
	target_chan->master_vol = vol;
	target_chan->entnum = entnum;
	target_chan->entchannel = entchannel;
	SND_Spatialize(target_chan);

	if(!target_chan->leftvol && !target_chan->rightvol)
		return; // not audible at all

	// new channel
	//sc = S_LoadSound(sfx);
	if(!sc)
	{
		target_chan->sfx = NULL;
		return; // couldn't load the sound's data
	}

	target_chan->sfx = sfx;
	target_chan->pos = 0.0;
	target_chan->end = paintedtime + sc->length;

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
		}
	}
}

void S_StopSound(int entnum, int entchannel)
{
	int i;

	for(i = 0; i < MAX_DYNAMIC_CHANNELS; i++)
	{
		if(channels[i].entnum == entnum && channels[i].entchannel == entchannel)
		{
			channels[i].end = 0;
			channels[i].sfx = NULL;
			return;
		}
	}
}

void S_StopAllSounds(qboolean clear)
{
	int i;

	if(!sound_started)
		return;

	total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS; // no statics

	for(i = 0; i < MAX_CHANNELS; i++)
		if(channels[i].sfx)
			channels[i].sfx = NULL;

	Q_memset(channels, 0, MAX_CHANNELS * sizeof(channel_t));

	if(clear)
		S_ClearBuffer();
}

void S_StopAllSoundsC()
{
	S_StopAllSounds(true);
}

void S_ClearBuffer()
{
	int clear;

//#ifdef _WIN32
	//if(!sound_started || !shm || (!shm->buffer && !pDSBuf))
//#else
	if(!sound_started || !shm || !shm->buffer)
//#endif
		return;

	if(shm->samplebits == 8)
		clear = 0x80;
	else
		clear = 0;
/*
#ifdef _WIN32
	if(pDSBuf)
	{
		DWORD dwSize;
		DWORD *pData;
		int reps;
		HRESULT hresult;

		reps = 0;

		while((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pData, &dwSize, NULL, NULL, 0)) != DS_OK)
		{
			if(hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_ClearBuffer: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				return;
			}

			if(++reps > 10000)
			{
				Con_Printf("S_ClearBuffer: DS: couldn't restore buffer\n");
				S_Shutdown();
				return;
			}
		}

		Q_memset(pData, clear, shm->samples * shm->samplebits / 8);

		pDSBuf->lpVtbl->Unlock(pDSBuf, pData, dwSize, NULL, 0);
	}
	else
#endif
*/
	{
		Q_memset(shm->buffer, clear, shm->samples * shm->samplebits / 8);
	}
}

/*
=================
S_StaticSound
=================
*/
void S_StaticSound(sfx_t *sfx, vec3_t origin, float vol, float attenuation)
{
	channel_t *ss;
	sfxcache_t *sc;

	if(!sfx)
		return;

	if(total_channels == MAX_CHANNELS)
	{
		Con_Printf("total_channels == MAX_CHANNELS\n");
		return;
	}

	ss = &channels[total_channels];
	total_channels++;

	//sc = S_LoadSound(sfx);
	if(!sc)
		return;

	if(sc->loopstart == -1)
	{
		Con_Printf("Sound %s not looped\n", sfx->name);
		return;
	}

	ss->sfx = sfx;
	VectorCopy(origin, ss->origin);
	ss->master_vol = vol;
	ss->dist_mult = (attenuation / 64) / sound_nominal_clip_dist;
	ss->end = paintedtime + sc->length;

	SND_Spatialize(ss);
}

//=============================================================================

/*
===================
S_UpdateAmbientSounds
===================
*/
void S_UpdateAmbientSounds()
{
	mleaf_t *l;
	float vol;
	int ambient_channel;
	channel_t *chan;

	if(!snd_ambient)
		return;

	// calc ambient sound levels
	if(!cl.worldmodel)
		return;

	//l = Mod_PointInLeaf(listener_origin, cl.worldmodel);
	if(!l || !ambient_level.value)
	{
		for(ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
			channels[ambient_channel].sfx = NULL;
		return;
	}

	for(ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
	{
		chan = &channels[ambient_channel];
		chan->sfx = ambient_sfx[ambient_channel];

		vol = ambient_level.value * l->ambient_sound_level[ambient_channel];
		if(vol < 8)
			vol = 0;

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
	}
}

/*
============
S_Update

Called once each time through the main loop
============
*/
void S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
	int i, j;
	int total;
	channel_t *ch;
	channel_t *combine;

	if(!sound_started || (snd_blocked > 0))
		return;

	VectorCopy(origin, listener_origin);
	VectorCopy(forward, listener_forward);
	VectorCopy(right, listener_right);
	VectorCopy(up, listener_up);

	// update general area ambient sound sources
	S_UpdateAmbientSounds();

	combine = NULL;

	// update spatialization for static and dynamic sounds
	ch = channels + NUM_AMBIENTS;
	for(i = NUM_AMBIENTS; i < total_channels; i++, ch++)
	{
		if(!ch->sfx)
			continue;
		SND_Spatialize(ch); // respatialize channel
		if(!ch->leftvol && !ch->rightvol)
			continue;

		// try to combine static sounds with a previous channel of the same
		// sound effect so we don't mix five torches every frame

		if(i >= MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS)
		{
			// see if it can just use the last one
			if(combine && combine->sfx == ch->sfx)
			{
				combine->leftvol += ch->leftvol;
				combine->rightvol += ch->rightvol;
				ch->leftvol = ch->rightvol = 0;
				continue;
			}
			// search for one
			combine = channels + MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS;
			for(j = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS; j < i; j++, combine++)
				if(combine->sfx == ch->sfx)
					break;

			if(j == total_channels)
			{
				combine = NULL;
			}
			else
			{
				if(combine != ch)
				{
					combine->leftvol += ch->leftvol;
					combine->rightvol += ch->rightvol;
					ch->leftvol = ch->rightvol = 0;
				}
				continue;
			}
		}
	}

	//
	// debugging output
	//
	if(snd_show.value)
	{
		total = 0;
		ch = channels;
		for(i = 0; i < total_channels; i++, ch++)
			if(ch->sfx && (ch->leftvol || ch->rightvol))
			{
				// Con_Printf ("%3i %3i %s\n", ch->leftvol, ch->rightvol,
				// ch->sfx->name);
				total++;
			}

		Con_Printf("----(%i)----\n", total);
	}

	// mix some sound
	S_Update_();
}

void GetSoundtime()
{
	int samplepos;
	static int buffers;
	static int oldsamplepos;
	int fullsamples;

	fullsamples = shm->samples / shm->channels;

	// it is possible to miscount buffers if it has wrapped twice between
	// calls to S_Update.  Oh well.
	samplepos = SNDDMA_GetDMAPos();

	if(samplepos < oldsamplepos)
	{
		buffers++; // buffer wrapped

		if(paintedtime >
		   0x40000000)
		{ // time to chop things off to avoid 32 bit limits
			buffers = 0;
			paintedtime = fullsamples;
			S_StopAllSounds(true);
		}
	}
	oldsamplepos = samplepos;

	soundtime = buffers * fullsamples + samplepos / shm->channels;
}

void S_ExtraUpdate()
{
#ifdef _WIN32
	//IN_Accumulate();
#endif

	if(snd_noextraupdate.value)
		return; // don't pollute timings
	S_Update_();
}

void S_Update_()
{
	unsigned endtime;
	int samps;

	if(!sound_started || (snd_blocked > 0))
		return;

	// Updates DMA time
	GetSoundtime();

	// check to make sure that we haven't overshot
	if(paintedtime < soundtime)
	{
		// Con_Printf ("S_Update_ : overflow\n");
		paintedtime = soundtime;
	}

	// mix ahead of current position
	endtime = soundtime + _snd_mixahead.value * shm->speed;
	samps = shm->samples >> (shm->channels - 1);
	if(endtime - soundtime > samps)
		endtime = soundtime + samps;

/*
#ifdef _WIN32
	// if the buffer was lost or stopped, restore it and/or restart it
	{
		DWORD dwStatus;

		if(pDSBuf)
		{
			if(pDSBuf->lpVtbl->GetStatus(pDSBuf, &dwStatus) != DD_OK)
				Con_Printf("Couldn't get sound buffer status\n");

			if(dwStatus & DSBSTATUS_BUFFERLOST)
				pDSBuf->lpVtbl->Restore(pDSBuf);

			if(!(dwStatus & DSBSTATUS_PLAYING))
				pDSBuf->lpVtbl->Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);
		}
	}
#endif
*/

	//S_PaintChannels(endtime);

	SNDDMA_Submit();
}

/*
===============================================================================

console functions

===============================================================================
*/

void S_Play()
{
	static int hash = 345;
	int i;
	char name[256];
	sfx_t *sfx;

	i = 1;
	while(i < Cmd_Argc())
	{
		if(!Q_strrchr(Cmd_Argv(i), '.'))
		{
			Q_strcpy(name, Cmd_Argv(i));
			Q_strcat(name, ".wav");
		}
		else
			Q_strcpy(name, Cmd_Argv(i));
		sfx = S_PrecacheSound(name);
		S_StartSound(hash++, 0, sfx, listener_origin, 1.0, 1.0);
		i++;
	}
}

void S_PlayVol()
{
	static int hash = 543;
	int i;
	float vol;
	char name[256];
	sfx_t *sfx;

	i = 1;
	while(i < Cmd_Argc())
	{
		if(!Q_strrchr(Cmd_Argv(i), '.'))
		{
			Q_strcpy(name, Cmd_Argv(i));
			Q_strcat(name, ".wav");
		}
		else
			Q_strcpy(name, Cmd_Argv(i));
		sfx = S_PrecacheSound(name);
		vol = Q_atof(Cmd_Argv(i + 1));
		S_StartSound(hash++, 0, sfx, listener_origin, vol, 1.0);
		i += 2;
	}
}

void S_SoundList()
{
	int i;
	sfx_t *sfx;
	sfxcache_t *sc;
	int size, total;

	total = 0;
	for(sfx = known_sfx, i = 0; i < num_sfx; i++, sfx++)
	{
		sc = Cache_Check(&sfx->cache);
		if(!sc)
			continue;
		size = sc->length * sc->width * (sc->stereo + 1);
		total += size;
		if(sc->loopstart >= 0)
			Con_Printf("L");
		else
			Con_Printf(" ");
		Con_Printf("(%2db) %6i : %s\n", sc->width * 8, size, sfx->name);
	}
	Con_Printf("Total resident: %i\n", total);
}

void S_LocalSound(char *sound)
{
	sfx_t *sfx;

	if(nosound.value)
		return;
	if(!sound_started)
		return;

	sfx = S_PrecacheSound(sound);
	if(!sfx)
	{
		Con_Printf("S_LocalSound: can't cache %s\n", sound);
		return;
	}
	S_StartSound(cl.viewentity, -1, sfx, vec3_origin, 1, 1);
}

void S_ClearPrecache()
{
}

void S_BeginPrecaching()
{
}

void S_EndPrecaching()
{
}
