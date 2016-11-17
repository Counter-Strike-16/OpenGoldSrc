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

// cmd.h -- Console command system

#pragma once

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

Commands can come from three sources, but the handler functions may choose
to dissallow the action or forward it to a remote server if the source is
not apropriate.

*/

//===========================================================================

typedef void (*xcommand_t)();

typedef enum
{
	src_client,		// came in over a net connection as a clc_stringcmd
					// host_client will be valid during this state.
	src_command		// from the command buffer
} cmd_source_t;

extern	cmd_source_t cmd_source;

void Cmd_StuffCmds_f();

void Cmd_Init();

// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_stringcmd instead of executed locally
void AddCommand(char *cmd_name, xcommand_t function);

// used by the cvar code to check for cvar / command name overlap
bool Cmd_Exists(char *cmd_name);

// attempts to match a partial command for automatic command line completion
// returns NULL if nothing fits
char *Cmd_CompleteCommand(char *partial);

// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are always safe
int Cmd_Argc();
/*const*/ char *Cmd_Argv(int arg);
/*const*/ char *Cmd_Args();

// Returns the position (1 to argc-1) in the command's argument list
// where the given parameter apears, or 0 if not present
int Cmd_CheckParm(char *parm);

// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.
void Cmd_TokenizeString(char *text);

// Parses a single line of text into arguments and tries to execute it as if it was typed at the console
// The text can come from the command buffer, a remote client, or stdin.
void Cmd_ExecuteString(char *text, cmd_source_t src);

// adds the current command line as a clc_stringcmd to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.
void Cmd_ForwardToServer();

// used by command functions to send output to either the graphics console or
// passed as a print message to the client
void Cmd_Print(char *text);