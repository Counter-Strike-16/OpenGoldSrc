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

#define SIDE_FRONT 0
#define SIDE_BACK 1
#define SIDE_ON 2

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct mplane_s
{
} mplane_t;

// FIXME: differentiate from texinfo SURF_ flags
#define SURF_PLANEBACK 2
#define SURF_DRAWSKY 4 // sky brush face
#define SURF_DRAWTURB 0x10
#define SURF_DRAWBACKGROUND 0x40
#define SURF_DRAWSKYBOX 0x80 // sky box

#define SURF_FLOW 0x100 //PGM

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
} medge_t;

typedef struct mtexinfo_s
{
	float vecs[2][4];
	float mipadjust;
	image_t *image;
	int flags;
	int numframes;
	struct mtexinfo_s *next; // animation chain
} mtexinfo_t;

typedef struct msurface_s
{
	int visframe; // should be drawn when node is crossed

	int dlightframe;
	int dlightbits;

	mplane_t *plane;
	int flags;

	int firstedge; // look up in model->surfedges[], negative numbers
	int numedges;  // are backwards edges

	// surface generation data
	struct surfcache_s *cachespots[MIPLEVELS];

	short texturemins[2];
	short extents[2];

	mtexinfo_t *texinfo;

	// lighting info
	byte styles[MAXLIGHTMAPS];
	byte *samples; // [numstyles*surfsize]

	struct msurface_s *nextalphasurface;
} msurface_t;

#define CONTENTS_NODE -1
typedef struct mnode_s
{
	// common with leaf
	int contents; // CONTENTS_NODE, to differentiate from leafs
	int visframe; // node needs to be traversed if current

	short minmaxs[6]; // for bounding box culling

	struct mnode_s *parent;

	// node specific
	mplane_t *plane;
	struct mnode_s *children[2];

	unsigned short firstsurface;
	unsigned short numsurfaces;
} mnode_t;

typedef struct mleaf_s
{
	// common with node
	int contents; // wil be something other than CONTENTS_NODE
	int visframe; // node needs to be traversed if current

	short minmaxs[6]; // for bounding box culling

	struct mnode_s *parent;

	// leaf specific
	int cluster;
	int area;

	msurface_t **firstmarksurface;
	int nummarksurfaces;
	int key; // BSP sequence number for leaf's contents
} mleaf_t;

//===================================================================

typedef struct model_s
{
	char name[MAX_QPATH];

	int registration_sequence;

	modtype_t type;
	int numframes;

	int flags;

	//
	// volume occupied by the model graphics
	//
	vec3_t mins, maxs;

	//
	// solid volume for clipping (sent from server)
	//
	qboolean clipbox;
	vec3_t clipmins, clipmaxs;

	//
	// brush model
	//
	int firstmodelsurface, nummodelsurfaces;

	int numsubmodels;
	dmodel_t *submodels;

	int numplanes;
	mplane_t *planes;

	int numleafs; // number of visible leafs, not counting 0
	mleaf_t *leafs;

	int numvertexes;
	mvertex_t *vertexes;

	int numedges;
	medge_t *edges;

	int numnodes;
	int firstnode;
	mnode_t *nodes;

	int numtexinfo;
	mtexinfo_t *texinfo;

	int numsurfaces;
	msurface_t *surfaces;

	int numsurfedges;
	int *surfedges;

	int nummarksurfaces;
	msurface_t **marksurfaces;

	dvis_t *vis;

	byte *lightdata;

	// for alias models and sprites
	image_t *skins[MAX_MD2SKINS];
	void *extradata;
	int extradatasize;
} model_t;

void Mod_TouchModel(char *name);

byte *Mod_ClusterPVS(int cluster, model_t *model);

void Mod_Modellist_f();

void Mod_FreeAll();
void Mod_Free(model_t *mod);

extern int registration_sequence;