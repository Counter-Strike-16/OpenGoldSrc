#include <stdio.h>
#include <windows.h>

//#include "metahook.h"
#include "engine_launcher_api.h"
#include "LoadBlob.h"
#include "ExceptHandle.h"
//#include "sys.h"
#include "IFileSystem.h"
#include "ICommandLine.h"
#include "IRegistry.h"

#pragma warning(disable : 4733)

IFileSystem *g_pFileSystem = NULL;

HINTERFACEMODULE LoadFilesystemModule()
{
	HINTERFACEMODULE hModule = Sys_LoadModule("filesystem_stdio");
	
	if(!hModule)
	{
		//Plat_MessageBox(eMsgBoxType_Error, "Fatal Error", "Could not load filesystem dll.\nFileSystem crashed during construction.");
		return NULL;
	};
	
	return hModule;
};

void SetEngineDLL(const char *&pszEngineDLL)
{
	pszEngineDLL = "hw.dll";
	
	registry->WriteString("EngineDLL", pszEngineDLL);
};

BOOL OnVideoModeFailed()
{
	registry->WriteInt("ScreenWidth", 640);
	registry->WriteInt("ScreenHeight", 480);
	registry->WriteInt("ScreenBPP", 16);
	registry->WriteString("EngineDLL", "sw.dll");
	
	return (Plat_MessageBox(eMsgBoxType_Warning, "Video mode change failure", "The specified video mode is not supported.\nThe game will now run in software mode.") == IDOK);
};

BlobFootprint_t g_blobfootprintClient;

#ifdef WIN32
	int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return Sys_Main();
	};
#else
	int main(int argc, char **argv)
	{
		return Sys_Main();
	};
#endif

int Sys_Main()
{
	Plat_Init();
	
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
	
	registry->Init();
	
	CommandLine()->CreateCmdLine(GetCommandLine());
	CommandLine()->AppendParm("-nomaster", NULL);
	
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
		HINTERFACEMODULE hFileSystem = LoadFilesystemModule();
		
		if(!hFileSystem)
			break;
		
		//MH_Init(szGameName);
		
		CreateInterfaceFn fsCreateInterface = (CreateInterfaceFn)Sys_GetFactory(hFileSystem);
		g_pFileSystem = (IFileSystem*)fsCreateInterface(FILESYSTEM_INTERFACE_VERSION, nullptr);
		g_pFileSystem->Mount();
		g_pFileSystem->AddSearchPath(Sys_GetLongPathName(), "ROOT");
		
		static char szNewCommandParams[2048];
		const char *pszEngineDLL = nullptr;
		int iResult = ENGINE_RESULT_NONE;
		
		szNewCommandParams[0] = 0;
		SetEngineDLL(pszEngineDLL);
		
		g_blobfootprintClient.m_hDll = NULL;
		
		IEngineAPI *engineAPI = nullptr;
		HINTERFACEMODULE hEngine;
		bool bUseBlobDLL = false;
		
		if(FIsBlob(pszEngineDLL))
		{
			Sys_CloseDEP();
			SetupExceptHandler3();
			NLoadBlobFile(pszEngineDLL, &g_blobfootprintClient, (void **)&engineAPI);
			bUseBlobDLL = true;
		}
		else
		{
			hEngine = Sys_LoadModule(pszEngineDLL);
			
			if(!hEngine)
			{
				static char msg[512];
				wsprintf(msg, "Could not load %s.\nPlease try again at a later time.", pszEngineDLL);
				MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
				break;
			};
			
			CreateInterfaceFn engineCreateInterface = (CreateInterfaceFn)Sys_GetFactory(hEngine);
			engineAPI = (IEngineAPI*)engineCreateInterface(VENGINE_LAUNCHER_API_VERSION, NULL);
			
			if(!engineCreateInterface || !engineAPI)
				Sys_FreeModule(hEngine);
		};
		
		if(engineAPI)
		{
			//MH_LoadEngine(bUseBlobDLL ? NULL : (HMODULE)hEngine);
			iResult = engineAPI->Run(hInstance, Sys_GetLongPathName(), CommandLine()->GetCmdLine(), szNewCommandParams, Sys_GetFactoryThis(), Sys_GetFactory(hFileSystem));
			//MH_ExitGame(iResult);
			
			if(bUseBlobDLL)
				FreeBlob(&g_blobfootprintClient);
			else
				Sys_FreeModule(hEngine);
		};
		
		if(iResult == ENGINE_RESULT_NONE || iResult > ENGINE_RESULT_UNSUPPORTEDVIDEO)
			break;
		
		bool bContinue = false;
		
		switch(iResult)
		{
			case ENGINE_RESULT_RESTART:
			{
				bContinue = true;
				break;
			};
			
			case ENGINE_RESULT_UNSUPPORTEDVIDEO:
			{
				bContinue = OnVideoModeFailed();
				break;
			};
		};
		
		CommandLine()->RemoveParm("-sw");
		CommandLine()->RemoveParm("-startwindowed");
		CommandLine()->RemoveParm("-windowed");
		CommandLine()->RemoveParm("-window");
		CommandLine()->RemoveParm("-full");
		CommandLine()->RemoveParm("-fullscreen");
		CommandLine()->RemoveParm("-soft");
		CommandLine()->RemoveParm("-software");
		CommandLine()->RemoveParm("-gl");
		CommandLine()->RemoveParm("-d3d");
		CommandLine()->RemoveParm("-w");
		CommandLine()->RemoveParm("-width");
		CommandLine()->RemoveParm("-h");
		CommandLine()->RemoveParm("-height");
		CommandLine()->RemoveParm("-novid");
		
		if(strstr(szNewCommandParams, "-game"))
			CommandLine()->RemoveParm("-game");
		
		if(strstr(szNewCommandParams, "+load"))
			CommandLine()->RemoveParm("+load");
		
		CommandLine()->AppendParm(szNewCommandParams, NULL);
		
		g_pFileSystem->Unmount();
		Sys_FreeModule(hFileSystem);
		//MH_Shutdown();
		
		if(!bContinue)
			break;
	};
	
	registry->Shutdown();
	
	if(hObject)
	{
		ReleaseMutex(hObject);
		CloseHandle(hObject);
	};
	
	//MH_Shutdown();
	TerminateProcess(GetCurrentProcess(), 1);
	
	Plat_Shutdown();
	return 1;
};