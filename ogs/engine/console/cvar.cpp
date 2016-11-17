/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

// cvar.c -- dynamic variable tracking

#ifdef SERVERONLY 
	#include "qwsvdef.h"
#else
	#include "quakedef.h"
#endif

cvar_t *cvar_vars;
const char *cvar_null_string = "";

/* <1853e> ../engine/cvar.c:26 */
void Cvar_Init(void)
{
#ifndef SWDS
	// TODO: add client code, possibly none
#endif
}

/* <18552> ../engine/cvar.c:30 */
void Cvar_Shutdown(void)
{
	// TODO: Check memory releasing
	cvar_vars = NULL;
}

/*
============
Cvar_FindVar
============
*/
/* <18566> ../engine/cvar.c:40 */
cvar_t *Cvar_FindVar(/*const*/ char *var_name) // CVarGetPointer
{
#ifndef SWDS
	g_engdstAddrs->pfnGetCvarPointer(&var_name);
#endif
	
	for(cvar_t *var = cvar_vars; var; var = var->next)
		if(!Q_strcmp(var_name, var->name)) // Q_stricmp
			return var;
	
	return NULL;
}

/*
============
Cvar_VariableValue
============
*/
/* <18606> ../engine/cvar.c:78 */
float Cvar_VariableValue(/*const*/ char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);
	
	if(!var)
		return 0.0f;
	
	return Q_atof(var->string);
};

/* <18666> ../engine/cvar.c:94 */
NOXREF int Cvar_VariableInt(const char *var_name)
{
	NOXREFCHECK;
	
	cvar_t *var = Cvar_FindVar(var_name);
	
	if(!var)
		return 0;
	
	return Q_atoi(var->string);
}

/*
============
Cvar_VariableString
============
*/
/* <186c6> ../engine/cvar.c:110 */
char *Cvar_VariableString(/*const*/ char *var_name)
{
	cvar_t *var = Cvar_FindVar(var_name);
	
	if(!var)
		return cvar_null_string;
	
	return var->string;
};

/*
============
Cvar_CompleteVariable
============
*/
char *Cvar_CompleteVariable(char *partial)
{
	cvar_t *cvar;
	int len = Q_strlen(partial);
	
	if(!len)
		return NULL;
	
	/*
	// check exact match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!strcmp (partial,cvar->name))
			return cvar->name;
	*/
		
	// check partial match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!Q_strncmp (partial,cvar->name, len))
			return cvar->name;

	return NULL;
}

/*
============
Cvar_Set
============
*/
void Cvar_Set(char *var_name, char *value)
{
	cvar_t	*var;
	qboolean changed;
	
	var = Cvar_FindVar (var_name);
	if (!var)
	{	// there is an error in C code if this happens
		Con_Printf ("Cvar_Set: variable %s not found\n", var_name);
		return;
	}

	changed = Q_strcmp(var->string, value);
	
	Z_Free (var->string);	// free the old value string
	
	var->string = Z_Malloc (Q_strlen(value)+1);
	Q_strcpy (var->string, value);
	var->value = Q_atof (var->string);
	
	if (var->server && changed)
	{
		if (sv.active)
			SV_BroadcastPrintf ("\"%s\" changed to \"%s\"\n", var->name, var->string);
	}
}

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue(char *var_name, float value)
{
	char val[32];
	
	sprintf(val, "%f",value);
	Cvar_Set(var_name, val);
};

/*
============
Cvar_RegisterVariable

Adds a freestanding variable to the variable list.
============
*/
void Cvar_RegisterVariable(cvar_t *variable)
{
	char	*oldstr;
	
// first check to see if it has already been defined
	if (Cvar_FindVar (variable->name))
	{
		Con_Printf ("Can't register variable %s, already defined\n", variable->name);
		return;
	}
	
// check for overlap with a command
	if (Cmd_Exists (variable->name))
	{
		Con_Printf ("Cvar_RegisterVariable: %s is a command\n", variable->name);
		return;
	}
		
// copy the value off, because future sets will Z_Free it
	oldstr = variable->string;
	variable->string = Z_Malloc (Q_strlen(variable->string)+1);	
	Q_strcpy (variable->string, oldstr);
	variable->value = Q_atof (variable->string);
	
// link the variable in
	variable->next = cvar_vars;
	cvar_vars = variable;
}

/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
bool Cvar_Command()
{
	cvar_t *v;

// check variables
	v = Cvar_FindVar (Cmd_Argv(0));
	if (!v)
		return false;
		
// perform a variable print or set
	if (Cmd_Argc() == 1)
	{
		Con_Printf ("\"%s\" is \"%s\"\n", v->name, v->string);
		return true;
	}

	Cvar_Set (v->name, Cmd_Argv(1));
	return true;
};

/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables(FILE *f)
{
	cvar_t	*var;
	
	for (var = cvar_vars ; var ; var = var->next)
		if (var->archive)
			fprintf (f, "%s \"%s\"\n", var->name, var->string);
};