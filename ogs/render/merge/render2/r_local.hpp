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
/// @brief private render defs

#pragma once

#ifndef GLQUAKE

#include "graphics/r_shared.hpp"

#define ALIAS_BASE_SIZE_RATIO (1.0 / 11.0)
// normalizing factor so player model works out to about
//  1 pixel per triangle

//===========================================================================

extern cvar_t r_draworder;
extern cvar_t r_speeds;
extern cvar_t r_timegraph;
extern cvar_t r_graphheight;
extern cvar_t r_clearcolor;
extern cvar_t r_waterwarp;
extern cvar_t r_fullbright;
extern cvar_t r_drawentities;
extern cvar_t r_aliasstats;
extern cvar_t r_dspeeds;
extern cvar_t r_drawflat;
extern cvar_t r_ambient;
extern cvar_t r_reportsurfout;
extern cvar_t r_maxsurfs;
extern cvar_t r_numsurfs;
extern cvar_t r_reportedgeout;
extern cvar_t r_maxedges;
extern cvar_t r_numedges;

//===========================================================================

const int DIST_NOT_SET = 98765;

extern float screenAspect;

//=============================================================================

extern int vstartscan;

//
// current entity info
//
extern vec3_t r_worldmodelorg;

void R_RenderFace(msurface_t *fa, int clipflags);
void R_RenderPoly(msurface_t *fa, int clipflags);
void R_RenderBmodelFace(bedge_t *pedges, msurface_t *psurf);
void R_TransformPlane(mplane_t *p, float *normal, float *dist);
void R_TransformFrustum();
void R_SetSkyFrame();
void R_DrawSurfaceBlock16();
void R_DrawSurfaceBlock8();
texture_t *R_TextureAnimation(texture_t *base);

void R_DrawSubmodelPolygons(model_t *pmodel, int clipflags);
void R_DrawSolidClippedSubmodelPolygons(model_t *pmodel);

void R_AddPolygonEdges(emitpoint_t *pverts, int numverts, int miplevel);

void R_AliasDrawModel(alight_t *plighting);

void R_InsertNewEdges(edge_t *edgestoadd, edge_t *edgelist);
void R_StepActiveU(edge_t *pedge);
void R_RemoveEdges(edge_t *pedge);

extern model_t *cl_worldmodel;

extern int vstartscan;

#define MAXBVERTINDEXES 1000 // new clipped vertices when clipping bmodels
                             //  to the world BSP
extern mvertex_t *r_ptverts, *r_ptvertsmax;

extern vec3_t sbaseaxis[3], tbaseaxis[3];
extern float entity_rotation[3][3];

extern int reinit_surfcache;

extern int r_currentkey;
extern int r_currentbkey;

typedef struct btofpoly_s
{
	int clipflags;
	msurface_t *psurf;
} btofpoly_t;

#define MAX_BTOFPOLYS 5000 // FIXME: tune this

extern int numbtofpolys;
extern btofpoly_t *pbtofpolys;

void R_InitTurb();
void R_ZDrawSubmodelPolys(model_t *clmodel);

//=========================================================
// Alias models
//=========================================================

extern int numverts;
extern int a_skinwidth;
extern mtriangle_t *ptriangles;
extern int numtriangles;
extern aliashdr_t *paliashdr;
extern mdl_t *pmdl;
extern float leftclip, topclip, rightclip, bottomclip;
extern int r_acliptype;
extern finalvert_t *pfinalverts;
extern auxvert_t *pauxverts;

qboolean R_AliasCheckBBox();

//=========================================================
// particle stuff

void R_DrawParticles();
void R_InitParticles();
void R_ClearParticles();
void R_ReadPointFile_f();

extern int screenwidth;

extern int r_bmodelactive;
extern vrect_t *pconupdate;

extern float r_aliastransition, r_resfudge;

void R_AliasClipTriangle(mtriangle_t *ptri);

extern float dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
extern float se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;

extern int r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
extern qboolean r_surfsonstack;
extern cshift_t cshift_water;
extern qboolean r_dowarpold, r_viewchanged;

extern mleaf_t *r_viewleaf, *r_oldviewleaf;

extern vec3_t r_emins, r_emaxs;
extern mnode_t *r_pefragtopnode;

void R_StoreEfrags(efrag_t **ppefrag);
void R_TimeRefresh_f();
void R_TimeGraph();
int R_LightPoint(vec3_t p);
void R_EmitEdge(mvertex_t *pv0, mvertex_t *pv1);
void R_ClipEdge(mvertex_t *pv0, mvertex_t *pv1, clipplane_t *clip);
void R_SplitEntityOnNode2(mnode_t *node);
void R_MarkLights(dlight_t *light, int bit, mnode_t *node);

#endif // GLQUAKE