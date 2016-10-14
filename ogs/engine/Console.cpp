void Con_Debug_f(void)
{
	if (con_debuglog)
	{
		Con_Printf("condebug disabled\n");
		con_debuglog = FALSE;
	}
	else
	{
		con_debuglog = TRUE;
		Con_Printf("condebug enabled\n");
	}
}

void Con_Init(void)
{
	con_debuglog = COM_CheckParm("-condebug");
	Con_DPrintf("Console initialized.\n");

#ifdef HOOK_ENGINE
	Cmd_AddCommand("condebug", (xcommand_t)GetOriginalFuncAddrOrDefault("Con_Debug_f", (void *)Con_Debug_f));
#else
	Cmd_AddCommand("condebug", Con_Debug_f);
#endif
}

/* <8dc81> ../engine/sys_dll.c:2385 */
void Con_DebugLog(const char *file, const char *fmt, ...)
{
	va_list argptr;
	static char data[8192];

	va_start(argptr, fmt);
	Q_vsnprintf(data, sizeof(data), fmt, argptr);
	va_end(argptr);

	data[sizeof(data) - 1] = 0;

#ifdef _WIN32

	int fd = _open(file, _O_WRONLY | _O_APPEND | _O_CREAT, _S_IREAD | _S_IWRITE);
	int len = Q_strlen(data);
	_write(fd, data, len);
	_close(fd);

#else // _WIN32

	FILE *fd = FS_Open(file, "at");
	FS_FPrintf(fd, "%s", data);
	FS_Close(fd);

#endif // _WIN32
}

/* <8dcfd> ../engine/sys_dll.c:2407 */
void EXT_FUNC Con_Printf(const char *fmt, ...)
{
	char Dest[4096];
	va_list va;

	va_start(va, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, va);
	va_end(va);

#ifdef REHLDS_FLIGHT_REC
	FR_Log("REHLDS_CON", Dest);
#endif

#ifdef REHLDS_FIXES
	if (sv_redirected == RD_NONE || sv_rcon_condebug.value > 0.0f)
#endif
	{
		Sys_Printf("%s", Dest);
	}

	if (sv_redirected)
	{
		if ((Q_strlen(outputbuf) + Q_strlen(Dest)) > sizeof(outputbuf) - 1)
			SV_FlushRedirect();
		Q_strncat(outputbuf, Dest, sizeof(outputbuf) - 1);
	}
	else
	{
		if (con_debuglog)
			Con_DebugLog("qconsole.log", "%s", Dest);
#ifndef SWDS
		if (host_initialized && con_initialized && g_pcls.state)
		{
			if (developer.value != 0.0f)
			{
				Q_strncpy(g_szNotifyAreaString, msg, 255);
				g_szNotifyAreaString[255] = 0;
				*con_times = realtime;
			}
			VGuiWrap2_ConPrintf(msg);
		}
#endif // SWDS
	}
}

/* <8dfae> ../engine/sys_dll.c:2442 */
void Con_SafePrintf(const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

#ifdef _WIN32
	char Dest[1024];
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
	va_end(argptr);
	Con_Printf("%s", Dest);
#else
	vfprintf(stdout, fmt, argptr);
	va_end(argptr);
	fflush(stdout);
#endif // _WIN32
}

/* <8e00b> ../engine/sys_dll.c:2459 */
#if defined(REHLDS_FIXES) && defined(REHLDS_FLIGHT_REC)
// Always print debug logs to the flight recorder
void EXT_FUNC Con_DPrintf(const char *fmt, ...)
{
	char Dest[4096];
	va_list argptr;
	va_start(argptr, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
	va_end(argptr);

	FR_Log("REHLDS_CONDBG", Dest);

	if (developer.value != 0.0f)
	{
#ifdef _WIN32
		OutputDebugStringA(Dest);
		if (con_debuglog)
			Con_DebugLog("qconsole.log", "%s", Dest);
#else
		vfprintf(stdout, fmt, argptr);
		fflush(stdout);
#endif // _WIN32
	}
}

#else //defined(REHLDS_FIXES) and defined(REHLDS_FLIGHT_REC)

void EXT_FUNC Con_DPrintf(const char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	if (developer.value != 0.0f)
	{
#ifdef _WIN32
		char Dest[4096];
		Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);

		OutputDebugStringA(Dest);
		if (con_debuglog)
			Con_DebugLog("qconsole.log", "%s", Dest);
#else
		vfprintf(stdout, fmt, argptr);
		fflush(stdout);
#endif // _WIN32
	}
	va_end(argptr);
}

#endif // defined(REHLDS_FIXES) and defined(REHLDS_FLIGHT_REC)
