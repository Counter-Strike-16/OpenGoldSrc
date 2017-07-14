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

//#include "common/event_api.h"
//#include "pm_shared/pm_defs.h"
//#include "common/pmtrace.h"

//const int EVENT_MAX_EVENTS = 256;

void EV_PlaySound( int ent, float* origin, int channel, const char* sample,
				   float volume, float attenuation, int fFlags, int pitch );

void EV_StopSound( int ent, int channel, const char* sample );

int EV_FindModelIndex( const char* pmodel );

int EV_IsLocal( int playernum );

int EV_LocalPlayerDucking();

void EV_LocalPlayerViewheight( float* viewheight );

void EV_LocalPlayerBounds( int hull, float* mins, float* maxs );

int EV_IndexFromTrace( pmtrace_t* pTrace );

physent_t* EV_GetPhysent( int idx );

void EV_SetUpPlayerPrediction( int dopred, int bIncludeLocalClient );

void EV_SetTraceHull( int hull );

void EV_PlayerTrace( float* start, float* end, int traceFlags, int ignore_pe, pmtrace_t* tr );

void EV_WeaponAnimation( int sequence, int body );

unsigned short EV_PrecacheEvent( int type, const char* psz );

void EV_PlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );

void EV_KillEvents( int entnum, const char* eventname );