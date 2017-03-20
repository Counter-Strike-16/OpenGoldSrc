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
/// @brief console variable system

#pragma once

#include "common/commontypes.h"
#include "maintypes.h"
#include "public/FileSystem.h"

/*
==============================================================

CVAR

==============================================================

cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

==============================================================
*/

#ifdef HOOK_ENGINE
#define cvar_vars (*pcvar_vars)
#endif

const int MAX_CVAR_VALUE = 1024;

typedef struct cvar_s cvar_t;

extern cvar_t *cvar_vars;

void Cvar_Init();
void Cvar_Shutdown();

cvar_t *Cvar_FindVar(const char *var_name);
NOXREF cvar_t *Cvar_FindPrevVar(const char *var_name);

float Cvar_VariableValue(const char *var_name);
NOXREF int Cvar_VariableInt(const char *var_name);
char *Cvar_VariableString(const char *var_name);

// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits
NOXREF const char *Cvar_CompleteVariable(const char *search, int forward);

void Cvar_DirectSet(struct cvar_s *var, const char *value);

void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);

void Cvar_RegisterVariable(cvar_t *variable);

NOXREF void Cvar_RemoveHudCvars();

const char *Cvar_IsMultipleTokens(const char *varname);

// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)
qboolean Cvar_Command();

// appends lines containing "set variable value" for all variables
// with the archive flag set to true.
//void Cvar_WriteVariables (char *path);
NOXREF void Cvar_WriteVariables(FileHandle_t f);

void Cmd_CvarListPrintCvar(cvar_t *var, FileHandle_t f);
void Cmd_CvarList_f();

NOXREF int Cvar_CountServerVariables();

void Cvar_UnlinkExternals();

void Cvar_CmdInit();

// returns an info string containing all the CVAR_SERVERINFO cvars
char *Cvar_Serverinfo();