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
/// @brief source of hl-compatible entity listener class

#include "hlcompat/HLEntityListener.hpp"

CHLEntityListener::CHLEntityListener(DLL_FUNCTIONS *apHLGameDLL, NEW_DLL_FUNCTIONS *apHLGameDLLEx)
{
	LogMsg("Constructing the hl-compatible entity listener component...");
	
	mpHLGameDLL = apHLGameDLL;
	mpHLGameDLLEx = apHLGameDLLEx;
};

CHLEntityListener::~CHLEntityListener()
{
	LogMsg("Destructing the hl-compatible entity listener component...");
};

int CHLEntityListener::OnEntitySpawn(IEntity *apEntity)
{
	return mpHLGameDLL->pfnSpawn(apEntity->ToPOD());
};

void CHLEntityListener::OnEntityThink(IEntity *apEntity)
{
	mpHLGameDLL->pfnThink(apEntity->ToPOD());
};

void CHLEntityListener::OnEntityUse(IEntity *apEntity, IEntity *apUsedBy)
{
	mpHLGameDLL->pfnUse(apEdict, apUsedBy);
};

void CHLEntityListener::OnEntityTouch(IEntity *apEntity, IEntity *apTouchedBy)
{
	mpHLGameDLL->pfnTouch(apEdict, apTouchedBy);
};

void CHLEntityListener::OnEntityBlocked(IEntity *apEntity, IEntity *apBlockedBy)
{
	mpHLGameDLL->pfnBlocked(apEdict, apBlockedBy);
};

void CHLEntityListener::EntityKeyValue(IEntity *apEntity, KeyValueData *apKeyValueData)
{
	mpHLGameDLL->pfnKeyValue(apEdict, apKeyValueData);
};

int CHLEntityListener::ShouldCollide(IEntity *apEntity, IEntity *apCollideWith)
{
	return mpHLGameDLLEx->pfnShouldCollide(apEdict, apCollideWith);
};

void CHLEntityListener::OnEntitySave(IEntity *apEntity, SAVERESTOREDATA *apSaveData)
{
	mpHLGameDLL->pfnSave(apEdict, apSaveData);
};

int CHLEntityListener::OnEntityRestore(IEntity *apEntity, SAVERESTOREDATA *apSaveData, int anGlobalEntity)
{
	return mpHLGameDLL->pfnRestore(apEdict, apSaveData, anGlobalEntity);
};

void CHLEntityListener::SetEntityAbsBox(IEntity *apEntity)
{
	mpHLGameDLL->pfnSetAbsBox(apEdict);
};

void CHLEntityListener::OnEntityFreePrivateData(IEntity *apEntity)
{
	mpHLGameDLLEx->pfnOnFreeEntPrivateData(apEdict);
};