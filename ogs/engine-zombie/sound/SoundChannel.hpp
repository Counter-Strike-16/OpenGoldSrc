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

#pragma once

// !!! if this is changed, the asm code must change !!!
typedef struct TSoundChannel
{
	TSoundChannel();
	
	void Spatialize();
	
	sfx_t		*sfx;			// sfx number
	
	int			leftvol;		// 0-255 volume
	int			rightvol;		// 0-255 volume
	
	int			end;			// end time in global paintsamples
	
	int 		pos;			// sample position in sfx
	
	int			looping;		// where to loop, -1 = no looping OBSOLETE?
	
	int			entnum;			// to allow overriding a specific sound
	int			entchannel;		//
	
	vec3_t		origin;			// only use if fixed_origin is set
	
	vec_t		dist_mult;		// distance multiplier (attenuation/clipK)
	
	int			master_vol;		// 0-255 master volume
	
	qboolean	fixed_origin;	// use origin instead of fetching entnum's origin
	qboolean	autosound;		// from an entity->sound, cleared each frame
} channel_t;