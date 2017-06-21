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
/// @brief chase camera code

#include "precompiled.hpp"
#include "scene/chase.hpp"
#include "console/cvar.hpp"

// TODO: why would this be a SP only cvar? - Solokiller
cvar_t	chase_back = {"chase_back", "100", FCVAR_SPONLY};
cvar_t	chase_up = {"chase_up", "16", FCVAR_SPONLY};
cvar_t	chase_right = {"chase_right", "0", FCVAR_SPONLY};
cvar_t	chase_active = {"chase_active", "0", FCVAR_SPONLY};

vec3_t	chase_pos = {};
vec3_t	chase_angles = {};

vec3_t	chase_dest = {};
vec3_t	chase_dest_angles = {};

void Chase_Init()
{
	Cvar_RegisterVariable(&chase_back);
	Cvar_RegisterVariable(&chase_up);
	Cvar_RegisterVariable(&chase_right);
	Cvar_RegisterVariable(&chase_active);
};

void Chase_Reset()
{
	// Nothing
	// For respawning and teleporting
	// Start position 12 units behind head
};

void TraceLine(vec3_t start, vec3_t end, vec3_t impact)
{
	trace_t	trace;

	Q_memset(&trace, 0, sizeof(trace));
	
	SV_RecursiveHullCheck (cl.worldmodel->hulls, 0, 0, 1, start, end, &trace);

	VectorCopy(trace.endpos, impact);
};

void Chase_Update()
{
	int		i;
	float	dist;
	vec3_t	forward, up, right;
	vec3_t	dest, stop;

	// if can't see player, reset
	AngleVectors (cl.viewangles, forward, right, up);

	// calc exact destination
	for (i=0 ; i<3 ; i++)
		chase_dest[i] = r_refdef.vieworg[i] 
		- forward[i]*chase_back.value
		- right[i]*chase_right.value;
	chase_dest[2] = r_refdef.vieworg[2] + chase_up.value;

	// find the spot the player is looking at
	// TODO: 4096 limit - Solokiller
	VectorMA (r_refdef.vieworg, 4096, forward, dest);
	TraceLine (r_refdef.vieworg, dest, stop);

	// calculate pitch to look at the same spot from camera
	VectorSubtract (stop, r_refdef.vieworg, stop);
	dist = DotProduct (stop, forward);
	if (dist < 1)
		dist = 1;
	r_refdef.viewangles[PITCH] = -atan(stop[2] / dist) / M_PI * 180;

	// move towards destination
	VectorCopy (chase_dest, r_refdef.vieworg);
};