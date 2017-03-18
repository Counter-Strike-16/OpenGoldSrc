#include "precompiled.hpp"
#include "system/client.hpp"
#include "video/IWindow.hpp"

void Sys_Error(const char *asMsg, ...)
{
};

void Sys_SplitPath(char const *,char *,char *,char *,char *)
{
};

void *FS_Open(char const *,char const *)
{
	return nullptr;
};

void FS_Close(void *)
{
};

int FS_FPrintf(void *,char *,...)
{
	return 0;
};

void Cmd_AddCommand(char *,void (__cdecl*)(void))
{
};

void Con_Printf(char const *,...)
{
};

void Con_DPrintf(char const *,...)
{
};

client_state_t cl;

int allow_cheats;

int COM_CheckParm(char *)
{
	return 0;
};

int com_argc;
char **com_argv;

IReHLDSPlatform *CRehldsPlatformHolder::get(void)
{
	return nullptr;
};

void Cbuf_AddText(char *)
{
};

void Cbuf_InsertText(char *)
{
};

char *va(char *,...)
{
	return nullptr;
};

void Host_GetHostInfo(float *,int *,int *,int *,char *)
{
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

int FileSystem_Init(char *,void *)
{
	return 0;
};

void FileSystem_Shutdown(void)
{
};

int giActive;

struct cvar_s servercfgfile;

IRegistry *registry = nullptr;

int Host_Frame(float,int,int *)
{
	return 0;
};

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

void __cdecl Cbuf_Execute(void)
{
};

void __cdecl Cvar_DirectSet(struct cvar_s *,char const *)
{
};

void __cdecl Cvar_Set(char const *,char const *)
{
};

struct cvar_s rate_;

struct client_static_s cls;

IWindow *VID_OpenWindow(void)
{
	return nullptr;
};

struct HWND__ *mainwindow;

unsigned int __cdecl crc32c(unsigned char const *,int)
{
	return 0;
};

void __cdecl MSG_WriteString(struct sizebuf_s *,char const *)
{
};									