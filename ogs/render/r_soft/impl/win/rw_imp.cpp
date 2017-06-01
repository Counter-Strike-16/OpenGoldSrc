/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

/*
** RW_IMP.C
**
** This file contains ALL Win32 specific stuff having to do with the
** software refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** SWimp_EndFrame
** SWimp_Init
** SWimp_SetPalette
** SWimp_Shutdown
*/

#include "..\..\r_local.hpp"
#include "rw_win.hpp"
#include "winquake.hpp"

// Console variables that we need to access from this module

swwstate_t sww_state;

/*
** SWimp_Init
**
** This routine is responsible for initializing the implementation
** specific stuff in a software rendering subsystem.
*/
int SWimp_Init(void *hInstance, void *wndProc)
{
	sww_state.hInstance = (HINSTANCE)hInstance;
	sww_state.wndproc = wndProc;

	return true;
}

/*
** SWimp_InitGraphics
**
** This initializes the software refresh's implementation specific
** graphics subsystem.  In the case of Windows it creates DIB or
** DDRAW surfaces.
**
** The necessary width and height parameters are grabbed from
** vid.width and vid.height.
*/
static qboolean SWimp_InitGraphics(qboolean fullscreen)
{
	// free resources in use
	SWimp_Shutdown();

	// create a new window
	VID_CreateWindow(vid.width, vid.height, WINDOW_STYLE);

	// initialize the appropriate subsystem
	if(!fullscreen)
	{
		if(!DIB_Init(&vid.buffer, &vid.rowbytes))
		{
			vid.buffer = 0;
			vid.rowbytes = 0;

			return false;
		}
	}
	else
	{
		if(!DDRAW_Init(&vid.buffer, &vid.rowbytes))
		{
			vid.buffer = 0;
			vid.rowbytes = 0;

			return false;
		}
	}

	return true;
}

/*
** SWimp_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.  In the Win32 case it uses BitBlt or BltFast depending
** on whether we're using DIB sections/GDI or DDRAW.
*/
void SWimp_EndFrame()
{
	if(!sw_state.fullscreen)
	{
		if(sww_state.palettized)
		{
			//			holdpal = SelectPalette(hdcScreen, hpalDIB, FALSE);
			//			RealizePalette(hdcScreen);
		}

		BitBlt(sww_state.hDC,
		       0, 0,
		       vid.width,
		       vid.height,
		       sww_state.hdcDIBSection,
		       0, 0,
		       SRCCOPY);

		if(sww_state.palettized)
		{
			//			SelectPalette(hdcScreen, holdpal, FALSE);
		}
	}
	else
	{
		RECT r;
		HRESULT rval;
		DDSURFACEDESC ddsd;

		r.left = 0;
		r.top = 0;
		r.right = vid.width;
		r.bottom = vid.height;

		sww_state.lpddsOffScreenBuffer->lpVtbl->Unlock(sww_state.lpddsOffScreenBuffer, vid.buffer);

		if(sww_state.modex)
		{
			if((rval = sww_state.lpddsBackBuffer->lpVtbl->BltFast(sww_state.lpddsBackBuffer,
			                                                      0, 0,
			                                                      sww_state.lpddsOffScreenBuffer,
			                                                      &r,
			                                                      DDBLTFAST_WAIT)) == DDERR_SURFACELOST)
			{
				sww_state.lpddsBackBuffer->lpVtbl->Restore(sww_state.lpddsBackBuffer);
				sww_state.lpddsBackBuffer->lpVtbl->BltFast(sww_state.lpddsBackBuffer,
				                                           0, 0,
				                                           sww_state.lpddsOffScreenBuffer,
				                                           &r,
				                                           DDBLTFAST_WAIT);
			}

			if((rval = sww_state.lpddsFrontBuffer->lpVtbl->Flip(sww_state.lpddsFrontBuffer,
			                                                    NULL, DDFLIP_WAIT)) == DDERR_SURFACELOST)
			{
				sww_state.lpddsFrontBuffer->lpVtbl->Restore(sww_state.lpddsFrontBuffer);
				sww_state.lpddsFrontBuffer->lpVtbl->Flip(sww_state.lpddsFrontBuffer, NULL, DDFLIP_WAIT);
			}
		}
		else
		{
			if((rval = sww_state.lpddsBackBuffer->lpVtbl->BltFast(sww_state.lpddsFrontBuffer,
			                                                      0, 0,
			                                                      sww_state.lpddsOffScreenBuffer,
			                                                      &r,
			                                                      DDBLTFAST_WAIT)) == DDERR_SURFACELOST)
			{
				sww_state.lpddsBackBuffer->lpVtbl->Restore(sww_state.lpddsFrontBuffer);
				sww_state.lpddsBackBuffer->lpVtbl->BltFast(sww_state.lpddsFrontBuffer,
				                                           0, 0,
				                                           sww_state.lpddsOffScreenBuffer,
				                                           &r,
				                                           DDBLTFAST_WAIT);
			}
		}

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		sww_state.lpddsOffScreenBuffer->lpVtbl->Lock(sww_state.lpddsOffScreenBuffer, NULL, &ddsd, DDLOCK_WAIT, NULL);

		vid.buffer = ddsd.lpSurface;
		vid.rowbytes = ddsd.lPitch;
	}
}

/*
** SWimp_SetMode
*/
rserr_t SWimp_SetMode(int *pwidth, int *pheight, int mode, qboolean fullscreen)
{
	const char *win_fs[] = { "W", "FS" };
	rserr_t retval = rserr_ok;

	ri.Con_Printf(PRINT_ALL, "setting mode %d:", mode);

	if(!ri.Vid_GetModeInfo(pwidth, pheight, mode))
	{
		ri.Con_Printf(PRINT_ALL, " invalid mode\n");
		return rserr_invalid_mode;
	}

	ri.Con_Printf(PRINT_ALL, " %d %d %s\n", *pwidth, *pheight, win_fs[fullscreen]);

	sww_state.initializing = true;
	if(fullscreen)
	{
		if(!SWimp_InitGraphics(1))
		{
			if(SWimp_InitGraphics(0))
			{
				// mode is legal but not as fullscreen
				fullscreen = 0;
				retval = rserr_invalid_fullscreen;
			}
			else
			{
				// failed to set a valid mode in windowed mode
				retval = rserr_unknown;
			}
		}
	}
	else
	{
		// failure to set a valid mode in windowed mode
		if(!SWimp_InitGraphics(fullscreen))
		{
			sww_state.initializing = true;
			return rserr_unknown;
		}
	}

	sw_state.fullscreen = fullscreen;
#if 0
	if ( retval != rserr_unknown )
	{
		if ( retval == rserr_invalid_fullscreen ||
			 ( retval == rserr_ok && !fullscreen ) )
		{
			SetWindowLong( sww_state.hWnd, GWL_STYLE, WINDOW_STYLE );
		}
	}
#endif
	R_GammaCorrectAndSetPalette((const unsigned char *)d_8to24table);
	sww_state.initializing = true;

	return retval;
}

/*
** SWimp_SetPalette
**
** System specific palette setting routine.  A NULL palette means
** to use the existing palette.  The palette is expected to be in
** a padded 4-byte xRGB format.
*/
void SWimp_SetPalette(const unsigned char *palette)
{
	// MGL - what the fuck was kendall doing here?!
	// clear screen to black and change palette
	//	for (i=0 ; i<vid.height ; i++)
	//		memset (vid.buffer + i*vid.rowbytes, 0, vid.width);

	if(!palette)
		palette = (const unsigned char *)sw_state.currentpalette;

	if(!sw_state.fullscreen)
	{
		DIB_SetPalette((const unsigned char *)palette);
	}
	else
	{
		DDRAW_SetPalette((const unsigned char *)palette);
	}
}

/*
** SWimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/
void SWimp_Shutdown()
{
	ri.Con_Printf(PRINT_ALL, "Shutting down SW imp\n");
	DIB_Shutdown();
	DDRAW_Shutdown();

	if(sww_state.hWnd)
	{
		ri.Con_Printf(PRINT_ALL, "...destroying window\n");
		ShowWindow(sww_state.hWnd, SW_SHOWNORMAL); // prevents leaving empty slots in the taskbar
		DestroyWindow(sww_state.hWnd);
		sww_state.hWnd = NULL;
		UnregisterClass(WINDOW_CLASS_NAME, sww_state.hInstance);
	}
}

/*
** SWimp_AppActivate
*/
void SWimp_AppActivate(qboolean active)
{
	if(active)
	{
		if(sww_state.hWnd)
		{
			SetForegroundWindow(sww_state.hWnd);
			ShowWindow(sww_state.hWnd, SW_RESTORE);
		}
	}
	else
	{
		if(sww_state.hWnd)
		{
			if(sww_state.initializing)
				return;
			if(vid_fullscreen->value)
				ShowWindow(sww_state.hWnd, SW_MINIMIZE);
		}
	}
}

//===============================================================================

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
	DWORD flOldProtect;

	if(!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE, &flOldProtect))
		ri.Sys_Error(ERR_FATAL, "Protection change failed\n");
}

/*
** Sys_SetFPCW
**
** For reference:
**
** 1
** 5               0
** xxxxRRPP.xxxxxxxx
**
** PP = 00 = 24-bit single precision
** PP = 01 = reserved
** PP = 10 = 53-bit double precision
** PP = 11 = 64-bit extended precision
**
** RR = 00 = round to nearest
** RR = 01 = round down (towards -inf, floor)
** RR = 10 = round up (towards +inf, ceil)
** RR = 11 = round to zero (truncate/towards 0)
**
*/
#if !id386
void Sys_SetFPCW()
{
}
#else
unsigned fpu_ceil_cw, fpu_chop_cw, fpu_full_cw, fpu_cw, fpu_pushed_cw;
unsigned fpu_sp24_cw, fpu_sp24_ceil_cw;

void Sys_SetFPCW()
{
	__asm xor eax, eax

	__asm fnstcw word ptr fpu_cw __asm mov ax,
	word ptr fpu_cw

	__asm and ah,
	0f0h __asm or ah, 003h;
	round to nearest mode, extended precision __asm mov fpu_full_cw, eax

	__asm and ah,
	0f0h __asm or ah, 00fh;
	RTZ / truncate / chop mode, extended precision __asm mov fpu_chop_cw, eax

	__asm and ah,
	0f0h __asm or ah, 00bh;
	ceil mode, extended precision __asm mov fpu_ceil_cw, eax

	__asm and ah,
	0f0h;
	round to nearest, 24 - bit single precision __asm mov fpu_sp24_cw, eax

	__asm and ah,
	0f0h;
	ceil mode, 24 - bit single precision __asm or ah, 008h;
	__asm mov fpu_sp24_ceil_cw, eax
}
#endif
