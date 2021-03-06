/*
=============
pfnGetModelType
=============
*/
int pfnGetModelType(model_t *apModel)
{
	if(!apModel)
		return mod_bad;
	
	return apModel->type;
};

/*
=============
pfnGetModelBounds
=============
*/
void pfnGetModelBounds(model_t *apModel, float *avMins, float *avMaxs)
{
	if(apModel)
	{
		if(avMins)
			VectorCopy(apModel->mins, avMins);
		
		if(avMaxs)
			VectorCopy(apModel->maxs, avMaxs);
	}
	else
	{
		DevError("Mod_GetBounds: NULL model passed");
		
		if(avMins)
			VectorClear(avMins);
		
		if(avMaxs)
			VectorClear(avMaxs);
	};
};
	
/*
=============
pfnCvar_RegisterVariable
=============
*/
cvar_t *pfnCvar_RegisterHUDVariable(const char *asName, const char *asValue, int anFlags)
{
	return (cvar_r*)Cvar_Get(asName, asValue, flags|CVAR_CLIENTDLL, "client cvar");
};

/*
=============
pfnAddClientCommand
=============
*/
int pfnAddClientCommand(const char *cmd_name, xcommand_t func)
{
	if(!cmd_name || !*cmd_name)
		return 0;
	
	// NOTE: if( func == NULL ) cmd will be forwarded to a server
	Cmd_AddClientCommand(cmd_name, func);
	return 1;
};