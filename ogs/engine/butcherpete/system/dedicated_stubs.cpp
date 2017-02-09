// Call these funcs in order to set their impl ro null for dedicated mode

void Con_Shutdown_Null();
void Con_NPrintf_Null(int idx, const char *fmt, ...);

typedef void (*pfnCon_Shutdown)();
typedef void (*pfnCon_NPrintf)(int idx, const char *fmt, ...);

pfnCon_Shutdown Con_Shutdown = nullptr;
pfnCon_NPrintf Con_NPrintf = nullptr;

void Sys_InitDedicatedMode()
{
	Con_Shutdown = Con_Shutdown_Null;
	Con_NPrintf = Con_NPrintf_Null;
};