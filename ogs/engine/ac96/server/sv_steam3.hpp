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

#pragma once

#include "common/SteamCommon.h"
#include "common/maintypes.h"
#include "system/common.hpp"
#include "system/server.hpp"
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"
#include "steam/steamclientpublic.h"

class CSteamID;

/**
*	Base class for Steam3 systems
*/
class CSteam3
{
protected:
	bool m_bLoggedOn;
	bool m_bLogOnResult;
	HSteamPipe m_hSteamPipe;

protected:
	CSteam3() : m_bLoggedOn(false), m_bLogOnResult(false), m_hSteamPipe(0){}
	virtual ~CSteam3(){}
	
	virtual void Shutdown() = 0;

	void GSSendUserStatusResponse(CSteamID &, int, int);
	bool InitModule();
};

class CSteam3Server : public CSteam3
{
public:
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientApprove, GSClientApprove_t, m_CallbackGSClientApprove);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientDeny, GSClientDeny_t, m_CallbackGSClientDeny);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientKick, GSClientKick_t, m_CallbackGSClientKick);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSPolicyResponse, GSPolicyResponse_t, m_CallbackGSPolicyResponse);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnLogonSuccess, SteamServersConnected_t, m_CallbackLogonSuccess);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnLogonFailure, SteamServerConnectFailure_t, m_CallbackLogonFailure);

protected:
	bool m_bHasActivePlayers;
	bool m_bWantToBeSecure;
	bool m_bLanOnly;
	CSteamID m_SteamIDGS;

public:
	NOBODY void SetServerType();
	NOBODY void SetSpawnCount(int count);
	NOBODY bool BSecure();
	NOBODY bool BLanOnly();

	bool BWantsSecure()
	{
		return m_bWantToBeSecure;
	}
	bool BLoggedOn()
	{
		return m_bLoggedOn;
	}
	uint64 GetSteamID();

	void OnGSClientDenyHelper(client_t *cl, EDenyReason eDenyReason, const char *pchOptionalText);
	client_t *ClientFindFromSteamID(class CSteamID &steamIDFind);

	CSteam3Server();

	void Activate();
	virtual void Shutdown();
	bool NotifyClientConnect(client_t *client, const void *pvSteam2Key, uint32 ucbSteam2Key);
	bool NotifyBotConnect(client_t *client);
	void NotifyClientDisconnect(client_t *cl);
	void NotifyOfLevelChange(bool bForce);
	void RunFrame();
	void SendUpdatedServerDetails();
};

class CSteam3Client : public CSteam3
{
public:
	STEAM_CALLBACK(CSteam3Client, OnClientGameServerDeny, ClientGameServerDeny_t, m_CallbackClientGameServerDeny);
	STEAM_CALLBACK(CSteam3Client, OnGameServerChangeRequested, GameServerChangeRequested_t, m_CallbackGameServerChangeRequested);
	STEAM_CALLBACK(CSteam3Client, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);

	CSteam3Client()
	    : m_CallbackClientGameServerDeny(this,
	                                     &CSteam3Client::OnClientGameServerDeny),
	      m_CallbackGameServerChangeRequested(
	      this, &CSteam3Client::OnGameServerChangeRequested),
	      m_CallbackGameOverlayActivated(this,
	                                     &CSteam3Client::OnGameOverlayActivated)
	{
	}

	virtual void Shutdown();

	int InitiateGameConnection(void *pData, int cbMaxData, uint64 steamID, uint32 unIPServer, uint16 usPortServer, bool bSecure);
	void TerminateConnection(uint32, uint16);
	void InitClient();
	void RunFrame();
};

#ifdef HOOK_ENGINE
#define s_Steam3Server (*ps_Steam3Server)
#define s_Steam3Client (*ps_Steam3Client)
#endif // HOOK_ENGINE

extern CSteam3Server *s_Steam3Server;
extern CSteam3Client s_Steam3Client;

extern bool (CSteam3Server::*pNotifyClientConnect)(client_t *client,
                                                   const void *pvSteam2Key,
                                                   uint32 ucbSteam2Key);

uint64 ISteamGameServer_CreateUnauthenticatedUserConnection();
bool ISteamGameServer_BUpdateUserData(uint64 steamid, const char *netname, uint32 score);
bool ISteamApps_BIsSubscribedApp(uint32 appid);
const char *Steam_GetCommunityName();
qboolean Steam_NotifyClientConnect(client_t *cl, const void *pvSteam2Key, unsigned int ucbSteam2Key);
qboolean Steam_NotifyClientConnect_internal(client_t *cl,
                                            const void *pvSteam2Key,
                                            unsigned int ucbSteam2Key);
qboolean Steam_NotifyClientConnect_api(IGameClient *cl, const void *pvSteam2Key, unsigned int ucbSteam2Key);

qboolean Steam_NotifyBotConnect(client_t *cl);
qboolean Steam_NotifyBotConnect_internal(client_t *cl);
qboolean Steam_NotifyBotConnect_api(IGameClient *cl);

void Steam_NotifyClientDisconnect(client_t *cl);
void Steam_NotifyClientDisconnect_internal(IGameClient *cl);
void Steam_NotifyClientDisconnect_api(IGameClient *cl);

void Steam_NotifyOfLevelChange();

void Steam_Shutdown();
void Steam_Activate();

void Steam_RunFrame();

void Steam_SetCVar(const char *pchKey, const char *pchValue);

void Steam_InitClient();
void Steam_ShutdownClient();

void Steam_ClientRunFrame();

int Steam_GSInitiateGameConnection(void *pData, int cbMaxData, uint64 steamID, uint32 unIPServer, uint16 usPortServer, qboolean bSecure);
void Steam_GSTerminateGameConnection(uint32 unIPServer, uint16 usPortServer);

uint64 Steam_GSGetSteamID();
qboolean Steam_GSBSecure();
qboolean Steam_GSBLoggedOn();
qboolean Steam_GSBSecurePreference();
TSteamGlobalUserID Steam_Steam3IDtoSteam2(uint64 unSteamID);
uint64 Steam_StringToSteamID(const char *pStr);
const char *Steam_GetGSUniverse();
CSteam3Server *Steam3Server();
CSteam3Client *Steam3Client();
void Master_SetMaster_f();
void Steam_HandleIncomingPacket(byte *data, int len, int fromip, uint16 port);