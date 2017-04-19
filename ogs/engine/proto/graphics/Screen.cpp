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
#include "graphics/Screen.hpp"

/*

background clear
rendering
turtle/net/ram icons
sbar
centerprint / slow centerprint
notify lines
intermission / finale overlay
loading plaque
console
menu

required background clears
required update regions


syncronous draw mode or async
One off screen buffer, with updates either copied or xblited
Need to double buffer?


async draw will require the refresh area to be cleared, because it will be
xblited, but sync draw can just ignore it.

sync
draw

CenterPrint ()
SlowPrint ()
Screen_Update ();
Con_Printf ();

net
turn off messages option

the refresh is always rendered, unless the console is full screen


console is:
        notify lines
        half
        full


*/

//============================================================================

/*
==================
SCR_Init
==================
*/
void CScreen::Init()
{
/*
	Cvar_RegisterVariable(&scr_fov);
	Cvar_RegisterVariable(&scr_viewsize);
	Cvar_RegisterVariable(&scr_conspeed);
	Cvar_RegisterVariable(&scr_showram);
	Cvar_RegisterVariable(&scr_showturtle);
	Cvar_RegisterVariable(&scr_showpause);
	Cvar_RegisterVariable(&scr_centertime);
	Cvar_RegisterVariable(&scr_printspeed);
	Cvar_RegisterVariable(&scr_allowsnap);
*/
	//
	// register our commands
	//
	//Cmd_AddCommand("screenshot", SCR_ScreenShot_f);
	//Cmd_AddCommand("snap", SCR_RSShot_f);
	//Cmd_AddCommand("sizeup", SCR_SizeUp_f);
	//Cmd_AddCommand("sizedown", SCR_SizeDown_f);

	scr_ram = W_GetLumpName("ram");
	scr_net = W_GetLumpName("net");
	scr_turtle = W_GetLumpName("turtle");

	scr_initialized = true;
};

void CScreen::Shutdown()
{
	scr_initialized = false;
};

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void CScreen::Update()
{
	static float oldscr_viewsize;
	//static float oldlcd_x;
	vrect_t vrect;

	if(scr_skipupdate || block_drawing)
		return;

	if(scr_disabled_for_loading)
	{
		/*
		if(realtime - scr_disabled_time > 60)
		{
			scr_disabled_for_loading = false;
			mpConsole->Printf("load failed.\n");
		}
		else
		*/
			return;
	};

#ifdef _WIN32
	{
		// don't suck up any cpu if minimized
		extern int Minimized;

		if(Minimized)
			return;
	};
#endif

	scr_copytop = 0;
	scr_copyeverything = 0;
	
	// No screen refresh on dedicated servers
	if ( cls.state == ca_dedicated )
		return;	// stdout only

	if(!scr_initialized || !con_initialized)
		return; // not initialized yet

	if(scr_viewsize.value != oldscr_viewsize)
	{
		oldscr_viewsize = scr_viewsize.value;
		vid.recalc_refdef = 1;
	};

	//
	// check for vid changes
	//
	if(oldfov != scr_fov.value)
	{
		oldfov = scr_fov.value;
		vid.recalc_refdef = true;
	};
	
	//if (oldlcd_x != lcd_x.value)
	{
		//oldlcd_x = lcd_x.value;
		//vid.recalc_refdef = true;
	};

	if(oldscreensize != scr_viewsize.value)
	{
		oldscreensize = scr_viewsize.value;
		vid.recalc_refdef = true;
	};

	if(oldsbar != cl_sbar.value)
	{
		oldsbar = cl_sbar.value;
		vid.recalc_refdef = true;
	};
	
	// something changed, so reorder the screen
	if(vid.recalc_refdef)
		CalcRefdef();

	//
	// do 3D refresh drawing, and then update the screen
	//
	//D_EnableBackBufferAccess(); // of all overlay stuff if drawing directly

	if(scr_fullupdate++ < vid.numpages)
	{
		// clear the entire screen
		scr_copyeverything = 1;
		Draw_TileClear(0, 0, vid.width, vid.height);
		Sbar_Changed();
	};

	pconupdate = NULL;

	SCR_SetUpToDrawConsole();
	SCR_EraseCenterString();

	//D_DisableBackBufferAccess(); // for adapters that can't stay mapped in
	                             //  for linear writes all the time

	VID_LockBuffer();
	V_RenderView();
	VID_UnlockBuffer();

	//D_EnableBackBufferAccess(); // of all overlay stuff if drawing directly

	if(scr_drawdialog)
	{
		Sbar_Draw();
		Draw_FadeScreen();
		SCR_DrawNotifyString();
		scr_copyeverything = true;
	}
	else if (scr_drawloading) // scr_draw_loading == 2 ?
	{
		DrawLoading();
		Sbar_Draw ();
		//mpRender->EndFrame();
		//return;
	}
	else if(cl.intermission == 1 && key_dest == key_game)
	{
		Sbar_IntermissionOverlay();
	}
	else if(cl.intermission == 2 && key_dest == key_game)
	{
		Sbar_FinaleOverlay();
		SCR_CheckDrawCenterString();
	}
	else if (cl.intermission == 3 && key_dest == key_game)
	{
		SCR_CheckDrawCenterString ();
	}
	else
	{
		SCR_DrawRam();
		SCR_DrawNet();
		SCR_DrawTurtle();
		SCR_DrawPause();
		SCR_DrawFPS();
		SCR_CheckDrawCenterString();
		Sbar_Draw();
		SCR_DrawConsole();
		M_Draw();
	};

	//D_DisableBackBufferAccess(); // for adapters that can't stay mapped in
	                             //  for linear writes all the time
	if(pconupdate)
		D_UpdateRects(pconupdate);

	V_UpdatePalette();

	//
	// update one of three areas
	//
	if(scr_copyeverything)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height;
		vrect.pnext = 0;

		VID_Update(&vrect);
	}
	else if(scr_copytop)
	{
		vrect.x = 0;
		vrect.y = 0;
		vrect.width = vid.width;
		vrect.height = vid.height - sb_lines;
		vrect.pnext = 0;

		VID_Update(&vrect);
	}
	else
	{
		vrect.x = scr_vrect.x;
		vrect.y = scr_vrect.y;
		vrect.width = scr_vrect.width;
		vrect.height = scr_vrect.height;
		vrect.pnext = 0;

		VID_Update(&vrect);
	};
	
	mpRender->EndFrame();
};

void CScreen::SizeUp()
{
};

void CScreen::SizeDown()
{
};

void CScreen::BringDownConsole()
{
};

void CScreen::CenterPrint(const char *str)
{
};

int CScreen::ModalMessage(const char *text)
{
	return 0;
};

/*
===============
SCR_BeginLoadingPlaque

================
*/
void CScreen::BeginLoadingPlaque(bool reconnect)
{
	mpSound->StopAllSounds(/*true*/);
	
	cl.sound_prepped = false;		// don't play ambients
	CDAudio_Stop ();
	
	// already set
	if (cls.disable_screen)
		return;
	
	if (developer->value)
		return;
	
	// if at console, don't bring up the plaque
	if (cls.state == ca_disconnected)
		return;
	
	if (cls.key_dest == key_console)
		return;
	
	if (cl.cinematictime > 0)
		scr_draw_loading = 2;	// clear to black first
	else
		scr_draw_loading = 1;
	
	Update(); //SCR_UpdateScreen();
	
	cls.disable_screen = Sys_Milliseconds(); // host.realtime;
	cls.disable_servercount = cl.servercount;
};

/*
===============
SCR_EndLoadingPlaque

================
*/
void CScreen::EndLoadingPlaque()
{
	//scr_disabled_for_loading = false;
	//scr_fullupdate = 0;
	cls.disable_screen = 0.0f;
	Con_ClearNotify();
};

/*
=================
SCR_CalcRefdef

Must be called whenever vid changes
Internal use only
=================
*/
void CScreen::CalcRefdef()
{
	vrect_t vrect;
	float size;

	scr_fullupdate = 0; // force a background redraw
	vid.recalc_refdef = 0;

	// force the status bar to redraw
	//Sbar_Changed(); // ?

	//========================================

	// bound viewsize
	if(scr_viewsize.value < 30)
		Cvar_Set("viewsize", "30");
	
	if(scr_viewsize.value > 120)
		Cvar_Set("viewsize", "120");

	// bound field of view
	if(scr_fov.value < 10)
		Cvar_Set("fov", "10");
	
	if(scr_fov.value > 170)
		Cvar_Set("fov", "170");

	r_refdef.fov_x = scr_fov.value;
	r_refdef.fov_y = CalcFov(r_refdef.fov_x, r_refdef.vrect.width, r_refdef.vrect.height);

	// intermission is always full screen
	if(cl.intermission)
		size = 120;
	else
		size = scr_viewsize.value;

	if(size >= 120)
		sb_lines = 0; // no status bar at all
	else if(size >= 110)
		sb_lines = 24; // no inventory
	else
		sb_lines = 24 + 16 + 8;

	// these calculations mirror those in R_Init() for r_refdef, but take no
	// account of water warping
	vrect.x = 0;
	vrect.y = 0;
	vrect.width = vid.width;
	vrect.height = vid.height;

	R_SetVrect(&vrect, &scr_vrect, sb_lines);

	// guard against going from one mode to another that's less than half the
	// vertical resolution
	if(scr_con_current > vid.height)
		scr_con_current = vid.height;

	// notify the refresh of the change
	R_ViewChanged(&vrect, sb_lines, vid.aspect);
};

qboolean scr_drawloading;
float scr_disabled_time;

/*
==============
SCR_DrawLoading

loading plaque over black screen
==============
*/
void CScreen::DrawLoading()
{
	if (!scr_drawloading) // if (!scr_draw_loading)
		return;
	
	int w, h;
	//mpRender->CinematicSetPalette(nullptr);
	scr_draw_loading = false;
	qpic_t *pic = Draw_CachePic("gfx/loading.lmp"); // re.DrawGetPicSize (&w, &h, "loading");
	Draw_Pic ( (vid.width - pic->width) * 0.5, (vid.height - 48 - pic->height) * 0.5, pic); // re.DrawPic ((viddef.width-w)/2, (viddef.height-h)/2, "loading");
};