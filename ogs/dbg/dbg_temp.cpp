void Error(const char *sMsg, ...)
{
};

void ClearErrorLogs()
{
};

void _LogFunctionTrace(const char *asMsg, ...)
{
	static FILE *hLogFile = fopen("trace.log", "w+");
	
	if(!hLogFile)
		return;

	va_list ArgList;
	char sMsg[512];

	va_start(ArgList, asMsg);
	vsnprintf(sMsg, sizeof(sMsg), asMsg, ArgList);
	va_end(ArgList);

	sprintf(sMsg, "%s\n", sMsg);
	fputs(sMsg, hLogFile);
	fflush(hLogFile);
};

void _LogFunctionTraceMaxTime()
{
};

void _StartFunctionTimer()
{
};