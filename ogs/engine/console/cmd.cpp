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

/* <4e74> ../engine/cmd.c:271 */
void Cmd_StuffCmds_f(void)
{
	int i;
	int s;
	char *build;

	if (Cmd_Argc() != 1)
	{
		Con_Printf("stuffcmds : execute command line parameters\n");
		return;
	}

	if (com_argc <= 1)
	{
		return;
	}

	// Get total length for the command line parameters
	s = 0;
	for (i = 1; i < com_argc; i++)
	{
		if (com_argv[i])
		{
			s += Q_strlen(com_argv[i]) + 1;
		}
	}

	if (s == 0)
	{
		return;
	}

	// Create buffer able to get all arguments
	build = (char *)Z_Malloc(s + com_argc * 2);
	build[0] = 0;

	//  Iterate thru arguments searching for ones starting with +
	for (i = 1; i < com_argc; i++)
	{
		if (com_argv[i] && com_argv[i][0] == '+')
		{
			// Add command or cvar
			Q_strcat(build, &com_argv[i][1]);
			// Then add all following parameters till we meet argument with + or -, which means next command/cvar/parameter
			i++;
			while (com_argv[i] && com_argv[i][0] != '+' && com_argv[i][0] != '-')
			{
				Q_strcat(build, " ");
				Q_strcat(build, com_argv[i]);
				i++;
			}
			// End up with new line which split commands for command processor
			Q_strcat(build, "\n");
			i--;
		}
	}

	if (build[0] != 0)
	{
		Cbuf_InsertText(build);
	}

	// Free buffers
	Z_Free(build);
}

/* <5e43> ../engine/cmd.c:347 */
void Cmd_Exec_f(void)
{
	const char *pszFileName;
	const char *pszFileExt;
	char *pszFileData;
	int nAddLen;
	FileHandle_t hFile;

	if (Cmd_Argc() != 2)
	{
		Con_Printf("exec <filename> : execute a script file\n");
		return;
	}

	pszFileName = Cmd_Argv(1);
	if (!pszFileName || pszFileName[0] == 0)
	{
		return;
	}

	if (Q_strstr(pszFileName, "\\")
		|| Q_strstr(pszFileName, ":")
		|| Q_strstr(pszFileName, "~")
		|| Q_strstr(pszFileName, "..")
		|| *pszFileName == '/')
	{
		Con_Printf("exec %s: invalid path.\n", pszFileName);
		return;
	}

	pszFileExt = COM_FileExtension((char *)pszFileName);
	if (Q_stricmp(pszFileExt, "cfg") && Q_stricmp(pszFileExt, "rc"))
	{
		Con_Printf("exec %s: not a .cfg or .rc file\n", pszFileName);
		return;
	}

	hFile = FS_OpenPathID(pszFileName, "rb", "GAMECONFIG");
	if (!hFile)
	{
		hFile = FS_OpenPathID(pszFileName, "rb", "GAME");
	}
	if (!hFile)
	{
		hFile = FS_Open(pszFileName, "rb");
	}

	if (!hFile)
	{
		if (!Q_strstr(pszFileName, "autoexec.cfg")
			&& !Q_strstr(pszFileName, "userconfig.cfg")
			&& !Q_strstr(pszFileName, "hw/opengl.cfg")
			&& !Q_strstr(pszFileName, "joystick.cfg")
			&& !Q_strstr(pszFileName, "game.cfg"))
		{
			Con_Printf("couldn't exec %s\n", pszFileName);
		}

		return;
	}

	nAddLen = FS_Size(hFile);
	pszFileData = (char *)Mem_Malloc(nAddLen + 1);

	if (!pszFileData)
	{
		Con_Printf("exec: not enough space for %s", pszFileName);
		FS_Close(hFile);
		return;
	}

	FS_Read(pszFileData, nAddLen, 1, hFile);
	pszFileData[nAddLen] = 0;
	FS_Close(hFile);

	Con_DPrintf("execing %s\n", pszFileName);

	if (cmd_text.cursize + nAddLen + 2 < cmd_text.maxsize)
	{
		Cbuf_InsertTextLines(pszFileData);
	}
	else
	{
		char *pszDataPtr = pszFileData;
		while (true)
		{
			Cbuf_Execute();	// TODO: This doesn't obey the rule to first execute commands from the file, and then the others in the buffer
			pszDataPtr = COM_ParseLine(pszDataPtr);

			if (com_token[0] == 0)
			{
				break;
			}

			Cbuf_InsertTextLines(com_token);
		}
	}

	Mem_Free(pszFileData);
}

/* <4ac1> ../engine/cmd.c:493 */
void Cmd_Echo_f(void)
{
	int i;
	int c = Cmd_Argc();

	for (i = 1; i < c; i++)
	{
		Con_Printf("%s ", Cmd_Argv(i));
	}

	Con_Printf("\n");
}

/* <4c38> ../engine/cmd.c:510 */
char *CopyString(char *in)
{
	char *out = (char *)Z_Malloc(Q_strlen(in) + 1);
	Q_strcpy(out, in);
	return out;
}

/* <4c63> ../engine/cmd.c:521 */
void Cmd_Alias_f(void)
{
	cmdalias_t *a;
	const char *s;
	char cmd[MAX_CMD_LINE];
	int i, c;

	if (Cmd_Argc() == 1)
	{
		// Output all aliases
		Con_Printf("Current alias commands:\n");

		for (a = cmd_alias; a; a = a->next)
		{
			Con_Printf("%s : %s", a->name, a->value);	// Don't need \n here, because each alias value is appended with it
		}

		return;
	}

	s = Cmd_Argv(1);

	if (Q_strlen(s) >= MAX_ALIAS_NAME)
	{
		Con_Printf("Alias name is too long\n");
		return;
	}

	if (Cvar_FindVar(s))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

	SetCStrikeFlags();	// TODO: Do this once somewhere at the server start

	if ((g_bIsCStrike || g_bIsCZero) &&
		(!Q_stricmp(s, "cl_autobuy")
		|| !Q_stricmp(s, "cl_rebuy")
		|| !Q_stricmp(s, "gl_ztrick")
		|| !Q_stricmp(s, "gl_ztrick_old")
		|| !Q_stricmp(s, "gl_d3dflip")))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

	// Say hello to my little friend! (c)
	if (g_bIsTFC && (!Q_stricmp(s, "_special") || !Q_stricmp(s, "special")))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

	// Gather arguments into one string
	cmd[0] = 0;
	c = Cmd_Argc();
	for (i = 2; i <= c; i++)
	{
		Q_strncat(cmd, Cmd_Argv(i), MAX_CMD_LINE - 2 - Q_strlen(cmd));	// always have a space for \n or ' ' and \0

		if (i != c)
		{
			Q_strcat(cmd, " ");
		}
	}
	Q_strcat(cmd, "\n");

	// Search for existing alias
	for (a = cmd_alias; a; a = a->next)
	{
		if (!Q_stricmp(a->name, s))
		{
			if (!Q_strcmp(a->value, cmd))
			{
				// Same value on the alias, return
				return;
			}
			// Release value, will realloc
			Z_Free(a->value);
			break;
		}
	}

	if (!a)
	{
		// Alloc new alias
		a = (cmdalias_t *)Z_Malloc(sizeof(cmdalias_t));
		a->next = cmd_alias;
		cmd_alias = a;

		Q_strncpy(a->name, s, ARRAYSIZE(a->name) - 1);
		a->name[ARRAYSIZE(a->name) - 1] = 0;
	}

	a->value = CopyString(cmd);
}

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
	Cmd_AddCommand("stuffcmds", Cmd_StuffCmds_f);
	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("echo", Cmd_Echo_f);
	Cmd_AddCommand("alias", Cmd_Alias_f);
	Cmd_AddCommand("wait", Cmd_Wait_f);
	
#ifndef SERVERONLY
	Cmd_AddCommand("cmd", Cmd_ForwardToServer_f);
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
int EXT_FUNC Cmd_Argc()
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
char *Cmd_Argv(int arg)
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
/*const*/ char * EXT_FUNC Cmd_Args()
{
#ifndef SWDS
	g_engdstAddrs->Cmd_Args();
#endif
	
	if(!cmd_args)
		return cmd_null_string;
	
	return cmd_args;
};

/* <5079> ../engine/cmd.c:632 */
struct cmd_function_s *Cmd_GetFirstCmd(void)
{
	return cmd_functions;
}

/* <55be> ../engine/cmd.c:773 */
NOXREF cmd_function_t *Cmd_FindCmd(char *cmd_name)
{
	NOXREFCHECK;
	
	for(cmd_function_t *cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->name))
			return cmd;
	};
	
	return nullptr;
};

/* <5611> ../engine/cmd.c:791 */
NOXREF cmd_function_t *Cmd_FindCmdPrev(char *cmd_name)
{
	NOXREFCHECK;
	
	if(cmd_functions == nullptr)
		return nullptr;
	
	for(cmd_function_t *cmd = cmd_functions; cmd->next; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->next->name))
			return cmd;
	};
	
	return nullptr;
};

/*
============
Cmd_AddCommand
============
*/
/* <5664> ../engine/cmd.c:812 */
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
			return true; // TRUE
	};
	
	return false; // FALSE
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