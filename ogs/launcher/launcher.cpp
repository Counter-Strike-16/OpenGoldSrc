#include "tier0/platform.h"
#include "public/interface.h"
#include "public/FileSystem.h"
#include "tier0/ICommandLine.h"
#include "engine_launcher_api.h"

IFileSystem *gpFileSystem = NULL;

int AppMain(void *hInstance);

#if defined(_WIN32) && !defined(_DEBUG)
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return AppMain(hInstance);
	};
#else
	int main(int argc, char **argv)
	{
		void *hInstance = NULL;

#ifdef _WIN32
		hInstance = GetModuleHandle(NULL); // Any better idea?
#endif

		return AppMain(hInstance);
	};
#endif

CSysModule *LoadFilesystemModule() // put name in args?
{
	CSysModule *hFSModule = Sys_LoadModule("filesystem_stdio");
	
	if(!hFSModule)
	{
		//Plat_MessageBox(eMsgBoxType_Error, "Fatal Error", "Could not load filesystem dll.\nFileSystem crashed during construction.");
		
		// fallback to null impl (tracing)
		if(!(hFSModule = Sys_LoadModule("filesystem_null")))
			return NULL;
	};
	
	return hFSModule;
};

char *Sys_GetLongPathName()
{
	char szShortPath[MAX_PATH];
	static char szLongPath[MAX_PATH];
	char *pszPath;

	szShortPath[0] = 0;
	szLongPath[0] = 0;

	if(GetModuleFileName(NULL, szShortPath, sizeof(szShortPath)))
	{
		GetLongPathName(szShortPath, szLongPath, sizeof(szLongPath));
		pszPath = strrchr(szLongPath, '\\');

		if(pszPath[0])
			pszPath[1] = 0;

		int len = strlen(szLongPath);

		if(len > 0)
		{
			if(szLongPath[len - 1] == '\\' || szLongPath[len - 1] == '/')
				szLongPath[len - 1] = 0;
		};
	};

	return szLongPath;
};

bool OnVideoModeFailed()
{
	//registry->WriteInt("ScreenWidth", 640);
	//registry->WriteInt("ScreenHeight", 480);
	//registry->WriteInt("ScreenBPP", 16);
	//registry->WriteString("EngineDLL", "sw.dll");
	
	return true; //(Plat_MessageBox(eMsgBoxType_Warning, "Video mode change failure", "The specified video mode is not supported.\nThe game will now run in software mode.") == IDOK);
};

int AppMain(void *hInstance) // Sys_Main
{
	//Plat_Init();

	//registry->Init();

	CommandLine()->CreateCmdLine(GetCommandLine());
	CommandLine()->AppendParm("-nomaster", NULL);
	
	while(1)
	{
		static char sNewCommandParams[2048];
		sNewCommandParams[0] = 0;

		CSysModule *hFileSystemLib = LoadFilesystemModule();

		if(!hFileSystemLib)
			break;

		//MH_Init(szGameName);

		CreateInterfaceFn fnFileSystemFactory = Sys_GetFactory(hFileSystemLib);

		if(!fnFileSystemFactory)
			return EXIT_FAILURE;

		gpFileSystem = (IFileSystem*)fnFileSystemFactory(FILESYSTEM_INTERFACE_VERSION, NULL);

		if(!gpFileSystem)
			break;

		gpFileSystem->Mount();
		gpFileSystem->AddSearchPath(Sys_GetLongPathName(), "ROOT");

		IEngineAPI *pEngineAPI = NULL;
		CSysModule *hEngineLib = NULL;
		bool bUseBlobDLL = false;

		if(1 > 3) //if(FIsBlob(pszEngineDLL))
		{
			//Sys_CloseDEP();
			//SetupExceptHandler3();
			//NLoadBlobFile(pszEngineDLL, &g_blobfootprintClient, (void **)&pEngineAPI);
			bUseBlobDLL = true;
		}
		else
		{
			hEngineLib = Sys_LoadModule("engine");

			if(!hEngineLib)
			{
				//static char msg[512];
				//wsprintf(msg, "Could not load %s.\nPlease try again at a later time.", pszEngineDLL);
				//MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
				break;
			};

			CreateInterfaceFn fnEngineFactory = Sys_GetFactory(hEngineLib);

			if(!fnEngineFactory)
			{
				Sys_UnloadModule(hEngineLib);
				break;
			};

			pEngineAPI = (IEngineAPI*)fnEngineFactory(VENGINE_LAUNCHER_API_VERSION, NULL);

			if(!pEngineAPI)
			{
				Sys_UnloadModule(hEngineLib);
				break;
			};

			//if(!fnEngineFactory || !pEngineAPI)
				//Sys_UnloadModule(hEngineLib);
		};

		int nResult = ENGINE_RESULT_NONE;

		if(pEngineAPI)
		{
			//MH_LoadEngine(bUseBlobDLL ? NULL : (HMODULE)hEngine);
			nResult = pEngineAPI->Run(hInstance, Sys_GetLongPathName(), (char*)CommandLine()->GetCmdLine(), sNewCommandParams, Sys_GetFactoryThis(), Sys_GetFactory(hFileSystemLib));
			//MH_ExitGame(iResult);

			if(bUseBlobDLL)
				int a = 5; //FreeBlob(&g_blobfootprintClient);
			else
				Sys_UnloadModule(hEngineLib);
		};

		if(nResult == ENGINE_RESULT_NONE || nResult > ENGINE_RESULT_UNSUPPORTEDVIDEO)
			break;

		bool bContinue = false;

		switch(nResult)
		{
		case ENGINE_RESULT_RESTART:
			bContinue = true;
			break;
		case ENGINE_RESULT_UNSUPPORTEDVIDEO:
			bContinue = OnVideoModeFailed();
			break;
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

		if(strstr(sNewCommandParams, "-game"))
			CommandLine()->RemoveParm("-game");

		if(strstr(sNewCommandParams, "+load"))
			CommandLine()->RemoveParm("+load");

		CommandLine()->AppendParm(sNewCommandParams, NULL);

		// Move after while?
		gpFileSystem->Unmount();
		Sys_UnloadModule(hFileSystemLib);
		//MH_Shutdown();

		if(!bContinue)
			break;
	};

	//registry->Shutdown();

	/*
	if(hObject)
	{
		ReleaseMutex(hObject);
		CloseHandle(hObject);
	};
	*/

	//MH_Shutdown();
	TerminateProcess(GetCurrentProcess(), 1);

	//Plat_Shutdown();
	return EXIT_SUCCESS;
};