#pragma once

void Con_Debug_f();
void Con_Init();
void Con_DebugLog(const char *file, const char *fmt, ...);
void Con_Printf(const char *fmt, ...);
void Con_SafePrintf(const char *fmt, ...);
void Con_DPrintf(const char *fmt, ...);