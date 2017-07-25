/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief sound module functions

#pragma once

#include "common/com_model.h"
#include "common/commontypes.h"

//====================================================================

typedef struct cvar_s cvar_t;

// max number of sentences in game. NOTE: this must match CVOXFILESENTENCEMAX in dlls\util.h!!!
const int CVOXFILESENTENCEMAX = 1536;

// ====================================================================
// User-setable variables
// ====================================================================

const int MAX_CHANNELS = 128; // 32
const int MAX_DYNAMIC_CHANNELS = 8;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

extern qboolean fakedma;
extern int fakedma_updates;
extern int paintedtime;

extern vec_t sound_nominal_clip_dist;

extern cvar_t s_loadas8bit;
extern cvar_t s_bgmvolume;
extern cvar_t s_volume;
extern cvar_t s_nosound;
extern cvar_t s_khz;
extern cvar_t s_show;
extern cvar_t s_mixahead;
extern cvar_t s_testsound;
extern cvar_t s_primary;

typedef struct sfx_s
{
	char name[MAX_QPATH];
	cache_user_t cache;
	int servercount;
} sfx_t;

/*
typedef struct sfx_s
{
	char 		name[MAX_QPATH];
	int			registration_sequence;
	sfxcache_t	*cache;
	char 		*truename;
} sfx_t;
*/

// !!! if this is changed, the asm code must change it too !!!
typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

// !!! if this is changed, it much be changed in asm code too !!!
typedef struct
{
	int length;
	int loopstart;
	int speed; // not needed, because converted on load?
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
	
	byte *buffer;
} dma_t;

extern volatile dma_t *shm;
extern volatile dma_t sn; // q2: non-volatile dma

//====================================================================

void S_TouchSound(char *sample);

void S_ClearBuffer();

void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float vol, float attenuation, int flags, int pitch);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch);

void S_ClearPrecache();

void S_BeginPrecaching();
void S_EndPrecaching();

void S_LocalSound(char *s);

void S_BlockSound();

void S_PrintStats();

/*
====================================================================

  SYSTEM SPECIFIC FUNCTIONS

====================================================================
*/

// picks a channel based on priorities, empty slots, number of channels
//channel_t *SND_PickChannel(int entnum, int entchannel); // S_

// gets the current DMA position
int SNDDMA_GetDMAPos();

void SND_InitScaletable(); // S_

// if origin is NULL, the sound will be dynamically sourced from the entity
void S_StartSound (vec3_t origin, int entnum, int entchannel, struct sfx_s *sfx, float fvol,  float attenuation, float timeofs);
void S_StartLocalSound (char *s);

void S_RawSamples (int samples, int rate, int width, int channels, byte *data);

void S_Activate (qboolean active);

void S_BeginRegistration ();
struct sfx_s *S_RegisterSound (char *sample);
void S_EndRegistration ();

// the sound code makes callbacks to the client for entitiy position
// information, so entities can be dynamically re-spatialized
void CL_GetEntitySoundOrigin (int ent, vec3_t org);

/*
// a playsound_t will be generated by each call to S_StartSound,
// when the mixer reaches playsound->begin, the playsound will
// be assigned to a channel
typedef struct playsound_s
{
	struct playsound_s	*prev, *next;
	sfx_t		*sfx;
	float		volume;
	float		attenuation;
	int			entnum;
	int			entchannel;
	qboolean	fixed_origin;	// use origin field instead of entnum's origin
	vec3_t		origin;
	unsigned	begin;			// begin on this sample
} playsound_t;

void	SNDDMA_BeginPainting (void);


extern	int		s_rawend;

extern	playsound_t	s_pendingplays;

#define	MAX_RAW_SAMPLES	8192
extern	portable_samplepair_t	s_rawsamples[MAX_RAW_SAMPLES];

void S_IssuePlaysound (playsound_t *ps);

void S_PaintChannels(int endtime);

*/

/*
class CSound
{
private:
	CSound(ISound *apSound) : mpSound(apSound){}
	
	bool Init();
	void Shutdown();
	
	void Update();
private:
	ISound *mpSound;
};
*/

////////////////////////////////////////////////////

// We need these funcs in order to support a dynamic sound dll

//qboolean S_LoadSoundDriver(const char *asName); // temp name
//void S_UnloadSoundDriver();

//void S_CheckChanges();

void S_Init();
void S_Shutdown();

//void S_Update();

void S_StopSound(int entnum, int entchannel);
void S_StopAllSounds(qboolean clear);

//void VOX_Init();

/*
extern cvar_t suitvolume;

extern bool g_fUseDInput;

sfx_t* S_PrecacheSound( const char* name );

void S_StartDynamicSound( int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch );

void Snd_AcquireBuffer();
void Snd_ReleaseBuffer();

void SetMouseEnable(int fState);
*/