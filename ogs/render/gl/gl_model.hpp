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

#include "common/com_model.h"

/*

d*_t structures are on-disk representations
m*_t structures are in-memory

*/

/*
==============================================================================

BRUSH MODELS

==============================================================================
*/

//
// in memory representation
//
// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
} mvertex_t;

typedef struct
{
	float radius;
	int headnode;
} mmodel_t;

#define SIDE_FRONT 0
#define SIDE_BACK 1
#define SIDE_ON 2

#define SURF_PLANEBACK 2
#define SURF_DRAWSKY 4
#define SURF_DRAWTURB 0x10
#define SURF_DRAWBACKGROUND 0x40
#define SURF_UNDERWATER 0x80

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
} medge_t;

typedef struct mtexinfo_s
{
	float vecs[2][4];
	int flags;
	int numframes;
	struct mtexinfo_s *next; // animation chain
	image_t *image;
} mtexinfo_t;

typedef struct glpoly_s
{
	int flags;                  // for SURF_UNDERWATER (not needed anymore?)
	float verts[4][VERTEXSIZE]; // variable sized (xyz s1t1 s2t2)
} glpoly_t;

typedef struct msurface_s
{
	int visframe; // should be drawn when node is crossed

	cplane_t *plane;
	int flags;

	int firstedge; // look up in model->surfedges[], negative numbers
	int numedges;  // are backwards edges

	short texturemins[2];
	short extents[2];

	int light_s, light_t;   // gl lightmap coordinates
	int dlight_s, dlight_t; // gl lightmap coordinates for dynamic lightmaps

	glpoly_t *polys; // multiple if warped
	struct msurface_s *texturechain;
	struct msurface_s *lightmapchain;

	mtexinfo_t *texinfo;

	// lighting info
	int dlightframe;
	int dlightbits;

	int lightmaptexturenum;
	byte styles[MAXLIGHTMAPS];
	float cached_light[MAXLIGHTMAPS]; // values currently used in lightmap
	byte *samples;                    // [numstyles*surfsize]
} msurface_t;

typedef struct mnode_s
{
	int contents; // -1, to differentiate from leafs
} mnode_t;

typedef struct mleaf_s
{
	
	// leaf specific
	int cluster;
	int area;
} mleaf_t;

//===================================================================

typedef struct model_s
{

	int registration_sequence;

	//
	// solid volume for clipping
	//
	qboolean clipbox;
	vec3_t clipmins, clipmaxs;

	int lightmap; // only for submodels

	dvis_t *vis;

	byte *lightdata;

	// for alias models and skins
	image_t *skins[MAX_MD2SKINS];

	int extradatasize;
	void *extradata;
} model_t;

byte *Mod_ClusterPVS(int cluster, model_t *model);

void Mod_Modellist_f();

void Mod_FreeAll();
void Mod_Free(model_t *mod);