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

#include "common/com_model.h"
#include "common/commontypes.h"

// max number of sentences in game. NOTE: this must match CVOXFILESENTENCEMAX in
// dlls\util.h!!!
const int CVOXFILESENTENCEMAX = 1536;

typedef struct sfx_s
{
	char name[MAX_QPATH];
	cache_user_t cache;
	int servercount;
} sfx_t;

void S_Init();
void S_Shutdown();

void S_AmbientOn();
void S_AmbientOff();

void S_TouchSound(char *sample);

void S_ClearBuffer();

void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float vol, float attenuation, int flags, int pitch);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch);
void S_StopSound(int entnum, int entchannel);

sfx_t *S_PrecacheSound(char *sample);

void S_ClearPrecache();

void S_Update(vec_t *origin, vec_t *v_forward, vec_t *v_right, vec_t *v_up);

void S_StopAllSounds(qboolean clear);

void S_BeginPrecaching();
void S_EndPrecaching();

void S_ExtraUpdate();

void S_LocalSound(char *s);

void S_BlockSound();

void S_PrintStats();

/*
typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

typedef struct
{
	int length;
	int loopstart;
	int speed;
	int width;
	int stereo;
	byte data[1]; // variable sized
} sfxcache_t;

typedef struct
{
	qboolean gamealive;
	qboolean soundalive;
	qboolean splitbuffer;
	int channels;
	int samples;          // mono samples in buffer
	int submission_chunk; // don't mix less than this #
	int samplepos;        // in mono samples
	int samplebits;
	int speed;
	unsigned char *buffer;
} dma_t;

typedef struct
{
	int rate;
	int width;
	int channels;
	int loopstart;
	int samples;
	int dataofs; // chunk starts this many bytes from file start
} wavinfo_t;

// picks a channel based on priorities, empty slots, number of channels
channel_t *SND_PickChannel(int entnum, int entchannel);

// spatializes a channel
void SND_Spatialize(channel_t *ch);

// initializes cycling through a DMA buffer and returns information on it
qboolean SNDDMA_Init(void);

// gets the current DMA position
int SNDDMA_GetDMAPos(void);

// shutdown the DMA xfer.
void SNDDMA_Shutdown(void);

// ====================================================================
// User-setable variables
// ====================================================================

#define MAX_CHANNELS 128
#define MAX_DYNAMIC_CHANNELS 8

extern channel_t channels[MAX_CHANNELS];
// 0 to MAX_DYNAMIC_CHANNELS-1	= normal entity sounds
// MAX_DYNAMIC_CHANNELS to MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS -1 = water, etc
// MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS to total_channels = static sounds

extern int total_channels;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

extern qboolean fakedma;
extern int fakedma_updates;
extern int paintedtime;
extern vec3_t listener_origin;
extern vec3_t listener_forward;
extern vec3_t listener_right;
extern vec3_t listener_up;
extern volatile dma_t *shm;
extern volatile dma_t sn;
extern vec_t sound_nominal_clip_dist;

extern cvar_t loadas8bit;
extern cvar_t bgmvolume;
extern cvar_t volume;

extern qboolean snd_initialized;

extern int snd_blocked;

wavinfo_t GetWavinfo(char *name, byte *wav, int wavlength);

void SND_InitScaletable(void);
void SNDDMA_Submit(void);

// if origin is NULL, the sound will be dynamically sourced from the entity
void S_StartSound (vec3_t origin, int entnum, int entchannel, struct sfx_s *sfx, float fvol,  float attenuation, float timeofs);
void S_StartLocalSound (char *s);

void S_RawSamples (int samples, int rate, int width, int channels, byte *data);

void S_Activate (qboolean active);

void S_BeginRegistration (void);
struct sfx_s *S_RegisterSound (char *sample);
void S_EndRegistration (void);

struct sfx_s *S_FindName (char *name, qboolean create);

// the sound code makes callbacks to the client for entitiy position
// information, so entities can be dynamically re-spatialized
void CL_GetEntitySoundOrigin (int ent, vec3_t org);
*/