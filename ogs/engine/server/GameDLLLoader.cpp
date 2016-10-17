// All game dll exported funcs are passing by func pointers
// Main set (DLL_FUNCTIONS) is required, additional (NEW_DLL_FUNCTIONS) is optional
// Moreover, funcs of main can be missed because of old interface version or someone's "experiments" so we need to handle that cases
// TODO: replace blank pointers by null impl from null-object impl

#include "server/GameDLLLoader.h"

CGameDLLLoader::CGameDLLLoader()
{
};

CGameDLLLoader::CGameDLLLoader()
{
};

void CGameDLLLoader::LoadEntityDLLs(const char *szBaseDir)
{
	FileHandle_t hLibListFile;
	unsigned int nFileSize;
	unsigned int nFileSize2;
	char *pszInputStream;
	int nBytesRead;
	char *pStreamPos;
	const char *findfn;
	NEW_DLL_FUNCTIONS_FN pNewAPI;
	APIFUNCTION2 pfnGetAPI2;
	APIFUNCTION pfnGetAPI;
	char szDllFilename[8192];
	char szDllWildcard[260];
	char szDllListFile[260];
	char szValue[256];
	char szKey[64];
	char szGameDir[64];
	int interface_version;

	SV_ResetModInfo();
	g_iextdllMac = 0;
	Q_memset(g_rgextdll, 0, sizeof(g_rgextdll));

	Q_strncpy(szGameDir, com_gamedir, sizeof(szGameDir) - 1);
	if (Q_stricmp(szGameDir, "valve"))
		gmodinfo.bIsMod = 1;

	Q_snprintf(szDllListFile, sizeof(szDllListFile), "%s", "liblist.gam");
	hLibListFile = FS_Open(szDllListFile, "rb");
	if (hLibListFile)
	{
		nFileSize = FS_Size(hLibListFile);
		nFileSize2 = nFileSize;
		if (!nFileSize || (signed int)nFileSize > 262144)
			Sys_Error("Game listing file size is bogus [%s: size %i]", "liblist.gam", nFileSize);

		pszInputStream = (char *)Mem_Malloc(nFileSize + 1);
		if (!pszInputStream)
			Sys_Error("Could not allocate space for game listing file of %i bytes", nFileSize2 + 1);

		nBytesRead = FS_Read(pszInputStream, nFileSize2, 1, hLibListFile);
		if (nBytesRead != nFileSize2)
			Sys_Error("Error reading in game listing file, expected %i bytes, read %i", nFileSize2, nBytesRead);

		pszInputStream[nFileSize2] = 0;
		pStreamPos = pszInputStream;
		com_ignorecolons = 1;
		while (1)
		{
			pStreamPos = COM_Parse(pStreamPos);
			if (Q_strlen(com_token) <= 0)
				break;

			Q_strncpy(szKey, com_token, sizeof(szKey) - 1);
			szKey[sizeof(szKey) - 1] = 0;
			pStreamPos = COM_Parse(pStreamPos);
			Q_strncpy(szValue, com_token, sizeof(szValue) - 1);
			szValue[sizeof(szValue) - 1] = 0;
#ifdef _WIN32
			if (Q_stricmp(szKey, "gamedll"))
#else // _WIN32
			if (Q_stricmp(szKey, "gamedll_linux"))
#endif // _WIN32
			{
				DLL_SetModKey(&gmodinfo, szKey, szValue);
			}
			else
			{
				int index = COM_CheckParm("-dll");
				if (index && index < com_argc - 1)
				{
					Q_strncpy(szValue, com_argv[index + 1], sizeof(szValue) - 1);
					szValue[sizeof(szValue) - 1] = 0;
				}
#ifdef _WIN32
				if (Q_strstr(szValue, ".dll"))
#else // _WIN32
				if (Q_strstr(szValue, ".so"))
#endif // _WIN32
				{
					FS_GetLocalPath(szValue, szDllFilename, sizeof(szDllFilename));
					Con_DPrintf("\nAdding:  %s/%s\n", szGameDir, szValue);
					LoadThisDll(szDllFilename);
				}
				else
				{
					Con_DPrintf("Skipping non-dll:  %s\n", szValue);
				}
			}
		}
		com_ignorecolons = 0;
		Mem_Free(pszInputStream);
		FS_Close(hLibListFile);
	}
	else
	{
#ifdef _WIN32
		Q_snprintf(szDllWildcard, sizeof(szDllWildcard), "%s\\*.dll", "valve\\dlls");
#else // _WIN32
		Q_snprintf(szDllWildcard, sizeof(szDllWildcard), "%s\\*.so", "valve\\dlls");
#endif // _WIN32
		for (findfn = Sys_FindFirst(szDllWildcard, 0); findfn; findfn = Sys_FindNext(0))
		{
			Q_snprintf(szDllFilename, sizeof(szDllWildcard), "%s/%s/%s", szBaseDir, "valve\\dlls", findfn);
			LoadThisDll(szDllFilename);
		}
		Sys_FindClose();
	}
	gNewDLLFunctions.pfnOnFreeEntPrivateData = NULL;
	gNewDLLFunctions.pfnGameShutdown = NULL;
	gNewDLLFunctions.pfnShouldCollide = NULL;
	gNewDLLFunctions.pfnCvarValue = NULL;
	gNewDLLFunctions.pfnCvarValue2 = NULL;
	pNewAPI = (NEW_DLL_FUNCTIONS_FN)GetDispatch("GetNewDLLFunctions");
	if (pNewAPI)
	{
		interface_version = NEW_DLL_FUNCTIONS_VERSION;
		pNewAPI(&gNewDLLFunctions, &interface_version);
	}

	pfnGetAPI2 = (APIFUNCTION2)GetDispatch("GetEntityAPI2");
	if (pfnGetAPI2)
	{
		interface_version = INTERFACE_VERSION;
		if (!pfnGetAPI2(&gEntityInterface, &interface_version))
		{
			Con_Printf("==================\n");
			Con_Printf("Game DLL version mismatch\n");
			Con_Printf("DLL version is %i, engine version is %i\n", interface_version, INTERFACE_VERSION);
			if (interface_version <= INTERFACE_VERSION)
				Con_Printf("The game DLL for %s appears to be outdated, check for updates\n", szGameDir);
			else
				Con_Printf("Engine appears to be outdated, check for updates\n");
			Con_Printf("==================\n");
			Host_Error("\n");
		}
	}
	else
	{
		pfnGetAPI = (APIFUNCTION)GetDispatch("GetEntityAPI");
		if (!pfnGetAPI)
			Host_Error("Couldn't get DLL API from %s!", szDllFilename);
		interface_version = INTERFACE_VERSION;
		if (!pfnGetAPI(&gEntityInterface, interface_version))
		{
			Con_Printf("==================\n");
			Con_Printf("Game DLL version mismatch\n");
			Con_Printf("The game DLL for %s appears to be outdated, check for updates\n", szGameDir);
			Con_Printf("==================\n");
			Host_Error("\n");
		}
	}

	Con_DPrintf("Dll loaded for %s %s\n", gmodinfo.bIsMod ? "mod" : "game", gEntityInterface.pfnGetGameDescription());
}

void CGameDLLLoader::ReleaseEntityDlls()
{
	extensiondll_t *pextdll;
	extensiondll_t *pextdllMac;

	if (!g_psvs.dll_initialized)
		return;

	FreeAllEntPrivateData();
	
	if (gNewDLLFunctions.pfnGameShutdown)
		gNewDLLFunctions.pfnGameShutdown();

	Cvar_UnlinkExternals();

	pextdll = &g_rgextdll[0];
	pextdllMac = &g_rgextdll[g_iextdllMac];

	while (pextdll < pextdllMac)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)pextdll->lDLLHandle);
#else
		dlclose(pextdll->lDLLHandle);
#endif // _WIN32

		pextdll->lDLLHandle = NULL;
		if (pextdll->functionTable)
		{
			Mem_Free((void *)pextdll->functionTable);
			pextdll->functionTable = NULL;
		}
		pextdll++;
	}
	g_psvs.dll_initialized = FALSE;
}