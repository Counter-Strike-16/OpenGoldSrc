/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief command buffer and command execution

#pragma once

#include "common/commontypes.h"
#include "common/maintypes.h"
#include "rehlds/cmd_rehlds.h"

// All command/alias names are case insensitive! Arguments not

const int MAX_CMD_BUFFER = 16384;
const int MAX_CMD_TOKENS = 80;
const int MAX_CMD_LINE = 1024;

typedef struct sizebuf_s sizebuf_t;
typedef struct cmdalias_s cmdalias_t;

//===========================================================================

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but remote
servers can also send across commands and entire text files can be execed.

The + command line options are also added to the command buffer.

*/

// clang-format off
#ifdef HOOK_ENGINE
	#define cmd_argc (*pcmd_argc)
	#define cmd_argv (*pcmd_argv)
	#define cmd_args (*pcmd_args)

	#define cmd_text (*pcmd_text)
	#define cmd_source (*pcmd_source)
	#define cmd_wait (*pcmd_wait)

	#define cmd_functions (*pcmd_functions)
	#define cmd_alias (*pcmd_alias)
#endif // HOOK_ENGINE
// clang-format on

extern int cmd_argc;
extern char *cmd_argv[80];
extern char *cmd_args;

extern sizebuf_t cmd_text;
extern cmd_source_t cmd_source;
extern qboolean cmd_wait;

extern cmd_function_t *cmd_functions;
extern cmdalias_t *cmd_alias;

/**
*	allocates an initial text buffer that will grow as needed
*/
void Cbuf_Init();

/**
*	as new commands are generated from the console or keybindings,
*	the text is added to the end of the command buffer.
*/
void Cbuf_AddText(const char *text);

/**
*	when a command wants to issue other commands immediately, the text is
*	inserted at the beginning of the buffer, before any remaining unexecuted
*	commands.
*/
void Cbuf_InsertText(const char *text);

/**
*	Inserts text at the beginning of the buffer, adding newlines between the text and the remainder of the buffer.
*	@see Cbuf_InsertText
*/
void Cbuf_InsertTextLines(const char *text);

/**
*	Pulls off \n terminated lines of text from the command buffer and sends
*	them through Cmd_ExecuteString.  Stops when the buffer is empty.
*	Normally called once per frame, but may be explicitly invoked.
*	Do not call inside a command function!
*/
void Cbuf_Execute();

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

Commands can come from three sources, but the handler functions may choose
to disallow the action or forward it to a remote server if the source is
not apropriate.

*/

void Cmd_Wait_f();
void Cmd_StuffCmds_f();
void Cmd_Exec_f();
void Cmd_Echo_f();
void Cmd_Alias_f();
void Cmd_CmdList_f();

char *CopyString(char *in);

struct cmd_function_s *Cmd_GetFirstCmd();

void Cmd_Init();
void Cmd_Shutdown();

// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are always safe.
int Cmd_Argc();
const char *Cmd_Argv(int arg);
const char *Cmd_Args();

/**
*	Takes a null terminated string.  Does not need to be /n terminated.
*	Breaks the string up into arg tokens.
*/
void Cmd_TokenizeString(const char *text);

NOXREF cmd_function_t *Cmd_FindCmd(const char *cmd_name);
cmd_function_t *Cmd_FindCmdPrev(const char *cmd_name);

/**
*	called by the init functions of other parts of the program to
*	register commands and functions to call for them.
*	The cmd_name is referenced later, so it should not be in temp memory
*	If function is NULL, the command will be forwarded to the server
*	as a clc_stringcmd instead of executed locally
*/
void Cmd_AddCommand(const char *cmd_name, xcommand_t function);

void Cmd_AddMallocCommand(const char *cmd_name, xcommand_t function, int flag);
NOXREF void Cmd_AddHUDCommand(const char *cmd_name, xcommand_t function);
NOXREF void Cmd_AddWrapperCommand(const char *cmd_name, xcommand_t function);
void Cmd_AddGameCommand(const char *cmd_name, xcommand_t function);

void Cmd_RemoveCmd(const char *cmd_name);
void Cmd_RemoveMallocedCmds(int flag);
NOXREF void Cmd_RemoveHudCmds();
void Cmd_RemoveGameCmds();
void Cmd_RemoveWrapperCmds();

/**
*	used by the cvar code to check for cvar / command name overlap
*/
qboolean Cmd_Exists(const char *cmd_name); // return bool?

/**
*	attempts to match a partial command for automatic command line completion
*	returns NULL if nothing fits
*/
NOXREF const char *Cmd_CompleteCommand(const char *partial, int forward); // const char? bool forward?

/**
*	Parses a single line of text into arguments and tries to execute it
*	as if it was typed at the console.
*	The text can come from the command buffer, a remote client, or stdin.
*/
void Cmd_ExecuteString(const char *text, cmd_source_t src);

/**
*	adds the current command line as a clc_stringcmd to the client message.
*	things like godmode, noclip, etc, are commands directed to the server,
*	so when they are typed in at the console, they will need to be forwarded.
*/
qboolean Cmd_ForwardToServerInternal(sizebuf_t *pBuf);
void Cmd_ForwardToServer();
qboolean Cmd_ForwardToServerUnreliable();

/**
*	Returns the position (1 to argc-1) in the command's argument list
*	where the given parameter apears, or 0 if not present
*/
NOXREF int Cmd_CheckParm(const char *parm);

cmdalias_t *Cmd_GetAliasList();