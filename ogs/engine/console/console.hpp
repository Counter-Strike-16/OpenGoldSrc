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

#include "common/commontypes.h"

const int CON_TEXTSIZE = 16384;

typedef struct
{
	char text[CON_TEXTSIZE];
	
	int  current; // line where next message will be printed
	int  x;       // offset in current line for next print
	int  display; // bottom of console displays this line
} console_t;

extern console_t  con_main;
extern console_t  con_chat;

extern console_t *con; // point to either con_main or con_chat

extern int con_ormask;

extern int      con_totallines;
extern qboolean con_initialized;
extern byte *   con_chars;
extern int      con_notifylines; // scan lines to clear for notify lines

void Con_DrawCharacter(int cx, int line, int num);

void Con_CheckResize();

void Con_Init();
void Con_Shutdown();

void Con_DrawConsole(int lines);

void Con_Print(const char *txt);
void Con_Printf(const char *fmt, ...);
void Con_DPrintf(const char *fmt, ...);
void Con_NPrintf(int idx, const char *fmt, ...);
void Con_SafePrintf(const char *fmt, ...);

void Con_DrawNotify();
void Con_ClearNotify();
void Con_NotifyBox(const char *text); // during startup for sound / cd warnings

void Con_DebugLog(const char *file, const char *fmt, ...);

void Con_Clear_f();
void Con_ToggleConsole_f();
void Con_Debug_f();