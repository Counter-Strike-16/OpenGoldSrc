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
#include "sound/Sound.hpp"
#include "sound/ISound.hpp"


bool CSound::Init(int anDesiredSpeed, int anDesiredBits)
{
	

	
	
	Cmd_AddCommand("play", S_Play);
	Cmd_AddCommand("playvol", S_PlayVol);
	Cmd_AddCommand("stopsound", S_StopAllSoundsC);
	Cmd_AddCommand("soundlist", S_SoundList);
	Cmd_AddCommand("soundinfo", S_SoundInfo_f);

	Cvar_RegisterVariable(&s_nosound);
	Cvar_RegisterVariable(&s_volume);
	Cvar_RegisterVariable(&s_precache);
	Cvar_RegisterVariable(&s_loadas8bit);
	Cvar_RegisterVariable(&s_bgmvolume);
	Cvar_RegisterVariable(&s_bgmbuffer);
	Cvar_RegisterVariable(&s_ambient_level);
	Cvar_RegisterVariable(&s_ambient_fade);
	Cvar_RegisterVariable(&snd_noextraupdate);
	Cvar_RegisterVariable(&snd_show);
	Cvar_RegisterVariable(&_snd_mixahead);
	
	if(host_parms.memsize < 0x800000)
	{
		Cvar_Set("loadas8bit", "1");
		Con_Printf("loading all sounds as 8bit\n");
	};
	
	mbInitialized = true;
	
	Startup();
	
	SND_InitScaletable();
	
	known_sfx = Hunk_AllocName(MAX_SFX * sizeof(sfx_t), "sfx_t");
	num_sfx = 0;

	// create a piece of DMA memory

	if(fakedma)
	{
		shm = (void *)Hunk_AllocName(sizeof(*shm), "shm");
		shm->splitbuffer = 0;
		shm->samplebits = 16;
		shm->speed = 22050;
		shm->channels = 2;
		shm->samples = 32768;
		shm->samplepos = 0;
		shm->soundalive = true;
		shm->gamealive = true;
		shm->submission_chunk = 1;
		shm->buffer = Hunk_AllocName(1 << 16, "shmbuf");
	};

	//	Con_Printf ("Sound sampling rate: %i\n", shm->speed);

	// provides a tick sound until washed clean

	//	if (shm->buffer)
	//		shm->buffer[4] = shm->buffer[5] = 0x7f;	// force a pop for
	//debugging

	//ambient_sfx[AMBIENT_WATER] = S_PrecacheSound("ambience/water1.wav");
	//ambient_sfx[AMBIENT_SKY] = S_PrecacheSound("ambience/wind2.wav");

	S_StopAllSounds(true);
	return true;
};

// =======================================================================
// Shutdown sound engine
// =======================================================================
void CSound::Shutdown()
{
	if(!mnStarted)
		return;
	
	mpImpl->Shutdown();

	//if(shm)
		//shm->gamealive = 0;

	//shm = 0;
	mnStarted = 0;
	
	Cmd_RemoveCommand("play");
	Cmd_RemoveCommand("stopsound");
	Cmd_RemoveCommand("soundlist");
	Cmd_RemoveCommand("soundinfo");

	//if(!fakedma)
		//mpImpl->Shutdown();
};

/*
============
S_Update

Called once each time through the main loop
============
*/
void CSound::Update()
{
	int i, j;

	if(!mnStarted || (mnBlocked > 0))
		return;

	// update general area ambient sound sources
	UpdateAmbientSounds();

	channel_t *combine = NULL;

	// update spatialization for static and dynamic sounds
	channel_t *ch = channels + NUM_AMBIENTS;
	for(i = NUM_AMBIENTS; i < total_channels; i++, ch++)
	{
		if(!ch->sfx)
			continue;
		ch->Spatialize(); // respatialize channel
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
				combine = NULL;
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
		int total = 0;
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
	Update_();
};

void CSound::ExtraUpdate()
{
#ifdef _WIN32
	IN_Accumulate();
#endif
	
	// don't pollute timings
	if(snd_noextraupdate.value)
		return;
	
	Update_();
};

void CSound::Update_()
{
	if(!mnStarted || (mnBlocked > 0))
		return;

	// Updates DMA time
	GetSoundtime();

	// check to make sure that we haven't overshot
	if(paintedtime < soundtime)
	{
		// Con_Printf ("S_Update_ : overflow\n");
		paintedtime = soundtime;
	};

	// mix ahead of current position
	unsigned endtime = soundtime + _snd_mixahead.value * shm->speed;
	int samps = shm->samples >> (shm->channels - 1);
	
	if(endtime - soundtime > samps)
		endtime = soundtime + samps;

	mpImpl->Update();
	
	S_PaintChannels(endtime);

	mpImpl->Submit();
};

void CSound::SetListenerParams(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
	VectorCopy(origin, listener_origin);
	VectorCopy(forward, listener_forward);
	VectorCopy(right, listener_right);
	VectorCopy(up, listener_up);
};

/*
================
S_Startup
================
*/
void CSound::Startup()
{
	if(!mbInitialized)
		return;
	
	if(!fakedma)
	{
		int rc = mpImpl->Init(anDesiredSpeed, anDesiredBits);

		if(!rc)
		{
#ifndef _WIN32
			Con_Printf("S_Startup: SNDDMA_Init failed.\n");
#endif
			mnStarted = 0;
			return;
		};
	};

	mnStarted = 1;
};

/*
==================
S_StopAllSounds
==================
*/
void CSound::StopAllSounds(bool abClear)
{
	if(!mnStarted)
		return;

	total_channels = MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS; // no statics

	for(int i = 0; i < MAX_CHANNELS; i++)
		if(channels[i].sfx)
			channels[i].sfx = NULL;

	Q_memset(channels, 0, MAX_CHANNELS * sizeof(channel_t));

	if(clear)
		S_ClearBuffer();
};

void CSound::StopAllSoundsC()
{
	StopAllSounds(true);
};

/*
===================
S_UpdateAmbientSounds
===================
*/
void CSound::UpdateAmbientSounds()
{
	int ambient_channel;

	if(!mnAmbient)
		return;

	// calc ambient sound levels
	if(!cl.worldmodel)
		return;

	mleaf_t *l = Mod_PointInLeaf(listener_origin, cl.worldmodel);
	
	if(!l || !ambient_level.value)
	{
		for(ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
			channels[ambient_channel].sfx = NULL;
		return;
	}

	for(ambient_channel = 0; ambient_channel < NUM_AMBIENTS; ambient_channel++)
	{
		CSoundChannel *chan = &channels[ambient_channel];
		chan->sfx = ambient_sfx[ambient_channel];

		float vol = ambient_level.value * l->ambient_sound_level[ambient_channel];
		
		if(vol < 8)
			vol = 0;
		
		chan->Update();
	};
};

// =======================================================================
// Start a sound effect
// =======================================================================
void CSound::StartSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation)
{
	channel_t *target_chan, *check;
	sfxcache_t *sc;
	int ch_idx;
	int skip;

	if(!mnStarted)
		return;

	if(!sfx)
		return;

	if(nosound.value)
		return;

	int vol = fvol * 255;

	// pick a channel to play on
	target_chan = SND_PickChannel(entnum, entchannel);
	
	if(!target_chan)
		return;
	
	target_chan->StartSound(entnum, entchannel, sfx, origin, fvol, attenuation);

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

void CSound::StopSound(int entnum, int entchannel)
{
	for(int i = 0; i < MAX_DYNAMIC_CHANNELS; ++i)
	{
		if(channels[i].entnum == entnum && channels[i].entchannel == entchannel)
		{
			channels[i].Stop();
			return;
		};
	};
};

CSoundChannel *CSound::AllocChannel()
{
	if(total_channels == MAX_CHANNELS)
	{
		Con_Printf("total_channels == MAX_CHANNELS\n");
		return;
	};

	CSoundChannel *ss = &channels[total_channels];
	total_channels++;
	
	return ss;
};