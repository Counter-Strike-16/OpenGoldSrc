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

#include <memory>
#include "common/maintypes.h"
#include "network/GameConnectionHandler.hpp"
#include "system/server.hpp"

class CNetServer; // NOTE: struct INetServer?
class CGameConnectionHandler;

//mpNetServer->SetConnectionHandler(mpConnectionHandler);

class CGameServer // : public IGameServer
{
public:
	CGameServer(CNetServer *apNetServer) : mpNetServer(apNetServer){}
	~CGameServer() = default;
	
	void Init(); // bool?
	void Shutdown();
	
	//void Clear();
	
	void Frame(float frametime);
	
	void ClearClientStates();
	
	void SetupMaxClients();
	
	void HandleConnectionlessPacket(netadr_t *adr, const char *c, const char *args);
	
	void BroadcastCommand(char *fmt, ...);
	void BroadMessage(const char *asMsg, ...);
	
	void ConnectClient();
	
	void InactivateClients();
	NOXREF void ReconnectAllClients();
	
	bool FindEmptySlot(int *pslot, client_t **ppClient);
	
	void CountPlayers(int *clients);
	
	bool IsActive() const;
	
	//CNetServer *GetNetServer() const {return mpNetServer;} // ?
private:
	void CheckCmdTimes();
	void CheckTimeouts();
	void SendClientMessages(float frametime);
	void CheckMapDifferences();
	void GatherStatistics();
	
	void UpdateToReliableMessages();
	
	void ConnectClient_internal();
	
	std::unique_ptr<CGameConnectionHandler> mpConnectionHandler;
	
	CNetServer *mpNetServer{nullptr}; // It's impl can be split into dll...
};