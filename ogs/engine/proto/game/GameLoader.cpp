/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "precompiled.hpp"
#include "system/System.hpp"
#include "system/common.hpp"
#include "system/Host.hpp"
#include "memory/mem.hpp"
#include "filesystem/FileSystem.hpp"
#include "resources/modinfo.hpp"
#include "console/console.hpp"
#include "console/cvar.hpp"
#include "world/pr_edict.hpp"
#include "system/server.hpp"

void LoadEntityDLLs(const char *szBaseDir)
{
	FileHandle_t hLibListFile;
	unsigned int nFileSize;
	unsigned int nFileSize2;
	char *pszInputStream;
	int nBytesRead;
	char *pStreamPos;
	const char *findfn;
	
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
	if(Q_stricmp(szGameDir, "valve"))
		gmodinfo.bIsMod = 1;

	Q_snprintf(szDllListFile, sizeof(szDllListFile), "%s", "liblist.gam");
	hLibListFile = FS_Open(szDllListFile, "rb");
	if(hLibListFile)
	{
		nFileSize = FS_Size(hLibListFile);
		nFileSize2 = nFileSize;
		if(!nFileSize || (signed int)nFileSize > 262144)
			Sys_Error("Game listing file size is bogus [%s: size %i]", "liblist.gam", nFileSize);

		pszInputStream = (char *)Mem_Malloc(nFileSize + 1);
		if(!pszInputStream)
			Sys_Error("Could not allocate space for game listing file of %i bytes",
			          nFileSize2 + 1);

		nBytesRead = FS_Read(pszInputStream, nFileSize2, 1, hLibListFile);
		if(nBytesRead != nFileSize2)
			Sys_Error(
			"Error reading in game listing file, expected %i bytes, read %i",
			nFileSize2,
			nBytesRead);

		pszInputStream[nFileSize2] = 0;
		pStreamPos = pszInputStream;
		com_ignorecolons = 1;
		while(1)
		{
			pStreamPos = COM_Parse(pStreamPos);
			if(Q_strlen(com_token) <= 0)
				break;

			Q_strncpy(szKey, com_token, sizeof(szKey) - 1);
			szKey[sizeof(szKey) - 1] = 0;
			pStreamPos = COM_Parse(pStreamPos);
			Q_strncpy(szValue, com_token, sizeof(szValue) - 1);
			szValue[sizeof(szValue) - 1] = 0;
			
#ifdef _WIN32
			if(Q_stricmp(szKey, "gamedll"))
#elif __linux__
			if(Q_stricmp(szKey, "gamedll_linux"))
#elif __APPLE__
			if(Q_stricmp(szKey, "gamedll_mac"))
#endif // _WIN32
			{
				DLL_SetModKey(&gmodinfo, szKey, szValue);
			}
			else
			{
				int index = COM_CheckParm("-dll");
				if(index && index < com_argc - 1)
				{
					Q_strncpy(szValue, com_argv[index + 1], sizeof(szValue) - 1);
					szValue[sizeof(szValue) - 1] = 0;
				}
				
#ifdef REHLDS_FIXES

				char *value_extension = Q_strrchr(szValue, '.');
				
#ifdef _WIN32
				if (value_extension && Q_strcmp(value_extension, ".dll") == 0)
#else // _WIN32
				if (value_extension && Q_strcmp(value_extension, ".so") == 0)
#endif // _WIN32

#else // REHLDS_FIXES

#ifdef _WIN32
				if(Q_strstr(szValue, ".dll"))
#else  // _WIN32
				if(Q_strstr(szValue, ".so"))
#endif // _WIN32

#endif // REHLDS_FIXES
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
#else  // _WIN32
		Q_snprintf(szDllWildcard, sizeof(szDllWildcard), "%s\\*.so", "valve\\dlls");
#endif // _WIN32
		for(findfn = Sys_FindFirst(szDllWildcard, 0); findfn;
		    findfn = Sys_FindNext(0))
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

	if(pNewAPI)
	{
		interface_version = NEW_DLL_FUNCTIONS_VERSION;
		pNewAPI(&gNewDLLFunctions, &interface_version);
	}

}

#ifdef _WIN32
HMODULE LoadWindowsDLL(LPCSTR lpLibFileName)
{
	if(!lpLibFileName)
		return NULL;

	FS_GetLocalCopy(lpLibFileName);
	return LoadLibraryA(lpLibFileName);
}
#endif //_WIN32

void LoadThisDll(const char *szDllFilename)
{
#ifdef _WIN32
	typedef void (__stdcall * PFN_GiveFnptrsToDll)(enginefuncs_t *, globalvars_t *);
#else
	typedef void (__cdecl * PFN_GiveFnptrsToDll)(enginefuncs_t *, globalvars_t *);
#endif // _WIN32
	
	PFN_GiveFnptrsToDll pfnGiveFnptrsToDll;
	extensiondll_t *pextdll;

#ifdef _WIN32
	HMODULE hDLL = LoadWindowsDLL(szDllFilename);
	if(!hDLL)
	{
		Con_Printf("LoadLibrary failed on %s (%d)\n", szDllFilename, GetLastError());
		goto IgnoreThisDLL;
	}
#else // _WIN32
	void *hDLL = dlopen(szDllFilename, RTLD_NOW);
	if(!hDLL)
	{
		Con_Printf("LoadLibrary failed on %s: %s\n", szDllFilename, dlerror());
		goto IgnoreThisDLL;
	}
#endif

#ifdef _WIN32
	pfnGiveFnptrsToDll = (PFN_GiveFnptrsToDll)GetProcAddress(hDLL, "GiveFnptrsToDll");
#else
	pfnGiveFnptrsToDll = (PFN_GiveFnptrsToDll)dlsym(hDLL, "GiveFnptrsToDll");
#endif // _WIN32

	if(pfnGiveFnptrsToDll == NULL)
	{
		Con_Printf("Couldn't get GiveFnptrsToDll in %s\n", szDllFilename);
		goto IgnoreThisDLL;
	}

	pfnGiveFnptrsToDll(&g_engfuncsExportedToDlls, &gGlobalVariables);
	if(g_iextdllMac == 50)
	{
		Con_Printf("Too many DLLs, ignoring remainder\n");
		goto IgnoreThisDLL;
	}

	pextdll = &g_rgextdll[g_iextdllMac++];
	Q_memset(pextdll, 0, sizeof(*pextdll));
	pextdll->lDLLHandle = hDLL;
	return;

IgnoreThisDLL:
	if(hDLL)
	{
#ifdef _WIN32
		FreeLibrary(hDLL);
#else
		dlclose(hDLL);
#endif // _WIN32
	}
}

void ReleaseEntityDlls()
{
	extensiondll_t *pextdll;
	extensiondll_t *pextdllMac;

	if(!g_psvs.dll_initialized)
		return;

	FreeAllEntPrivateData();

	if(gNewDLLFunctions.pfnGameShutdown)
		gNewDLLFunctions.pfnGameShutdown();

	Cvar_UnlinkExternals();

	pextdll = &g_rgextdll[0];
	pextdllMac = &g_rgextdll[g_iextdllMac];

	while(pextdll < pextdllMac)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)pextdll->lDLLHandle);
#else
		dlclose(pextdll->lDLLHandle);
#endif // _WIN32

		pextdll->lDLLHandle = NULL;
		if(pextdll->functionTable)
		{
			Mem_Free((void *)pextdll->functionTable);
			pextdll->functionTable = NULL;
		}
		pextdll++;
	}
	g_psvs.dll_initialized = FALSE;
}