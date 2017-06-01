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
** VID_CreateWindow
*/
const char WINDOW_CLASS_NAME[] = "OGS"; // Valve001?



// return HWND?
// glw_state.hWnd = VID_CreateWindow(...);
// sww_state.hWnd = VID_CreateWindow(...);
bool VID_CreateWindow(HWND &aRetWnd, WNDPROC hWndProc, HINSTANCE hInst, TWindowProps &aWinProps /*, int stylebits*/)
{
	
	
	// gs doesn't have any of these cvars
	cvar_t *vid_xpos = ri.Cvar_Get("vid_xpos", "0", 0);
	cvar_t *vid_ypos = ri.Cvar_Get("vid_ypos", "0", 0);
	cvar_t *vid_fullscreen = ri.Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	
	int stylebits = WINDOW_STYLE;
	int x = 0;
	int y = 0;
	int w, h;
	int exstyle = 0;
	
	if(vid_fullscreen->value) // if(aWinProps.fullscreen)
	{
		exstyle = WS_EX_TOPMOST;
		stylebits = WS_POPUP | WS_VISIBLE;
	};
	
	
	
	// init all the gl stuff for the window
	//if(!GLimp_InitGL())
	//{
		//ri.Con_Printf(PRINT_ALL, "VID_CreateWindow() - GLimp_InitGL failed\n");
		//return false;
	//}
	
	
	
	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow(aWinProps.width, aWinProps.height);
	
	return true;
};