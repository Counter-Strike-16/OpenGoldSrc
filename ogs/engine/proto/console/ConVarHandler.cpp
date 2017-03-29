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
#include "console/ConVarHandler.hpp"
#include "console/IConsole.hpp"
#include "console/IConVar.hpp"
#include "system/common.hpp"

char cvar_null_string[] = "";

CConVarHandler::CConVarHandler(IConsole *apConsole) : mpConsole(apConsole)
{
};

/*
============
Cvar_Init

Reads in all archived cvars
============
*/
void CConVarHandler::Init()
{
#ifndef SWDS
	// TODO: add client code, possibly none
#endif

	Cvar_CmdInit();
};

void CConVarHandler::Shutdown()
{
};

void CConVarHandler::SetVarString(const char *var_name, const char *value)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if(!var)
	{
		mpConsole->DPrintf("%s: variable \"%s\" not found\n", __FUNCTION__, var_name);
		return;
	};

	Cvar_DirectSet(var, value);
};

/*
============
Cvar_SetValue
============
*/
void CConVarHandler::SetVarValue(const char *var_name, float value)
{
	char val[32];

#ifndef SWDS
	g_engdstAddrs.Cvar_SetValue((char**)&var_name, &value);
#endif

	if(fabs(value - (double)(signed int)value) >= 0.000001)
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%f", value);
	else
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%d", (signed int)value);
	
	val[ARRAYSIZE(val) - 1] = 0;

	SetVarString(var_name, val);
};

char *CConVarHandler::GetVarString(const char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if(var)
		return var->string;

	return cvar_null_string;
};

/*
============
Cvar_VariableValue
============
*/
float CConVarHandler::GetVarValue(const char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if(var)
		return (float)Q_atof(var->string);

	return 0.0f;
};

/*
============
Cvar_VariableInteger
============
*/
NOXREF int CConVarHandler::GetVarInt(const char *var_name)
{
	NOXREFCHECK;

	cvar_t *var = Cvar_FindVar(var_name);

	if(var)
		return Q_atoi(var->string);

	return 0;
};