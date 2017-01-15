
void Key_Bind_Null_f()
{
}

void CL_Drop ()
{
}

void CL_Frame (int msec)
{
}

void Con_Print (char *text)
{
}

void Cmd_ForwardToServer ()
{
	char *cmd;

	cmd = Cmd_Argv(0);
	Com_Printf ("Unknown command \"%s\"\n", cmd);
}

void SCR_DebugGraph (float value, int color)
{
}

void SCR_BeginLoadingPlaque ()
{
}

void SCR_EndLoadingPlaque ()
{
}

void Key_Init ()
{
	Cmd_AddCommand ("bind", Key_Bind_Null_f);
}

