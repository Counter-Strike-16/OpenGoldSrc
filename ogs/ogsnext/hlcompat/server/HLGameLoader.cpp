/*
*	This file is part of the Magenta Engine
*
*	Copyright (C) 2016 Sh1ft0x0EF
*
*	Magenta Engine is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	Magenta Engine is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with Magenta Engine. If not, see <http://www.gnu.org/licenses/>.
*
*	In addition, as a special exception, the author gives permission to
*	link the code of this program with the Half-Life Game Engine("HL
*	Engine") and Modified Game Libraries("MODs") developed by Valve,
*	L.L.C("Valve").  You must obey the GNU General Public License in all
*	respects for all of the code used other than the HL Engine and MODs
*	from Valve.  If you modify this file, you may extend this exception
*	to your version of the file, but you are not obligated to do so.  If
*	you do not wish to do so, delete this exception statement from your
*	version.
*/

#include "hlcompat/HLGameLoader.hpp"
#include "hlcompat/HLGame.hpp"
#include "debug/Debug.hpp"
#include "engine/progdefs.h"

// Exports
using pfnLinkEntity = void (__cdecl *)(entvars_t *pev);
using pfnGiveFnPtrsToDLL = void (__stdcall *)(enginefuncs_t *apEngFuncs, globalvars_t *apGlobals); // GiveEngineFuncsToDLL

CHLGameLoader::CHLGameLoader()
{
	DebugLog("Constructing the hl-compatible game loader component...");
};

CHLGameLoader::~CHLGameLoader()
{
	DebugLog("Destructing the hl-compatible game loader component...");
};

IGame *CHLGameLoader::LoadGame(const tString &asPath)
{
	pfnGiveFnPtrsToDLL fnGiveFnPtrsToDLL;
	APIFUNCTION fnGetEntityAPI;
	APIFUNCTION2 fnGetEntityAPI2;
	NEW_DLL_FUNCTIONS_FN fnGetNewDllFuncs;
	//enginefuncs_t gpEngfuncs;
	//globalvars_t gpGlobals;
	//playermove_t gpMove;
	
	// Fill it in
	//svgame.pmove = &gpMove;
	//mpGlobals = &gpGlobals;
	
	// Make a local copy of engfuncs to prevent overwrite it with bots.dll
	//memcpy(&gpEngfuncs, &gEngfuncs, sizeof(gpEngfuncs));
	
	// Try to load a new dll
	if(!(mpGameLib = LibUtil::LoadLibrary(asPath.c_str())))
		return nullptr;
	
	// Try to find a format of gamedll
	DevMsg("Trying to detect game dll format...");
	
	// Engine funcs
	if(!(fnGiveFnPtrsToDLL = (pfnGiveFnPtrsToDLL)LibUtil::GetProcAddr(mpGameLib, "GiveFnptrsToDll")))
		return nullptr;
	
	// Main export set
	if(!(fnGetEntityAPI2 = (APIFUNCTION2)LibUtil::GetProcAddr(mpGameLib, "GetEntityAPI2", false))) // Check if secondary api is present
		if(!(fnGetEntityAPI = (APIFUNCTION)LibUtil::GetProcAddr(mpGameLib, "GetEntityAPI"))) // Check if primary api is present
			return nullptr;
	
	DevMsg("Detected legacy hlsdk format...");
	
	// Extended export set
	fnGetNewDllFuncs = (NEW_DLL_FUNCTIONS_FN)LibUtil::GetProcAddr(mpGameLib, "GetNewDLLFunctions", false);
	
	// Provide gamedll pointers to enginefuncs and globalvars
	fnGiveFnPtrsToDLL(nullptr /*&gpEngFuncs*/, nullptr /*mpGlobals*/);
	
	if(!mpHLCompatGame)
		mpHLCompatGame = std::make_unique<CHLCompatGame>();
	
	if(!mpHLCompatGame->DLLInit(fnGetEntityAPI, fnGetEntityAPI2, fnGetNewDllFuncs))
		return nullptr;
	
	return mpHLCompatGame.get(); // release?
};

void CHLGameLoader::UnloadGame()
{
};