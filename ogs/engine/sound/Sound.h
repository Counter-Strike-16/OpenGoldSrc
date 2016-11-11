/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// Sound.h -- client sound i/o functions

#pragma once

extern "C"
{

};

struct sfx_s;

class CSound
{
public:
	CSound();
	~CSound();
	
	void Init();
	void Shutdown();
	
	void Startup();
	
	void StartSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol,  float attenuation);
	void StaticSound(sfx_t *sfx, vec3_t origin, float vol, float attenuation);
	void LocalSound(char *s);
	
	void StopSound(int entnum, int entchannel);
	void StopAllSounds(bool clear);
	
	void ClearBuffer();
	
	void Update(vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up);
	void ExtraUpdate();
	
	void BlockSound();
	void UnblockSound();
	
	sfx_t *PrecacheSound(char *sample);
	sfxcache_t *LoadSound(sfx_t *s);
	
	void TouchSound(char *sample);
	
	void ClearPrecache();
	
	void BeginPrecaching();
	void EndPrecaching();
	
	void PaintChannels(int endtime);
	void InitPaintChannels();
};