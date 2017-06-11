/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#pragma once

extern float pixelAspect;

typedef struct surf_s
{
	struct surf_s *next;   // active surface stack in r_edge.c
	struct surf_s *prev;   // used in r_edge.c for active surf stack
	struct espan_s *spans; // pointer to linked list of spans to draw
	int key;               // sorting key (BSP order)
	int last_u;            // set during tracing
	int spanstate;         // 0 = not in span
	                       // 1 = in span
	                       // -1 = in inverted span (end before
	                       //  start)
	int flags;             // currentface flags
	void *data;            // associated data like msurface_t
	cl_entity_t *entity;
	float nearzi; // nearest 1/z on surface, for mipmapping
	qboolean insubmodel;
	float d_ziorigin, d_zistepu, d_zistepv;

	int pad[2]; // to 64 bytes
} surf_t;

extern vec3_t modelorg, base_modelorg;

extern float xcenter, ycenter;
extern float xscale, yscale;
extern float xscaleinv, yscaleinv;
extern float xscaleshrink, yscaleshrink;

extern int d_lightstylevalue[256]; // 8.8 frac of base light value

extern void TransformVector(vec3_t in, vec3_t out);

extern int r_skymade;
extern void R_MakeSky();

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define ALIAS_ONSEAM 0x0020 // also defined in modelgen.h;
                            //  must be kept in sync

#define MAXVERTS 16                    // max points in a surface polygon
#define MAXWORKINGVERTS (MAXVERTS + 4) // max points in an intermediate
                                       //  polygon (while processing)

extern void R_DrawLine(polyvert_t *polyvert0, polyvert_t *polyvert1);


#define MAXVERTS 16                    // max points in a surface polygon
#define MAXWORKINGVERTS (MAXVERTS + 4) // max points in an intermediate
                                       //  polygon (while processing)
// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define MAXHEIGHT 1024
#define MAXWIDTH 1280
#define MAXDIMENSION ((MAXHEIGHT > MAXWIDTH) ? MAXHEIGHT : MAXWIDTH)

#define SIN_BUFFER_SIZE (MAXDIMENSION + CYCLE)

#define INFINITE_DISTANCE 0x10000 // distance that's always guaranteed to
                                  //  be farther away than anything in
                                  //  the scene

extern int cachewidth;
extern pixel_t *cacheblock;

extern int r_drawnpolycount;

extern cvar_t r_clearcolor;

extern int sintable[SIN_BUFFER_SIZE];
extern int intsintable[SIN_BUFFER_SIZE];

extern vec3_t vup, base_vup;
extern vec3_t vpn, base_vpn;
extern vec3_t vright, base_vright;
extern entity_t *currententity;

#define NUMSTACKEDGES 2400
#define MINEDGES NUMSTACKEDGES
#define NUMSTACKSURFACES 800

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct espan_s
{
	int u, v, count;
	struct espan_s *pnext;
} espan_t;

// FIXME: compress, make a union if that will help
// insubmodel is only 1, flags is fewer than 32, spanstate could be a byte


extern surf_t *surfaces, *surface_p, *surf_max;

// surfaces are generated in back to front order by the bsp, so if a surf
// pointer is greater than another one, it should be drawn in front
// surfaces[1] is the background, and is used as the active surface stack.
// surfaces[0] is a dummy, because index 0 is used to indicate no surface
//  attached to an edge_t

//===================================================================

extern vec3_t sxformaxis[4]; // s axis transformed into viewspace
extern vec3_t txformaxis[4]; // t axis transformed into viewspac



extern int ubasestep, errorterm, erroradjustup, erroradjustdown;
