#include "console/Console.h"

float EngFunc_GetCvarFloat(char *szName)
{
};

char *EngFunc_GetCvarString(char *szName)
{
};

struct cvar_s *EngFunc_RegisterVariable(char *szName, char *szValue, int flags)
{
};

int EngFunc_AddCommand(char *cmd_name, void (*function)())
{
};

void Con_NPrintf(int pos, char *fmt, ...)
{
};

void Con_NXPrintf(struct con_nprint_s *info, char *fmt, ...)
{
};

int Con_IsVisible()
{
};

struct cvar_s *EngFunc_GetCvarPointer(const char *szName)
{
};

struct cvar_s *EngFunc_GetFirstCvarPtr()
{
};

void *EngFunc_GetFirstCmdFunctionHandle()
{
};

void *EngFunc_GetNextCmdFunctionHandle(void *cmdhandle)
{
};

const char *EngFunc_GetCmdFunctionName(void *cmdhandle)
{
};

cmdalias_t *EngFunc_GetAliases()
{
};