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

#pragma once

#include "common/const.h"
#include "common/com_model.h"
#include "modelgen.h"
#include "spritegn.h"
#include "common/crc.h"
#include "bspfile.h"

#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40 
#define ALIAS_MODEL_VERSION	0x006

#define MAX_KNOWN_MODELS	1024

#define	TEX_SPECIAL		1		// sky or slime, no lightmap or 256 subdivision

/* <4b3fe> ../engine/model.h:210 */
typedef struct mspriteframe_t
{
	int				width;
	int				height;
	void			*pcachespot;
	float			up, down, left, right;
	byte			pixels[4];
} mspriteframe_s;

/* <4b485> ../engine/model.h:219 */
typedef struct mspritegroup_s
{
	int				numframes;
	float			*intervals;
	mspriteframe_t	*frames[1];
} mspritegroup_t;

/* <4b4df> ../engine/model.h:226 */
typedef struct mspriteframedesc_s
{
	spriteframetype_t type;
	mspriteframe_t	*frameptr;
} mspriteframedesc_t;

typedef struct msprite_s
{
	short int		type;
	short int		texFormat;
	int				maxwidth, maxheight;
	int				numframes;
	int				paloffset;
	float			beamlength;
	void			*cachespot;
	mspriteframedesc_t frames[1];
} msprite_t;

typedef struct maliasframedesc_s
{
	aliasframetype_t type;
	trivertx_t		bboxmin, bboxmax;
	int				frame;
	char			name[16];
} maliasframedesc_t;

typedef struct maliasskindesc_s
{
	aliasskintype_t type;
	void			*pcachespot;
	int				skin;
} maliasskindesc_t;

typedef struct maliasgroupframedesc_s
{
	trivertx_t		bboxmin, bboxmax;
	int				frame;
} maliasgroupframedesc_t;

typedef struct maliasgroup_s
{
	int				numframes;
	int				intervals;
	maliasgroupframedesc_t frames[1];
} maliasgroup_t;

typedef struct maliasskingroup_s
{
	int				numskins;
	int				intervals;
	maliasskindesc_t skindescs[1];
} maliasskingroup_t;

/* <4b741> ../engine/model.h:293 */
typedef struct mtriangle_s
{
	int				facesfront;
	int				vertindex[3];
} mtriangle_t;

/* <4b779> ../engine/model.h:298 */
typedef struct aliashdr_s
{
	int				model;
	int				stverts;
	int				skindesc;
	int				triangles;
	int				palette;
	maliasframedesc_t frames[1];
} aliashdr_t;

typedef struct cachepic_s
{
	char			name[64];
	cache_user_t	cache;
} cachepic_t;

typedef struct cachewad_s cachewad_t;
typedef void (*PFNCACHE)(cachewad_t *, unsigned char *);

typedef struct cachewad_s
{
	char			*name;
	cachepic_t		*cache;
	int				cacheCount;
	int				cacheMax;
	struct lumpinfo_s *lumps;
	int				lumpCount;
	int				cacheExtra;
	PFNCACHE		pfnCacheBuild;
	int				numpaths;
	char			**basedirs;
	int				*lumppathindices;
#ifndef SWDS
	int				tempWad;
#endif // SWDS
} cachewad_t;

typedef struct mod_known_info_s
{
	qboolean		shouldCRC;
	qboolean		firstCRCDone;
	CRC32_t			initialCRC;
} mod_known_info_t;