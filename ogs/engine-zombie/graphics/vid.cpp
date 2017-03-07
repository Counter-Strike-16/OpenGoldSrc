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

#include "precompiled.hpp"
#include "graphics/vid.hpp"

cvar_t vid_ref = {"vid_ref", "soft", FCVAR_ARCHIVE};
cvar_t vid_xpos = {"vid_xpos", "3", FCVAR_ARCHIVE};
cvar_t vid_ypos = {"vid_ypos", "22", FCVAR_ARCHIVE};
cvar_t vid_fullscreen = {"vid_fullscreen", "0", FCVAR_ARCHIVE};
cvar_t vid_gamma = {"vid_gamma", "1", FCVAR_ARCHIVE};

/*
============
VID_Restart_f

Console command to restart the video mode and render DLL. We do this
simply by setting the modified flag for the vid_ref variable, which will
cause the entire video mode and render DLL to be reset on the next frame.
============
*/
void VID_Restart_f()
{
	vid_ref->modified = true;
};

void VID_Front_f()
{
	SetWindowLong(mainwindow, GWL_EXSTYLE, WS_EX_TOPMOST);
	SetForegroundWindow(mainwindow);
};

/*
============
VID_Init
============
*/
void VID_Init()
{
	// Create the video variables so we know how to start the graphics drivers
	Cvar_RegisterVariable(&vid_ref);
	Cvar_RegisterVariable(&vid_xpos);
	Cvar_RegisterVariable(&vid_ypos);
	Cvar_RegisterVariable(&vid_fullscreen);
	Cvar_RegisterVariable(&vid_gamma);

	// Add some console commands that we want to handle
	Cmd_AddCommand("vid_restart", VID_Restart_f);
	Cmd_AddCommand("vid_front", VID_Front_f);

	// this is a gross hack but necessary to clamp the mode for 3Dfx
#if 0
	{
		cvar_t *gl_driver = Cvar_Get( "gl_driver", "opengl32", 0 );
		cvar_t *gl_mode = Cvar_Get( "gl_mode", "3", 0 );

		if ( stricmp( gl_driver->string, "3dfxgl" ) == 0 )
		{
			Cvar_SetValue( "gl_mode", 3 );
			viddef.width  = 640;
			viddef.height = 480;
		}
	}
#endif

	// Start the graphics mode and load render DLL
	VID_CheckChanges();
};

/*
============
VID_Shutdown
============
*/
void VID_Shutdown()
{
	if(reflib_active)
	{
		gpRender->Shutdown();
		VID_FreeReflib();
	};
};

/*
============
VID_CheckChanges

This function gets called once just before drawing each frame, and it's sole
purpose in life
is to check to see if any of the video mode parameters have changed, and if they
have to
update the rendering DLL and/or video mode to match.
============
*/
void VID_CheckChanges()
{
	char name[100];

	if(vid_ref->modified)
	{
		cl.force_refdef = true; // can't use a paused refdef
		S_StopAllSounds();
	};
	
	while(vid_ref->modified)
	{
		/*
		** render has changed
		*/
		vid_ref->modified = false;
		vid_fullscreen->modified = true;
		cl.refresh_prepped = false;
		cls.disable_screen = true;

		Q_sprintf(name, sizeof(name), "r_%s", vid_ref->string);

		if(!VID_LoadRefresh(name))
		{
			if(strcmp(vid_ref->string, "soft") == 0)
				Com_Error(ERR_FATAL, "Couldn't fall back to software refresh!");
			Cvar_Set("vid_ref", "soft");

			/*
			** drop the console if we fail to load a render
			*/
			if(cls.key_dest != key_console)
				Con_ToggleConsole_f();
		};
		
		cls.disable_screen = false;
	};

	/*
	** update our window position
	*/
	if(vid_xpos->modified || vid_ypos->modified)
	{
		if(!vid_fullscreen->value)
			VID_UpdateWindowPosAndSize(vid_xpos->value, vid_ypos->value);

		vid_xpos->modified = false;
		vid_ypos->modified = false;
	};
};