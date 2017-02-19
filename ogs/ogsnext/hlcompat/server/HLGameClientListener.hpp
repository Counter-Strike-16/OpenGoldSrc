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

// HLGameClientListener.hpp - header of hl-compatible game client listener class

#pragma once

#include "game/server/IGameClientListener.hpp"
#include "engine/eiface.h"

class CHLGameClientListener : public IGameClientListener
{
public:
	CHLGameClientListener(DLL_FUNCTIONS *apHLGameDLL, NEW_DLL_FUNCTIONS *apHLGameDLLEx);
	~CHLGameClientListener();
	
	void Update(float afTimeStep);
	
	bool OnClientConnect(IGameClient *apClient, const char *asName, const char *asAdr, char *asRejectMsg, int anMaxRejectMsgLen);
	//bool OnClientConnect(IGameClient *apClient, const char *asName, const char *asAddress, char asRejectReason[128]);
	
	void OnClientDisconnect(IGameClient *apClient);
	
	void UpdateClientData(IGameClient *apClient, int abSendWeapons, clientdata_t *apClientData);
	
	void OnClientKill(IGameClient *apClient);
	
	int ClientInconsistentFile(IGameClient *apClient, const char *asFileName, char *asDisconnectMsg);
	
	void OnClientActivate(IGameClient *apClient, bool abLoadGame);
	
	void ClientPutInServer(IGameClient *apClient/*, char const *asPlayerName*/);
	
	void OnClientCommand(IGameClient *apClient/*, const CCommand &apArgs*/);
	
	void OnSettingsChanged(IGameClient *apClient);
	void OnUserInfoChanged(IGameClient *apClient, char *asInfoBuffer);
	
	void ClientSetupVisibility(edict_t *apViewEntity, IGameClient *apClient, byte *pvs, int pvssize);
	void SetupEntityVisibility(edict_t *apEntity, IGameClient *apClient, byte **apPVS, byte **apPAS);
	
	//float ProcessUsercmds(IGameClient *apClient, bf_read *buf, int anCmds, int anTotalCmds, int anDroppedPackets, bool abIgnore, bool abPaused);
	
	//CPlayerState *GetState(IGameClient *apClient);
	
	//void GetEarPosition(IGameClient *apClient, Vector *avEarOrigin);
	
	int GetReplayDelay(IGameClient *apClient, int &anEntity);
	
	//void ClientCommandKeyValues(edict_t *apClient, KeyValues *apKeyValues);
	
	void OnClientSpawn(IGameClient *apClient);
	
	void QueryClientCvarValue(const edict_t *apEntity, const char *asValue);
	void QueryClientCvarValueEx(const edict_t *apEntity, int anRequestID, const char *asCvarName, const char *asValue);
	
	void PlayerPreThink(IGameClient *apClient);
	void PlayerPostThink(IGameClient *apClient);
	
	void OnClientCustomization(IGameClient *apClient, customization_t *apCustomization);
	
	void ClientCmdStart(IGameClient *apClient, const usercmd_t *apCmd, uint anRandomSeed);
	void ClientCmdEnd(IGameClient *apClient);
	
	int GetPlayerWeaponData(IGameClient *apClient, weapon_data_t *apWeaponData);
private:
	DLL_FUNCTIONS *mpHLGameDLL{nullptr};
	NEW_DLL_FUNCTIONS *mpHLGameDLLEx{nullptr};
};