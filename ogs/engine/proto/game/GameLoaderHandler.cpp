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
#include "game/GameLoaderHandler.hpp"
#include "game/IGameLoader.hpp"

void CGameLoaderHandler::Add(IGameLoader *apLoader)
{
};

IGame *CGameLoaderHandler::LoadGame(const char *asPath)
{
	// Pass a dll module path
	// Iterate through each registered loader trying to load it
	// If successful a valid ptr to IGame will be returned
	// Otherwise a nullptr will be returned means that the
	// specified module cannot be loaded with our game loaders
	
	// Try to load the game module
	CFactorySharedLib GameLib(asPath.c_str());
	
	if(!GameLib)
		return nullptr;
	
	// Try to find it's format
	mpConsole->DevPrintf("Trying to detect the game module format...");
	
	IGame *pGame = nullptr;
	
	for(auto It : mlstLoaders)
	{
		pGame = It->LoadGame(&GameLib); // add error code?
		
		if(pGame)
		{
			mpConsole->DevPrintf("Dll loaded for %s %s\n", gmodinfo.bIsMod ? "mod" : "game", pGame->GetGameDescription());
			return pGame;
		};
	};
	
	mpConsole->DevPrintf("Couldn't load the game module! (Probably unsupported game format)");
	return nullptr;
};