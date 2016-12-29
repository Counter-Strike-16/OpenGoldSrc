DBG_INTERFACE void Error(const char *asMsg, ...);
DBG_INTERFACE void ClearErrorLogs();

DBG_INTERFACE void _LogFunctionTrace(const char *asMsg, ...);
DBG_INTERFACE void _LogFunctionTraceMaxTime();

DBG_INTERFACE void _StartFunctionTimer();