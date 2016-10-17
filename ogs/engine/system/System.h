#pragma once

/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

class CSystem
{
public:
	CSystem();
	~CSystem();
	
	void Init();
	
	void AppActivate();
	
	// loads the game dll and calls the api init function
	void *Sys_GetGameAPI(void *parms);
	
	void UnloadGame();
	
	char *ConsoleInput();
	void ConsoleOutput(char *string);
	
	void SendKeyEvents();
	
	void Error(char *error, ...);
	
	void Quit();
	
	char *GetClipboardData();
	
	void CopyProtect();
};