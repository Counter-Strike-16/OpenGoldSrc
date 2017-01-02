/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#include <stdio.h>
#include <windows.h>

//#include "metahook.h"
#include "LoadBlob.hpp"
#include "ExceptHandle.hpp"
//#include "sys.hpp"

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
	BOOL (*IsDebuggerPresent)
	() = (BOOL(*)())GetProcAddress(GetModuleHandle("kernel32.dll"), "IsDebuggerPresent");

	if(!IsDebuggerPresent())
	{
		hObject        = CreateMutex(NULL, FALSE, "ValveHalfLifeLauncherMutex");
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