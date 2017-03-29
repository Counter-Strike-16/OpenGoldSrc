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
#include "console/ConVarList.hpp"
#include "console/IConsole.hpp"
#include "console/IConVar.hpp"
#include "system/common.hpp"

CConVarList::CConVarList(IConsole *apConsole) : mpConsole(apConsole)
{
};

/*
============
Cvar_Init

Reads in all archived cvars
============
*/
void CConVarList::Init()
{
#ifndef SWDS
// TODO: add client code, possibly none
#endif

	Cvar_CmdInit();
};

void CConVarList::Shutdown()
{
	// TODO: Check memory releasing
	cvar_vars = nullptr;
};

/*
============
Cvar_FindVar
============
*/
cvar_t *CConVarList::FindVar(const char *var_name)
{
#ifndef SWDS
	g_engdstAddrs.pfnGetCvarPointer(&var_name);
#endif

	cvar_t *var = nullptr;

	for(var = cvar_vars; var; var = var->next)
	{
		if(!Q_stricmp(var_name, var->name))
			break;
	};

	return var;
};

NOXREF cvar_t *CConVarList::FindPrevVar(const char *var_name)
{
	NOXREFCHECK;

	for(cvar_t *var = cvar_vars; var && var->next; var = var->next)
	{
		if(!Q_stricmp(var_name, var->next->name))
			return var;
	};

	return nullptr;
};

void EXT_FUNC CConVarList::RegisterVariable(cvar_t *variable)
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

	// Insert in alphabetical order
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

NOXREF int CConVarList::CountServerVariables()
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

void CConVarList::UnlinkExternals()
{
	cvar_t *pVar = cvar_vars;
	cvar_t **pList = &cvar_vars;

	while(pVar)
	{
		if(pVar->flags & FCVAR_EXTDLL)
			*pList = pVar->next;
		else
			pList = &pVar->next;

		pVar = *pList;
	};
};

NOXREF void CConVarList::RemoveHudCvars()
{
	NOXREFCHECK;

	cvar_t *pVar;
	cvar_t **pList;

	pVar = cvar_vars;
	pList = &cvar_vars;

	while(pVar)
	{
		if(pVar->flags & FCVAR_CLIENTDLL)
		{
			*pList = pVar->next;
			Z_Free(pVar->string);
			Z_Free(pVar);
		}
		else
			pList = &pVar->next;

		pVar = *pList;
	};
};

/*
============
Cvar_WriteVariables

Appends lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
NOXREF void CConVarList::WriteVariables(CFile *apFile)
{
	NOXREFCHECK;
	
	for(cvar_t *var = cvar_vars; var; var = var->next)
	{
		if(var->flags & FCVAR_ARCHIVE)
			apFile->Printf("%s \"%s\"\n", var->name, var->string);
	};
};