#if 0

// Call these funcs in order to set their impl to null for dedicated mode

namespace null
{

void Con_Shutdown();
void Con_NPrintf(int idx, const char *fmt, ...);

}; // namespace null

typedef void (*pfnCon_Shutdown)();
typedef void (*pfnCon_NPrintf)(int idx, const char *fmt, ...);

pfnCon_Shutdown Con_Shutdown = nullptr;
pfnCon_NPrintf Con_NPrintf = nullptr;

void Sys_InitDedicatedMode()
{
	Con_Shutdown = null::Con_Shutdown;
	Con_NPrintf = null::Con_NPrintf;
};

#endif