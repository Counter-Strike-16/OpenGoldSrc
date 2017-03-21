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
/// @brief wav sound files loader

#include "precompiled.hpp"
#include "sound/SoundLoaderWAV.hpp"
#include "memory/zone.hpp"
#include "system/system.hpp"
#include "console/console.hpp"

/*
===============================================================================

WAV loading

===============================================================================
*/

sfxcache_t *CSoundLoaderWAV::Load()
{
	wavinfo_t info = GetWavinfo(s->name, data, com_filesize);
	
	if(info.channels != 1)
	{
		Con_Printf("%s is a stereo sample\n", s->name);
		return NULL;
	};
	
	float stepscale = (float)info.rate / shm->speed;
	
	int len = info.samples / stepscale;

	len = len * info.width * info.channels;

	sfxcache_t *sc = (sfxcache_t*)Cache_Alloc(&s->cache, len + sizeof(sfxcache_t), s->name);
	
	if(!sc)
		return NULL;

	sc->length = info.samples;
	sc->loopstart = info.loopstart;
	sc->speed = info.rate;
	sc->width = info.width;
	sc->stereo = info.channels;

	ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);
	return sc;
};

void CSoundLoaderWAV::Unload()
{
};

/*
============
GetWavinfo
============
*/
wavinfo_t CSoundLoaderWAV::GetWavinfo(char *name, byte *wav, int wavlength)
{
	wavinfo_t info;
	int i;
	int format;
	int samples;

	memset(&info, 0, sizeof(info));

	if(!wav)
		return info;

	iff_data = wav;
	iff_end = wav + wavlength;

	// find "RIFF" chunk
	FindChunk("RIFF");
	if(!(data_p && !Q_strncmp((char*)data_p + 8, "WAVE", 4)))
	{
		Con_Printf("Missing RIFF/WAVE chunks\n");
		return info;
	}

	// get "fmt " chunk
	iff_data = data_p + 12;
	// DumpChunks ();

	FindChunk("fmt ");
	if(!data_p)
	{
		Con_Printf("Missing fmt chunk\n");
		return info;
	}
	data_p += 8;
	format = GetLittleShort();
	if(format != 1)
	{
		Con_Printf("Microsoft PCM format only\n");
		return info;
	}

	info.channels = GetLittleShort();
	info.rate = GetLittleLong();
	data_p += 4 + 2;
	info.width = GetLittleShort() / 8;

	// get cue chunk
	FindChunk("cue ");
	if(data_p)
	{
		data_p += 32;
		info.loopstart = GetLittleLong();
		//		Con_Printf("loopstart=%d\n", sfx->loopstart);

		// if the next chunk is a LIST chunk, look for a cue length marker
		FindNextChunk("LIST");
		if(data_p)
		{
			if(!strncmp((const char*)data_p + 28, "mark", 4))
			{ // this is not a proper parse, but it works with cooledit...
				data_p += 24;
				i = GetLittleLong(); // samples in loop
				info.samples = info.loopstart + i;
				//				Con_Printf("looped length: %i\n", i);
			}
		}
	}
	else
		info.loopstart = -1;

	// find data chunk
	FindChunk("data");
	if(!data_p)
	{
		Con_Printf("Missing data chunk\n");
		return info;
	}

	data_p += 4;
	samples = GetLittleLong() / info.width;

	if(info.samples)
	{
		if(samples < info.samples)
			Sys_Error("Sound %s has a bad loop length", name);
	}
	else
		info.samples = samples;

	info.dataofs = data_p - wav;

	return info;
};

void CSoundLoaderWAV::FindChunk(const char *name)
{
	last_chunk = iff_data;
	FindNextChunk(name);
};

void CSoundLoaderWAV::FindNextChunk(const char *name)
{
	while(1)
	{
		data_p = last_chunk;

		if(data_p >= iff_end)
		{ // didn't find the chunk
			data_p = NULL;
			return;
		}

		data_p += 4;
		iff_chunk_len = GetLittleLong();
		if(iff_chunk_len < 0)
		{
			data_p = NULL;
			return;
		}
		//		if (iff_chunk_len > 1024*1024)
		//			Sys_Error ("FindNextChunk: %i length is past the 1 meg sanity
		//limit", iff_chunk_len);
		data_p -= 8;
		last_chunk = data_p + 8 + ((iff_chunk_len + 1) & ~1);
		if(!Q_strncmp((const char*)data_p, name, 4))
			return;
	}
};

short CSoundLoaderWAV::GetLittleShort()
{
	short val = 0;
	val = *data_p;
	val = val + (*(data_p + 1) << 8);
	data_p += 2;
	return val;
};

int CSoundLoaderWAV::GetLittleLong()
{
	int val = 0;
	val = *data_p;
	val = val + (*(data_p + 1) << 8);
	val = val + (*(data_p + 2) << 16);
	val = val + (*(data_p + 3) << 24);
	data_p += 4;
	return val;
};

#if 0
void CSoundLoaderWAV::DumpChunks()
{
	char	str[5];
	
	str[4] = 0;
	data_p=iff_data;
	
	do
	{
		memcpy (str, data_p, 4);
		data_p += 4;
		iff_chunk_len = GetLittleLong();
		Con_Printf ("0x%x : %s (%d)\n", (int)(data_p - 4), str, iff_chunk_len);
		data_p += (iff_chunk_len + 1) & ~1;
	}
	while (data_p < iff_end);
};
#endif