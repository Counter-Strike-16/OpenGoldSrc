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

/// @file
/// @brief private rasterization driver defs

#pragma once

#include "r_shared.hpp"

#define R_SKY_SMASK 0x007F0000
#define R_SKY_TMASK 0x007F0000

#define SURFCACHE_SIZE_AT_320X200 600 * 1024

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct sspan_s
{
	int u, v, count;
} sspan_t;

extern cvar_t d_subdiv16;

extern float d_sdivzstepu, d_tdivzstepu, d_zistepu;
extern float d_sdivzstepv, d_tdivzstepv, d_zistepv;
extern float d_sdivzorigin, d_tdivzorigin, d_ziorigin;

fixed16_t sadjust, tadjust;
fixed16_t bbextents, bbextentt;

void D_DrawSpans8(espan_t *pspans);

void D_SpriteDrawSpans(sspan_t *pspan);

void D_DrawSkyScans8(espan_t *pspan);
void D_DrawSkyScans16(espan_t *pspan);

void R_ShowSubDiv();
void (*prealspandrawer)();

extern int D_MipLevelForScale(float scale);

#if id386
extern void D_PolysetAff8Start();
extern void D_PolysetAff8End();
#endif

extern int *d_pscantable;
extern int d_scantable[MAXHEIGHT];

extern int d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

extern int d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

extern void (*d_drawspans)(espan_t *pspan);