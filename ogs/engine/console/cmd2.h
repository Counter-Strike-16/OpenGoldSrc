
#include "maintypes.h"
#include "common.h"
#include "cmd_rehlds.h"

/*
All command/alias names are case insensitive! Arguments not.
*/

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

void Cmd_Wait_f(void);
void Cbuf_Init(void);
void Cbuf_AddText(char *text);
void Cbuf_InsertText(char *text);
void Cbuf_InsertTextLines(char *text);
void Cbuf_Execute(void);

void Cmd_Exec_f(void);
void Cmd_Echo_f(void);
char *CopyString(char *in);
void Cmd_Alias_f(void);
struct cmd_function_s *Cmd_GetFirstCmd(void);
void Cmd_Shutdown(void);

NOXREF cmd_function_t *Cmd_FindCmd(char *cmd_name);
NOXREF cmd_function_t *Cmd_FindCmdPrev(char *cmd_name);
void Cmd_AddCommand(char *cmd_name, xcommand_t function);
void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function, int flag);
NOXREF void Cmd_AddHUDCommand(char *cmd_name, xcommand_t function);
NOXREF void Cmd_AddWrapperCommand(char *cmd_name, xcommand_t function);
void Cmd_AddGameCommand(char *cmd_name, xcommand_t function);
void Cmd_RemoveMallocedCmds(int flag);
NOXREF void Cmd_RemoveHudCmds(void);
void Cmd_RemoveGameCmds(void);
void Cmd_RemoveWrapperCmds(void);
qboolean Cmd_Exists(const char *cmd_name);
NOXREF char *Cmd_CompleteCommand(char *search, int forward);
void Cmd_ExecuteString(char *text, cmd_source_t src);
qboolean Cmd_ForwardToServerInternal(sizebuf_t *pBuf);
void Cmd_ForwardToServer(void);
qboolean Cmd_ForwardToServerUnreliable(void);
NOXREF int Cmd_CheckParm(char *parm);
void Cmd_CmdList_f(void);
