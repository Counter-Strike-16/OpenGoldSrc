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

#pragma once

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

struct IConsole;

// exports

void Cvar_RegisterVariable(cvar_t *variable);

cvar_t *Cvar_FindVar(const char *var_name);

cvar_t *Cvar_GetList(); // GetFirst/GetHead

void Cvar_DirectSet(struct cvar_s *var, const char *value);

void Cvar_Set(const char *var_name, const char *value);
void Cvar_SetValue(const char *var_name, float value);

char *Cvar_VariableString(const char *var_name);
float Cvar_VariableValue(const char *var_name);

//

class CConVarHandler
{
public:
	CConVarHandler(IConsole *apConsole);
	
	void Init();
	void Shutdown();
	
	const char *CompleteVar(const char *asSearch, bool abForward);
private:
};