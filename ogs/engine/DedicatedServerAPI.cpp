//#include "precompiled.h"
#include "DedicatedServerAPI.h"

#ifndef HOOK_ENGINE
	EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);
#endif

bool CDedicatedServerAPI::Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	dedicated_ = (IDedicatedExports*)launcherFactory(VENGINE_DEDICATEDEXPORTS_API_VERSION, NULL);
	if(!dedicated_)
		return false;
	
#ifdef REHLDS_CHECKS
	Q_strncpy(this->m_OrigCmd, cmdline, ARRAYSIZE(this->m_OrigCmd));
	this->m_OrigCmd[ARRAYSIZE(this->m_OrigCmd) - 1] = 0;
#else
	Q_strcpy(this->m_OrigCmd, cmdline);
#endif
	
	if(!Q_strstr(cmdline, "-nobreakpad"))
		CRehldsPlatformHolder::get()->SteamAPI_UseBreakpadCrashHandler(va("%d", build_number()), "Aug  8 2013", "11:17:26", 0, 0, 0);
	
	TraceInit("Sys_InitArgv( m_OrigCmd )", "Sys_ShutdownArgv()", 0);
	Sys_InitArgv(m_OrigCmd);
	eng->SetQuitting(IEngine::QUIT_NOTQUITTING);
	registry->Init();
	
	g_bIsDedicatedServer = TRUE;
	TraceInit("FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0);
	
	if(FileSystem_Init(basedir, filesystemFactory) && game->Init(0) && eng->Load(true, basedir, cmdline))
	{
		char text[256];
		Q_snprintf(text, ARRAYSIZE(text), "exec %s\n", servercfgfile.string);
		text[255] = 0;
		Cbuf_InsertText(text);
		return true;
	};
	
	return false;
};

int CDedicatedServerAPI::Shutdown()
{
	eng->Unload();
	game->Shutdown();
	
	TraceShutdown("FileSystem_Shutdown()", 0);
	FileSystem_Shutdown();
	
	registry->Shutdown();
	
	TraceShutdown("Sys_ShutdownArgv()", 0);
	dedicated_ = NULL;
	return giActive;
};

bool CDedicatedServerAPI::RunFrame()
{
	if(eng->GetQuitting())
		return false;
	
	eng->Frame();
	return true;
};

void CDedicatedServerAPI::AddConsoleText(char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
	Host_GetHostInfo(fps, nActive, NULL, nMaxPlayers, pszMap);
};