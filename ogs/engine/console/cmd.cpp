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

CCmdSystem::CCmdSystem()
{
};

CCmdSystem::CCmdSystem()
{
};

/*
============
Cmd_Init
============
*/
void CCmdSystem::Init()
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

/*
============
Cmd_Argc
============
*/
int CCmdSystem::GetArgCount()
{
	return cmd_argc;
};

/*
============
Cmd_Argv
============
*/
char *CCmdSystem::GetArgValue(int arg)
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
char *CCmdSystem::GetArgString()
{
	if(!cmd_args)
		return cmd_null_string;
	
	return cmd_args;
};

/*
============
Cmd_AddCommand
============
*/
void CCmdSystem::AddCommand(char *cmd_name, xcommand_t function)
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
bool CCmdSystem::IsCmdExists(char *cmd_name)
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
char *CCmdSystem::CompleteCommand(char *partial)
{
	int len = Q_strlen(partial);
	
	if(!len)
		return NULL;
	
	// check functions
	for(cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
		if(!Q_strncmp(partial,cmd->name, len))
			return cmd->name;
	
	return NULL;
};