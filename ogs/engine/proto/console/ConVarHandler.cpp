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
	// TODO: Check memory releasing
	cvar_vars = nullptr;
};

void CConVarHandler::SetVarString(const char *var_name, const char *value)
{
	cvar_t *var = mpVarList->Find(var_name);

	if(!var)
	{
		mpConsole->DevPrintf("%s: variable \"%s\" not found\n", __FUNCTION__, var_name);
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

	if(fabs(value - (double)(signed int)value) >= 0.000001f)
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%f", value);
	else
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%d", (signed int)value);
	
	val[ARRAYSIZE(val) - 1] = 0;

	SetVarString(var_name, val);
};

char *CConVarHandler::GetVarString(const char *var_name)
{
	cvar_t *var = mpVarList->Find(var_name);

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
	cvar_t *var = mpVarList->Find(var_name);

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

	cvar_t *var = mpVarList->Find(var_name);

	if(var)
		return Q_atoi(var->string);

	return 0;
};

/*
============
Cvar_FindVar
============
*/
NOXREF cvar_t *CConVarHandler::Find(const char *var_name)
{
	NOXREFCHECK;
	
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

 cvar_t *CConVarHandler::Find(const char *var_name)
{
	

	for(cvar_t *var = cvar_vars; var && var->next; var = var->next)
	{
		if(!Q_stricmp(var_name, var->next->name))
			return var;
	};

	return nullptr;
};

void EXT_FUNC CConVarHandler::Register(cvar_t *variable)
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

//CountServerVariables()
//{
	// check for FCVAR_SERVER flag
//};
NOXREF int CConVarHandler::CountFlaggedVariables(int anFlags)
{
	NOXREFCHECK;
	
	int i = 0;
	
	for(auto var = cvar_vars; var; var = var->next)
	{
		if(var->flags & anFlags)
			++i;
	};
	
	return i;
};

void CConVarHandler::UnlinkExternals()
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

NOXREF void CConVarHandler::RemoveHudCvars()
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
NOXREF void CConVarHandler::WriteVariables(CFile *apFile)
{
	NOXREFCHECK;
	
	for(cvar_t *var = cvar_vars; var; var = var->next)
	{
		if(var->flags & FCVAR_ARCHIVE)
			apFile->Printf("%s \"%s\"\n", var->name, var->string);
	};
};