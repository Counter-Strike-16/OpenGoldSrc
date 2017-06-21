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

#include "video/WindowWin.hpp"

CWindowWin::~CWindowWin()
{
	Close();
};

bool CWindowWin::Open()
{
	WNDCLASS wc;
	RECT r;
	
	// Register the frame class
	wc.style = 0;
	wc.lpfnWndProc = hWndProc; //(WNDPROC)glw_state.wndproc; // (WNDPROC)sww_state.wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst; //glw_state.hInstance; // sww_state.hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (void *)COLOR_GRAYTEXT;
	wc.lpszMenuName = 0;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	
	if(!RegisterClass(&wc))
		return false; //ri.Sys_Error(ERR_FATAL, "Couldn't register window class");
	
	r.left = 0;
	r.top = 0;
	r.right = aWinProps.width;
	r.bottom = aWinProps.height;
	
	AdjustWindowRect(&r, stylebits, FALSE);
	
	w = r.right - r.left;
	h = r.bottom - r.top;
	
	if(!fullscreen)
	{
		x = (GetSystemMetrics(SM_CXSCREEN) - w) * 0.5f; //x = vid_xpos->value;
		y = (GetSystemMetrics(SM_CYSCREEN) - h) * 0.5f; //y = vid_ypos->value;
	};
	
	mhWnd = CreateWindowEx(
	exstyle,
	WINDOW_CLASS_NAME,
	"OpenGoldSrc", // OGS
	stylebits,
	x, y, w, h,
	NULL,
	NULL,
	hInst,
	NULL);
	
	if(!hWnd)
		return false; //ri.Sys_Error(ERR_FATAL, "Couldn't create window");
	
	ShowWindow(hWnd, SW_SHOWNORMAL); //SW_SHOW
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	
	return true;
};

void CWindowWin::Close()
{
};