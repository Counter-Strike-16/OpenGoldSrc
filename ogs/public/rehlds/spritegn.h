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

/// @file
/// @brief header file for sprite generation program

// **********************************************************
// * This file must be identical in the spritegen directory *
// * and in the Quake directory, because it's used to       *
// * pass data from one to the other via .spr files.        *
// **********************************************************

//-------------------------------------------------------
// This program generates .spr sprite package files.
// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>
//-------------------------------------------------------

#pragma once

#include "modelgen.h"

#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I') // little-endian "IDSP"

const int SPRITE_VERSION = 2;

typedef enum spriteframetype_e
{
	SPR_SINGLE = 0,
	SPR_GROUP,
	SPR_ANGLED
} spriteframetype_t;

/* <4aea6> ../engine/spritegn.h:50 */
typedef struct dsprite_s
{
	int				ident;
	int				version;
	int				type;
	int				texFormat;
	float			boundingradius;
	int				width;
	int				height;
	int				numframes;
	float			beamlength;
	synctype_t		synctype;
} dsprite_t;

/* <4af46> ../engine/spritegn.h:74 */
typedef struct dspriteframe_s
{
	int				origin[2];
	int				width;
	int				height;
} dspriteframe_t;

/* <4af84> ../engine/spritegn.h:80 */
typedef struct dspritegroup_s
{
	int				numframes;
} dspritegroup_t;

/* <4afa6> ../engine/spritegn.h:84 */
typedef struct dspriteinterval_s
{
	float			interval;
} dspriteinterval_t;

/* <4afe8> ../engine/spritegn.h:90 */
typedef struct dspriteframetype_s
{
	spriteframetype_t type;
} dspriteframetype_t;