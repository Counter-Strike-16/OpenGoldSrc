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

// MetamodGameClientListener.hpp - header of game client listener component for metamod

#ifndef METAMOD_GAMECLIENTLISTENER_HPP
#define METAMOD_GAMECLIENTLISTENER_HPP

#include "game/server/IGameClientListener.hpp"

class CMetamodGameClientListener : public IGameClientListener
{
public:
	bool OnClientConnect(edict_t *apClientEdict, const char *asName, const char *asAdr, char *asRejectMsg, int anMaxRejectMsgLen);
	//bool OnClientConnect(edict_t *apClientEdict, const char *asName, const char *asAddress, char asRejectReason[128]);
	void OnClientDisconnect(edict_t *apClientEdict);
	
	void UpdateClientData(edict_t *apClientEdict, int abSendWeapons, clientdata_t *apClientData);
	
	void OnClientKill(edict_t *apClientEdict);
	
	int ClientInconsistentFile(edict_t *apClientEdict, const char *asFileName, char *asDisconnectMsg);
	
	void OnClientActivate(edict_t *apClientEdict, bool abLoadGame);
	
	void ClientPutInServer(edict_t *apClientEdict /*, char const *asPlayerName*/);
	
	void OnClientCommand(edict_t *apClientEdict /*, const CCommand &apArgs*/);
	
	void OnClientSettingsChanged(edict_t *apClientEdict);
	void OnClientUserInfoChanged(edict_t *apClientEdict, char *asInfoBuffer);
	
	void SetupClientVisibility(edict_t *apViewEntity, edict_t *apClientEdict, byte *pvs, int pvssize);
	void SetupEntityVisibility(edict_t *apEntity, edict_t *apClientEdict, byte **apPVS, byte **apPAS);
	
	//float ProcessClientUsercmds(edict_t *apClientEdict, bf_read *buf, int anCmds, int anTotalCmds, int anDroppedPackets, bool abIgnore, bool abPaused);
	
	//CPlayerState *GetPlayerState(edict_t *apClientEdict);
	
	//void GetClientEarPosition(edict_t *apClientEdict, Vector *avEarOrigin);
	
	int GetClientReplayDelay(edict_t *apClientEdict, int &anEntity);
	
	//void ClientCommandKeyValues(edict_t *apClientEdict, KeyValues *apKeyValues);
	
	void OnPlayerSpawn(edict_t *apClientEdict);
	
	void QueryClientCvarValue(const edict_t *apEntity, const char *asValue);
	void QueryClientCvarValueEx(const edict_t *apEntity, int anRequestID, const char *asCvarName, const char *asValue);
	
	void PlayetPreThink(edict_t *apClientEdict);
	void PlayerPostThink(edict_t *apClientEdict);
	
	void OnClientCustomization(edict_t *apClientEdict, customization_t *apCustomization);
	
	void OnClientCmdStart(edict_t *apClientEdict, const usercmd_t *apCmd, uint anRandomSeed);
	void OnClientCmdEnd(edict_t *apClientEdict);
	
	int GetPlayerWeaponData(edict_t *apClientEdict, weapon_data_t *apWeaponData);
};

#endif // METAMOD_GAMECLIENTLISTENER_HPP