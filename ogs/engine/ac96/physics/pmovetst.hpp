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
#include "pm_shared/pm_defs.h"
#include "rehlds/model.h"
#include "world/world.hpp"
//#include "common/pmtrace.h"

#ifdef HOOK_ENGINE
	#define g_contentsresult (*pg_contentsresult)
	#define box_hull_0 (*pbox_hull_0)
	#define box_clipnodes_0 (*pbox_clipnodes_0)
	#define box_planes_0 (*pbox_planes_0)
#endif // HOOK_ENGINE

extern int g_contentsresult;
extern hull_t box_hull_0;
extern box_clipnodes_t box_clipnodes_0;
extern box_planes_t box_planes_0;

float PM_TraceModel(physent_t *pEnt, vec_t *start, vec_t *end, trace_t *trace);
void PM_GetModelBounds(struct model_s *mod, vec_t *mins, vec_t *maxs);
int PM_GetModelType(struct model_s *mod);
void PM_InitBoxHull(void);
hull_t *PM_HullForBox(vec_t *mins, vec_t *maxs);

int PM_HullPointContents(hull_t *hull, int num, vec_t *p);
int PM_LinkContents(vec_t *p, int *pIndex);
int PM_PointContents(vec_t *p, int *truecontents);

int PM_WaterEntity(vec_t *p);
int PM_TruePointContents(vec_t *p);

hull_t *PM_HullForStudioModel(model_t *pModel, vec_t *offset, float frame, int sequence, const vec_t *angles, const vec_t *origin, const unsigned char *pcontroller, const unsigned char *pblending, int *pNumHulls);
hull_t *PM_HullForBsp(physent_t *pe, vec_t *offset);

int _PM_TestPlayerPosition(vec_t *pos, pmtrace_t *ptrace, int (*pfnIgnore)(physent_t *));
int PM_TestPlayerPosition(vec_t *pos, pmtrace_t *ptrace);
int PM_TestPlayerPositionEx(vec_t *pos, pmtrace_t *ptrace, int (*pfnIgnore)(physent_t *));

pmtrace_t _PM_PlayerTrace(vec_t *start, vec_t *end, int traceFlags, int numphysent, physent_t *physents, int ignore_pe, int (*pfnIgnore)(physent_t *));
pmtrace_t PM_PlayerTrace(vec_t *start, vec_t *end, int traceFlags, int ignore_pe);
pmtrace_t PM_PlayerTraceEx(vec_t *start, vec_t *end, int traceFlags, int (*pfnIgnore)(physent_t *));

struct pmtrace_s *PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe);
struct pmtrace_s *PM_TraceLineEx(float *start, float *end, int flags, int usehull, int (*pfnIgnore)(physent_t *));

qboolean PM_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, const vec_t *p1, const vec_t *p2, pmtrace_t *trace);