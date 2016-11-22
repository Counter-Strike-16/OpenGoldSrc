#include "precompiled.h"

int cmd_argc;
char *cmd_argv[80];

// Complete arguments string
char *cmd_args;

cmd_source_t cmd_source;
qboolean cmd_wait;
cmdalias_t *cmd_alias;

//int trashtest;
//int *trashspot;

cmd_function_t *cmd_functions;
char *const cmd_null_string = "";


/* <4aad> ../engine/cmd.c:47 */
void Cmd_Wait_f(void)
{
	cmd_wait = 1;
}

void Cbuf_InsertText(char *text)
{
	int currLen = cmd_text.cursize;

#ifdef REHLDS_FIXES
	if (currLen)
		Q_memmove(cmd_text.data + addLen, cmd_text.data, currLen);

	Q_memcpy(cmd_text.data, text, addLen);
	cmd_text.cursize += addLen;

#else
	
#endif // REHLDS_FIXES
}



void Cbuf_Execute(void)
{
	while (cmd_text.cursize)
	{
		// find a \n or ; line break
		text = (char *)cmd_text.data;

		quotes = 0;
		for (i = 0; i < cmd_text.cursize; i++)
		{
			if (text[i] == '"')
				quotes++;
			if (!(quotes & 1) && text[i] == ';')
				break;	// don't break if inside a quoted string
			if (text[i] == '\n')
				break;
		}

#ifdef REHLDS_FIXES
		// save `i` if we truncate command
		int len;

		if (i > MAX_CMD_LINE - 1)
			len = MAX_CMD_LINE - 1;
		else
			len = i;

		Q_memcpy(line, text, len);
		line[len] = 0;
#else // REHLDS_FIXES
		if (i > MAX_CMD_LINE - 1)
		{
			i = MAX_CMD_LINE - 1;
		}

		Q_memcpy(line, text, i);
		line[i] = 0;
#endif // REHLDS_FIXES

		// delete the text from the command buffer and move remaining commands down
		// this is necessary because commands (exec, alias) can insert data at the
		// beginning of the text buffer

		if (i == cmd_text.cursize)
		{
			cmd_text.cursize = 0;
		}
		else
		{
			i++;
			cmd_text.cursize -= i;
#ifdef REHLDS_FIXES
			// dst overlaps src
			Q_memmove(text, text + i, cmd_text.cursize);
#else // REHLDS_FIXES
			Q_memcpy(text, text + i, cmd_text.cursize);
#endif // REHLDS_FIXES
		}

		// execute the command line
		Cmd_ExecuteString(line, src_command);

		if (cmd_wait)
		{
			// skip out while text still remains in buffer, leaving it
			// for next frame
			cmd_wait = FALSE;
			break;
		}
	}
}

/* <5091> ../engine/cmd.c:646 */
void Cmd_Init(void)
{
	Cmd_AddCommand("cmd", Cmd_ForwardToServer);
	Cmd_AddCommand("wait", Cmd_Wait_f);
	Cmd_AddCommand("cmdlist", Cmd_CmdList_f);
}

/* <5547> ../engine/cmd.c:689 */
const char* EXT_FUNC Cmd_Argv(int arg)
{
#ifndef SWDS
	g_engdstAddrs->Cmd_Argv(&arg);
#endif

	if (arg >= 0 && arg < cmd_argc)
	{
		return cmd_argv[arg];
	}
	return "";	// TODO: Possibly better to return NULL here, but require to check all usages
}

/* <5575> ../engine/cmd.c:715 */
/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
Takes a null terminated string. Does not need to be \n terminated.
Breaks the string up into arg tokens.
============
*/
void EXT_FUNC Cmd_TokenizeString(char *text)
{
	int i;
	int arglen;

	// clear args from the last string
	for (i = 0; i < cmd_argc; i++)
	{
		Z_Free(cmd_argv[i]);
		cmd_argv[i] = NULL;
	}
	cmd_argc = 0;
	cmd_args = NULL;

	while (true)
	{
		// Skip whitespace up to a \n
#ifdef REHLDS_FIXES
		while (*text && (uint8_t)*text <= ' ' && *text != '\n')
#else // REHLDS_FIXES
		while (*text && *text <= ' ' && *text != '\n')
#endif // REHLDS_FIXES
		{
			++text;
		}

		// A newline separates commands in the buffer
		if (*text == '\n' || *text == 0)
		{
			break;
		}

		// Store complete args string pointer
		if (cmd_argc == 1)
		{
			cmd_args = text;
		}

		// Break into token
		text = COM_Parse(text);

		// If nothing was parsed
		if (!text)
		{
			return;
		}

		arglen = Q_strlen(com_token) + 1;
		if (arglen >= 516)	// TODO: What is that magic number?
		{
			return;
		}

		cmd_argv[cmd_argc] = (char *)Z_Malloc(arglen);
		Q_strcpy(cmd_argv[cmd_argc++], com_token);

		if (cmd_argc >= MAX_CMD_TOKENS)
		{
			// Will not fit any more
			return;
		}
	}
}

void Cmd_InsertCommand(cmd_function_t *cmd)
{
	cmd_function_t *c, **p;

	// Commands list is alphabetically sorted, search where to push
	c = cmd_functions;
	p = &cmd_functions;
	while (c)
	{
		if (Q_stricmp(c->name, cmd->name) > 0)
		{
			// Current command name is bigger, insert before it
			cmd->next = c;
			*p = cmd;
			return;
		}
		p = &c->next;
		c = c->next;
	}

	// All commands in the list are lower then the new one
	cmd->next = NULL;
	*p = cmd;
}


// Use this for engine inside call only, not from user code, because it doesn't alloc string for the name.
void Cmd_AddCommand(char *cmd_name, xcommand_t function)
{
	cmd_function_t *cmd;

	if (host_initialized)
	{
		Sys_Error(__FUNCTION__ " after host_initialized");
	}

	// Check in variables list
	if (Cvar_FindVar(cmd_name) != NULL)
	{
		Con_Printf(__FUNCTION__ ": \"%s\" already defined as a var\n", cmd_name);
		return;
	}

	// Check if this command is already defined
	if (Cmd_Exists(cmd_name))
	{
		Con_Printf(__FUNCTION__ ": \"%s\" already defined\n", cmd_name);
		return;
	}

	// Create cmd_function
	cmd = (cmd_function_t *)Hunk_Alloc(sizeof(cmd_function_t));
	cmd->name = cmd_name;
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = 0;

	Cmd_InsertCommand(cmd);
}

/* <5700> ../engine/cmd.c:891 */
// Use this for call from user code, because it alloc string for the name.
void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function, int flag)
{
	cmd_function_t *cmd;

	// Check in variables list
	if (Cvar_FindVar(cmd_name) != NULL)
	{
		Con_Printf(__FUNCTION__ ": \"%s\" already defined as a var\n", cmd_name);
		return;
	}

	// Check if this command is already defined
	if (Cmd_Exists(cmd_name))
	{
		Con_Printf(__FUNCTION__ ": \"%s\" already defined\n", cmd_name);
		return;
	}

	// Create cmd_function
	cmd = (cmd_function_t *)Mem_Malloc(sizeof(cmd_function_t));
	cmd->name = CopyString(cmd_name);	// alloc string, so it will not dissapear on side modules unloading and to maintain the same name during run
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = flag;

	Cmd_InsertCommand(cmd);
}

/* <5776> ../engine/cmd.c:934 */
NOXREF void Cmd_AddHUDCommand(char *cmd_name, xcommand_t function)
{
	NOXREFCHECK;

	Cmd_AddMallocCommand(cmd_name, function, FCMD_HUD_COMMAND);
}

/* <5824> ../engine/cmd.c:946 */
NOXREF void Cmd_AddWrapperCommand(char *cmd_name, xcommand_t function)
{
	NOXREFCHECK;

	Cmd_AddMallocCommand(cmd_name, function, FCMD_WRAPPER_COMMAND);
}

/* <58d2> ../engine/cmd.c:960 */
void EXT_FUNC Cmd_AddGameCommand(char *cmd_name, xcommand_t function)
{
	Cmd_AddMallocCommand(cmd_name, function, FCMD_GAME_COMMAND);
}

/* <5980> ../engine/cmd.c:970 */
void Cmd_RemoveMallocedCmds(int flag)
{
	cmd_function_t *c, **p;

	c = cmd_functions;
	p = &cmd_functions;
	while (c)
	{
		if (c->flags & flag)
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

/* <59f9> ../engine/cmd.c:1002 */
NOXREF void Cmd_RemoveHudCmds(void)
{
	NOXREFCHECK;

	Cmd_RemoveMallocedCmds(FCMD_HUD_COMMAND);
}

/* <5a4c> ../engine/cmd.c:1012 */
void Cmd_RemoveGameCmds(void)
{
	Cmd_RemoveMallocedCmds(FCMD_GAME_COMMAND);
}

/* <5a9f> ../engine/cmd.c:1022 */
void Cmd_RemoveWrapperCmds(void)
{
	Cmd_RemoveMallocedCmds(FCMD_WRAPPER_COMMAND);
}


qboolean Cmd_Exists(const char *cmd_name)
{
	cmd_function_t *cmd = cmd_functions;

	while (cmd)
	{
		if (!Q_stricmp(cmd_name, cmd->name))
		{
			return TRUE;
		}

		cmd = cmd->next;
	}

	return FALSE;
}

 char *Cmd_CompleteCommand(char *search, int forward)
{
	

	// TODO: We have a command name length limit here: prepare for unforeseen consequences!
	static char lastpartial[256];
	char partial[256];
	cmd_function_t *cmd;
	int len;
	char *pPartial;

	Q_strncpy(partial, search, 255);
	partial[255] = 0;
	len = Q_strlen(partial);

	// Trim tail spaces
	for (pPartial = partial + len - 1; pPartial >= partial && *pPartial == ' '; pPartial--, len--)
	{
		*pPartial = 0;
	}

	if (!len)
	{
		return NULL;
	}

	if (!Q_stricmp(partial, lastpartial))
	{
		// Same partial, find this then next/prev cvar, if any.
		// TODO: But where it match for entered by user partial? Because we store full name
		cmd = Cmd_FindCmd(partial);
		if (cmd)
		{
			cmd = forward == 1 ? cmd->next : Cmd_FindCmdPrev(cmd->name);
			if (cmd)
			{
				Q_strncpy(lastpartial, cmd->name, 255);
				lastpartial[255] = 0;
				return cmd->name;
			}
		}
	}

	// Find first matching cvar
	for (cmd = cmd_functions; cmd != NULL; cmd = cmd->next)
	{
		if (!Q_strnicmp(partial, cmd->name, len))
		{
			// Store matched cvar name
			Q_strncpy(lastpartial, cmd->name, 255);
			lastpartial[255] = 0;
			return cmd->name;
		}
	}

	return NULL;
}

bool EXT_FUNC ValidateCmd_API(const char* cmd, cmd_source_t src, IGameClient* client) {
	return true;
}

void EXT_FUNC Cmd_ExecuteString_internal(const char* cmdName, cmd_source_t src, IGameClient* client) {
	// Search in functions
	cmd_function_t *cmd = cmd_functions;
	while (cmd)
	{
		if (!Q_stricmp(cmdName, cmd->name))
		{
			cmd->function();

			if (g_pcls.demorecording && (cmd->flags & FCMD_HUD_COMMAND) && !g_pcls.spectator)
			{
				CL_RecordHUDCommand(cmd->name);
			}

			return;
		}

		cmd = cmd->next;
	}

	// Search in aliases
	cmdalias_t *a = cmd_alias;
	while (a)
	{
		if (!Q_stricmp(cmdName, a->name))
		{

			Cbuf_InsertText(a->value);
			return;
		}

		a = a->next;
	}

	// Search in cvars
	if (!Cvar_Command())
	{
		// Send to a server if nothing processed locally and connected
		if (g_pcls.state >= ca_connected)
			Cmd_ForwardToServer();
#ifdef REHLDS_FIXES
		else if (sv_echo_unknown_cmd.string[0] == '1' && src == src_command)
			Con_Printf("unknown command \"%s\"\n", cmdName);
#endif
	}
}

/* <5d4e> ../engine/cmd.c:1133 */
void Cmd_ExecuteString(char *text, cmd_source_t src)
{
	cmd_source = src;
	Cmd_TokenizeString(text);

	if (!Cmd_Argc())
	{
		return;
	}

	IGameClient* cl = (src == src_client) ? GetRehldsApiClient(host_client) : NULL;
	if (!g_RehldsHookchains.m_ValidateCommand.callChain(ValidateCmd_API, cmd_argv[0], src, cl))
		return;

	g_RehldsHookchains.m_ExecuteServerStringCmd.callChain(Cmd_ExecuteString_internal, cmd_argv[0], src, cl);
}

/* <5c15> ../engine/cmd.c:1181 */
qboolean Cmd_ForwardToServerInternal(sizebuf_t *pBuf)
{
	const char *cmd_name = Cmd_Argv(0);

	if (g_pcls.state <= ca_disconnected)
	{
		if (Q_stricmp(cmd_name, "setinfo"))
		{
			Con_Printf("Can't \"%s\", not connected\n", cmd_name);
		}

		return FALSE;
	}

	if (g_pcls.demoplayback || g_bIsDedicatedServer)
	{
		return FALSE;
	}

	char tempData[4096];
	sizebuf_t tempBuf;

	tempBuf.buffername = __FUNCTION__ "::tempBuf";
	tempBuf.data = (byte *)tempData;
	tempBuf.maxsize = 4096;
	tempBuf.cursize = 0;
	tempBuf.flags = SIZEBUF_ALLOW_OVERFLOW;

	MSG_WriteByte(&tempBuf, clc_stringcmd);

	if (Q_stricmp(cmd_name, "cmd"))
	{
		SZ_Print(&tempBuf, cmd_name);
		SZ_Print(&tempBuf, " ");
	}

	SZ_Print(&tempBuf, Cmd_Argc() <= 1 ? "\n" : Cmd_Args());

	if (tempBuf.flags & SIZEBUF_OVERFLOWED)
	{
		return FALSE;
	}

	if (tempBuf.cursize + pBuf->cursize <= pBuf->maxsize)
	{
		SZ_Write(pBuf, tempBuf.data, tempBuf.cursize);
		return TRUE;
	}

	return FALSE;
}

/* <5cfd> ../engine/cmd.c:1247 */
void Cmd_ForwardToServer(void)
{
	if (Q_stricmp(Cmd_Argv(0), "cmd") || Q_stricmp(Cmd_Argv(1), "dlfile"))
	{
		Cmd_ForwardToServerInternal(&g_pcls.netchan.message);
	}
}

/* <5f1b> ../engine/cmd.c:1258 */
qboolean Cmd_ForwardToServerUnreliable(void)
{
	return Cmd_ForwardToServerInternal(&g_pcls.datagram);
}

/* <5f35> ../engine/cmd.c:1273 */
// Returns the position (1 to argc-1) in the command's argument list
// where the given parameter apears, or 0 if not present.
NOXREF int Cmd_CheckParm(char *parm)
{
	NOXREFCHECK;

	if (!parm)
	{
		Sys_Error(__FUNCTION__ ": NULL");
	}

	int c = Cmd_Argc();

	for (int i = 1; i < c; i++)
	{
		if (!Q_stricmp(Cmd_Argv(i), parm))
		{
			return i;
		}
	}

	return 0;
}

/* <4b17> ../engine/cmd.c:1301 */
void Cmd_CmdList_f(void)
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
	if (iArgs > 1)
	{
		arg1 = Cmd_Argv(1);

		if (!Q_stricmp(arg1, "?"))
		{
			Con_Printf("CmdList           : List all commands\nCmdList [Partial] : List commands starting with 'Partial'\nCmdList log [Partial] : Logs commands to file \"cmdlist.txt\" in the gamedir.\n");
			return;
		}

		if (!Q_stricmp(arg1, "log"))
		{
			// Open log
			int i;
			for (i = 0; i < 100; i++)
			{
				Q_snprintf(szTemp, ARRAYSIZE(szTemp) - 1, "cmdlist%02d.txt", i);
				szTemp[ARRAYSIZE(szTemp) - 1] = 0;

				fp = FS_Open(szTemp, "r");
				if (!fp)
				{
					break;
				}
				FS_Close(fp);
			}

			if (i >= 100)
			{
				Con_Printf("Can't cmdlist! Too many existing cmdlist output files in the gamedir!\n");
				return;
			}

			f = FS_Open(szTemp, "wt");
			if (!f)
			{
				Con_Printf("Couldn't open \"%s\" for writing!\n", szTemp);
				return;
			}
			bLogging = TRUE;

			// Get next argument into partial, if present
			if (iArgs >= 2)
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

	for (cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if (partial && Q_strnicmp(cmd->name, partial, ipLen))
		{
			continue;
		}

		Con_Printf("%s\n", cmd->name);

		if (bLogging)
		{
			FS_FPrintf(f, "%s\n", cmd->name);
		}

		iCmds++;
	}

	if (partial && *partial)
	{
		Con_Printf("--------------\n%3i Commands for [%s]\nCmdList ? for syntax\n", iCmds, partial);
	}
	else
	{
		Con_Printf("--------------\n%3i Total Commands\nCmdList ? for syntax\n", iCmds);
	}

	// Close log
	if (bLogging)
	{
		FS_Close(f);
		Con_Printf("cmdlist logged to %s\n", szTemp);
	}
}
