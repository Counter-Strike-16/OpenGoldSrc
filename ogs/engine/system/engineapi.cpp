#include "precompiled.h"
#include "system/engineapi.h"
#include "system/iengine.h"

static CEngineAPI g_CEngineAPI;

/*
IBaseInterface *CreateCEngineAPI()
{
	return &g_CEngineAPI;
};

InterfaceReg g_CreateCEngineAPI = InterfaceReg(CreateCEngineAPI, "VENGINE_LAUNCHER_API_VERSION002");
*/

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineAPI, IEngineAPI, VENGINE_LAUNCHER_API_VERSION, g_CEngineAPI);

int CEngineAPI::Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	if(!eng->Load(false, basedir, cmdline))
		return 1;
	
	while(!eng->GetQuitting()) // ?
		eng->Frame();
	
	eng->Unload();
	return 0;
};