/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "client.h"
#include "game.h"

extern "C" {
//void PM_Move(struct playermove_s *ppmove, int server);
//void PM_Init(struct playermove_s *ppmove);
//char PM_FindTextureType(char *name);
};

int DispatchSpawn(edict_t *pent)
{
	return 0;
};

void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd){};

void DispatchTouch(edict_t *pentTouched, edict_t *pentOther){};

void DispatchUse(edict_t *pentUsed, edict_t *pentOther){};

void DispatchThink(edict_t *pent){};

void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther){};

void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData){};

int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
	return 0;
};

void DispatchObjectCollsionBox(edict_t *pent){};

void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount){};

void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount){};

void SaveGlobalState(SAVERESTOREDATA *apSaveRestoreData){};

void RestoreGlobalState(SAVERESTOREDATA *apSaveRestoreData){};

void ResetGlobalState(){};

static DLL_FUNCTIONS gFunctionTable =
{
  GameDLLInit,               //pfnGameInit
  DispatchSpawn,             //pfnSpawn
  DispatchThink,             //pfnThink
  DispatchUse,               //pfnUse
  DispatchTouch,             //pfnTouch
  DispatchBlocked,           //pfnBlocked
  DispatchKeyValue,          //pfnKeyValue
  DispatchSave,              //pfnSave
  DispatchRestore,           //pfnRestore
  DispatchObjectCollsionBox, //pfnAbsBox

  SaveWriteFields, //pfnSaveWriteFields
  SaveReadFields,  //pfnSaveReadFields

  SaveGlobalState,    //pfnSaveGlobalState
  RestoreGlobalState, //pfnRestoreGlobalState
  ResetGlobalState,   //pfnResetGlobalState

  ClientConnect,         //pfnClientConnect
  ClientDisconnect,      //pfnClientDisconnect
  ClientKill,            //pfnClientKill
  ClientPutInServer,     //pfnClientPutInServer
  ClientCommand,         //pfnClientCommand
  ClientUserInfoChanged, //pfnClientUserInfoChanged
  ServerActivate,        //pfnServerActivate
  ServerDeactivate,      //pfnServerDeactivate

  PlayerPreThink,  //pfnPlayerPreThink
  PlayerPostThink, //pfnPlayerPostThink

  StartFrame,       //pfnStartFrame
  ParmsNewLevel,    //pfnParmsNewLevel
  ParmsChangeLevel, //pfnParmsChangeLevel

  GetGameDescription,  //pfnGetGameDescription    Returns string describing current .dll game.
  PlayerCustomization, //pfnPlayerCustomization   Notifies .dll of new customization for player.

  SpectatorConnect,    //pfnSpectatorConnect      Called when spectator joins server
  SpectatorDisconnect, //pfnSpectatorDisconnect   Called when spectator leaves the server
  SpectatorThink,      //pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)

  Sys_Error, //pfnSys_Error				Called when engine has encountered an error

  nullptr, //PM_Move,					//pfnPM_Move
  nullptr, //PM_Init,					//pfnPM_Init				Server version of player movement initialization
  nullptr, //PM_FindTextureType,			//pfnPM_FindTextureType

  SetupVisibility,          //pfnSetupVisibility        Set up PVS and PAS for networking for this client
  UpdateClientData,         //pfnUpdateClientData       Set up data sent only to specific client
  AddToFullPack,            //pfnAddToFullPack
  CreateBaseline,           //pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
  RegisterEncoders,         //pfnRegisterEncoders		Callbacks for network encoding
  GetWeaponData,            //pfnGetWeaponData
  CmdStart,                 //pfnCmdStart
  CmdEnd,                   //pfnCmdEnd
  ConnectionlessPacket,     //pfnConnectionlessPacket
  GetHullBounds,            //pfnGetHullBounds
  CreateInstancedBaselines, //pfnCreateInstancedBaselines
  InconsistentFile,         //pfnInconsistentFile
  AllowLagCompensation      //pfnAllowLagCompensation
};

extern "C" {
int GetEntityAPI(DLL_FUNCTIONS *apFunctionTable, int anInterfaceVersion)
{
	printf("In GetEntityAPI...");

	if(!apFunctionTable || anInterfaceVersion != INTERFACE_VERSION)
	{
		printf("False! (%d, %d)\n", apFunctionTable, anInterfaceVersion);
		return FALSE;
	};

	printf("True!\n");
	memcpy(apFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));
	return TRUE;
};

int GetEntityAPI2(DLL_FUNCTIONS *apFunctionTable, int *anInterfaceVersion)
{
	printf("In GetEntityAPI2...");

	if(!apFunctionTable || *anInterfaceVersion != INTERFACE_VERSION)
	{
		printf("False! (%d, %d)\n", apFunctionTable, anInterfaceVersion);
		// Tell engine what version we had, so it can figure out who is out of date
		*anInterfaceVersion = INTERFACE_VERSION;
		return FALSE;
	};

	printf("True!\n");
	memcpy(apFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));
	return TRUE;
};
};