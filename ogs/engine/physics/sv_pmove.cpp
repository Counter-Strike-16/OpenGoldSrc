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

#include "precompiled.hpp"

void PM_SV_PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	EV_SV_Playback(flags | FEV_NOTHOST, clientindex, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

void PM_SV_PlaySound(int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
	SV_StartSound(1, host_client->edict, channel, sample, (int)(volume * 255.0), attenuation, fFlags, pitch);
}

const char *PM_SV_TraceTexture(int ground, vec_t *vstart, vec_t *vend)
{
	if(ground < 0 || ground >= pmove->numphysent)
		return NULL;

	physent_t *pe = &pmove->physents[ground];
	if(!pe->model || pe->info < 0 || pe->info >= g_psv.max_edicts)
		return NULL;

	edict_t *pent = &g_psv.edicts[pe->info];

	/* Unreachable code
  if (!pent)
          return NULL;
  */
	return TraceTexture(pent, vstart, vend);
}
