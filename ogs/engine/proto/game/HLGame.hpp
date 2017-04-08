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
/// @brief old api implementation of game module to support HLSDK gamedlls

#pragma once

#include "game/server/IGame.hpp"
//#include "system/IEventListener.hpp"

class CHLGame : public IGame//, public IEventListener
{
public:
	CHLGame(CSystem *apSystem);
	~CHLGame();
	
	bool Load(const APIFUNCTION &afnGetEntityAPI, const APIFUNCTION2 &afnGetEntityAPI2, const NEW_DLL_FUNCTIONS_FN &afnGetNewDllFuncs);
	
	bool Init(CreateInterfaceFn afnEngineFactory);
	void Shutdown();
	
	bool LevelInit(char const *asMapName, char const *asMapEntities, char const *asOldLevel, char const *asLandmarkName, bool abLoadGame, bool abBackground);
	void LevelShutdown();
	
	void RegisterEncoders();
	
	void PostInit();
	
	void OnNewLevel();
	void OnChangeLevel();
	
	void OnServerActivate(edict_t *apEdictList, uint anEdictCount, uint anMaxPlayers);
	void OnServerDeactivate();
	
	void Update();
	
	const char *GetGameDescription();
	
	bool ShouldHideServer();
	
	bool AllowLagCompensation();
	
	int OnConnectionlessPacket(const netadr_t *apFrom, const char *asArgs, char *asResponseBuffer, int *anBufferSize);
	
	void HandleSysError(const char *asMsg);
	
	void GetPlayerLimits(int &anMinPlayers, int &anMaxPlayers, int &anDefaultMaxPlayers) const;
	
	void CreateInstancedBaselines();
	
	int AddToFullPack(entity_state_t *apEntityState, int anE, edict_t *apEnt, edict_t *apHost, int anHostFlags, int anPlayer, unsigned char *apSet);
	
	void CreateBaseline(int anPlayer, int anEntIndex, entity_state_t *apBaseline, edict_t *apEntity, int anPlayerModelIndex, vec3_t avPlayerMins, vec3_t avPlayerMaxs);
	
	int GetHullBounds(int anHullNumber, float *afMins, float *afMaxs);
	
	void SaveWriteFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an);
	void SaveReadFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an);
	
	void OnSaveGlobalState(SAVERESTOREDATA *apSaveRestoreData);
	void OnRestoreGlobalState(SAVERESTOREDATA *apSaveRestoreData);
	void OnResetGlobalState();
private:
	DLL_FUNCTIONS *mpFuncs{nullptr}; // DLL exported funcs
	NEW_DLL_FUNCTIONS *mpNewFuncs{nullptr}; // New dll exported funcs (may be null)
	
	CSystem *mpSystem{nullptr}; // temp; purpose?
};