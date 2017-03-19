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
/// @brief public interface to render functions

#pragma once

const int TOP_RANGE = 16; // soldier uniform colors
const int BOTTOM_RANGE = 96;

//=============================================================================

typedef struct cl_entity_s
{
	struct model_s *model; // NULL = no model

	int visframe;          // last frame this entity was
	                       // found in an active leaf
	                       // only used for static objects
} cl_entity_t;

typedef struct
{
	int vrectright, vrectbottom;           // right & bottom screen coords
	int aliasvrectright, aliasvrectbottom; // scaled Alias versions
	float vrectrightedge;                  // rightmost right edge we care about,
	                                       //  for use in edge list
	float fvrectx, fvrecty;                // for floating-point compares
	float fvrectx_adj, fvrecty_adj;        // left and top edges, for clamping
	int vrect_x_adj_shift20;               // (vrect.x + 0.5 - epsilon) << 20
	int vrectright_adj_shift20;            // (vrectright + 0.5 - epsilon) << 20
	float fvrectright_adj, fvrectbottom_adj;
	// right and bottom edges, for clamping
	float fvrectright;           // rightmost edge, for Alias clamping
	float fvrectbottom;          // bottommost edge, for Alias clamping
	float horizontalFieldOfView; // at Z = 1.0, this many X is visible
	                             // 2.0 = 90 degrees
	float xOrigin;               // should probably allways be 0.5
	float yOrigin;               // between be around 0.3 to 0.5
} refdef_t;

//
// refresh
//
extern int reinit_surfcache;

extern struct texture_s *r_notexture_mip;

void R_Init();
void R_InitTextures();
void R_InitEfrags();
void R_RenderView(); // must set r_refdef first
void R_ViewChanged(vrect_t *pvrect, int lineadj, float aspect);
// called whenever r_refdef or vid change
void R_InitSky(/*struct texture_s *mt*/); // called at level load

void R_AddEfrags(cl_entity_t *ent);
void R_RemoveEfrags(cl_entity_t *ent);

void R_MarkLeaves();

void R_ParseParticleEffect();
void R_RunParticleEffect(vec3_t org, vec3_t dir, int color, int count);
void R_RocketTrail(vec3_t start, vec3_t end, int type);

void R_EntityParticles(cl_entity_t *ent);
void R_BlobExplosion(vec3_t org);
void R_ParticleExplosion(vec3_t org);
void R_LavaSplash(vec3_t org);
void R_TeleportSplash(vec3_t org);

void R_PushDlights();
void R_InitParticles();
void R_ClearParticles();
void R_DrawParticles();
void R_DrawWaterSurfaces();

//
// surface cache related
//
extern int reinit_surfcache;    // if 1, surface cache is currently empty and
extern qboolean r_cache_thrash; // set if thrashing the surface cache

int D_SurfaceCacheForRes(int width, int height);

void D_DeleteSurfaceCache();
void D_InitCaches(void *buffer, int size);
void R_SetVrect(vrect_t *pvrect, vrect_t *pvrectin, int lineadj);

void R_SetStackBase();

void R_ForceCVars(qboolean multiplayer);