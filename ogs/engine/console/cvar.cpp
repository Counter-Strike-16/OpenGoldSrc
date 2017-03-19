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

#include "precompiled.hpp"
#include "console/cvar.hpp"
#include "console/cmd.hpp"
#include "console/console.hpp"
#include "memory/zone.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "system/sizebuf.hpp"
#include "system/system.hpp"
#include "system/unicode_strtools.h"
#include "network/net_msg.hpp"
#include "system/client.hpp"
#include "system/server.hpp"
#include "network/sv_steam3.hpp"
#include "system/sv_log.hpp"

/*
	All cvar names are case insensitive! Values not
*/

cvar_t *cvar_vars;
char cvar_null_string[] = "";

/*
============
Cvar_Init

Reads in all archived cvars
============
*/
void Cvar_Init()
{
#ifndef SWDS
// TODO: add client code, possibly none
#endif

	Cvar_CmdInit();
};

void Cvar_Shutdown()
{
	// TODO: Check memory releasing
	cvar_vars = NULL;
};

/*
============
Cvar_FindVar
============
*/
cvar_t *Cvar_FindVar(const char *var_name)
{
#ifndef SWDS
	g_engdstAddrs.pfnGetCvarPointer(&var_name);
#endif

	cvar_t *var = NULL;

	for(var = cvar_vars; var; var = var->next)
	{
		if(!Q_stricmp(var_name, var->name))
			break;
	};

	return var;
};

NOXREF cvar_t *Cvar_FindPrevVar(const char *var_name)
{
	NOXREFCHECK;

	for(cvar_t *var = cvar_vars; var && var->next; var = var->next)
	{
		if(!Q_stricmp(var_name, var->next->name))
			return var;
	};

	return NULL;
};

/*
============
Cvar_VariableValue
============
*/
float Cvar_VariableValue(const char *var_name)
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
NOXREF int Cvar_VariableInt(const char *var_name)
{
	NOXREFCHECK;

	cvar_t *var = Cvar_FindVar(var_name);

	if(var)
		return Q_atoi(var->string);

	return 0;
};

char *Cvar_VariableString(const char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if(var)
		return var->string;

	return cvar_null_string;
};

NOXREF const char *Cvar_CompleteVariable(const char *search, int forward)
{
	NOXREFCHECK;

	// TODO: We have a cvar name length limit here: prepare for unforeseen
	// consequences!
	static char lastpartial[256];
	char partial[256];
	cvar_t *cvar;
	int len;
	char *pPartial;

	Q_strncpy(partial, search, 255);
	partial[255] = 0;
	len = Q_strlen(partial);

	// Trim tail spaces
	for(pPartial = partial + len - 1; pPartial >= partial && *pPartial == ' '; pPartial--, len--)
		*pPartial = 0;

	if(!len)
		return NULL;

	if(!Q_stricmp(partial, lastpartial))
	{
		// Same partial, find this then next/prev cvar, if any.
		// TODO: But where it match for entered by user partial? Because we store
		// full name
		cvar = Cvar_FindVar(partial);
		if(cvar)
		{
			cvar = forward == 1 ? cvar->next : Cvar_FindPrevVar(cvar->name);
			if(cvar)
			{
				Q_strncpy(lastpartial, cvar->name, 255);
				lastpartial[255] = 0;
				return cvar->name;
			};
		};
	};

	// Find first matching cvar
	for(cvar = cvar_vars; cvar != NULL; cvar = cvar->next)
	{
		if(!Q_strnicmp(partial, cvar->name, len))
		{
			// Store matched cvar name
			Q_strncpy(lastpartial, cvar->name, 255);
			lastpartial[255] = 0;
			return cvar->name;
		};
	};

	return NULL;
};

void EXT_FUNC Cvar_DirectSet_internal(struct cvar_s *var, const char *value)
{
	if(!var || !value)
		return;

	const char *pszValue = value;
	char szNew[MAX_CVAR_VALUE];
	szNew[0] = 0;

	if(var->flags & FCVAR_PRINTABLEONLY)
	{
		if(Q_UnicodeValidate(value))
		{
			Q_strncpy(szNew, value, ARRAYSIZE(szNew) - 1);
			szNew[ARRAYSIZE(szNew) - 1] = 0;
		}
		else
		{
			// Copy only printable chars
			// TODO: Why not UTF-8 too?
			const char *pS = pszValue;
			char *pD = szNew;

			while(*pS)
			{
				if(*pS < 32 || *pS > 126)
				{
					pS++;
					continue;
				};
				
				*pD++ = *pS++;
			};
			
			*pD = 0;
		};

		if(!Q_UnicodeValidate(szNew))
		{
			// Call the artillery
			Q_UnicodeRepair(szNew);
		};

		if(szNew[0] == 0)
			Q_strcpy(szNew, "empty");

		pszValue = szNew;
	};

	if(var->flags & FCVAR_NOEXTRAWHITEPACE)
	{
		if(pszValue != szNew)
		{
			Q_strncpy(szNew, value, ARRAYSIZE(szNew) - 1);
			szNew[ARRAYSIZE(szNew) - 1] = 0;
		};

		Q_StripUnprintableAndSpace(szNew);

		pszValue = szNew;
	};

	qboolean changed = Q_strcmp(var->string, pszValue);

	if(var->flags & FCVAR_USERINFO)
	{
		if(cls.state == ca_dedicated)
		{
			char *info = Cvar_Serverinfo();
			Info_SetValueForKey(info, var->name, pszValue, MAX_INFO_STRING);
			SV_BroadcastCommand("fullserverinfo \"%s\"\n", info);
		}
#ifndef SWDS
		else
		{
			Info_SetValueForKey(cls.userinfo, var->name, pszValue, MAX_INFO_STRING);

			if(changed && cls.state >= ca_connected)
			{
				MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
				SZ_Print(&cls.netchan.message,
				         va("setinfo \"%s\" \"%s\"\n", var->name, pszValue));
			};
		};
#endif
	};

	if(changed && var->flags & FCVAR_SERVER)
	{
		if(!(var->flags & FCVAR_UNLOGGED))
		{
			if(var->flags & FCVAR_PROTECTED)
			{
				Log_Printf("Server cvar \"%s\" = \"%s\"\n", var->name, "***PROTECTED***");
				SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, "***PROTECTED***");
			}
			else
			{
				Log_Printf("Server cvar \"%s\" = \"%s\"\n", var->name, pszValue);
				SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, pszValue);
			};
		};

		if(!(var->flags & FCVAR_PROTECTED))
		{
			Steam_SetCVar(var->name, pszValue);
		}
		else if(pszValue[0] && Q_stricmp(pszValue, "none"))
		{
			Steam_SetCVar(var->name, "1");
		}
		else
		{
			Steam_SetCVar(var->name, "0");
		};
	};

	Z_Free(var->string);
	var->string = (char *)Z_Malloc(Q_strlen(pszValue) + 1);
	Q_strcpy(var->string, pszValue);
	var->value = (float)Q_atof(var->string);
};

void Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	g_RehldsHookchains.m_Cvar_DirectSet.callChain(Cvar_DirectSet_internal, var, value);
};

void Cvar_Set(const char *var_name, const char *value)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if(!var)
	{
		Con_DPrintf("%s: variable \"%s\" not found\n", __FUNCTION__, var_name);
		return;
	};

	Cvar_DirectSet(var, value);
};

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue(const char *var_name, float value)
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

	Cvar_Set(var_name, val);
};

void EXT_FUNC Cvar_RegisterVariable(cvar_t *variable)
{
	char *oldstr;
	cvar_t *v, *c;
	cvar_t dummyvar;

	if(Cvar_FindVar(variable->name))
	{
		Con_Printf("Can't register variable \"%s\", already defined\n",
		           variable->name);
		return;
	};

	if(Cmd_Exists(variable->name))
	{
		Con_Printf("%s: \"%s\" is a command\n", __FUNCTION__, variable->name);
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

NOXREF void Cvar_RemoveHudCvars()
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

// Returns first token if there is more than one, else returns NULL.
const char *Cvar_IsMultipleTokens(const char *varname)
{
	static char firstToken[516];
	int tokens;
	char *name;

	firstToken[0] = 0;
	tokens = 0;
	name = (char *)varname;

	name = COM_Parse(name);

	if(com_token[0] == 0)
	{
		return NULL; // original function returns firstToken in this situation,
		             // which is "", not NULL, but it should be ok this way
	};
	
	if(name == NULL)
		return NULL; // only one token

	// Store first token
	Q_strncpy(firstToken, com_token, ARRAYSIZE(firstToken) - 1);
	firstToken[ARRAYSIZE(firstToken) - 1] = 0;

	// Parse for another token
	name = COM_Parse(name);

	if(com_token[0] == 0)
		return NULL; // only one token

	return firstToken; // multiple tokens, return first one
};

qboolean Cvar_Command()
{
	cvar_t *v;
	const char *arg0 = Cmd_Argv(0);
	const char *firstToken = Cvar_IsMultipleTokens(arg0);

	if(firstToken)
	{
		v = Cvar_FindVar(firstToken);
		if(v)
		{
			Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
			return TRUE;
		};
	}
	else
	{
		v = Cvar_FindVar(arg0);
		if(v)
		{
			if(Cmd_Argc() == 1)
				Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
			else
			{
				if(v->flags & FCVAR_SPONLY && cls.state >= ca_connecting && cl.maxclients > 1)
					Con_Printf("Can't set %s in multiplayer\n", v->name);
				else
					Cvar_Set(v->name, Cmd_Argv(1));
			};

			return TRUE;
		};
	};

	return FALSE;
};

/*
============
Cvar_WriteVariables

Appends lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
NOXREF void Cvar_WriteVariables(FileHandle_t f)
{
	NOXREFCHECK;
	
	for(cvar_t *var = cvar_vars; var; var = var->next)
	{
		if(var->flags & FCVAR_ARCHIVE)
			FS_FPrintf(f, "%s \"%s\"\n", var->name, var->string);
	};
};

void Cmd_CvarListPrintCvar(cvar_t *var, FileHandle_t f)
{
	char szOutstr[256];

#ifdef REHLDS_FIXES
	// Do correct output of string valued cvars
	Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-28s : %16s", var->name, var->string);
#else  // REHLDS_FIXES
	if(var->value == (float)(int)var->value)
		Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-15s : %8i", var->name, (int)var->value);
	else
		Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-15s : %8.3f", var->name, var->value);
#endif // REHLDS_FIXES
	szOutstr[ARRAYSIZE(szOutstr) - 1] = 0;

	if(var->flags & FCVAR_ARCHIVE)
		Q_strcat(szOutstr, ", a");
	
	if(var->flags & FCVAR_SERVER)
		Q_strcat(szOutstr, ", sv");
	
	if(var->flags & FCVAR_USERINFO)
		Q_strcat(szOutstr, ", i");

	Q_strcat(szOutstr, "\n");

	Con_Printf("%s", szOutstr);

	if(f)
		FS_FPrintf(f, "%s", szOutstr);
};

void Cmd_CvarList_f()
{
	cvar_t *var = nullptr;
	
	int iCvars = 0;
	int iArgs = Cmd_Argc();

	const char *partial = "";
	const char *arg1 = "";
	
	int ipLen = 0;
	
	qboolean bAOnly = FALSE;
	qboolean bSOnly = FALSE;
	
	char szTemp[MAX_PATH] = {'\0'};
	
	FileHandle_t f = NULL;
	FileHandle_t fp = NULL;
	
	qboolean bLogging = FALSE;

	if(iArgs > 1)
	{
		arg1 = Cmd_Argv(1);

		if(!Q_stricmp(arg1, "?"))
		{
			Con_Printf("CvarList           : List all cvars\nCvarList [Partial] : "
			           "List cvars starting with 'Partial'\nCvarList log [Partial] : "
			           "Logs cvars to file \"cvarlist.txt\" in the gamedir.\n");
			return;
		};

		if(!Q_stricmp(arg1, "log"))
		{
			// Open log
			int i;
			for(i = 0; i < 100; i++)
			{
				Q_snprintf(szTemp, ARRAYSIZE(szTemp) - 1, "cvarlist%02d.txt", i);
				szTemp[ARRAYSIZE(szTemp) - 1] = 0;

				fp = FS_Open(szTemp, "r");
				
				if(!fp)
					break;
				
				FS_Close(fp);
			};

			if(i >= 100)
			{
				Con_Printf("Can't cvarlist! Too many existing cvarlist output files in "
				           "the gamedir!\n");
				return;
			};

			f = FS_Open(szTemp, "wt");
			
			if(!f)
			{
				Con_Printf("Couldn't open \"%s\" for writing!\n", szTemp);
				return;
			};
			
			bLogging = TRUE;

			// Get next argument into partial, if present
			if(iArgs > 2)
			{
				partial = Cmd_Argv(2);
				ipLen = Q_strlen(partial);
			};
		}
		else if(!Q_stricmp(arg1, "-a"))
			bAOnly = TRUE;
		else if(!Q_stricmp(arg1, "-s"))
			bSOnly = TRUE;
		else
		{
			partial = arg1;
			ipLen = Q_strlen(partial);
		};
	};

	// Print cvars
	Con_Printf("CVar List\n--------------\n");

	for(var = cvar_vars; var; var = var->next)
	{
		if(bAOnly && !(var->flags & FCVAR_ARCHIVE))
			continue;
		
		if(bSOnly && !(var->flags & FCVAR_SERVER))
			continue;
		
		if(partial && Q_strnicmp(var->name, partial, ipLen))
			continue;

		Cmd_CvarListPrintCvar(var, f);
		iCvars++;
	};

	if(partial && *partial)
	{
		Con_Printf("--------------\n%3i CVars for [%s]\nCvarList ? for syntax\n",
		           iCvars,
		           partial);
	}
	else
	{
		Con_Printf("--------------\n%3i Total CVars\nCvarList ? for syntax\n",
		           iCvars);
	};

	// Close log
	if(bLogging)
	{
		FS_Close(f);
		Con_Printf("cvarlist logged to %s\n", szTemp);
	};
};

NOXREF int Cvar_CountServerVariables()
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

void Cvar_UnlinkExternals()
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

void Cvar_CmdInit()
{
	Cmd_AddCommand("cvarlist", Cmd_CvarList_f);
};