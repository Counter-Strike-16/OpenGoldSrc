
 const char *Cvar_CompleteVariable(const char *search, int forward)
{
	// TODO: We have a cvar name length limit here: prepare for unforeseen consequences!
	static char lastpartial[256];
	char partial[256];
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


void Cvar_SetValue(const char *var_name, float value)
{

	if (fabs(value - (double)(signed int)value) >= 0.000001f)
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%f", value);
	else
		Q_snprintf(val, ARRAYSIZE(val) - 1, "%d", (signed int)value);
}


void  Cvar_RegisterVariable(cvar_t *variable)
{
	cvar_t *v, *c;
	cvar_t dummyvar;

	
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

/* <18e8a> ../engine/cvar.c:853 */
void Cvar_CmdInit(void)
{
	Cmd_AddCommand("cvarlist", Cmd_CvarList_f);
}
