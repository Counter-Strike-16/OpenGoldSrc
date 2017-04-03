/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2015-2017 OGS Dev Team
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
/// @brief hl-compatible game dll class

#include "hlcompat/HLGameDLL.hpp"
#include "utils/LibUtil.hpp"
#include "engine/eiface.h"
#include <cstring>

CHLGameDLL::CHLGameDLL()
{
	LogMsg("Constructing the hl-compatible game dll component...");
	
	mpDLLFuncs = new DLL_FUNCTIONS;
	mpDLLFuncsEx = new NEW_DLL_FUNCTIONS;
	
	// Make sure that new dll functions is cleared
	//memset(&mpDLLFuncsEx, 0, sizeof(mpDLLFuncsEx));
};

CHLGameDLL::~CHLGameDLL()
{
	if(mpDLLFuncs)
		delete mpDLLFuncs;
	
	if(mpDLLFuncsEx)
		delete mpDLLFuncsEx;
	
	mpDLLFuncs = nullptr;
	mpDLLFuncsEx = nullptr;
	
	LogMsg("Destructing the hl-compatible game dll component...");
};

bool CHLGameDLL::DLLInit(const APIFUNCTION &afnGetEntityAPI, const APIFUNCTION2 &afnGetEntityAPI2, const NEW_DLL_FUNCTIONS_FN &afnGetNewDllFuncs)
{
	int nVersion;
	
	// Get extended callbacks
	if(afnGetNewDllFuncs)
	{
		nVersion = NEW_DLL_FUNCTIONS_VERSION;
		
		if(!afnGetNewDllFuncs(mpDLLFuncsEx, &nVersion))
		{
			if(nVersion != NEW_DLL_FUNCTIONS_VERSION)
				DevWarning("SV_LoadProgs: new interface version is %d, should be %d", NEW_DLL_FUNCTIONS_VERSION, nVersion);
			
			memset(&mpDLLFuncsEx, 0, sizeof(mpDLLFuncsEx));
		};
	};
	
	nVersion = INTERFACE_VERSION;
	
	if(afnGetEntityAPI2)
	{
		if(!afnGetEntityAPI2(mpDLLFuncs, &nVersion))
		{
			DevWarning("SV_LoadProgs: interface version is %d, should be %d", INTERFACE_VERSION, nVersion);
			
			// Fallback to old API
			if(!afnGetEntityAPI(mpDLLFuncs, nVersion))
				return false;
		}
		else
			DevMsg(eMsgType_AIConsole, "SV_LoadProgs: ^2initailized extended EntityAPI ^7ver. %d", nVersion);
	}
	else
		if(!afnGetEntityAPI(mpDLLFuncs, nVersion))
			return false;
		
	return true;
};

bool CHLGameDLL::DLLInit(CreateInterfaceFn afnEngineFactory)
{
	DevMsg("Initializing the hl-compatible game dll component...");
	return true;
};

void CHLGameDLL::DLLShutdown()
{
};

void CHLGameDLL::GameInit()
{
	mpDLLFuncs->pfnGameInit();
};

void CHLGameDLL::GameShutdown()
{
	if(mpDLLFuncsEx)
		mpDLLFuncsEx->pfnGameShutdown();
};

const char *CHLGameDLL::GetGameDescription()
{
	return mpDLLFuncs->pfnGetGameDescription();
};

bool CHLGameDLL::AllowLagCompensation()
{
	return mpDLLFuncs->pfnAllowLagCompensation() ? true : false;
};

bool CHLGameDLL::LevelInit(char const *asMapName, char const *asMapEntities, char const *asOldLevel, char const *asLandmarkName, bool abLoadGame, bool abBackground)
{
	return true;
};

void CHLGameDLL::LevelShutdown()
{
};

void CHLGameDLL::RegisterEncoders()
{
	mpDLLFuncs->pfnRegisterEncoders();
};

void CHLGameDLL::PostInit()
{
};

void CHLGameDLL::SysError(const char *asMsg)
{
	mpDLLFuncs->pfnSys_Error(asMsg);
};

void CHLGameDLL::GetPlayerLimits(int &anMinPlayers, int &anMaxPlayers, int &anDefaultMaxPlayers) const
{
};

void CHLGameDLL::Update()
{
	mpDLLFuncs->pfnStartFrame();
};

void CHLGameDLL::OnServerActivate(edict_t *apEdictList, uint anEdictCount, uint anMaxPlayers)
{
	mpDLLFuncs->pfnServerActivate(apEdictList, anEdictCount, anMaxPlayers);
};

void CHLGameDLL::OnServerDeactivate()
{
	mpDLLFuncs->pfnServerDeactivate();
};

void CHLGameDLL::OnNewLevel()
{
	mpDLLFuncs->pfnParmsNewLevel();
};

void CHLCompatGameDLL::OnChangeLevel()
{
	mpDLLFuncs->pfnParmsChangeLevel();
};

bool CHLGameDLL::ShouldHideServer()
{
	return false;
};

int CHLGameDLL::OnConnectionlessPacket(const netadr_t *apFrom, const char *asArgs, char *asResponseBuffer, int *anBufferSize)
{
	return mpDLLFuncs->pfnConnectionlessPacket(apFrom, asArgs, asResponseBuffer, anBufferSize);
};

void CHLGameDLL::SaveWriteFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an)
{
	mpDLLFuncs->pfnSaveWriteFields(apSaveRestoreData, as, ap, apTypeDesc, an);
};

void CHLGameDLL::SaveReadFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an)
{
	mpDLLFuncs->pfnSaveReadFields(apSaveRestoreData, as, ap, apTypeDesc, an);
};

void CHLGameDLL::OnSaveGlobalState(SAVERESTOREDATA *apSaveRestoreData)
{
	mpDLLFuncs->pfnSaveGlobalState(apSaveRestoreData);
};

void CHLGameDLL::OnRestoreGlobalState(SAVERESTOREDATA *apSaveRestoreData)
{
	mpDLLFuncs->pfnRestoreGlobalState(apSaveRestoreData);
};

void CHLGameDLL::OnResetGlobalState()
{
	mpDLLFuncs->pfnResetGlobalState();
};

int CHLGameDLL::AddToFullPack(entity_state_t *apEntityState, int anE, edict_t *apEnt, edict_t *apHost, int anHostFlags, int anPlayer, unsigned char *apSet)
{
	return mpDLLFuncs->pfnAddToFullPack(apEntityState, anE, apEnt, apHost, anHostFlags, anPlayer, apSet);
};

void CHLGameDLL::CreateBaseline(int anPlayer, int anEntIndex, entity_state_t *apBaseline, edict_t *apEntity, int anPlayerModelIndex, vec3_t avPlayerMins, vec3_t avPlayerMaxs)
{
	mpDLLFuncs->pfnCreateBaseline(anPlayer, anEntIndex, apBaseline, apEntity, anPlayerModelIndex, avPlayerMins, avPlayerMaxs);
};

void CHLGameDLL::CreateInstancedBaselines()
{
	mpDLLFuncs->pfnCreateInstancedBaselines();
};

int CHLGameDLL::GetHullBounds(int anHullNumber, float *afMins, float *afMaxs)
{
	return mpDLLFuncs->pfnGetHullBounds(anHullNumber, afMins, afMaxs);
};