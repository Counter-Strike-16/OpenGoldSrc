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

#pragma once

#include "rehlds/maintypes.h"

#ifdef HOOK_ENGINE
#define r_pixbytes (*pr_pixbytes)
#define gl_vsync (*pgl_vsync)
#define scr_con_current (*pscr_con_current)
#endif //HOOK_ENGINE

extern int r_pixbytes;
extern cvar_t gl_vsync;
extern float scr_con_current;

void VID_SetPalette(unsigned char *palette);
void VID_ShiftPalette(unsigned char *palette);
void VID_WriteBuffer(const char *pFilename);
NOBODY int VID_Init(unsigned short *palette);

void D_FlushCaches();
void R_SetStackBase();
void SCR_UpdateScreen();
void V_Init();
void Draw_Init();
void SCR_Init();
void R_Init();
void R_ForceCVars(qboolean multiplayer);

void SCR_BeginLoadingPlaque(qboolean reconnect);
void SCR_EndLoadingPlaque();

void R_InitSky();
void R_MarkLeaves();
void R_InitTextures();

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints);
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction);
void SetLoadingProgressBarStatusText(const char *statusText);
