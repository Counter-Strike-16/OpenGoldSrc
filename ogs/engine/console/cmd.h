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

#pragma once

#include "maintypes.h"
#include "system/common.h"
#include "rehlds/cmd_rehlds.h"

// All command/alias names are case insensitive! Arguments not

#define MAX_CMD_BUFFER	16384
#define MAX_CMD_TOKENS	80
#define MAX_CMD_LINE	1024

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but remote
servers can also send across commands and entire text files can be execed.

The + command line options are also added to the command buffer.

*/

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

Commands can come from three sources, but the handler functions may choose
to disallow the action or forward it to a remote server if the source is
not apropriate.

*/

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

extern int cmd_argc;
extern char *cmd_argv[80];
extern char *cmd_args;

extern sizebuf_t cmd_text;
extern cmd_source_t cmd_source;
extern qboolean cmd_wait;

extern cmd_function_t *cmd_functions;
extern cmdalias_t *cmd_alias;

void Cmd_Wait_f();
void Cbuf_Init();
void Cbuf_AddText(char *text);
void Cbuf_InsertText(char *text);
void Cbuf_InsertTextLines(char *text);
void Cbuf_Execute();
void Cmd_StuffCmds_f();
void Cmd_Exec_f();
void Cmd_Echo_f();
char *CopyString(char *in);
void Cmd_Alias_f();
struct cmd_function_s *Cmd_GetFirstCmd();
void Cmd_Init();
void Cmd_Shutdown();
int Cmd_Argc();
const char *Cmd_Argv(int arg);
const char *Cmd_Args();
void Cmd_TokenizeString(char *text);
NOXREF cmd_function_t *Cmd_FindCmd(char *cmd_name);
NOXREF cmd_function_t *Cmd_FindCmdPrev(char *cmd_name);
void Cmd_AddCommand(char *cmd_name, xcommand_t function);
void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function, int flag);
NOXREF void Cmd_AddHUDCommand(char *cmd_name, xcommand_t function);
NOXREF void Cmd_AddWrapperCommand(char *cmd_name, xcommand_t function);
void Cmd_AddGameCommand(char *cmd_name, xcommand_t function);
void Cmd_RemoveMallocedCmds(int flag);
NOXREF void Cmd_RemoveHudCmds();
void Cmd_RemoveGameCmds();
void Cmd_RemoveWrapperCmds();
qboolean Cmd_Exists(const char *cmd_name);
NOXREF char *Cmd_CompleteCommand(char *search, int forward);
void Cmd_ExecuteString(char *text, cmd_source_t src);
qboolean Cmd_ForwardToServerInternal(sizebuf_t *pBuf);
void Cmd_ForwardToServer();
qboolean Cmd_ForwardToServerUnreliable();
NOXREF int Cmd_CheckParm(char *parm);
void Cmd_CmdList_f();