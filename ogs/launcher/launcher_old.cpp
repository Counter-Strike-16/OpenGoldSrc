#include <stdio.h>
#include <windows.h>

//#include "metahook.h"
#include "LoadBlob.h"
#include "ExceptHandle.h"
//#include "sys.h"

#include "IRegistry.h"

#pragma warning(disable : 4733)



void SetEngineDLL(const char *&pszEngineDLL)
{
	pszEngineDLL = "hw.dll";
	
	registry->WriteString("EngineDLL", pszEngineDLL);
};

BlobFootprint_t g_blobfootprintClient;

int Sys_Main()
{
	
	
	HANDLE hObject = NULL;
	BOOL (*IsDebuggerPresent)() = (BOOL (*)())GetProcAddress(GetModuleHandle("kernel32.dll"), "IsDebuggerPresent");
	
	if(!IsDebuggerPresent())
	{
		hObject = CreateMutex(NULL, FALSE, "ValveHalfLifeLauncherMutex");
		DWORD dwStatus = WaitForSingleObject(hObject, 0);
		
		if(dwStatus && dwStatus != WAIT_ABANDONED)
		{
			Plat_MessageBox(eMsgBoxType_Error, "Error", "Could not launch game.\nOnly one instance of this game can be run at a time.");
			return 0;
		};
	};
	
	char szFileName[256];
	Sys_GetExecutableName(szFileName, sizeof(szFileName));
	char *szExeName = strrchr(szFileName, '\\') + 1;
	
	if(stricmp(szExeName, "hl.exe") && CommandLine()->CheckParm("-game") == NULL)
	{
		szExeName[strlen(szExeName) - 4] = '\0';
		CommandLine()->AppendParm("-game", szExeName);
	};
	
	const char *_szGameName = "";
	static char szGameName[32];
	const char *szGameStr = CommandLine()->CheckParm("-game", &_szGameName);
	strcpy(szGameName, _szGameName);
	
	if(szGameStr && !strnicmp(&szGameStr[6], "czero", 5))
		CommandLine()->AppendParm("-forcevalve", NULL);
	
	if(registry->ReadInt("CrashInitializingVideoMode", 0))
	{
		registry->WriteInt("CrashInitializingVideoMode", 0);
		
		if(strcmp(registry->ReadString("EngineDLL", "hw.dll"), "hw.dll"))
		{
			if(registry->ReadInt("EngineD3D", 0))
			{
				registry->WriteInt("EngineD3D", 0);
				
				if(MessageBox(NULL, "The game has detected that the previous attempt to start in D3D video mode failed.\nThe game will now run attempt to run in openGL mode.", "Video mode change failure", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
					return 0;
			}
			else
			{
				registry->WriteString("EngineDLL", "sw.dll");
				
				if(MessageBox(NULL, "The game has detected that the previous attempt to start in openGL video mode failed.\nThe game will now run in software mode.", "Video mode change failure", MB_OKCANCEL | MB_ICONWARNING) != IDOK)
					return 0;
			};
			
			registry->WriteInt("ScreenWidth", 640);
			registry->WriteInt("ScreenHeight", 480);
			registry->WriteInt("ScreenBPP", 16);
		};
	};
	
	while(1)
	{
		
		
		const char *pszEngineDLL = nullptr;
		
		
		
		SetEngineDLL(pszEngineDLL);
		
		g_blobfootprintClient.m_hDll = NULL;
	};
	
	
	
	
	return 1;
};