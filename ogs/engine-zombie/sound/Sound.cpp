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

#include "sound/Sound.hpp"
#include "sound/ISound.hpp"

/*
================
S_Init
================
*/
bool CSound::Init()
{
	Con_Printf("\nSound Initialization\n");

	if(COM_CheckParm("-nosound"))
		return true;
	
	if(COM_CheckParm("-simsound"))
		fakedma = true;
	
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
		int rc = mpImpl->Init();

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