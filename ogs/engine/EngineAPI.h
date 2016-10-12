#ifndef ENGINEAPI_H
#define ENGINEAPI_H

class CEngineAPI : public IEngineAPI
{
public:
	int Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory);
};

#endif // ENGINEAPI_H