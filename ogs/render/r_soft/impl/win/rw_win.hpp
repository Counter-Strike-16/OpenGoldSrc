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

#pragma once

#include <windows.h>
#include <ddraw.h>

typedef struct
{
	HINSTANCE hInstance;
	void *wndproc;
	HDC hDC;   // global DC we're using
	HWND hWnd; // HWND of parent window

	HDC hdcDIBSection;       // DC compatible with DIB section
	HBITMAP hDIBSection;     // DIB section
	unsigned char *pDIBBase; // DIB base pointer, NOT used directly for rendering!

	HPALETTE hPal;             // palette we're using
	HPALETTE hpalOld;          // original system palette
	COLORREF oldsyscolors[20]; // original system colors

	HINSTANCE hinstDDRAW;      // library instance for DDRAW.DLL
	LPDIRECTDRAW lpDirectDraw; // pointer to DirectDraw object

	LPDIRECTDRAWSURFACE lpddsFrontBuffer;     // video card display memory front buffer
	LPDIRECTDRAWSURFACE lpddsBackBuffer;      // system memory backbuffer
	LPDIRECTDRAWSURFACE lpddsOffScreenBuffer; // system memory backbuffer
	LPDIRECTDRAWPALETTE lpddpPalette;         // DirectDraw palette

	qboolean palettized; // true if desktop is paletted
	qboolean modex;

	qboolean initializing;
} swwstate_t;

extern swwstate_t sww_state;

/*
** DIB code
*/
qboolean DIB_Init(unsigned char **ppbuffer, int *ppitch);
void DIB_Shutdown();
void DIB_SetPalette(const unsigned char *palette);

qboolean DDRAW_Init(unsigned char **ppbuffer, int *ppitch);
void DDRAW_Shutdown();
void DDRAW_SetPalette(const unsigned char *palette);