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
/// @brief console command system

#include "precompiled.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "console/Console.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/sizebuf.hpp"
#include "system/System.hpp"
#include "system/Host.hpp"
#include "filesystem/FileSystem.hpp"
#include "network/net_msg.hpp"
#include "network/protocol.hpp"
#include "system/client.hpp"
#include "system/server.hpp"

const int MAX_ARGS = 80;

int cmd_argc;
char *cmd_argv[MAX_ARGS];

// Complete arguments string
char *cmd_args = NULL;

cmd_source_t cmd_source;
cmdalias_t *cmd_alias;

// int trashtest;
// int *trashspot;

char *const cmd_null_string = "";

// Crappy
CCmdBuffer *gpCmdBuffer = nullptr;

//=============================================================================

/*
==============================================================================

						SCRIPT COMMANDS

==============================================================================
*/

/*
============
Cmd_Wait_f

Causes execution of the remainder of the command buffer to be delayed until
next frame.  This allows commands like:
bind g "impulse 5 ; +attack ; wait ; -attack ; impulse 2"
============
*/
void Cmd_Wait_f()
{
	gpCmdBuffer->SetWait(true);
}

/*
===============
Cmd_StuffCmds_f

Adds command line parameters as script statements
Commands lead with a +, and continue until a - or another +
quake +prog jctest.qp +cmd amlev1
quake -nosound +cmd amlev1
===============
*/
void Cmd_StuffCmds_f()
{
	int i;
	int s;
	char *build;

	if(Cmd_Argc() != 1)
	{
		Con_Printf("stuffcmds : execute command line parameters\n");
		return;
	}

	if(com_argc <= 1)
		return;

	// Get total length for the command line parameters
	s = 0;
	for(i = 1; i < com_argc; i++)
	{
		if(com_argv[i])
			s += Q_strlen(com_argv[i]) + 1;
	}

	if(s == 0)
		return;

	// Create buffer able to get all arguments
	build = (char *)Z_Malloc(s + com_argc * 2);
	build[0] = 0;

	//  Iterate thru arguments searching for ones starting with +
	for(i = 1; i < com_argc; i++)
	{
		if(com_argv[i] && com_argv[i][0] == '+')
		{
			// Add command or cvar
			Q_strcat(build, &com_argv[i][1]);
			// Then add all following parameters till we meet argument with + or -,
			// which means next command/cvar/parameter
			i++;
			while(com_argv[i] && com_argv[i][0] != '+' && com_argv[i][0] != '-')
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

	if(build[0] != 0)
		Cbuf_InsertText(build);

	// Free buffers
	Z_Free(build);
}

/*
===============
Cmd_Exec_f
===============
*/
void Cmd_Exec_f()
{
	const char *pszFileName;
	const char *pszFileExt;
	char *pszFileData;
	int nAddLen;
	FileHandle_t hFile;

	if(Cmd_Argc() != 2)
	{
		Con_Printf("exec <filename> : execute a script file\n");
		return;
	}

	pszFileName = Cmd_Argv(1);
	
	if(!pszFileName || pszFileName[0] == 0)
		return;

	if(Q_strstr(pszFileName, "\\") || Q_strstr(pszFileName, ":") ||
	   Q_strstr(pszFileName, "~") || Q_strstr(pszFileName, "..") ||
	   *pszFileName == '/')
	{
		Con_Printf("exec %s: invalid path.\n", pszFileName);
		return;
	}

	pszFileExt = COM_FileExtension((char *)pszFileName);
	if(Q_stricmp(pszFileExt, "cfg") && Q_stricmp(pszFileExt, "rc"))
	{
		Con_Printf("exec %s: not a .cfg or .rc file\n", pszFileName);
		return;
	}

	hFile = FS_OpenPathID(pszFileName, "rb", "GAMECONFIG");
	if(!hFile)
	{
		hFile = FS_OpenPathID(pszFileName, "rb", "GAME");
	}
	if(!hFile)
	{
		hFile = FS_Open(pszFileName, "rb");
	}

	if(!hFile)
	{
		if(!Q_strstr(pszFileName, "autoexec.cfg") &&
		   !Q_strstr(pszFileName, "userconfig.cfg") &&
		   !Q_strstr(pszFileName, "hw/opengl.cfg") &&
		   !Q_strstr(pszFileName, "joystick.cfg") &&
		   !Q_strstr(pszFileName, "game.cfg"))
		{
			Con_Printf("couldn't exec %s\n", pszFileName);
		}

		return;
	}

	nAddLen = FS_Size(hFile);
	pszFileData = (char *)Mem_Malloc(nAddLen + 1);

	if(!pszFileData)
	{
		Con_Printf("exec: not enough space for %s", pszFileName);
		FS_Close(hFile);
		return;
	}

	FS_Read(pszFileData, nAddLen, 1, hFile);
	pszFileData[nAddLen] = 0;
	FS_Close(hFile);

	Con_DPrintf("execing %s\n", pszFileName);

	if(cmd_text.cursize + nAddLen + 2 < cmd_text.maxsize)
	{
		Cbuf_InsertTextLines(pszFileData);
	}
	else
	{
		char *pszDataPtr = pszFileData;
		while(true)
		{
			Cbuf_Execute(); // TODO: This doesn't obey the rule to first execute
			                // commands from the file, and then the others in the
			                // buffer
			pszDataPtr = COM_ParseLine(pszDataPtr);

			if(com_token[0] == 0)
			{
				break;
			}

			Cbuf_InsertTextLines(com_token);
		}
	}

	Mem_Free(pszFileData);
};

/*
===============
Cmd_Echo_f

Just prints the rest of the line to the console
===============
*/
void Cmd_Echo_f()
{
	for(int i = 1; i < Cmd_Argc(); ++i)
		pConsole->Printf("%s ", Cmd_Argv(i));

	pConsole->Printf("\n");
};

char *CopyString(char *in)
{
	char *out = (char *)Z_Malloc(Q_strlen(in) + 1);
	Q_strcpy(out, in);
	return out;
};

/*
===============
Cmd_Alias_f

Creates a new command that executes a command string (possibly ; seperated)
===============
*/
void Cmd_Alias_f()
{
	cmdalias_t *a;
	const char *s;
	char cmd[MAX_CMD_LINE];
	int i, c;

	if(Cmd_Argc() == 1)
	{
		// Output all aliases
		Con_Printf("Current alias commands:\n");

		for(a = cmd_alias; a; a = a->next)
		{
			Con_Printf("%s : %s", a->name, a->value); // Don't need \n here, because
			                                          // each alias value is appended
			                                          // with it
		}

		return;
	}

	s = Cmd_Argv(1);

	if(Q_strlen(s) >= MAX_ALIAS_NAME)
	{
		Con_Printf("Alias name is too long\n");
		return;
	}

	if(Cvar_FindVar(s))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

#ifndef REHLDS_FIXES
	SetCStrikeFlags();	// DONE: Do this once somewhere at the server start
#endif

	if((g_bIsCStrike || g_bIsCZero) &&
	   (!Q_stricmp(s, "cl_autobuy") || !Q_stricmp(s, "cl_rebuy") ||
	    !Q_stricmp(s, "gl_ztrick") || !Q_stricmp(s, "gl_ztrick_old") ||
	    !Q_stricmp(s, "gl_d3dflip")))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

	// Say hello to my little friend! (c)
	if(g_bIsTFC && (!Q_stricmp(s, "_special") || !Q_stricmp(s, "special")))
	{
		Con_Printf("Alias name is invalid\n");
		return;
	}

	// Gather arguments into one string
	cmd[0] = 0;
	c = Cmd_Argc();
	for(i = 2; i <= c; i++)
	{
		Q_strncat(cmd, Cmd_Argv(i),
		          MAX_CMD_LINE - 2 - Q_strlen(cmd)); // always have a space for \n or ' ' and \0

		if(i != c)
		{
			Q_strcat(cmd, " ");
		}
	}
	Q_strcat(cmd, "\n");

	// Search for existing alias
	for(a = cmd_alias; a; a = a->next)
	{
		if(!Q_stricmp(a->name, s))
		{
			if(!Q_strcmp(a->value, cmd))
			{
				// Same value on the alias, return
				return;
			}
			// Release value, will realloc
			Z_Free(a->value);
			break;
		}
	}

	if(!a)
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

void Cmd_Init()
{
	Cmd_AddCommand("stuffcmds", Cmd_StuffCmds_f);
	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("echo", Cmd_Echo_f);
	Cmd_AddCommand("alias", Cmd_Alias_f);
	Cmd_AddCommand("cmd", Cmd_ForwardToServer);
	Cmd_AddCommand("wait", Cmd_Wait_f);
	Cmd_AddCommand("cmdlist", Cmd_CmdList_f);
}

void Cmd_Shutdown()
{
	for(int i = 0; i < cmd_argc; i++)
		Z_Free(cmd_argv[i]);
	
	Q_memset(cmd_argv, 0, sizeof(cmd_argv));
	cmd_argc = 0;
	cmd_args = NULL;

	cmd_functions = NULL; // TODO: Check that memory from functions is released too
}



/*
Parses the given string into command line tokens.
Takes a null terminated string. Does not need to be \n terminated.
Breaks the string up into arg tokens.
*/
void EXT_FUNC Cmd_TokenizeString(char *text)
{
	int i;
	int arglen;

	// clear args from the last string
	for(i = 0; i < cmd_argc; i++)
	{
		Z_Free(cmd_argv[i]);
		cmd_argv[i] = NULL;
	};
	
	cmd_argc = 0;
	cmd_args = NULL;

	while(true)
	{
// Skip whitespace up to a \n
#ifdef REHLDS_FIXES
		while(*text && (uint8_t)*text <= ' ' && *text != '\n')
#else  // REHLDS_FIXES
		while(*text && *text <= ' ' && *text != '\n')
#endif // REHLDS_FIXES
		{
			++text;
		}

		// A newline separates commands in the buffer
		if(*text == '\n' || *text == 0)
		{
			break;
		}

		// Store complete args string pointer
		if(cmd_argc == 1)
		{
			cmd_args = text;
		}

		// Break into token
		text = COM_Parse(text);

		// If nothing was parsed
		if(!text)
			return;

		arglen = Q_strlen(com_token) + 1;
		if(arglen >= 516) // TODO: What is that magic number?
			return;

		cmd_argv[cmd_argc] = (char *)Z_Malloc(arglen);
		Q_strcpy(cmd_argv[cmd_argc++], com_token);

		if(cmd_argc >= MAX_CMD_TOKENS)
		{
			// Will not fit any more
			return;
		}
	}
}

cmd_function_t *Cmd_FindCmdPrev(char *cmd_name)
{
	cmd_function_t *cmd = NULL;

	if(cmd_functions == NULL)
		return NULL;

	for(cmd = cmd_functions; cmd->next; cmd = cmd->next)
	{
		if(!Q_stricmp(cmd_name, cmd->next->name))
			return cmd;
	}

	return NULL;
}

// Use this for engine inside call only, not from user code, because it doesn't
// alloc string for the name.
void Cmd_AddCommand(char *cmd_name, xcommand_t function)
{
	// Create cmd_function
	cmd_function_t *cmd = (cmd_function_t *)Hunk_Alloc(sizeof(cmd_function_t));
	
	cmd->name = cmd_name;
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = 0;

	Cmd_InsertCommand(cmd);
}

// It almost fully duplicates the Cmd_AddCommand

// Use this for call from user code, because it alloc string for the name.
void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function, int flag)
{
	cmd_function_t *cmd;

	// Check in variables list
	if(Cvar_FindVar(cmd_name) != NULL)
	{
		Con_Printf("%s: \"%s\" already defined as a var\n", __FUNCTION__, cmd_name);
		return;
	}

	// Check if this command is already defined
	if(Cmd_Exists(cmd_name))
	{
		Con_Printf("%s: \"%s\" already defined\n", __FUNCTION__, cmd_name);
		return;
	}

	// Create cmd_function
	cmd = (cmd_function_t *)Mem_Malloc(sizeof(cmd_function_t));
	cmd->name = CopyString(cmd_name); // alloc string, so it will not dissapear on
	                                  // side modules unloading and to maintain
	                                  // the same name during run
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = flag;

	Cmd_InsertCommand(cmd);
}

NOXREF void Cmd_AddWrapperCommand(char *cmd_name, xcommand_t function)
{
	NOXREFCHECK;

	Cmd_AddMallocCommand(cmd_name, function, FCMD_WRAPPER_COMMAND);
}

/*
============
Cmd_RemoveCommand
============
*/
void EXT_FUNC Cmd_RemoveCmd(char *cmd_name)
{
	auto prev = Cmd_FindCmdPrev(cmd_name);
	
	if(prev)
	{
		auto cmd = prev->next;
		prev->next = cmd->next;

		Z_Free(cmd->name);
		Mem_Free(cmd);
	};
}

void Cmd_RemoveMallocedCmds(int flag)
{
	cmd_function_t *c, **p;

	c = cmd_functions;
	p = &cmd_functions;
	while(c)
	{
		if(c->flags & flag)
		{
			*p = c->next;
			Z_Free(c->name);
			Mem_Free(c);
			c = *p;
			continue;
		}
		p = &c->next;
		c = c->next;
	}
}

NOXREF void Cmd_RemoveHudCmds()
{
	NOXREFCHECK;

	Cmd_RemoveMallocedCmds(FCMD_HUD_COMMAND);
}

void Cmd_RemoveGameCmds()
{
	Cmd_RemoveMallocedCmds(FCMD_GAME_COMMAND);
}

void Cmd_RemoveWrapperCmds()
{
	Cmd_RemoveMallocedCmds(FCMD_WRAPPER_COMMAND);
}

NOXREF char *Cmd_CompleteCommand(char *search, int forward)
{
	NOXREFCHECK;

	// TODO: We have a command name length limit here: prepare for unforeseen
	// consequences!
	static char lastpartial[256];
	char partial[256];
	cmd_function_t *cmd;
	int len;
	char *pPartial;

	Q_strncpy(partial, search, 255);
	partial[255] = 0;
	len = Q_strlen(partial);

	// Trim tail spaces
	for(pPartial = partial + len - 1; pPartial >= partial && *pPartial == ' ';
	    pPartial--, len--)
	{
		*pPartial = 0;
	}

	if(!len)
	{
		return NULL;
	}

	if(!Q_stricmp(partial, lastpartial))
	{
		// Same partial, find this then next/prev cvar, if any.
		// TODO: But where it match for entered by user partial? Because we store
		// full name
		cmd = Cmd_FindCmd(partial);
		if(cmd)
		{
			cmd = forward == 1 ? cmd->next : Cmd_FindCmdPrev(cmd->name);
			if(cmd)
			{
				Q_strncpy(lastpartial, cmd->name, 255);
				lastpartial[255] = 0;
				return cmd->name;
			}
		}
	}

	// Find first matching cmd
	for(cmd = cmd_functions; cmd != NULL; cmd = cmd->next)
	{
		if(!Q_strnicmp(partial, cmd->name, len))
		{
			// Store matched cvar name
			Q_strncpy(lastpartial, cmd->name, 255);
			lastpartial[255] = 0;
			return cmd->name;
		}
	}

	return NULL;
}

bool EXT_FUNC ValidateCmd_API(const char *cmd, cmd_source_t src, IGameClient *client)
{
	return true;
}

void EXT_FUNC Cmd_ExecuteString_internal(const char *cmdName, cmd_source_t src, IGameClient *client)
{
	// Search in functions
	cmd_function_t *cmd = cmd_functions;
	
	while(cmd)
	{
		if(!Q_stricmp(cmdName, cmd->name))
		{
			cmd->function();

			if(cls.demorecording && (cmd->flags & FCMD_HUD_COMMAND) && !cls.spectator)
				CL_RecordHUDCommand(cmd->name);

			return;
		}

		cmd = cmd->next;
	}

	// Search in aliases
	cmdalias_t *a = cmd_alias;
	while(a)
	{
		if(!Q_stricmp(cmdName, a->name))
		{
			Cbuf_InsertText(a->value);
			return;
		}

		a = a->next;
	}

	// Search in cvars
	if(!Cvar_Command())
	{
		// Send to a server if nothing processed locally and connected
		if(cls.state >= ca_connected)
			Cmd_ForwardToServer();
#ifdef REHLDS_FIXES
		else if(sv_echo_unknown_cmd.string[0] == '1' && src == src_command)
			Con_Printf("unknown command \"%s\"\n", cmdName);
#endif
	}
}

/*
============
Cmd_ExecuteString

A complete command line has been parsed, so try to execute it
FIXME: lookupnoadd the token to speed search?
============
*/
void Cmd_ExecuteString(char *text, cmd_source_t src)
{
	cmd_source = src;
	Cmd_TokenizeString(text);
	
	// execute the command line
	
	// no tokens
	if(!Cmd_Argc())
		return;

	IGameClient *cl = (src == src_client) ? GetRehldsApiClient(host_client) : NULL;
	
	if(!g_RehldsHookchains.m_ValidateCommand.callChain(ValidateCmd_API, cmd_argv[0], src, cl))
		return;

	g_RehldsHookchains.m_ExecuteServerStringCmd.callChain(Cmd_ExecuteString_internal, cmd_argv[0], src, cl);
}

qboolean Cmd_ForwardToServerInternal(sizebuf_t *pBuf)
{
	const char *cmd_name = Cmd_Argv(0);

	if(cls.state <= ca_disconnected)
	{
		if(Q_stricmp(cmd_name, "setinfo"))
			Con_Printf("Can't \"%s\", not connected\n", cmd_name);

		return FALSE;
	}

	if(cls.demoplayback || g_bIsDedicatedServer)
		return FALSE;

	char tempData[4096];
	sizebuf_t tempBuf;

	sprintf((char*)tempBuf.buffername, "%s::tempBuf", __FUNCTION__);

	tempBuf.data = (byte *)tempData;
	tempBuf.maxsize = 4096;
	tempBuf.cursize = 0;
	tempBuf.flags = SIZEBUF_ALLOW_OVERFLOW;

	MSG_WriteByte(&tempBuf, clc_stringcmd);

	if(Q_stricmp(cmd_name, "cmd"))
	{
		SZ_Print(&tempBuf, cmd_name);
		SZ_Print(&tempBuf, " ");
	}

	SZ_Print(&tempBuf, Cmd_Argc() <= 1 ? "\n" : Cmd_Args());

	if(tempBuf.flags & SIZEBUF_OVERFLOWED)
		return FALSE;

	if(tempBuf.cursize + pBuf->cursize <= pBuf->maxsize)
	{
		SZ_Write(pBuf, tempBuf.data, tempBuf.cursize);
		return TRUE;
	}

	return FALSE;
}

void Cmd_ForwardToServer()
{
	if(Q_stricmp(Cmd_Argv(0), "cmd") || Q_stricmp(Cmd_Argv(1), "dlfile"))
		Cmd_ForwardToServerInternal(&cls.netchan.message);
}

qboolean Cmd_ForwardToServerUnreliable()
{
	return Cmd_ForwardToServerInternal(&cls.datagram);
}

/*
================
Cmd_CheckParm

Returns the position (1 to argc-1) in the command's argument list
where the given parameter apears, or 0 if not present
================
*/
NOXREF int Cmd_CheckParm(char *parm)
{
	NOXREFCHECK;

	if(!parm)
		CSystem::Error("%s: NULL", __FUNCTION__); // it's not that critical

	for(int i = 1; i < Cmd_Argc(); ++i)
	{
		if(!Q_stricmp(Cmd_Argv(i), parm)) // Q_strcasecmp
			return i;
	};

	return 0;
}

void Cmd_CmdList_f()
{
	cmd_function_t *cmd;
	int iCmds;
	int iArgs;
	const char *partial, *arg1;
	int ipLen;
	char szTemp[MAX_PATH];
	FileHandle_t f;
	FileHandle_t fp;
	qboolean bLogging;

	iCmds = 0;
	partial = NULL;
	f = NULL;
	fp = NULL;
	bLogging = FALSE;

	iArgs = Cmd_Argc();
	if(iArgs > 1)
	{
		arg1 = Cmd_Argv(1);

		if(!Q_stricmp(arg1, "?"))
		{
			Con_Printf("CmdList           : List all commands\nCmdList [Partial] : "
			           "List commands starting with 'Partial'\nCmdList log [Partial] "
			           ": Logs commands to file \"cmdlist.txt\" in the gamedir.\n");
			return;
		}

		if(!Q_stricmp(arg1, "log"))
		{
			// Open log
			int i;
			for(i = 0; i < 100; i++)
			{
				Q_snprintf(szTemp, ARRAYSIZE(szTemp) - 1, "cmdlist%02d.txt", i);
				szTemp[ARRAYSIZE(szTemp) - 1] = 0;

				fp = FS_Open(szTemp, "r");
				if(!fp)
				{
					break;
				}
				FS_Close(fp);
			}

			if(i >= 100)
			{
				Con_Printf("Can't cmdlist! Too many existing cmdlist output files in "
				           "the gamedir!\n");
				return;
			}

			f = FS_Open(szTemp, "wt");
			if(!f)
			{
				Con_Printf("Couldn't open \"%s\" for writing!\n", szTemp);
				return;
			}
			bLogging = TRUE;

			// Get next argument into partial, if present
			if(iArgs >= 2)
			{
				partial = Cmd_Argv(2);
				ipLen = Q_strlen(partial);
			}
		}
		else
		{
			partial = arg1;
			ipLen = Q_strlen(partial);
		}
	}

	// Print commands
	Con_Printf("Command List\n--------------\n");

	for(cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if(partial && Q_strnicmp(cmd->name, partial, ipLen))
			continue;

		Con_Printf("%s\n", cmd->name);

		if(bLogging)
			FS_FPrintf(f, "%s\n", cmd->name);

		iCmds++;
	}

	if(partial && *partial)
		Con_Printf("--------------\n%3i Commands for [%s]\nCmdList ? for syntax\n",
		           iCmds,
		           partial);
	else
		Con_Printf("--------------\n%3i Total Commands\nCmdList ? for syntax\n",
		           iCmds);

	// Close log
	if(bLogging)
	{
		FS_Close(f);
		Con_Printf("cmdlist logged to %s\n", szTemp);
	}
}
