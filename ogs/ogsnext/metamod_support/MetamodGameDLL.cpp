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

// MetamodGameDLL.cpp - source of game dll component for metamod

#include "MetamodGameDLL.hpp"
#include "dllapi.h"

bool CMetamodGameDLL::DLLInit(CreateInterfaceFn afnEngineFactory/*, CreateInterfaceFn afnPhysicsFactory, CreateInterfaceFn afnFileSystemFactory, CGlobalVars *apGlobals*/)
{
};

void CMetamodGameDLL::DLLShutdown()
{
};

void CMetamodGameDLL::GameInit()
{
	META_DLLAPI_HANDLE_void(FN_GAMEINIT, pfnGameInit, void, (VOID_ARG));
	RETURN_API_void();
};

void CMetamodGameDLL::GameShutdown()
{
	META_NEWAPI_HANDLE_void(FN_GAMESHUTDOWN, pfnGameShutdown, void, (VOID_ARG));
	RETURN_API_void();
};

bool CMetamodGameDLL::LevelInit(char const *asMapName, char const *asMapEntities, char const *asOldLevel, char const *asLandmarkName, bool abLoadGame, bool abBackground)
{
};

void CMetamodGameDLL::LevelShutdown()
{
};

void CMetamodGameDLL::RegisterEncoders()
{
	META_DLLAPI_HANDLE_void(FN_REGISTERENCODERS, pfnRegisterEncoders, void, (VOID_ARG));
	RETURN_API_void();
};

void CMetamodGameDLL::PostInit()
{
};

void CMetamodGameDLL::OnNewLevel()
{
	META_DLLAPI_HANDLE_void(FN_PARMSNEWLEVEL, pfnParmsNewLevel, void, (VOID_ARG));
	RETURN_API_void();
};

void CMetamodGameDLL::OnChangeLevel()
{
	META_DLLAPI_HANDLE_void(FN_PARMSCHANGELEVEL, pfnParmsChangeLevel, void, (VOID_ARG));
	RETURN_API_void();
};

void CMetamodGameDLL::OnServerActivate(edict_t *apEdictList, uint anEdictCount, uint anMaxPlayers)
{
	META_DLLAPI_HANDLE_void(FN_SERVERACTIVATE, pfnServerActivate, p2i, (pEdictList, edictCount, clientMax));
	RETURN_API_void();
};

void CMetamodGameDLL::OnServerDeactivate()
{
	META_DLLAPI_HANDLE_void(FN_SERVERDEACTIVATE, pfnServerDeactivate, void, (VOID_ARG));
	// Update loaded plugins.  Look for new plugins in inifile, as well as
	// any plugins waiting for a changelevel to load.  
	//
	// This is done in ServerDeactivate rather than Activate, as the latter
	// isn't actually the first routine to be called on a new map.  In
	// particular DispatchKeyValue and DispatchSpawn are called before
	// Activate, and we want any newly loaded plugins to be able to catch
	// these.
	//
	// So, we do this from Deactivate, which is the _last_ routine called
	// from the previous map.  It's also called right before shutdown,
	// which means whenever hlds quits, it'll reload the plugins just
	// before it exits, which is rather silly, but oh well.
	Plugins->refresh(PT_CHANGELEVEL);
	Plugins->unpause_all();
	// Plugins->retry_all(PT_CHANGELEVEL);
	g_Players.clear_all_cvar_queries();
	requestid_counter = 0;
	RETURN_API_void();
};

void CMetamodGameDLL::Frame(/*bool abSimulating*/)
{
	meta_debug_value = (int)meta_debug.value;
	
	META_DLLAPI_HANDLE_void(FN_STARTFRAME, pfnStartFrame, void, (VOID_ARG));
	RETURN_API_void();
};

const char *CMetamodGameDLL::GetGameDescription()
{
	META_DLLAPI_HANDLE(const char *, NULL, FN_GETGAMEDESCRIPTION, pfnGetGameDescription, void, (VOID_ARG));
	RETURN_API(const char *);
};

bool CMetamodGameDLL::ShouldHideServer()
{
};

bool CMetamodGameDLL::AllowLagCompensation()
{
	META_DLLAPI_HANDLE(int, 0, FN_ALLOWLAGCOMPENSATION, pfnAllowLagCompensation, void, (VOID_ARG));
	RETURN_API(int);
};

int CMetamodGameDLL::OnConnectionlessPacket(const netadr_t *apFrom, const char *asArgs, char *asResponseMsg, int *anBufferSize)
{
	META_DLLAPI_HANDLE(int, 0, FN_CONNECTIONLESSPACKET, pfnConnectionlessPacket, 4p, (net_from, args, response_buffer, response_buffer_size));
	RETURN_API(int);
};

void CMetamodGameDLL::SysError(const char *asMsg)
{
	META_DLLAPI_HANDLE_void(FN_SYS_ERROR, pfnSys_Error, p, (error_string));
	RETURN_API_void();
};

void CMetamodGameDLL::GetPlayerLimits(int &anMinPlayers, int &anMaxPlayers, int &anDefaultMaxPlayers) const
{
};

void CMetamodGameDLL::CreateInstancedBaselines()
{
	META_DLLAPI_HANDLE_void(FN_CREATEINSTANCEDBASELINES, pfnCreateInstancedBaselines, void, (VOID_ARG));
	RETURN_API_void();
};

int CMetamodGameDLL::AddToFullPack(entity_state_t *apEntityState, int anE, edict_t *apEnt, edict_t *apHost, int anHostFlags, int anPlayer, unsigned char *apSet)
{
	META_DLLAPI_HANDLE(int, 0, FN_ADDTOFULLPACK, pfnAddToFullPack, pi2p2ip, (state, e, ent, host, hostflags, player, pSet));
	RETURN_API(int);
};

void CMetamodGameDLL::CreateBaseline(int anPlayer, int anEntIndex, entity_state_t *apBaseline, edict_t *apEntity, int anPlayerModelIndex, vec3_t avPlayerMins, vec3_t avPlayerMaxs)
{
	META_DLLAPI_HANDLE_void(FN_CREATEBASELINE, pfnCreateBaseline, 2i2pi2p, (player, eindex, baseline, entity, playermodelindex, (float*)player_mins, (float*)player_maxs));
	RETURN_API_void();
};

int CMetamodGameDLL::GetHullBounds(int anHullNumber, float *afMins, float *afMaxs)
{
	META_DLLAPI_HANDLE(int, 0, FN_GETHULLBOUNDS, pfnGetHullBounds, i2p, (hullnumber, mins, maxs));
	RETURN_API(int);
};

/*
void CMetamodGameDLL::SetCommandClient(int anIndex)
{
};

void CMetamodGameDLL::PostClientMessagesSent()
{
};

void CMetamodGameDLL::GetBugReportInfo(char *asBuf, int anLen)
{
};

void CMetamodGameDLL::NetworkIDValidated(const char *asUserName, const char *asNetworkID)
{
};
*/

void CMetamodGameDLL::SaveWriteFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an)
{
	META_DLLAPI_HANDLE_void(FN_SAVEWRITEFIELDS, pfnSaveWriteFields, 4pi, (pSaveData, pname, pBaseData, pFields, fieldCount));
	RETURN_API_void();
};

void CMetamodGameDLL::SaveReadFields(SAVERESTOREDATA *apSaveRestoreData, const char *as, void *ap, TYPEDESCRIPTION *apTypeDesc, int an)
{
	META_DLLAPI_HANDLE_void(FN_SAVEREADFIELDS, pfnSaveReadFields, 4pi, (pSaveData, pname, pBaseData, pFields, fieldCount));
	RETURN_API_void();
};

void CMetamodGameDLL::OnSaveGlobalState(SAVERESTOREDATA *apSaveRestoreData)
{
	META_DLLAPI_HANDLE_void(FN_SAVEGLOBALSTATE, pfnSaveGlobalState, p, (pSaveData));
	RETURN_API_void();
};

void CMetamodGameDLL::OnRestoreGlobalState(SAVERESTOREDATA *apSaveRestoreData)
{
	META_DLLAPI_HANDLE_void(FN_RESTOREGLOBALSTATE, pfnRestoreGlobalState, p, (pSaveData));
	RETURN_API_void();
};

void CMetamodGameDLL::OnResetGlobalState()
{
	META_DLLAPI_HANDLE_void(FN_RESETGLOBALSTATE, pfnResetGlobalState, void, (VOID_ARG));
	RETURN_API_void();
};