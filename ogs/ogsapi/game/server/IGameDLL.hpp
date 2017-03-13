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
/// @brief game dll interface

#pragma once

#include "public/interface.h"

const char OGS_GAMEDLL_INTERFACE_VERSION[] = "OGSGameDLL001";

struct IGameDLL : public IBaseInterface
{
	virtual bool Init(CreateInterfaceFn afnEngineFactory) = 0;
	virtual void Shutdown() = 0;
	
	virtual void Frame() = 0;
	
	void (*pfnGameInit)();
	
	void (*pfnKeyValue)(edict_t *pentKeyvalue, KeyValueData *pkvd);
	void (*pfnSave)(edict_t *pent, SAVERESTOREDATA *pSaveData);
	int (*pfnRestore)(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
	void (*pfnSetAbsBox)(edict_t *pent);

	void (*pfnSaveWriteFields)(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
	void (*pfnSaveReadFields)(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);

	void (*pfnSaveGlobalState)(SAVERESTOREDATA *);
	void (*pfnRestoreGlobalState)(SAVERESTOREDATA *);
	void (*pfnResetGlobalState)();

	void (*pfnServerActivate)(edict_t *pEdictList, int edictCount, int clientMax);
	void (*pfnServerDeactivate)();

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

	int (*pfnAllowLagCompensation)();
	
	void (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
	int (*pfnShouldCollide)(edict_t *pentTouched, edict_t *pentOther);

	void (*pfnCvarValue)(const edict_t *pEnt, const char *value);
	void (*pfnCvarValue2)(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
};