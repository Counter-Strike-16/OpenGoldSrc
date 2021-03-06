/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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

typedef struct cvar_s cvar_t;
typedef struct trace_s trace_t;
typedef struct mplane_s mplane_t;
typedef struct hull_s hull_t;
typedef struct dclipnode_s dclipnode_t;
typedef struct mnode_s mnode_t;

typedef struct areanode_s
{
	int axis;
	float dist;
	struct areanode_s *children[2];
	link_t trigger_edicts;
	link_t solid_edicts;
} areanode_t;

typedef struct moveclip_s // TODO: Move it to world.cpp someday
{
	vec3_t boxmins;
	vec3_t boxmaxs;
	const float *mins;
	const float *maxs;
	vec3_t mins2;
	vec3_t maxs2;
	const float *start;
	const float *end;
	trace_t trace;
	short int type;
	short int ignoretrans;
	edict_t *passedict;
	qboolean monsterClipBrush;
} moveclip_t;

typedef dclipnode_t box_clipnodes_t[6];
typedef mplane_t box_planes_t[6];
typedef mplane_t beam_planes_t[6];

#ifdef HOOK_ENGINE

#define box_hull (*pbox_hull)
#define beam_hull (*pbeam_hull)
#define box_clipnodes (*pbox_clipnodes)
#define box_planes (*pbox_planes)
#define beam_planes (*pbeam_planes)
#define sv_areanodes (*psv_areanodes)
#define sv_numareanodes (*psv_numareanodes)

#endif // HOOK_ENGINE

extern hull_t box_hull;
extern hull_t beam_hull;
extern box_clipnodes_t box_clipnodes;
extern box_planes_t box_planes;
extern beam_planes_t beam_planes;
extern areanode_t sv_areanodes[32];
extern int sv_numareanodes;

extern cvar_t sv_force_ent_intersection;

void ClearLink(link_t *l);
void RemoveLink(link_t *l);
void InsertLinkBefore(link_t *l, link_t *before);
NOXREF void InsertLinkAfter(link_t *l, link_t *after);

void SV_InitBoxHull(void);
hull_t *SV_HullForBox(const vec_t *mins, const vec_t *maxs);
NOXREF hull_t *SV_HullForBeam(const vec_t *start, const vec_t *end, const vec_t *size);
struct hull_s *SV_HullForBsp(edict_t *ent, const vec_t *mins, const vec_t *maxs, vec_t *offset);
hull_t *SV_HullForEntity(edict_t *ent, const vec_t *mins, const vec_t *maxs, vec_t *offset);

areanode_t *SV_CreateAreaNode(int depth, vec_t *mins, vec_t *maxs);

// called after the world model has been loaded, before linking any entities
void SV_ClearWorld(void);

// call before removing an entity, and before trying to move one,
// so it doesn't clip against itself
// flags ent->v.modified
void SV_UnlinkEdict(edict_t *ent);

void SV_TouchLinks(edict_t *ent, areanode_t *node);
void SV_FindTouchedLeafs(edict_t *ent, mnode_t *node, int *topnode);

// Needs to be called any time an entity changes origin, mins, maxs, or solid
// flags ent->v.modified
// sets ent->v.absmin and ent->v.absmax
// if touchtriggers, calls prog functions for the intersected triggers
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);

int SV_HullPointContents(hull_t *hull, int num, const vec_t *p);
int SV_LinkContents(areanode_t *node, const vec_t *pos);
int SV_PointContents(const vec_t *p);

edict_t *SV_TestEntityPosition(edict_t *ent);

//bool SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t* trace );
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, const vec_t *p1, const vec_t *p2, trace_t *trace);

void SV_SingleClipMoveToEntity(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, trace_t *trace);
trace_t SV_ClipMoveToEntity(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end);
void SV_ClipToLinks(areanode_t *node, moveclip_t *clip);
void SV_ClipToWorldbrush(areanode_t *node, moveclip_t *clip);
void SV_MoveBounds(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, vec_t *boxmins, vec_t *boxmaxs);
trace_t SV_MoveNoEnts(const vec_t *start, vec_t *mins, vec_t *maxs, const vec_t *end, int type, edict_t *passedict);
trace_t SV_Move(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int type, edict_t *passedict, qboolean monsterClipBrush);

#ifdef REHLDS_OPT_PEDANTIC
trace_t SV_Move_Point(const vec_t *start, const vec_t *end, int type, edict_t *passedict);
#endif // REHLDS_OPT_PEDANTIC