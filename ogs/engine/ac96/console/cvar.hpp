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
#include "common/maintypes.h"
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

/*

cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
in C code.

it is sufficient to initialize a cvar_t with just the first two fields, or
you can add ,FCVAR_* flags for variables that you want saved to the configuration
file when the game is quit:

cvar_t	r_draworder = {"r_draworder","1"};
cvar_t	scr_screensize = {"screensize","1",FCVAR_ARCHIVE};

Cvars must be registered before use, or they will have a 0 value instead of the float interpretation of the string.  Generally, all cvar_t declarations should be registered in the apropriate init function before any console commands are executed:
Cvar_RegisterVariable (&host_framerate);


C code usually just references a cvar in place:
if ( r_draworder.value )

It could optionally ask for the value to be looked up for a string name:
if (Cvar_VariableValue ("r_draworder"))

The user can access cvars from the console in two ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.
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

/**
*	returns 0 if not defined or non numeric
*/
float Cvar_VariableValue(const char *var_name);

/**
*	returns 0 if not defined or non numeric
*/
NOXREF int Cvar_VariableInt(const char *var_name);

/**
*	returns an empty string if not defined
*/
char *Cvar_VariableString(const char *var_name); // return const char?

/**
*	attempts to match a partial variable name for command line completion
*	returns nullptr if nothing fits
*/
// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits
NOXREF const char *Cvar_CompleteVariable(const char *search, int forward); // bool forward?

/**
*	Sets a cvar's value.
*/
void Cvar_DirectSet(struct cvar_s *var, const char *value);

/**
*	equivalent to "<name> <variable>" typed at the console
*/
void Cvar_Set(const char *var_name, const char *value);

/**
*	expands value to a string and calls Cvar_Set
*/
void Cvar_SetValue(const char *var_name, float value);

/**
*	Adds a freestanding variable to the variable list.
*	registers a cvar that already has the name, string, and optionally the
*	archive elements set.
*/
void Cvar_RegisterVariable(cvar_t *variable);

NOXREF void Cvar_RemoveHudCvars();

const char *Cvar_IsMultipleTokens(const char *varname);

/**
*	called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
*	command.  Returns true if the command was a variable reference that
*	was handled. (print or change)
*	Handles variable inspection and changing from the console
*/
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)
qboolean Cvar_Command(); // return bool?

/**
*	Writes lines containing "set variable value" for all variables
*	with the archive flag set to true.
*/
// appends lines containing "set variable value" for all variables
// with the archive flag set to true.
NOXREF void Cvar_WriteVariables(FileHandle_t f);

void Cmd_CvarListPrintCvar(cvar_t *var, FileHandle_t f);
void Cmd_CvarList_f();

NOXREF int Cvar_CountServerVariables();

void Cvar_UnlinkExternals();

void Cvar_CmdInit();

// returns an info string containing all the CVAR_SERVERINFO cvars
char *Cvar_Serverinfo();