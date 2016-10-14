#pragma once

#include "engine_hlds_api.h"
#include "idedicatedexports.h"

class CDedicatedServerAPI : public IDedicatedServerAPI
{
public:
	virtual bool Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory);
	virtual int Shutdown();
	
	virtual bool RunFrame();
	
	virtual void AddConsoleText(char *text);
	
	virtual void UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap);
private:
	char m_OrigCmd[1024];
};