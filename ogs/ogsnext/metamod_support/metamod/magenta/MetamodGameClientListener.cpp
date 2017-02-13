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

// MetamodGameClientListener.cpp - source of game client listener component for metamod

#include "MetamodGameClientListener.hpp"
#include "dllapi.h"

bool CMetamodGameClientListener::OnClientConnect(edict_t *apClientEdict, const char *asName, const char *asAdr, char *asRejectMsg, int anMaxRejectMsgLen)
{
	g_Players.clear_player_cvar_query(pEntity);
	META_DLLAPI_HANDLE(qboolean, TRUE, FN_CLIENTCONNECT, pfnClientConnect, 4p, (pEntity, pszName, pszAddress, szRejectReason));
	RETURN_API(qboolean);
};

/*
bool CMetamodGameClientListener::OnClientConnect(edict_t *apClientEdict, const char *asName, const char *asAddress, char asRejectReason[128])
{
};
*/

void CMetamodGameClientListener::OnClientDisconnect(edict_t *apClientEdict)
{
	g_Players.clear_player_cvar_query(pEntity);
	META_DLLAPI_HANDLE_void(FN_CLIENTDISCONNECT, pfnClientDisconnect, p, (pEntity));
	RETURN_API_void();
};

void CMetamodGameClientListener::UpdateClientData(edict_t *apClientEdict, int abSendWeapons, clientdata_t *apClientData)
{
	META_DLLAPI_HANDLE_void(FN_UPDATECLIENTDATA, pfnUpdateClientData, pip, (ent, sendweapons, cd));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClientKill(edict_t *apClientEdict)
{
	META_DLLAPI_HANDLE_void(FN_CLIENTKILL, pfnClientKill, p, (pEntity));
	RETURN_API_void();
};

int CMetamodGameClientListener::ClientInconsistentFile(edict_t *apClientEdict, const char *asFileName, char *asDisconnectMsg)
{
	META_DLLAPI_HANDLE(int, 0, FN_INCONSISTENTFILE, pfnInconsistentFile, 3p, (player, filename, disconnect_message));
	RETURN_API(int);
};

void CMetamodGameClientListener::OnClientActivate(edict_t *apClientEdict, bool abLoadGame)
{
};

void CMetamodGameClientListener::ClientPutInServer(edict_t *apClientEdict /*, char const *asPlayerName*/)
{
	META_DLLAPI_HANDLE_void(FN_CLIENTPUTINSERVER, pfnClientPutInServer, p, (pEntity));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClientCommand(edict_t *apClientEdict /*, const CCommand &apArgs*/)
{
	if(Config->clientmeta && strmatch(CMD_ARGV(0), "meta"))
		client_meta(pEntity);
	
	META_DLLAPI_HANDLE_void(FN_CLIENTCOMMAND, pfnClientCommand, p, (pEntity));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClientSettingsChanged(edict_t *apClientEdict)
{
};

void CMetamodGameClientListener::OnClientUserInfoChanged(edict_t *apClientEdict, char *asInfoBuffer)
{
	META_DLLAPI_HANDLE_void(FN_CLIENTUSERINFOCHANGED, pfnClientUserInfoChanged, 2p, (pEntity, infobuffer));
	RETURN_API_void();
};

void CMetamodGameClientListener::SetupClientVisibility(edict_t *apViewEntity, edict_t *apClientEdict, byte *pvs, int pvssize)
{
};

void CMetamodGameClientListener::SetupEntityVisibility(edict_t *apEntity, edict_t *apClientEdict, byte **apPVS, byte **apPAS)
{
	META_DLLAPI_HANDLE_void(FN_SETUPVISIBILITY, pfnSetupVisibility, 4p, (pViewEntity, pClient, pvs, pas));
	RETURN_API_void();
};

/*
float CMetamodGameClientListener::ProcessClientUsercmds(edict_t *apClientEdict, bf_read *buf, int anCmds, int anTotalCmds, int anDroppedPackets, bool abIgnore, bool abPaused)
{
};

CPlayerState *CMetamodGameClientListener::GetPlayerState(edict_t *apClientEdict)
{
};

void CMetamodGameClientListener::GetClientEarPosition(edict_t *apClientEdict, Vector *avEarOrigin)
{
};
*/

int CMetamodGameClientListener::GetClientReplayDelay(edict_t *apClientEdict, int &anEntity)
{
};

/*
void CMetamodGameClientListener::ClientCommandKeyValues(edict_t *apClientEdict, KeyValues *apKeyValues)
{
};
*/

void CMetamodGameClientListener::OnPlayerSpawn(edict_t *apClientEdict)
{
};

void CMetamodGameClientListener::QueryClientCvarValue(const edict_t *apEntity, const char *asValue)
{
	g_Players.clear_player_cvar_query(pEnt);
	META_NEWAPI_HANDLE_void(FN_CVARVALUE, pfnCvarValue, 2p, (pEnt, value));
	
	RETURN_API_void();
};

void CMetamodGameClientListener::QueryClientCvarValueEx(const edict_t *apEntity, int anRequestID, const char *asCvarName, const char *asValue)
{
	META_NEWAPI_HANDLE_void(FN_CVARVALUE2, pfnCvarValue2, pi2p, (pEnt, requestID, cvarName, value));
	
	RETURN_API_void();
};

void CMetamodGameClientListener::PlayetPreThink(edict_t *apClientEdict)
{
	META_DLLAPI_HANDLE_void(FN_PLAYERPRETHINK, pfnPlayerPreThink, p, (pEntity));
	RETURN_API_void();
};

void CMetamodGameClientListener::PlayerPostThink(edict_t *apClientEdict)
{
	META_DLLAPI_HANDLE_void(FN_PLAYERPOSTTHINK, pfnPlayerPostThink, p, (pEntity));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClient/*Player*/Customization(edict_t *apClientEdict, customization_t *apCustomization)
{
	META_DLLAPI_HANDLE_void(FN_PLAYERCUSTOMIZATION, pfnPlayerCustomization, 2p, (pEntity, pCust));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClientCmdStart(edict_t *apClientEdict, const usercmd_t *apCmd, uint anRandomSeed)
{
	META_DLLAPI_HANDLE_void(FN_CMDSTART, pfnCmdStart, 2pui, (apClientEdict, apCmd, anRandomSeed));
	RETURN_API_void();
};

void CMetamodGameClientListener::OnClientCmdEnd(edict_t *apClientEdict)
{
	META_DLLAPI_HANDLE_void(FN_CMDEND, pfnCmdEnd, p, (apClientEdict));
	RETURN_API_void();
};

int CMetamodGameClientListener::GetPlayerWeaponData(edict_t *apClientEdict, weapon_data_t *apWeaponData)
{
	META_DLLAPI_HANDLE(int, 0, FN_GETWEAPONDATA, pfnGetWeaponData, 2p, (player, info));
	RETURN_API(int);
};