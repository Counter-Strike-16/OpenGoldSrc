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

#pragma once

// Wrapper for game dll funcs and new dll funcs that could be optionally exported
class CGameHandle
{
public:
	// Allow to dinamically change the whole set of export funcs
	void SetDLLFuncs(const DLL_FUNCTIONS &aFuncs);
	void GetDLLFuncs(DLL_FUNCTIONS &aFuncs);
	
	void SetNewDLLFuncs(const NEW_DLL_FUNCTIONS &aNewFuncs);
	void GetNewDLLFuncs(NEW_DLL_FUNCTIONS &aNewFuncs);
	
	void GameInit();
	void GameShutdown();
	
	int DispatchEntitySpawn(edict_t *pent);
	void DispatchEntityThink(edict_t *pent);
	void DispatchEntityUse(edict_t *pentUsed, edict_t *pentOther);
	void DispatchEntityTouch(edict_t *pentTouched, edict_t *pentOther);
	void DispatchEntityBlocked(edict_t *pentBlocked, edict_t *pentOther);
	void DispatchEntityKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd);
	void DispatchEntitySave(edict_t *pent, SAVERESTOREDATA *pSaveData);
	int DispatchEntityRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
	void DispatchEntitySetAbsBox(edict_t *pent);

	void SaveWriteFields(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
	void SaveReadFields(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);

	void SaveGlobalState(SAVERESTOREDATA *);
	void RestoreGlobalState(SAVERESTOREDATA *);
	void ResetGlobalState();

	qboolean DispatchClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);

	void DispatchClientDisconnect(edict_t *pEntity);
	void DispatchClientKill(edict_t *pEntity);
	void DispatchClientClientPutInServer(edict_t *pEntity);
	void DispatchClientClientCommand(edict_t *pEntity);
	void DispatchClientClientUserInfoChanged(edict_t *pEntity, char *infobuffer);

	void DispatchServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void DispatchServerDeactivate();

	void (*pfnPlayerPreThink)(edict_t *pEntity);
	void (*pfnPlayerPostThink)(edict_t *pEntity);

	void (*pfnStartFrame)();
	void (*pfnParmsNewLevel)();
	void (*pfnParmsChangeLevel)();

	const char *(*pfnGetGameDescription)();

	void (*pfnPlayerCustomization)(edict_t *pEntity, customization_t *pCustom);

	void (*pfnSpectatorConnect)(edict_t *pEntity);
	void (*pfnSpectatorDisconnect)(edict_t *pEntity);
	void (*pfnSpectatorThink)(edict_t *pEntity);

	void (*pfnSys_Error)(const char *error_string);

	void (*pfnPM_Move)(struct playermove_s *ppmove, qboolean server);
	void (*pfnPM_Init)(struct playermove_s *ppmove);
	char (*pfnPM_FindTextureType)(char *name);
	void (*pfnSetupVisibility)(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas);
	void (*pfnUpdateClientData)(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
	int (*pfnAddToFullPack)(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
	void (*pfnCreateBaseline)(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
	void (*pfnRegisterEncoders)();
	int (*pfnGetWeaponData)(struct edict_s *player, struct weapon_data_s *info);

	void (*pfnCmdStart)(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
	void (*pfnCmdEnd)(const edict_t *player);

	int (*pfnConnectionlessPacket)(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);

	int (*pfnGetHullBounds)(int hullnumber, float *mins, float *maxs);

	void (*pfnCreateInstancedBaselines)();

	int (*pfnInconsistentFile)(const struct edict_s *player, const char *filename, char *disconnect_message);
	
	int AllowLagCompensation();
	
	void OnFreeEntPrivateData(edict_t *pEnt);
	
	int ShouldCollide(edict_t *pentTouched, edict_t *pentOther);
	
	void CvarValue(const edict_t *pEnt, const char *value);
	void CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
private:
	DLL_FUNCTIONS mFuncs;
	NEW_DLL_FUNCTIONS mNewFuncs;
};

class CGameHandler
{
public:
	CGameHandler() : mpGame(nullptr){}
	
	CGameHandle *operator->(){return mpGame;}
	CGameHandle &operator*(){return *mpGame;}
private:
	CGameHandle *mpGame;
};