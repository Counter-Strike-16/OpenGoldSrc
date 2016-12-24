/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#ifndef SV_PHYS_H
#define SV_PHYS_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"
#include "rehlds/model.h"
#include "console/cvar.h"

#ifdef HOOK_ENGINE
#define sv_maxvelocity (*psv_maxvelocity)
#define sv_gravity (*psv_gravity)
#define sv_bounce (*psv_bounce)
#define sv_stepsize (*psv_stepsize)
#define sv_friction (*psv_friction)
#define sv_stopspeed (*psv_stopspeed)
#define g_moved_from (*pg_moved_from)
#define g_moved_edict (*pg_moved_edict)
#endif

extern cvar_t sv_maxvelocity;
extern cvar_t sv_gravity;
extern cvar_t sv_bounce;
extern cvar_t sv_stepsize;
extern cvar_t sv_friction;
extern cvar_t sv_stopspeed;

extern vec3_t *g_moved_from;
extern edict_t **g_moved_edict;

NOXREF void SV_CheckAllEnts(void);
void SV_CheckVelocity(edict_t *ent);
qboolean SV_RunThink(edict_t *ent);
void SV_Impact(edict_t *e1, edict_t *e2, trace_t *ptrace);
int ClipVelocity(vec_t *in, vec_t *normal, vec_t *out, float overbounce);
int SV_FlyMove(edict_t *ent, float time, trace_t *steptrace);
void SV_AddGravity(edict_t *ent);
NOXREF void SV_AddCorrectGravity(edict_t *ent);
NOXREF void SV_FixupGravityVelocity(edict_t *ent);
trace_t SV_PushEntity(edict_t *ent, vec_t *push);
void SV_PushMove(edict_t *pusher, float movetime);
int SV_PushRotate(edict_t *pusher, float movetime);
void SV_Physics_Pusher(edict_t *ent);
qboolean SV_CheckWater(edict_t *ent);
float SV_RecursiveWaterLevel(vec_t *center, float out, float in, int count);
float SV_Submerged(edict_t *ent);
void SV_Physics_None(edict_t *ent);
void SV_Physics_Follow(edict_t *ent);
void SV_Physics_Noclip(edict_t *ent);
void SV_CheckWaterTransition(edict_t *ent);
void SV_Physics_Toss(edict_t *ent);
void PF_WaterMove(edict_t *pSelf);
void SV_Physics_Step(edict_t *ent);
void SV_Physics(void);
trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore);

#endif // SV_PHYS_H