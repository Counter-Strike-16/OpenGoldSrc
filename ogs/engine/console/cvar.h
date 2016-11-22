/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

// cvar.h

#pragma once
#ifndef OGS_CVAR_H
#define OGS_CVAR_H

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

extern "C"
{

void CvarSystem_Register(cvar_t *pCvar);
//void EngFunc_Cvar_RegisterVariable(cvar_t *variable);

float CvarSystem_GetFloat(const char *szVarName);
const char *CvarSystem_GetString(const char *szVarName);

void CvarSystem_SetFloat(const char *szVarName, float flValue);
void CvarSystem_SetString(const char *szVarName, const char *szValue);

cvar_t *EngFunc_CVarGetPointer(const char *szVarName);

void EngFunc_Cvar_DirectSet(struct cvar_s *var, char *value);

};

#include "maintypes.h"
#include "cvardef.h"
#include "FileSystem.h"


#define MAX_CVAR_VALUE 1024

#ifdef HOOK_ENGINE
#define cvar_vars (*pcvar_vars)
#endif // HOOK_ENGINE

extern cvar_t *cvar_vars;

void Cvar_Init(void);
void Cvar_Shutdown(void);
cvar_t *Cvar_FindVar(const char *var_name);
NOXREF cvar_t *Cvar_FindPrevVar(const char *var_name);
float Cvar_VariableValue(const char *var_name);
NOXREF int Cvar_VariableInt(const char *var_name);
char *Cvar_VariableString(const char *var_name);
NOXREF const char *Cvar_CompleteVariable(const char *search, int forward);
void Cvar_DirectSet(struct cvar_s *var, const char *value);
void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);
void Cvar_RegisterVariable(cvar_t *variable);
NOXREF void Cvar_RemoveHudCvars(void);
const char *Cvar_IsMultipleTokens(const char *varname);
qboolean Cvar_Command(void);
NOXREF void Cvar_WriteVariables(FileHandle_t f);
void Cmd_CvarListPrintCvar(cvar_t *var, FileHandle_t f);
void Cmd_CvarList_f(void);
NOXREF int Cvar_CountServerVariables(void);
void Cvar_UnlinkExternals(void);
void Cvar_CmdInit(void);

#endif // OGS_CVAR_H