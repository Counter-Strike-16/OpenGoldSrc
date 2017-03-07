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
/// @brief null video driver to aid porting efforts
/// this assumes that one of the refs is statically linked to the executable (not)

#include "precompiled.hpp"
#include "graphics/vid.hpp"
#include "system/client.hpp"
#include "commondef.hpp"
#include "d_local.hpp"

float scr_con_current;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int r_pixbytes = 1;
cvar_t gl_vsync = { "gl_vsync", "1", 0, 0.0f, NULL };

#else // HOOK_ENGINE

int r_pixbytes;
cvar_t gl_vsync;

#endif // HOOK_ENGINE

viddef_t	vid; // viddef // global video state

const int BASEWIDTH = 640; // 800
const int BASEHEIGHT = 480; // 600

byte	vid_buffer[BASEWIDTH*BASEHEIGHT];
short	zbuffer[BASEWIDTH*BASEHEIGHT];
byte	surfcache[256*1024];

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

refexport_t re;

refexport_t GetRefAPI(refimport_t rimp);

void VID_SetPalette(unsigned char *palette){};

void VID_ShiftPalette(unsigned char *palette){};

void VID_WriteBuffer(const char *pFilename){};

int VID_Init(unsigned short *palette) // was uchar* in qw
{
	/*
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong(*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;

	d_pzbuffer = zbuffer;
	D_InitCaches(surfcache, sizeof(surfcache));
	*/
	return 1;
};

void VID_Init()
{
	refimport_t ri;

	viddef.width = 320;
	viddef.height = 240;

	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Con_Printf = VID_Printf;
	ri.Sys_Error = VID_Error;
	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_Gamedir = FS_Gamedir;
	ri.Vid_NewWindow = VID_NewWindow;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Vid_GetModeInfo = VID_GetModeInfo;

	re = GetRefAPI(ri);

	if(re.api_version != API_VERSION)
		Com_Error(ERR_FATAL, "Re has incompatible api_version");

	// call the init function
	if(re.Init(NULL, NULL) == -1)
		Com_Error(ERR_FATAL, "Couldn't start refresh");
}

void VID_Shutdown()
{
	//if(re.Shutdown)
		//re.Shutdown();
};

void VID_Update(vrect_t *rects)
{
};

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
};

/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
};

void VID_CheckChanges()
{
};

void VID_MenuInit()
{
};

void VID_MenuDraw()
{
};

const char *VID_MenuKey(int k)
{
	return "";
};

void D_FlushCaches(){};

void R_SetStackBase(){};

void V_Init(){};

void Draw_Init(){};

void R_Init(){};

void R_ForceCVars(qboolean multiplayer){};

void R_InitSky(){};

void R_MarkLeaves(){};

void R_InitTextures()
{
	r_notexture_mip = (texture_t *)Hunk_AllocName(404, "notexture");

	r_notexture_mip->height = 16;
	r_notexture_mip->width = 16;

	r_notexture_mip->offsets[0] = 64;
	r_notexture_mip->offsets[1] = 320;
	r_notexture_mip->offsets[2] = 384;
	r_notexture_mip->offsets[3] = 400;

	for(int m = 0; m < 4; ++m)
	{
		int texSize = 16 >> m;

		byte *dest = (byte *)r_notexture_mip + r_notexture_mip->offsets[m];

		for(int y = 0; y < texSize; ++y)
		{
			for(int x = 0; x < texSize; ++x, ++dest)
			{
				if(x < (texSize * 0.5f) == y < (texSize * 0.5f))
					*dest = -1;
				else
					*dest = 0;
			};
		};
	};
};

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints){};

void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction){};
void SetLoadingProgressBarStatusText(const char *statusText){};