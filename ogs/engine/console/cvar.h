/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// CvarSystem.h

#pragma once

/*

cvar_t variables are used to hold scalar or string variables that can be 
changed or displayed at the console or prog code as well as accessed directly
in C code.

it is sufficient to initialize a cvar_t with just the first two fields, or
you can add a true flag for variables that you want to be saved to the configuration
file when the game is quit:

cvar_t	r_draworder = {"r_draworder","1"};
cvar_t	scr_screensize = {"screensize","1",true};

Cvars must be registered before use, or they will have a 0 value instead of 
the float interpretation of the string. Generally, all cvar_t declarations 
should be registered in the apropriate init function before any console 
commands are executed:
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

typedef struct cvar_s
{
	char	*name;
	char	*string;
	qboolean archive;		// set to true to cause it to be saved to vars.rc
	qboolean server;		// notifies players when changed
	//qboolean info;		// added to serverinfo or userinfo when changed
	float	value;
	struct cvar_s *next;
} cvar_t;

// registers a cvar that allready has the name, string, and optionally the
// archive elements set
void Cvar_RegisterVariable(cvar_t *variable);

// equivelant to "<name> <variable>" typed at the console
void Cvar_Set(char *var_name, char *value);

// expands value to a string and calls Cvar_Set
void Cvar_SetValue (char *var_name, float value);

// returns 0 if not defined or non numeric
float Cvar_VariableValue (char *var_name);

// returns an empty string if not defined
char *Cvar_VariableString (char *var_name);

// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits
char *Cvar_CompleteVariable (char *partial);

// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)
qboolean Cvar_Command();

// Writes lines containing "set variable value" for all variables
// with the archive flag set to true
void Cvar_WriteVariables (FILE *f);

cvar_t *Cvar_FindVar (char *var_name);

extern cvar_t *cvar_vars;

float CVarGetFloat(const char *szVarName);
const char *CVarGetString(const char *szVarName);
cvar_t *CVarGetPointer(const char *szVarName);
void CVarSetFloat(const char *szVarName, float flValue);
void CVarSetString(const char *szVarName, const char *szValue);
void CVarRegister(cvar_t *pCvar);