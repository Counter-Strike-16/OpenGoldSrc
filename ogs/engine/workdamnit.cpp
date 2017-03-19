#include "precompiled.hpp"
#include "system/client.hpp"
#include "video/IWindow.hpp"

void Sys_Error(const char *asMsg, ...)
{
};

void Sys_SplitPath(char const *,char *,char *,char *,char *)
{
};

client_state_t cl;

int allow_cheats;

IReHLDSPlatform *CRehldsPlatformHolder::get(void)
{
	return nullptr;
};

void Sys_InitArgv(char *)
{
};

void TraceInit(char const *,char const *,int)
{
};

void TraceShutdown(char const *,int)
{
};

int giActive;

struct cvar_s servercfgfile;

IRegistry *registry = nullptr;

extern "C" void ClientDLL_MouseEvent(int)
{
};

double __cdecl Sys_FloatTime(void)
{
	return 0.0f;
};

void __cdecl GameSetSubState(int)
{
};

void __cdecl GameSetState(int)
{
};

char const * __cdecl GetRateRegistrySetting(char const *)
{
	return "";
};

int __cdecl Sys_InitGame(char const *,char *,void *,int)
{
	return 0;
};

void __cdecl Sys_ShutdownGame(void)
{
};

struct cvar_s rate_;

struct client_static_s cls;

IWindow *VID_OpenWindow(void)
{
	return nullptr;
};

struct HWND__ *mainwindow;

char * __cdecl Cvar_Serverinfo(void)
{
	return nullptr;
};

void __cdecl SV_BroadcastCommand(char *,...)
{
};

void __cdecl SV_BroadcastPrintf(char const *,...)
{
};

int __cdecl Q_UnicodeValidate(char const *)
{
	return 0;
};

int __cdecl Q_StripUnprintableAndSpace(char *)
{
	return 0;
};

int __cdecl Q_UnicodeRepair(char *)
{
	return 0;
};

//CRehldsHookchains g_RehldsHookchains;

//_g_engdstAddrs; 