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

/// @file

#include "precompiled.hpp"
#include "game/LegacyGameClientListener.hpp"
#include "game/LegacyGame.hpp"

bool CLegacyGameClientListener::OnClientConnect(IGameClient *apClient, const char *asName, const char *asAdr, char *asRejectMsg, int anMaxRejectMsgLen)
{
	return false;
};

/*
bool CLegacyGameClientListener::OnClientConnect(IGameClient *apClient, const char *asName, const char *asAddress, char asRejectReason[128])
{
	return false;
};
*/

void CLegacyGameClientListener::OnClientDisconnect(IGameClient *apClient)
{
	mpGame->OnClientDisconnect(apClient);
};

void CLegacyGameClientListener::UpdateClientData(IGameClient *apClient, int abSendWeapons, clientdata_t *apClientData)
{
	mpGame->pfnUpdateClientData(apClient, abSendWeapons, apClientData);
};

void CLegacyGameClientListener::OnClientKill(IGameClient *apClient)
{
	mpGame->OnClientKill(apClient);
};

int CLegacyGameClientListener::ClientInconsistentFile(IGameClient *apClient, const char *asFileName, char *asDisconnectMsg)
{
	return mpGame->pfnInconsistentFile(apClient, asFileName, asDisconnectMsg);
};

void CLegacyGameClientListener::OnClientActivate(IGameClient *apClient, bool abLoadGame)
{
};

void CLegacyGameClientListener::ClientPutInServer(IGameClient *apClient /*, char const *asPlayerName*/)
{
	mpGame->OnClientPutInServer(apClient);
};

void CLegacyGameClientListener::OnClientCommand(IGameClient *apClient /*, const CCommand &apArgs*/)
{
	mpGame->OnClientCommand(apClient);
};

void CLegacyGameClientListener::OnClientSettingsChanged(IGameClient *apClient)
{
};

void CLegacyGameClientListener::OnClientUserInfoChanged(IGameClient *apClient, char *asInfoBuffer)
{
	mpGame->OnClientUserInfoChanged(apClient, asInfoBuffer);
};

void CLegacyGameClientListener::ClientSetupVisibility(edict_t *apViewEntity, IGameClient *apClient, byte *pvs, int pvssize)
{
};

void CLegacyGameClientListener::SetupEntityVisibility(edict_t *apEntity, IGameClient *apClient, byte **apPVS, byte **apPAS)
{
};

/*
void CLegacyGameClientListener::ClientCommandKeyValues(IGameClient *apClient, KeyValues *apKeyValues)
{
};
*/

void CLegacyGameClientListener::OnClientSpawn(IGameClient *apClient)
{
};

void CLegacyGameClientListener::QueryClientCvarValue(const edict_t *apEntity, const char *asValue)
{
	mpGame->pfnCvarValue(apEntity, asValue);
};

void CLegacyGameClientListener::QueryClientCvarValueEx(const edict_t *apEntity, int anRequestID, const char *asCvarName, const char *asValue)
{
	mpGame->pfnCvarValue2(apEntity, anRequestID, asCvarName, asValue);
};

void CLegacyGameClientListener::PlayerPreThink(IGameClient *apClient)
{
	mpGame->pfnPlayerPreThink(apClient);
};

void CLegacyGameClientListener::PlayerPostThink(IGameClient *apClient)
{
	mpGame->pfnPlayerPostThink(apClient);
};

void CLegacyGameClientListener::OnClientCustomization(IGameClient *apClient, customization_t *apCustomization)
{
	mpGame->pfnPlayerCustomization(apClient, apCustomization);
};

void CLegacyGameClientListener::ClientCmdStart(IGameClient *apClient, const usercmd_t *apCmd, uint anRandomSeed)
{
	mpGame->pfnCmdStart(apClient, apCmd, anRandomSeed);
};

void CLegacyGameClientListener::ClientCmdEnd(IGameClient *apClient)
{
	mpGame->pfnCmdEnd(apClient);
};

int CLegacyGameClientListener::GetPlayerWeaponData(IGameClient *apClient, weapon_data_t *apWeaponData)
{
	return mpGame->pfnGetWeaponData(apClient, apWeaponData);
};