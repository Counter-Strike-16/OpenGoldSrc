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

#include "common/commontypes.h"
#include "common/maintypes.h"

#ifdef HOOK_ENGINE

#define c_yes (*pc_yes)
#define c_no (*pc_no)

#endif // HOOK_ENGINE

constexpr auto DI_NODIR = -1;

typedef struct edict_s edict_t;

extern int c_yes;
extern int c_no;

qboolean SV_CheckBottom(edict_t *ent);
qboolean SV_movetest(edict_t *ent, vec_t *move, qboolean relink);
qboolean SV_movestep(edict_t *ent, vec_t *move, qboolean relink);
qboolean SV_StepDirection(edict_t *ent, float yaw, float dist);
qboolean SV_FlyDirection(edict_t *ent, vec_t *direction);
void SV_FixCheckBottom(edict_t *ent);
NOXREF void SV_NewChaseDir(edict_t *actor, edict_t *enemy, float dist);
NOXREF qboolean SV_CloseEnough(edict_t *ent, edict_t *goal, float dist);
NOXREF qboolean SV_ReachedGoal(edict_t *ent, vec_t *vecGoal, float flDist);
void SV_NewChaseDir2(edict_t *actor, vec_t *vecGoal, float dist);
void SV_MoveToOrigin_I(edict_t *ent, const float *pflGoal, float dist, int iStrafe);