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
/// @brief export api for the network module

#include "common/net_api.h"
#include "network/network.hpp"

/*
=================

NetworkApi implementation

=================
*/

namespace
{
void NET_Status(struct net_status_s *status)
{
	ASSERT(status != NULL);

	status->connected =
	NET_IsLocalAddress(cls.netchan.remote_address) ? false : true;
	status->connection_time = host.realtime - cls.netchan.connect_time;
	status->remote_address = cls.netchan.remote_address;
	status->packet_loss = cls.packet_loss / 100; // percent
	status->latency = cl.frame.latency;
	status->local_address = net_local;
	status->rate = cls.netchan.rate;
};

void NET_SendRequest(int context, int request, int flags, double timeout, struct netadr_s *remote_address, net_api_response_func_t response)
{
	net_request_t *nr = NULL;
	string req;
	int i;

	if(!response)
	{
		MsgDev(
		D_ERROR,
		"Net_SendRequest: no callbcak specified for request with context %i!\n",
		context);
		return;
	}

	// find a free request
	for(i = 0; i < MAX_REQUESTS; i++)
	{
		nr = &clgame.net_requests[i];
		if(!nr->pfnFunc || nr->timeout < host.realtime)
			break;
	}

	if(i == MAX_REQUESTS)
	{
		double max_timeout = 0;

		// no free requests? use older
		for(i = 0, nr = NULL; i < MAX_REQUESTS; ++i)
		{
			if((host.realtime - clgame.net_requests[i].timesend) > max_timeout)
			{
				max_timeout = host.realtime - clgame.net_requests[i].timesend;
				nr = &clgame.net_requests[i];
			}
		}
	}

	ASSERT(nr != NULL);

	// clear slot
	Q_memset(nr, 0, sizeof(*nr));

	// create a new request
	nr->timesend = host.realtime;
	nr->timeout = nr->timesend + timeout;
	nr->pfnFunc = response;
	nr->resp.context = context;
	nr->resp.type = request;
	nr->resp.remote_address = *remote_address;
	nr->flags = flags;

	if(request == NETAPI_REQUEST_SERVERLIST)
	{
		// UNDONE: build request for master-server
	}
	else
	{
		// send request over the net
		Q_snprintf(req, sizeof(req), "netinfo %i %i %i", PROTOCOL_VERSION, context, request);
		Netchan_OutOfBandPrint(NS_CLIENT, nr->resp.remote_address, req);
	}
};

void NET_CancelRequest(int context)
{
	// find a specified request
	for(int i = 0; i < MAX_REQUESTS; ++i)
	{
		if(clgame.net_requests[i].resp.context == context)
		{
			MsgDev(D_NOTE, "Request with context %i cancelled\n", context);
			Q_memset(&clgame.net_requests[i], 0, sizeof(net_request_t));
			break;
		}
	}
};

void NET_CancelAllRequests()
{
	Q_memset(clgame.net_requests, 0, sizeof(clgame.net_requests));
};

}; // namespace

// clang-format off

net_api_t gNetAPI =
    {
        NET_Init, // Net_Config(true)?
        NET_Status,

        NET_SendRequest,
        NET_CancelRequest,
        NET_CancelAllRequests,

        NET_AdrToString,
        NET_CompareAdr,
        NET_StringToAdr,

        Info_ValueForKey,
        Info_RemoveKey,
        Info_SetValueForKey};

// clang-format on