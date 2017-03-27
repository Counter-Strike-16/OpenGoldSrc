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
#include "system/common.hpp"

CConVarHandler::CConVarHandler(IConsole *apConsole, CFileSystem *apFileSystem)
: mpConsole(apConsole), mpFileSystem(apFileSystem)
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
	// TODO: Check memory releasing
	cvar_vars = nullptr;
};

void EXT_FUNC CConVarHandler::RegisterVariable(cvar_t *variable)
{
	char *oldstr;
	cvar_t *v, *c;
	cvar_t dummyvar;

	if(FindVar(variable->name))
	{
		mpConsole->Printf("Can't register variable \"%s\", already defined\n", variable->name);
		return;
	};

	if(Cmd_Exists(variable->name))
	{
		mpConsole->Printf("%s: \"%s\" is a command\n", __FUNCTION__, variable->name);
		return;
	};

	oldstr = variable->string;

	// Alloc string, so it will not dissapear on side modules unloading and to
	// maintain the same name during run
	variable->string = (char *)Z_Malloc(Q_strlen(variable->string) + 1);
	Q_strcpy(variable->string, oldstr);
	variable->value = (float)Q_atof(oldstr);

	dummyvar.name = " ";
	dummyvar.next = cvar_vars;

	v = cvar_vars;
	c = &dummyvar;

	// Insert with alphabetic order
	while(v)
	{
		if(Q_stricmp(v->name, variable->name) > 0)
			break;

		c = v;
		v = v->next;
	};

	c->next = variable;
	variable->next = v;
	cvar_vars = dummyvar.next;
};

NOXREF int CConVarHandler::CountServerVariables()
{
	NOXREFCHECK;
	
	cvar_t *var = nullptr;
	int i = 0;
	
	for(i, var = cvar_vars; var; var = var->next)
	{
		if(var->flags & FCVAR_SERVER)
			++i;
	};
	
	return i;
};

/*
============
Cvar_WriteVariables

Appends lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
NOXREF void CConVarHandler::WriteVariables(FileHandle_t f)
{
	NOXREFCHECK;
	
	for(cvar_t *var = cvar_vars; var; var = var->next)
	{
		if(var->flags & FCVAR_ARCHIVE)
			mpFileSystem->FPrintf(f, "%s \"%s\"\n", var->name, var->string);
	};
};