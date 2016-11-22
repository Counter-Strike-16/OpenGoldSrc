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

// why wrapping, gs?

/* <7fb75> ../engine/pr_edict.c:747 */
float EXT_FUNC CVarGetFloat(const char *szVarName)
{
	return Cvar_VariableValue(szVarName);
}

/* <7fba2> ../engine/pr_edict.c:753 */
const char* EXT_FUNC CVarGetString(const char *szVarName)
{
	return Cvar_VariableString(szVarName);
}

/* <7fbcf> ../engine/pr_edict.c:759 */
cvar_t* EXT_FUNC CVarGetPointer(const char *szVarName)
{
	return Cvar_FindVar(szVarName);
}

/* <7fbfc> ../engine/pr_edict.c:765 */
void EXT_FUNC CVarSetFloat(const char *szVarName, float flValue)
{
	Cvar_SetValue(szVarName, flValue);
}

/* <7fc34> ../engine/pr_edict.c:771 */
void EXT_FUNC CVarSetString(const char *szVarName, const char *szValue)
{
	Cvar_Set(szVarName, szValue);
}

/* <7fc6c> ../engine/pr_edict.c:777 */
void EXT_FUNC CVarRegister(cvar_t *pCvar)
{
	if (pCvar)
	{
		pCvar->flags |= FCVAR_EXTDLL; // gamedll cvar
		Cvar_RegisterVariable(pCvar);
	}
}

//todo: clientdll cvar with full args set