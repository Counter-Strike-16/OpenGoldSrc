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
#include "console/LegacyExports.hpp"

void Con_Print(const char *txt)
{
};

void Con_Printf(const char *fmt, ...)
{
};

void Con_DPrintf(const char *fmt, ...)
{
};

void Con_NPrintf(int idx, const char *fmt, ...)
{
};

void Con_NXPrintf(struct con_nprint_s *info, char *fmt, ...)
{
};

qboolean Con_Visible()
{
	return false;
};

//============================================================================

int EXT_FUNC Cmd_Argc()
{
#ifndef SWDS
	g_engdstAddrs.Cmd_Argc();
#endif

	return cmd_argc;
};

/*
============
Cmd_Argv
============
*/
const char *EXT_FUNC Cmd_Argv(int arg)
{
#ifndef SWDS
	g_engdstAddrs.Cmd_Argv(&arg);
#endif

	if(arg >= 0 && arg < cmd_argc)
		return cmd_argv[arg];
	
	return ""; // TODO: Possibly better to return NULL here, but require to check
	           // all usages
};

/*
============
Cmd_Args

Returns a single string containing argv(1) to argv(argc()-1)
============
*/
const char *EXT_FUNC Cmd_Args()
{
#ifndef SWDS
	//g_engdstAddrs.Cmd_Args(); // not present
#endif

	return cmd_args;
};

NOXREF void Cmd_AddHUDCommand(char *cmd_name, xcommand_t function)
{
	NOXREFCHECK;

	Cmd_AddMallocCommand(cmd_name, function, FCMD_HUD_COMMAND);
};

void EXT_FUNC Cmd_AddGameCommand(char *cmd_name, xcommand_t function)
{
	Cmd_AddMallocCommand(cmd_name, function, FCMD_GAME_COMMAND);
};

struct cmd_function_s *Cmd_GetFirstCmd()
{
	return cmd_functions;
};

struct cmd_function_s *Cmd_GetNextCmd(struct cmd_function_s *pCmd)
{
	return nullptr;
};

cmdalias_t *Cmd_GetAliasList()
{
	return nullptr;
};

//============================================================================

void EXT_FUNC Cvar_RegisterVariable(cvar_t *variable)
{
	gpConVarHandler->RegisterVariable(variable);
};

cvar_t *Cvar_FindVar(const char *var_name)
{
	return nullptr;
};

cvar_t *Cvar_GetList()
{
	return nullptr;
};

void Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	g_RehldsHookchains.m_Cvar_DirectSet.callChain(Cvar_DirectSet_internal, var, value);
};

void Cvar_Set(const char *var_name, const char *value)
{
};

void Cvar_SetValue(const char *var_name, float value)
{
};

char *Cvar_VariableString(const char *var_name)
{
	return nullptr;
};

float Cvar_VariableValue(const char *var_name)
{
	return 0.0f;
};