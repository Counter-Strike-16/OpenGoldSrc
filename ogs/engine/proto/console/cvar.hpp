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
//#include "public/FileSystem.h"

#ifdef HOOK_ENGINE
	#define cvar_vars (*pcvar_vars)
#endif

class CFile;

constexpr auto MAX_CVAR_VALUE = 1024;

NOXREF cvar_t *Cvar_FindPrevVar(const char *var_name);

NOXREF int Cvar_VariableInt(const char *var_name);

// from pr_edict
float CVarGetFloat(const char *szVarName);
const char *CVarGetString(const char *szVarName);
cvar_t *CVarGetPointer(const char *szVarName);
void CVarSetFloat(const char *szVarName, float flValue);
void CVarSetString(const char *szVarName, const char *szValue);
void CVarRegister(cvar_t *pCvar);
//

// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits
NOXREF const char *Cvar_CompleteVariable(const char *search, int forward);

NOXREF void Cvar_RemoveHudCvars();

const char *Cvar_IsMultipleTokens(const char *varname);

// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)
qboolean Cvar_Command();

void Cmd_CvarListPrintCvar(cvar_t *var, CFile *f);

NOXREF int Cvar_CountServerVariables();

void Cvar_UnlinkExternals();

void Cvar_CmdInit();

// returns an info string containing all the CVAR_SERVERINFO cvars
char *Cvar_Serverinfo();

// returns an info string containing all the CVAR_USERINFO cvars
//char *Cvar_Userinfo();