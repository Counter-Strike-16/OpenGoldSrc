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
/// @brief video driver defs

#pragma once

#include "common/commontypes.h"
#include "common/wrect.h"
#include "common/maintypes.h"
//#include "tier0/platform.h"

#ifdef HOOK_ENGINE
	#define r_pixbytes (*pr_pixbytes)
	#define gl_vsync (*pgl_vsync)
	#define scr_con_current (*pscr_con_current)
#endif

struct IWindow;

const int VID_CBITS = 6;
const int VID_GRADES = (1 << VID_CBITS);

typedef struct cvar_s cvar_t;

extern int r_pixbytes;
extern cvar_t gl_vsync;
extern float scr_con_current;

#ifndef _WIN32
	typedef void* HWND; // already defined in osconfig...
#endif

extern HWND mainwindow; // cl_hwnd for q2 code

//void CenterWindow(HWND hWnd, int width, int height, bool lefttopjustify);

extern int window_center_x, window_center_y;
extern RECT window_rect;

// a pixel can be one, two, or four bytes
typedef byte pixel_t;

// already defined in tier0/basetypes.h
/*
typedef struct vrect_s
{
	int				x,y,width,height;
	struct vrect_s	*pnext;
} vrect_t;
*/

typedef struct
{
	pixel_t *buffer;            // invisible buffer
	pixel_t *colormap;          // 256 * VID_GRADES size
	unsigned short *colormap16; // 256 * VID_GRADES size
	int fullbright;             // index of first fullbright color
	unsigned rowbytes;          // may be > width if displayed in a window
	unsigned width;
	unsigned height;
	float aspect; // width / height -- < 0 is taller than wide
	int numpages;
	int recalc_refdef; // if true, recalc vid-based stuff
	pixel_t *conbuffer;
	int conrowbytes;
	unsigned conwidth;
	unsigned conheight;
	int maxwarpwidth;
	int maxwarpheight;
	pixel_t *direct; // direct drawing to framebuffer, if not NULL
} viddef_t;

extern viddef_t vid; // viddef; global video state

extern unsigned short d_8to16table[256];
extern unsigned d_8to24table[256];

extern void (*vid_menudrawfn)();
extern void (*vid_menukeyfn)(int key);

// called at startup and after any gamma correction
void VID_SetPalette(unsigned char *palette);

// called for bonus and pain flashes, and for underwater color changes
void VID_ShiftPalette(unsigned char *palette);

// Video module initialization, etc
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again
NOBODY int VID_Init(unsigned short *palette); // was void (uchar*)

// Called at shutdown
void VID_Shutdown();

// flushes the given rectangles from the view buffer to the screen
void VID_Update(wrect_t *rects); // was vrect_t

// sets the mode; only used by the engine for resetting to mode 0 (the
// base mode) on memory allocation failures
int VID_SetMode(int modenum, unsigned char *palette);

// called only on Win32, when pause happens, so the mouse can be released
void VID_HandlePause(qboolean pause);

void VID_LockBuffer();
void VID_UnlockBuffer();

#ifdef GLQUAKE
qboolean VID_Is8bit();
#endif

void VID_WriteBuffer(const char *pFilename);

void VID_ForceLockState(int lk);
int VID_ForceUnlockedAndReturnState();

void VID_CheckChanges();

void VID_SetDefaultMode();

IWindow *VID_OpenWindow();

/*

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints);
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction);
void SetLoadingProgressBarStatusText(const char *statusText);

void VID_MenuInit();
void VID_MenuDraw();
const char *VID_MenuKey(int anKey); // void VID_MenuKey(int key);
*/