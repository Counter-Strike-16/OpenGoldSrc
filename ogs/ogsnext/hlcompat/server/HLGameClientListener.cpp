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

// HLGameClientListener.cpp - source of hl-compatible game client listener class

#include "hlcompatgamedll/HLGameClientListener.hpp"
#include "debug/Debug.hpp"

CHLGameClientListener::CHLGameClientListener(DLL_FUNCTIONS *apHLGameDLL, NEW_DLL_FUNCTIONS *apHLGameDLLEx)
{
	LogMsg("Constructing the hl-compatible game client listener component...");
	
	mpHLGameDLL = apHLGameDLL;
	mpHLGameDLLEx = apHLGameDLLEx;
};

CHLGameClientListener::~CHLGameClientListener()
{
	LogMsg("Destructing the hl-compatible game client listener component...");
};

bool CHLGameClientListener::OnClientConnect(IGameClient *apClient, const char *asName, const char *asAdr, char *asRejectMsg, int anMaxRejectMsgLen)
{
	return false;
};

/*
bool CHLGameClientListener::OnClientConnect(IGameClient *apClient, const char *asName, const char *asAddress, char asRejectReason[128])
{
	return false;
};
*/

void CHLGameClientListener::OnClientDisconnect(IGameClient *apClient)
{
	mpHLGameDLL->pfnClientDisconnect(apClient);
};

void CHLGameClientListener::UpdateClientData(IGameClient *apClient, int abSendWeapons, clientdata_t *apClientData)
{
	mpHLGameDLL->pfnUpdateClientData(apClient, abSendWeapons, apClientData);
};

void CHLGameClientListener::OnClientKill(IGameClient *apClient)
{
	mpHLGameDLL->pfnClientKill(apClient);
};

int CHLGameClientListener::ClientInconsistentFile(IGameClient *apClient, const char *asFileName, char *asDisconnectMsg)
{
	return mpHLGameDLL->pfnInconsistentFile(apClient, asFileName, asDisconnectMsg);
};

void CHLGameClientListener::OnClientActivate(IGameClient *apClient, bool abLoadGame)
{
};

void CHLGameClientListener::ClientPutInServer(IGameClient *apClient /*, char const *asPlayerName*/)
{
	mpHLGameDLL->pfnClientPutInServer(apClient);
};

void CHLGameClientListener::OnClientCommand(IGameClient *apClient /*, const CCommand &apArgs*/)
{
	mpHLGameDLL->pfnClientCommand(apClient);
};

void CHLGameClientListener::OnClientSettingsChanged(IGameClient *apClient)
{
};

void CHLGameClientListener::OnClientUserInfoChanged(IGameClient *apClient, char *asInfoBuffer)
{
	mpHLGameDLL->pfnClientUserInfoChanged(apClient, asInfoBuffer);
};

void CHLGameClientListener::ClientSetupVisibility(edict_t *apViewEntity, IGameClient *apClient, byte *pvs, int pvssize)
{
};

void CHLGameClientListener::SetupEntityVisibility(edict_t *apEntity, IGameClient *apClient, byte **apPVS, byte **apPAS)
{
};

/*
float CHLGameClientListener::ProcessClientUsercmds(IGameClient *apClient, bf_read *buf, int anCmds, int anTotalCmds, int anDroppedPackets, bool abIgnore, bool abPaused)
{
	return 0.0f;
};
*/

/*
CPlayerState *CHLGameClientListener::GetPlayerState(IGameClient *apClient)
{
	return nullptr;
};

void CHLGameClientListener::GetClientEarPosition(IGameClient *apClient, Vector *avEarOrigin)
{
};
*/

int CHLGameClientListener::GetClientReplayDelay(IGameClient *apClient, int &anEntity)
{
	return 0;
};

/*
void CHLGameClientListener::ClientCommandKeyValues(IGameClient *apClient, KeyValues *apKeyValues)
{
};
*/

void CHLGameClientListener::OnClientSpawn(IGameClient *apClient)
{
};

void CHLGameClientListener::QueryClientCvarValue(const edict_t *apEntity, const char *asValue)
{
	mpHLGameDLLEx->pfnCvarValue(apEntity, asValue);
};

void CHLGameClientListener::QueryClientCvarValueEx(const edict_t *apEntity, int anRequestID, const char *asCvarName, const char *asValue)
{
	mpHLGameDLLEx->pfnCvarValue2(apEntity, anRequestID, asCvarName, asValue);
};

void CHLGameClientListener::PlayerPreThink(IGameClient *apClient)
{
	mpHLGameDLL->pfnPlayerPreThink(apClient);
};

void CHLGameClientListener::PlayerPostThink(IGameClient *apClient)
{
	mpHLGameDLL->pfnPlayerPostThink(apClient);
};

void CHLGameClientListener::OnClientCustomization(IGameClient *apClient, customization_t *apCustomization)
{
	mpHLGameDLL->pfnPlayerCustomization(apClient, apCustomization);
};

void CHLGameClientListener::ClientCmdStart(IGameClient *apClient, const usercmd_t *apCmd, uint anRandomSeed)
{
	mpHLGameDLL->pfnCmdStart(apClient, apCmd, anRandomSeed);
};

void CHLGameClientListener::ClientCmdEnd(IGameClient *apClient)
{
	mpHLGameDLL->pfnCmdEnd(apClient);
};

int CHLGameClientListener::GetPlayerWeaponData(IGameClient *apClient, weapon_data_t *apWeaponData)
{
	return mpHLGameDLL->pfnGetWeaponData(apClient, apWeaponData);
};