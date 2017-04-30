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

#include "network/IConnectionHandler.hpp"

class CGameServer;

class CGameConnectionHandler : public IConnectionHandler
{
public:
	CGameConnectionHandler(CGameServer *apServer) : mpServer(apServer){}
	
	bool ConnectClient(netadr_t *adr) override;
private:
	void RejectConnection(netadr_t *adr, char *fmt, ...);
	void RejectConnectionForPassword(netadr_t *adr);
	
	int CheckProtocol(netadr_t *adr, int nProtocol);
	int CheckProtocol_internal(netadr_t *adr, int nProtocol);

	bool CheckChallenge_api(const netadr_t &adr, int nChallengeValue);
	int CheckChallenge(netadr_t *adr, int nChallengeValue);
	
	int CheckKeyInfo(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
	int CheckKeyInfo_internal(netadr_t *adr, char *protinfo, unsigned short *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
	
	int CheckIPRestrictions(netadr_t *adr, int nAuthProtocol);
	int CheckIPRestrictions_internal(netadr_t *adr, int nAuthProtocol);
	
	int CheckUserInfo(netadr_t *adr, char *userinfo, bool bIsReconnecting, int nReconnectSlot, char *name);
	
	int FinishCertificateCheck(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
	int FinishCertificateCheck_internal(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
	
	int CheckIPConnectionReuse(netadr_t *adr);
	
	CGameServer *mpServer{nullptr};
};