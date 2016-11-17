

#include "precompiled.h"



/*
	All cvar names are case insensitive! Values not.
*/

/* <185b6> ../engine/cvar.c:58 */
NOXREF cvar_t *Cvar_FindPrevVar(const char *var_name)
{
	NOXREFCHECK;

	cvar_t *var;

	for (var = cvar_vars; var && var->next; var = var->next)
	{
		if (!Q_stricmp(var_name, var->next->name))
			return var;
	}
	return NULL;
}

/* <1872a> ../engine/cvar.c:126 */
NOXREF const char *Cvar_CompleteVariable(const char *search, int forward)
{
	NOXREFCHECK;

	// TODO: We have a cvar name length limit here: prepare for unforeseen consequences!
	static char lastpartial[256];
	char partial[256];
	cvar_t *cvar;
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
		cvar = Cvar_FindVar(partial);
		if (cvar)
		{
			cvar = forward == 1 ? cvar->next : Cvar_FindPrevVar(cvar->name);
			if (cvar)
			{
				Q_strncpy(lastpartial, cvar->name, 255);
				lastpartial[255] = 0;
				return cvar->name;
			}
		}
	}

	// Find first matching cvar
	for (cvar = cvar_vars; cvar != NULL; cvar = cvar->next)
	{
		if (!Q_strnicmp(partial, cvar->name, len))
		{
			// Store matched cvar name
			Q_strncpy(lastpartial, cvar->name, 255);
			lastpartial[255] = 0;
			return cvar->name;
		}
	}

	return NULL;
}

/* <18805> ../engine/cvar.c:198 */
void EXT_FUNC Cvar_DirectSet_internal(struct cvar_s *var, const char *value)
{
	if (!var || !value)
	{
		return;
	}

	const char *pszValue = value;
	char szNew[MAX_CVAR_VALUE];
	szNew[0] = 0;

	if (var->flags & FCVAR_PRINTABLEONLY)
	{
		if (Q_UnicodeValidate(value))
		{
			Q_strncpy(szNew, value, ARRAYSIZE(szNew) - 1);
			szNew[ARRAYSIZE(szNew) - 1] = 0;
		}
		else
		{
			// Copy only printable chars
			// TODO: Why not UTF-8 too?
			const char *pS = pszValue;
			char *pD = szNew;

			while (*pS)
			{
				if (*pS < 32 || *pS > 126)
				{
					pS++;
					continue;
				}
				*pD++ = *pS++;
			}
			*pD = 0;
		}

		if (!Q_UnicodeValidate(szNew))
		{
			// Call the artillery
			Q_UnicodeRepair(szNew);
		}

		if (szNew[0] == 0)
		{
			Q_strcpy(szNew, "empty");
		}

		pszValue = szNew;
	}

	if (var->flags & FCVAR_NOEXTRAWHITEPACE)
	{
		if (pszValue != szNew)
		{
			Q_strncpy(szNew, value, ARRAYSIZE(szNew) - 1);
			szNew[ARRAYSIZE(szNew) - 1] = 0;
		}

		Q_StripUnprintableAndSpace(szNew);

		pszValue = szNew;
	}

	qboolean changed = Q_strcmp(var->string, pszValue);

	if (var->flags & FCVAR_USERINFO)
	{
		if (g_pcls.state == ca_dedicated)
		{
			char *info = Info_Serverinfo();
			Info_SetValueForKey(info, var->name, pszValue, MAX_INFO_STRING);
			SV_BroadcastCommand("fullserverinfo \"%s\"\n", info);
		}
#ifndef SWDS
		else
		{
			Info_SetValueForKey(g_pcls.userinfo, var->name, pszValue, MAX_INFO_STRING);

			if (changed && g_pcls.state >= ca_connected)
			{
				MSG_WriteByte(&g_pcls.netchan.message, clc_stringcmd);
				SZ_Print(&g_pcls.netchan.message, va("setinfo \"%s\" \"%s\"\n", var->name, pszValue));
			}
		}
#endif
	}

	if (changed && var->flags & FCVAR_SERVER)
	{
		if (!(var->flags & FCVAR_UNLOGGED))
		{
			if (var->flags & FCVAR_PROTECTED)
			{
				Log_Printf("Server cvar \"%s\" = \"%s\"\n", var->name, "***PROTECTED***");
				SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, "***PROTECTED***");
			}
			else
			{
				Log_Printf("Server cvar \"%s\" = \"%s\"\n", var->name, pszValue);
				SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var->name, pszValue);
			}
		}

		if (!(var->flags & FCVAR_PROTECTED))
		{
			Steam_SetCVar(var->name, pszValue);
		}
		else if (pszValue[0] && Q_stricmp(pszValue, "none"))
		{
			Steam_SetCVar(var->name, "1");
		}
		else
		{
			Steam_SetCVar(var->name, "0");
		}
	}

	Z_Free(var->string);
	var->string = (char *)Z_Malloc(Q_strlen(pszValue) + 1);
	Q_strcpy(var->string, pszValue);
	var->value = (float)Q_atof(var->string);
}

void Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	g_RehldsHookchains.m_Cvar_DirectSet.callChain(Cvar_DirectSet_internal, var, value);
}

/* <188e9> ../engine/cvar.c:347 */
void Cvar_Set(const char *var_name, const char *value)
{
	cvar_t *var = Cvar_FindVar(var_name);

	if (!var)
	{
		Con_DPrintf(__FUNCTION__ ": variable \"%s\" not found\n", var_name);
		return;
	}

	Cvar_DirectSet(var, value);
}

/* <1893e> ../engine/cvar.c:365 */
void Cvar_SetValue(const char *var_name, float value)
{
	char val[32];

#ifndef SWDS
	g_engdstAddrs->Cvar_SetValue(&var_name, &value);
#endif

	if (fabs(value - (double)(signed int)value) >= 0.000001)
	{
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%f", value);
	}
	else
	{
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%d", (signed int)value);
	}
	val[ARRAYSIZE(val) - 1] = 0;

	Cvar_Set(var_name, val);
}

/* <189df> ../engine/cvar.c:391 */
void EXT_FUNC Cvar_RegisterVariable(cvar_t *variable)
{
	char *oldstr;
	cvar_t *v, *c;
	cvar_t dummyvar;

	if (Cvar_FindVar(variable->name))
	{
		Con_Printf("Can't register variable \"%s\", already defined\n", variable->name);
		return;
	}

	if (Cmd_Exists(variable->name))
	{
		Con_Printf(__FUNCTION__ ": \"%s\" is a command\n", variable->name);
		return;
	}

	oldstr = variable->string;

	// Alloc string, so it will not dissapear on side modules unloading and to maintain the same name during run
	variable->string = (char *)Z_Malloc(Q_strlen(variable->string) + 1);
	Q_strcpy(variable->string, oldstr);
	variable->value = (float)Q_atof(oldstr);

	dummyvar.name = " ";
	dummyvar.next = cvar_vars;

	v = cvar_vars;
	c = &dummyvar;

	// Insert with alphabetic order
	while (v)
	{
		if (Q_stricmp(v->name, variable->name) > 0)
		{
			break;
		}

		c = v;
		v = v->next;
	}

	c->next = variable;
	variable->next = v;
	cvar_vars = dummyvar.next;
}

/* <18a7e> ../engine/cvar.c:452 */
NOXREF void Cvar_RemoveHudCvars(void)
{
	NOXREFCHECK;

	cvar_t *pVar;
	cvar_t **pList;

	pVar = cvar_vars;
	pList = &cvar_vars;

	while (pVar)
	{
		if (pVar->flags & FCVAR_CLIENTDLL)
		{
			*pList = pVar->next;
			Z_Free(pVar->string);
			Z_Free(pVar);
		}
		else
		{
			pList = &pVar->next;
		}

		pVar = *pList;
	}
}

/* <18ac9> ../engine/cvar.c:499 */
// Returns first token if there is more than one, else returns NULL.
const char *Cvar_IsMultipleTokens(const char *varname)
{
	static char firstToken[516];
	int tokens;
	char *name;

	firstToken[0] = 0;
	tokens = 0;
	name = (char *)varname;

	name = COM_Parse(name);

	if (com_token[0] == 0)
	{
		return NULL;	// original function returns firstToken in this situation, which is "", not NULL, but it should be ok this way
	}
	if (name == NULL)
	{
		return NULL;	// only one token
	}

	// Store first token
	Q_strncpy(firstToken, com_token, ARRAYSIZE(firstToken) - 1);
	firstToken[ARRAYSIZE(firstToken) - 1] = 0;

	// Parse for another token
	name = COM_Parse(name);

	if (com_token[0] == 0)
	{
		return NULL;	// only one token
	}

	return firstToken;	// multiple tokens, return first one
}

/* <18b6e> ../engine/cvar.c:542 */
qboolean Cvar_Command(void)
{
	cvar_t *v;
	const char *arg0 = Cmd_Argv(0);
	const char *firstToken = Cvar_IsMultipleTokens(arg0);

	if (firstToken)
	{
		v = Cvar_FindVar(firstToken);
		if (v)
		{
			Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
			return TRUE;
		}
	}
	else
	{
		v = Cvar_FindVar(arg0);
		if (v)
		{
			if (Cmd_Argc() == 1)
			{
				Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
			}
			else
			{
				if (v->flags & FCVAR_SPONLY && g_pcls.state >= ca_connecting && g_pcl.maxclients > 1)
				{
					Con_Printf("Can't set %s in multiplayer\n", v->name);
				}
				else
				{
					Cvar_Set(v->name, Cmd_Argv(1));
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

/* <18ca4> ../engine/cvar.c:601 */
NOXREF void Cvar_WriteVariables(FileHandle_t f)
{
	NOXREFCHECK;

	cvar_t *var;

	for (var = cvar_vars; var; var = var->next)
	{
		if (var->flags & FCVAR_ARCHIVE)
		{
			FS_FPrintf(f, "%s \"%s\"\n", var->name, var->string);
		}
	}
}

/* <18cdc> ../engine/cvar.c:627 */
void Cmd_CvarListPrintCvar(cvar_t *var, FileHandle_t f)
{
	char szOutstr[256];

#ifdef REHLDS_FIXES
	// Do correct output of string valued cvars
	Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-28s : %16s", var->name, var->string);
#else // REHLDS_FIXES
	if (var->value == (float)(int)var->value)
	{
		Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-15s : %8i", var->name, (int)var->value);
	}
	else
	{
		Q_snprintf(szOutstr, ARRAYSIZE(szOutstr) - 1, "%-15s : %8.3f", var->name, var->value);
	}
#endif // REHLDS_FIXES
	szOutstr[ARRAYSIZE(szOutstr) - 1] = 0;

	if (var->flags & FCVAR_ARCHIVE)
	{
		Q_strcat(szOutstr, ", a");
	}
	if (var->flags & FCVAR_SERVER)
	{
		Q_strcat(szOutstr, ", sv");
	}
	if (var->flags & FCVAR_USERINFO)
	{
		Q_strcat(szOutstr, ", i");
	}

	Q_strcat(szOutstr, "\n");

	Con_Printf("%s", szOutstr);

	if (f)
	{
		FS_FPrintf(f, "%s", szOutstr);
	}
}

/* <18d23> ../engine/cvar.c:671 */
void Cmd_CvarList_f(void)
{
	cvar_t *var;
	int iCvars;
	int iArgs;
	const char *partial, *arg1;
	int ipLen;
	qboolean bAOnly;
	qboolean bSOnly;
	char szTemp[MAX_PATH];
	FileHandle_t f;
	FileHandle_t fp;
	qboolean bLogging;

	iCvars = 0;
	partial = NULL;
	bAOnly = FALSE;
	bSOnly = FALSE;
	f = NULL;
	fp = NULL;
	bLogging = FALSE;

	iArgs = Cmd_Argc();
	if (iArgs > 1)
	{
		arg1 = Cmd_Argv(1);

		if (!Q_stricmp(arg1, "?"))
		{
			Con_Printf("CvarList           : List all cvars\nCvarList [Partial] : List cvars starting with 'Partial'\nCvarList log [Partial] : Logs cvars to file \"cvarlist.txt\" in the gamedir.\n");
			return;
		}

		if (!Q_stricmp(arg1, "log"))
		{
			// Open log
			int i;
			for (i = 0; i < 100; i++)
			{
				Q_snprintf(szTemp, ARRAYSIZE(szTemp) - 1, "cvarlist%02d.txt", i);
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
				Con_Printf("Can't cvarlist! Too many existing cvarlist output files in the gamedir!\n");
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
			if (iArgs > 2)
			{
				partial = Cmd_Argv(2);
				ipLen = Q_strlen(partial);
			}
		}
		else if (!Q_stricmp(arg1, "-a"))
		{
			bAOnly = TRUE;
		}
		else if (!Q_stricmp(arg1, "-s"))
		{
			bSOnly = TRUE;
		}
		else
		{
			partial = arg1;
			ipLen = Q_strlen(partial);
		}
	}

	// Print cvars
	Con_Printf("CVar List\n--------------\n");

	for (var = cvar_vars; var; var = var->next)
	{
		if (bAOnly && !(var->flags & FCVAR_ARCHIVE))
		{
			continue;
		}
		if (bSOnly && !(var->flags & FCVAR_SERVER))
		{
			continue;
		}
		if (partial && Q_strnicmp(var->name, partial, ipLen))
		{
			continue;
		}

		Cmd_CvarListPrintCvar(var, f);
		iCvars++;
	}

	if (partial && *partial)
	{
		Con_Printf("--------------\n%3i CVars for [%s]\nCvarList ? for syntax\n", iCvars, partial);
	}
	else
	{
		Con_Printf("--------------\n%3i Total CVars\nCvarList ? for syntax\n", iCvars);
	}

	// Close log
	if (bLogging)
	{
		FS_Close(f);
		Con_Printf("cvarlist logged to %s\n", szTemp);
	}
}

/* <18e0f> ../engine/cvar.c:806 */
NOXREF int Cvar_CountServerVariables(void)
{
	NOXREFCHECK;

	int i;
	cvar_t *var;

	for (i = 0, var = cvar_vars; var; var = var->next)
	{
		if (var->flags & FCVAR_SERVER)
		{
			++i;
		}
	}
	return i;
}

/* <18e4a> ../engine/cvar.c:829 */
void Cvar_UnlinkExternals(void)
{
	cvar_t *pVar;
	cvar_t **pList;

	pVar = cvar_vars;
	pList = &cvar_vars;

	while (pVar)
	{
		if (pVar->flags & FCVAR_EXTDLL)
		{
			*pList = pVar->next;
		}
		else
		{
			pList = &pVar->next;
		}

		pVar = *pList;
	}
}

/* <18e8a> ../engine/cvar.c:853 */
void Cvar_CmdInit(void)
{
	Cmd_AddCommand("cvarlist", Cmd_CvarList_f);
}
