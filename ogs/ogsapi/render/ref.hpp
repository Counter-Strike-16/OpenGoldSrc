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

#include "common/commontypes.hpp"

#define MAX_DLIGHTS 32
#define MAX_ENTITIES 128
#define MAX_PARTICLES 4096
#define MAX_LIGHTSTYLES 256

#define SHELL_RED_COLOR 0xF2
#define SHELL_GREEN_COLOR 0xD0
#define SHELL_BLUE_COLOR 0xF3

#define SHELL_RG_COLOR 0xDC
//#define SHELL_RB_COLOR		0x86
#define SHELL_RB_COLOR 0x68
#define SHELL_BG_COLOR 0x78

//ROGUE
#define SHELL_DOUBLE_COLOR 0xDF // 223
#define SHELL_HALF_DAM_COLOR 0x90
#define SHELL_CYAN_COLOR 0x72
//ROGUE

#define SHELL_WHITE_COLOR 0xD7

#define ENTITY_FLAGS 68

typedef struct
{
	float rgb[3]; // 0.0 - 2.0
	float white;  // highest of rgb
} lightstyle_t;

typedef struct
{
	int   x, y, width, height; // in virtual screen coordinates
	float fov_x, fov_y;
	float vieworg[3];
	float viewangles[3];
	float blend[4]; // rgba 0-1 full screen blend
	float time;     // time is uesed to auto animate
	int   rdflags;  // RDF_UNDERWATER, etc

	byte *areabits; // if not NULL, only areas with set bits will be drawn

	lightstyle_t *lightstyles; // [MAX_LIGHTSTYLES]

	int          num_entities;
	cl_entity_t *entities;

	int       num_dlights;
	dlight_t *dlights;

	int         num_particles;
	particle_t *particles;
} refdef_t;

typedef struct
{
	
	void (*DrawGetPicSize)(int *w, int *h, char *name); // will return 0 0 if not found
	void (*DrawPic)(int x, int y, char *name);
	void (*DrawStretchPic)(int x, int y, int w, int h, char *name);
	void (*DrawChar)(int x, int y, int c);
	void (*DrawTileClear)(int x, int y, int w, int h, char *name);
	void (*DrawFill)(int x, int y, int w, int h, int c);
	void (*DrawFadeScreen)();

	// Draw images for cinematic rendering (which can have a different palette). Note that calls
	void (*DrawStretchRaw)(int x, int y, int w, int h, int cols, int rows, byte *data);

} refexport_t;

//
// these are the functions imported by the refresh module
//
typedef struct
{
	void (*Sys_Error)(int err_level, char *str, ...);

	// files will be memory mapped read only
	// the returned buffer may be part of a larger pak file,
	// or a discrete file from anywhere in the quake search path
	// a -1 return means the file does not exist
	// NULL can be passed for buf to just determine existance
	int (*FS_LoadFile)(char *name, void **buf);
	void (*FS_FreeFile)(void *buf);

	// gamedir will be the current directory that generated
	// files should be stored to, ie: "f:\halflife\valve"
	char *(*FS_Gamedir)();

	cvar_t *(*Cvar_Get)(char *name, char *value, int flags);

	qboolean (*Vid_GetModeInfo)(int *width, int *height, int mode);
	void (*Vid_MenuInit)();
	void (*Vid_NewWindow)(int width, int height);
} refimport_t;