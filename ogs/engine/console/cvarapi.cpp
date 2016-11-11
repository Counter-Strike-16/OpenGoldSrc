#include "console/cvarapi.h"

#ifndef OGS_NULL_CVAR_API_IMPL
void CvarSystem_Register(cvar_t *pCvar)
{
};

void EngFunc_Cvar_RegisterVariable(cvar_t *variable)
{
};

float CvarSystem_GetFloat(const char *szVarName)
{
	return 0.0f;
};

const char *CvarSystem_GetString(const char *szVarName)
{
	return "";
};

void CvarSystem_SetFloat(const char *szVarName, float flValue)
{
};

void CvarSystem_SetString(const char *szVarName, const char *szValue)
{
};

cvar_t *EngFunc_CVarGetPointer(const char *szVarName)
{
	return nullptr;
};

void EngFunc_Cvar_DirectSet(struct cvar_s *var, char *value)
{
};
#else // OGS_NULL_CVAR_API_IMPL
void CvarSystem_Register(cvar_t *pCvar)
{
};

void EngFunc_Cvar_RegisterVariable(cvar_t *variable)
{
};

float CvarSystem_GetFloat(const char *szVarName)
{
	return 0.0f;
};

const char *CvarSystem_GetString(const char *szVarName)
{
	return "";
};

void CvarSystem_SetFloat(const char *szVarName, float flValue)
{
};

void CvarSystem_SetString(const char *szVarName, const char *szValue)
{
};

cvar_t *EngFunc_CVarGetPointer(const char *szVarName)
{
	return nullptr;
};

void EngFunc_Cvar_DirectSet(struct cvar_s *var, char *value)
{
};
#endif // OGS_NULL_CVAR_API_IMPL