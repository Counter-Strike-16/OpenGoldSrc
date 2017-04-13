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

extern float scr_con_current;
extern float scr_conlines; // lines of console to display

extern int scr_fullupdate; // set to 0 to force full redraw
extern int sb_lines;

extern int clearnotify; // set to 0 whenever notify text is drawn

extern cvar_t scr_viewsize; // cvar_t *
extern cvar_t crosshair; // cvar_t *

extern cvar_t scr_downloading; // cvar_t *

extern qboolean block_drawing;

class CSound;

class CScreen
{
public:
	CScreen(CSound *apSound) = default;
	~CScreen() = default;
	
	void Init();
	void Shutdown();

	void Update();

	//void BringDownConsole();

	void CenterPrint(const char *str);
	//void CenterStringOff();

	//int ModalMessage(const char *text);

	void BeginLoadingPlaque(bool reconnect);
	void EndLoadingPlaque();

	/*
	void DebugGraph (float value, int color);

	void TouchPics ();

	void RunConsole ();

	extern	vrect_t		scr_vrect;		// position of render window

	extern	char		crosshair_pic[MAX_QPATH];
	extern	int			crosshair_width, crosshair_height;

	void SCR_AddDirtyPoint (int x, int y);
	void SCR_DirtyScreen ();

	//
	// scr_cin.c
	//
	void SCR_PlayCinematic (char *name);
	qboolean SCR_DrawCinematic ();
	void SCR_RunCinematic ();
	void SCR_StopCinematic ();
	void SCR_FinishCinematic ();

	void SCR_UpdateWholeScreen();
	*/
private:
	//void SizeUp();
	//void SizeDown();
	
	CSound *mpSound{nullptr};
	
	// only the render window will be updated unless these variables are flagged
	int scr_copytop{0};
	int scr_copyeverything{0};
	
	bool scr_initialized{false}; // ready to draw
	bool scr_skipupdate{false};
	bool scr_disabled_for_loading{false};
};