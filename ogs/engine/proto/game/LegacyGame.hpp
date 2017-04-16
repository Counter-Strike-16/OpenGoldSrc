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

// Wrapper for game dll funcs and new dll funcs that could be optionally exported
class CLegacyGame : public IGame//, public IEventListener
{
public:
	CLegacyGame(CSystem *apSystem);
	CLegacyGame(CSystem *apSystem, DLL_FUNCTIONS aFuncs, NEW_DLL_FUNCTIONS aNewFuncs);
	~CLegacyGame();
	
	// Allow to dinamically change the whole set of export funcs
	//void SetDLLFuncs(const DLL_FUNCTIONS &aFuncs);
	//void GetDLLFuncs(DLL_FUNCTIONS &aFuncs);
	
	//void SetNewDLLFuncs(const NEW_DLL_FUNCTIONS &aNewFuncs);
	//void GetNewDLLFuncs(NEW_DLL_FUNCTIONS &aNewFuncs);
	
	bool Init(DLL_FUNCTIONS aFuncs, NEW_DLL_FUNCTIONS aNewFuncs);
	
	bool Init(CreateInterfaceFn afnEngineFactory);
	void Shutdown();
	
	void GameInit();
	void GameShutdown();
	
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
	
	int DispatchEntitySpawn(edict_t *pent);
	void DispatchEntityThink(edict_t *pent);
	void DispatchEntityUse(edict_t *pentUsed, edict_t *pentOther);
	void DispatchEntityTouch(edict_t *pentTouched, edict_t *pentOther);
	void DispatchEntityBlocked(edict_t *pentBlocked, edict_t *pentOther);
	void DispatchEntityKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd);
	void DispatchEntitySave(edict_t *pent, SAVERESTOREDATA *pSaveData);
	int DispatchEntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
	void DispatchEntitySetAbsBox(edict_t *pent);
	
	void SaveWriteFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an);
	void SaveReadFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an);
	
	void SaveGlobalState(SAVERESTOREDATA *apSaveRestoreData);
	void RestoreGlobalState(SAVERESTOREDATA *apSaveRestoreData);
	void ResetGlobalState();
	
	qboolean DispatchClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);

	void DispatchClientDisconnect(edict_t *pEntity);
	void DispatchClientKill(edict_t *pEntity);
	void DispatchClientClientPutInServer(edict_t *pEntity);
	void DispatchClientClientCommand(edict_t *pEntity);
	void DispatchClientClientUserInfoChanged(edict_t *pEntity, char *infobuffer);

	void DispatchServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void DispatchServerDeactivate();

	void PlayerPreThink(edict_t *pEntity);
	void PlayerPostThink(edict_t *pEntity);

	void StartFrame();
	void ParmsNewLevel();
	void ParmsChangeLevel();

	const char *GetGameDescription();

	void PlayerCustomization(edict_t *pEntity, customization_t *pCustom);

	void SpectatorConnect(edict_t *pEntity);
	void SpectatorDisconnect(edict_t *pEntity);
	void SpectatorThink(edict_t *pEntity);

	void Sys_Error(const char *error_string);

	void (*pfnPM_Move)(struct playermove_s *ppmove, qboolean server);
	void (*pfnPM_Init)(struct playermove_s *ppmove);
	char (*pfnPM_FindTextureType)(char *name);
	void (*pfnSetupVisibility)(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas);
	void (*pfnUpdateClientData)(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
	int (*pfnAddToFullPack)(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
	void (*pfnCreateBaseline)(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
	void (*pfnRegisterEncoders)();
	int (*pfnGetWeaponData)(struct edict_s *player, struct weapon_data_s *info);

	void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
	void CmdEnd(const edict_t *player);

	int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);

	int GetHullBounds(int hullnumber, float *mins, float *maxs);

	void CreateInstancedBaselines();

	int InconsistentFile(const struct edict_s *player, const char *filename, char *disconnect_message);
	
	int AllowLagCompensation();
	
	void OnFreeEntPrivateData(edict_t *pEnt);
	
	int ShouldCollide(edict_t *pentTouched, edict_t *pentOther);
	
	void CvarValue(const edict_t *pEnt, const char *value);
	void CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
private:
	DLL_FUNCTIONS *mpFuncs{nullptr}; // DLL exported funcs
	NEW_DLL_FUNCTIONS *mpNewFuncs{nullptr}; // New dll exported funcs (may be null)
	
	//DLL_FUNCTIONS mFuncs;
	//NEW_DLL_FUNCTIONS mNewFuncs;
	
	CSystem *mpSystem{nullptr}; // temp; purpose?
};