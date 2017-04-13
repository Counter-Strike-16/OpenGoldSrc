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
/// @brief console module legacy API implementation

#pragma once

//============================================================================
// Console
//============================================================================

void Con_Print(const char *txt);
void Con_Printf(const char *fmt, ...); // _format(1);
void Con_DPrintf(const char *fmt, ...); // _format(1);
void Con_NPrintf(int idx, const char *fmt, ...); //( int idx, char *fmt, ... ) _format(2);
void Con_NXPrintf(struct con_nprint_s *info, char *fmt, ...) //_format(2);

qboolean Con_Visible(); // bool Con_IsVisible();

//============================================================================
// Console Commands
//============================================================================

// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are always safe.
int Cmd_Argc();
const char *Cmd_Argv(int arg);
const char *Cmd_Args();

NOXREF void Cmd_AddHUDCommand(char *cmd_name, xcommand_t function); // Cmd_AddClientCommand; int pfnAddClientCommand( const char *cmd_name, xcommand_t func );
void Cmd_AddGameCommand(char *cmd_name, xcommand_t function);

struct cmd_function_s *Cmd_GetFirstCmd();
struct cmd_function_s *Cmd_GetNextCmd(struct cmd_function_s *pCmd);

cmdalias_t *Cmd_GetAliasList();

//============================================================================
// Console Variables
//============================================================================

void Cvar_RegisterVariable(cvar_t *variable);

cvar_t *Cvar_FindVar(const char *var_name);

cvar_t *Cvar_GetList(); // GetFirst/GetHead

void Cvar_DirectSet(struct cvar_s *var, const char *value);

void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);

char *Cvar_VariableString(const char *var_name);
float Cvar_VariableValue(const char *var_name);