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

class CNetServer
{
public:
	void Start(int anPort);
	void Stop();
	
	void Frame(float frametime);
	
	void HandleRconPacket(netadr_t *adr);
	void HandleConnectionlessPacket(netadr_t *adr);
	
	void BroadcastCommand(char *fmt, ...);
	void BroadcastPrintf(const char *fmt, ...);
	
	void ReconnectClient(int anID);
	void DisconnectClient(int anID);
	
	void ReconnectAll();
	void DisconnectAll();
	
	NOXREF int GetChallengeNr(netadr_t *adr);
	NOXREF void ReplyServerChallenge(netadr_t *adr);
	
	int GetMaxClients();
private:
	void ReadPackets();
	
	void HandleRcon(netadr_t *net_from_);
	int Rcon_Validate();
	bool CheckRconFailure(netadr_t *adr);
protected:
	virtual HandleConnectionlessPacket(const char *c, const char *args){}
};