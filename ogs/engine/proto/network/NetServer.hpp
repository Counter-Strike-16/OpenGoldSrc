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
/// @brief network server; accepts clients and used for authority

#pragma once

typedef struct netadr_s netadr_t;

struct IConnectionHandler;

class CNetServer
{
public:
	bool Start(int anPort);
	void Stop();
	
	void SetConnectionHandler(IConnectionHandler *apHandler);
	IConnectionHandler *GetConnectionHandler() const {return mpConnectionHandler;}
	
	void Frame(float frametime);
	
	bool FilterPacket();
	void SendBan();
	
	void CheckForRcon();
	
	void HandleRconPacket(netadr_t *adr);
	void HandleConnectionlessPacket(netadr_t *adr);
	
	void BroadcastCommand(char *fmt, ...);
	void BroadcastPrintf(const char *fmt, ...);
	
	void ConnectClient(netadr_t *adr);
	
	void KickPlayer(int nPlayerSlot, int nReason);
	
	void ReconnectClient(int anID);
	void DisconnectClient(int anID);
	
	void ReconnectAll();
	void DisconnectAll();
	
	NOXREF int GetChallengeNr(netadr_t *adr);
	NOXREF void ReplyServerChallenge(netadr_t *adr);
	
	int GetClientCount() const;
	int GetFakeClientCount() const;
	
	int GetMaxClients() const;
private:
	void ReadPackets(float frametime);
	
	void SVC_GetChallenge(netadr_t *adr);
	void SVC_ServiceChallenge(netadr_t *adr);
	
	void HandleRcon(netadr_t *net_from_);
	int Rcon_Validate();
	bool CheckRconFailure(netadr_t *adr);
	void AddFailedRcon(netadr_t *adr);
	
	IConnectionHandler *mpConnectionHandler{nullptr};
	IConnectionHandler *mpDefaultConnHandler{nullptr};
protected:
	virtual void HandleConnectionlessPacket(netadr_t *adr, const char *c, const char *args){}
	virtual void ConnectClient_internal(){}
};