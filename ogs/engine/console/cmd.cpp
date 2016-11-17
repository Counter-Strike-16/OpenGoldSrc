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

#include "console/cmd.h"

/*
=============================================================================

					COMMAND EXECUTION

=============================================================================
*/

typedef struct cmd_function_s
{
	struct cmd_function_s	*next;
	char					*name;
	xcommand_t				function;
} cmd_function_t;

static int		cmd_argc;
static char		*cmd_argv[MAX_STRING_TOKENS];
static char		*cmd_null_string = "";
static char		cmd_args[MAX_STRING_CHARS];

static cmd_function_t *cmd_functions; // possible commands to execute

/*
============
Cmd_Init
============
*/
void Cmd_Init()
{
	//
	// register our commands
	//
	AddCommand("stuffcmds", Cmd_StuffCmds_f);
	AddCommand("exec", Cmd_Exec_f);
	AddCommand("echo", Cmd_Echo_f);
	AddCommand("alias", Cmd_Alias_f);
	AddCommand("wait", Cmd_Wait_f);
	
#ifndef SERVERONLY
	AddCommand("cmd", Cmd_ForwardToServer_f);
#endif
};

/* <5521> ../engine/cmd.c:663 */
void Cmd_Shutdown(void)
{
	for (int i = 0; i < cmd_argc; i++)
		Z_Free(cmd_argv[i]);
	
	Q_memset(cmd_argv, 0, sizeof(cmd_argv));
	cmd_argc = 0;
	cmd_args = NULL;

	cmd_functions = NULL;	// TODO: Check that memory from functions is released too
}

/*
============
Cmd_Argc
============
*/
/* <5536> ../engine/cmd.c:677 */
int EXT_FUNC Cmd_GetArgCount()
{
#ifndef SWDS
	g_engdstAddrs->Cmd_Argc();
#endif
	
	return cmd_argc;
};

/*
============
Cmd_Argv
============
*/
char *Cmd_GetArgValue(int arg)
{
	if(arg >= cmd_argc)
		return cmd_null_string;
	
	return cmd_argv[arg];	
};

/*
============
Cmd_Args

Returns a single string containing argv(1) to argv(argc()-1)
============
*/
/* <5565> ../engine/cmd.c:703 */
/*const*/ char * EXT_FUNC Cmd_GetArgString()
{
#ifndef SWDS
	g_engdstAddrs->Cmd_Args();
#endif
	
	if(!cmd_args)
		return cmd_null_string;
	
	return cmd_args;
};

/*
============
Cmd_AddCommand
============
*/
void Cmd_AddCommand(char *cmd_name, xcommand_t function)
{
	// because hunk allocation would get stomped
	if(host_initialized)
		Sys_Error("Cmd_AddCommand after host_initialized");
	
	// fail if the command is a variable name
	if(Cvar_VariableString(cmd_name)[0])
	{
		Con_Printf("Cmd_AddCommand: %s already defined as a var\n", cmd_name);
		return;
	};
	
	cmd_function_t *cmd = NULL;
	
	// fail if the command already exists
	for(cmd = cmd_functions; cmd; cmd=cmd->next)
	{
		if (!Q_strcmp(cmd_name, cmd->name))
		{
			Con_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
			return;
		};
	};
	
	cmd = Hunk_Alloc(sizeof(cmd_function_t)); // Z_Malloc
	cmd->name = cmd_name;
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;
};

/*
============
Cmd_Exists
============
*/
/* <5af2> ../engine/cmd.c:1035 */
/*qboolean*/ bool Cmd_Exists(/*const*/ char *cmd_name)
{
	for(cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if(!Q_strcmp(cmd_name,cmd->name))
			return true;
	};
	
	return false;
};

/*
============
Cmd_CompleteCommand
============
*/
/* <5b30> ../engine/cmd.c:1055 */
NOXREF char *Cmd_CompleteCommand(char *partial)
{
	NOXREFCHECK;
	
	int len = Q_strlen(partial);
	
	if(!len)
		return NULL;
	
	// check functions
	for(cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
		if(!Q_strncmp(partial,cmd->name, len))
			return cmd->name;
	
	return NULL;
};