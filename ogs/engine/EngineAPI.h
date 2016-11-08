// Same interface as in Source, used to start the engine as local host (server + client)
// Older versions of GoldSrc are using C interfaces for these purposes (?) (look at dedicated
// server launcher in 2.3 SDK)

#ifndef OGS_ENGINEAPI_H
#define OGS_ENGINEAPI_H

#include "engine_launcher_api.h"

class CEngineAPI : public IEngineAPI
{
public:
	int Run(void *instance,
			char *basedir,
			char *cmdline,
			char *postRestartCmdLineArgs,
			CreateInterfaceFn launcherFactory,
			CreateInterfaceFn filesystemFactory);
};

#endif // OGS_ENGINEAPI_H