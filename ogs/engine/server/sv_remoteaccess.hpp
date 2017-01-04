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

#include "IGameServerData.h"
#include "tier1/utlbuffer.h"
#include "tier1/utllinkedlist.h"

class CServerRemoteAccess : public IGameServerData
{
public:
	CServerRemoteAccess();
	virtual ~CServerRemoteAccess(){}
	
	virtual void WriteDataRequest(const void *buffer, int bufferSize);
	virtual int ReadDataResponse(void *data, int len);

	void WriteDataRequest_noVirt(const void *buffer, int bufferSize);
	int ReadDataResponse_noVirt(void *data, int len);

	void SendMessageToAdminUI(const char *message);
	void RequestValue(int requestID, const char *variable);
	void SetValue(const char *variable, const char *value);
	void ExecCommand(const char *cmdString);
	bool LookupValue(const char *variable, CUtlBuffer &value);
	const char *LookupStringValue(const char *variable);
	void GetUserBanList(CUtlBuffer &value);
	void GetPlayerList(CUtlBuffer &value);
	void GetMapList(CUtlBuffer &value);
	
	struct DataResponse_t
	{
		CUtlBuffer packet;
	};
private:
	CUtlLinkedList<DataResponse_t, int> m_ResponsePackets;
	int m_iBytesSent;
	int m_iBytesReceived;
};

#ifdef HOOK_ENGINE
#define g_ServerRemoteAccess (*pg_ServerRemoteAccess)
#endif

extern class CServerRemoteAccess g_ServerRemoteAccess;

extern void NotifyDedicatedServerUI(const char *message);