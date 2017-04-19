/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
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
/// @brief old api game module loader component

#include "precompiled.hpp"
#include "game/LegacyGameLoader.hpp"
#include "game/LegacyGame.hpp"

// Exports
using pfnLinkEntity = void (__cdecl *)(entvars_t *pev);
using pfnGiveFnPtrsToDLL = void (__stdcall *)(enginefuncs_t *apEngFuncs, globalvars_t *apGlobals); // GiveEngineFuncsToDLL

IGame *CLegacyGameLoader::LoadGame(CFactorySharedLib *apGameLib)
{
	APIFUNCTION fnGetEntityAPI; // pfnGetAPI;
	APIFUNCTION2 fnGetEntityAPI2; // pfnGetAPI2;
	//enginefuncs_t gpEngfuncs;
	//globalvars_t gpGlobals;
	//playermove_t gpMove;
	
	// Fill it in
	//svgame.pmove = &gpMove;
	//mpGlobals = &gpGlobals;
	
	NEW_DLL_FUNCTIONS_FN fnGetNewDllFuncs; // pNewAPI;
	
	// Try to find an extended export set first
	fnGetNewDllFuncs = (NEW_DLL_FUNCTIONS_FN)apGameLib->GetExportFunc("GetNewDLLFunctions", false);
	//pNewAPI = (NEW_DLL_FUNCTIONS_FN)GetDispatch("GetNewDLLFunctions");
	
	int nInterfaceVersion = 0;
	
	// Get extended callbacks
	if(fnGetNewDllFuncs)
	{
		nInterfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		
		if(!fnGetNewDllFuncs(mpNewFuncs, &nInterfaceVersion))
		{
			if(nInterfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
				DevWarning("SV_LoadProgs: new interface version is %d, should be %d", NEW_DLL_FUNCTIONS_VERSION, nVersion);
			
			memset(&mpNewFuncs, 0, sizeof(mpNewFuncs));
		};
	};
	
	nInterfaceVersion = INTERFACE_VERSION;
	
	// Main export set
	// Check if secondary api is present
	fnGetEntityAPI2 = (APIFUNCTION2)apGameLib->GetExportFunc("GetEntityAPI2", false);
	//pfnGetAPI2 = (APIFUNCTION2)GetDispatch("GetEntityAPI2");
	
	if(fnGetEntityAPI2)
	{
		//nInterfaceVersion = INTERFACE_VERSION;
		
		//if(!pfnGetAPI2(&gEntityInterface, &nInterfaceVersion))
		if(!fnGetEntityAPI2(&gEntityInterface, &nInterfaceVersion))
		{
			//DevWarning("SV_LoadProgs: interface version is %d, should be %d", INTERFACE_VERSION, nVersion);
			
			mpConsole->Printf("==================\n");
			mpConsole->Printf("Game DLL version mismatch\n");
			mpConsole->Printf("DLL version is %i, engine version is %i\n", nInterfaceVersion, INTERFACE_VERSION);
			
			if(nInterfaceVersion <= INTERFACE_VERSION)
				mpConsole->Printf("The game DLL for %s appears to be outdated, check for updates\n", szGameDir);
			else
				mpConsole->Printf("Engine appears to be outdated, check for updates\n");
			
			mpConsole->Printf("==================\n");
			Host_Error("\n");
		}
		//else
			//DevMsg(eMsgType_AIConsole, "SV_LoadProgs: ^2initailized extended EntityAPI ^7ver. %d", nVersion);
	}
	else
	{
		// Check if primary api is present
		fnGetEntityAPI = (APIFUNCTION)apGameLib->GetExportFunc("GetEntityAPI");
		//pfnGetAPI = (APIFUNCTION)GetDispatch("GetEntityAPI");
		
		if(!fnGetEntityAPI) // if(!pfnGetAPI)
			return nullptr; // 	Host_Error("Couldn't get DLL API from %s!", szDllFilename);
		
		nInterfaceVersion = INTERFACE_VERSION;
		
		//if(!pfnGetAPI(&gEntityInterface, nInterfaceVersion))
		if(!fnGetEntityAPI(&gEntityInterface, nInterfaceVersion))
		{
			mpConsole->Printf("==================\n");
			mpConsole->Printf("Game DLL version mismatch\n");
			mpConsole->Printf("The game DLL for %s appears to be outdated, check for updates\n", szGameDir);
			mpConsole->Printf("==================\n");
			
			Host_Error("\n");
		};
	};
	
	mpConsole->DevPrintf("Detected legacy API format!");
	
	pfnGiveFnPtrsToDLL fnGiveFnPtrsToDLL;
	
	// Engine funcs
	if(!(fnGiveFnPtrsToDLL = (pfnGiveFnPtrsToDLL)apGameLib->GetExportFunc("GiveFnptrsToDll")))
		return nullptr;
	
	// Make a local copy of engfuncs to prevent overwrite of it by bots.dll
	//memcpy(&gpEngfuncs, &gEngfuncs, sizeof(gpEngfuncs));
	
	// Provide gamedll pointers to enginefuncs and globalvars
	fnGiveFnPtrsToDLL(nullptr /*&gpEngFuncs*/, nullptr /*mpGlobals*/);
	
	if(!mpLegacyGame)
		mpLegacyGame = std::make_unique<CLegacyGame>();
	
	if(!mpLegacyGame->Init(gEntityInterface, mpNewFuncs))
		return nullptr;
	
	return mpLegacyGame.get(); // release?
};

void CHLGameLoader::UnloadGame()
{
};