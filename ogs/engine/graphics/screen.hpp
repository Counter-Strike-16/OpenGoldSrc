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

void SCR_Init();

void SCR_UpdateScreen();

void SCR_SizeUp();
void SCR_SizeDown();

void SCR_BringDownConsole();
void SCR_CenterPrint(char *str);

int SCR_ModalMessage(char *text);

void SCR_BeginLoadingPlaque(qboolean reconnect);
void SCR_EndLoadingPlaque();

extern float scr_con_current;
extern float scr_conlines; // lines of console to display

extern int scr_fullupdate; // set to 0 to force full redraw
extern int sb_lines;

extern int clearnotify; // set to 0 whenever notify text is drawn
extern qboolean scr_disabled_for_loading;

extern cvar_t scr_viewsize;

extern cvar_t scr_viewsize;

// only the refresh window will be updated unless these variables are flagged
extern int scr_copytop;
extern int scr_copyeverything;

qboolean scr_skipupdate;

qboolean block_drawing;