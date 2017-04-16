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

#include "console/IConVarSystem.hpp"

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
class CFile;

typedef struct cvar_s cvar_t;

class CConVarHandler : public IConVarSystem
{
public:
	CConVarHandler(IConsole *apConsole) : mpConsole(apConsole){}
	~CConVarHandler() = default;
	
	void Init();
	void Shutdown();
	
	virtual void Register(IConVar *apVar);

	virtual IConVar *Find(const char *asName);
	
	const char *CompleteVar(const char *asSearch, bool abForward);
	
	// appends lines containing "set variable value" for all variables
	// with the archive flag set to true.
	//void Cvar_WriteVariables (char *path);
	NOXREF void WriteVariables(CFile *f);
private:
	void Cmd_CvarList_f();
	
	IConsole *mpConsole{nullptr};

	cvar_t *cvar_vars{nullptr};
};