#pragma once
#ifndef OGS_CVARAPI_HPP
#define OGS_CVARAPI_HPP

extern "C"
{

void CvarSystem_Register(cvar_t *pCvar);
//void EngFunc_Cvar_RegisterVariable(cvar_t *variable);

float CvarSystem_GetFloat(const char *szVarName);
const char *CvarSystem_GetString(const char *szVarName);

void CvarSystem_SetFloat(const char *szVarName, float flValue);
void CvarSystem_SetString(const char *szVarName, const char *szValue);

cvar_t *EngFunc_CVarGetPointer(const char *szVarName);

void EngFunc_Cvar_DirectSet(struct cvar_s *var, char *value);

};

#endif // OGS_CVARAPI_HPP