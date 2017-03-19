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
#include "system/system.hpp"
#include "system/common.hpp"
#include "sound/SoundManager.hpp"

bool CSoundManager::Init(int anMaxSfx)
{
	mnMaxSfx = anMaxSfx;
	
	return true;
};

/*
==================
S_PrecacheSound

==================
*/
sfx_t *CSoundManager::PrecacheSound(const char *name)
{
	if(!sound_started || nosound.value)
		return NULL;

	sfx_t *sfx = FindByName(name);

	// cache it in
	//if(precache.value)
		//S_LoadSound(sfx);

	return sfx;
};

/*
==================
S_FindName

==================
*/
sfx_t *CSoundManager::FindByName(const char *name)
{
	int		i;

	if (!name)
		Sys_Error ("%s: blank name entered!\n", __FUNCTION__);

	if (Q_strlen(name) >= MAX_QPATH)
		Sys_Error ("Sound name too long: %s", name);

	// see if already loaded
	for (i=0 ; i < num_sfx ; i++)
		if (!Q_strcmp(known_sfx[i].name, name))
			return &known_sfx[i];

	if (num_sfx == mnMaxSfx)
		Sys_Error ("%s: out of sfx_t", __FUNCTION__);
	
	sfx_t *sfx = &known_sfx[i];
	Q_strcpy (sfx->name, name);

	num_sfx++;
	return sfx;
};