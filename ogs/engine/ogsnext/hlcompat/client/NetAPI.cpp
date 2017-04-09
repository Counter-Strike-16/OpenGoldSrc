static net_api_t gNetApi =
{
	NetAPI_InitNetworking,
	NetAPI_Status,
	NetAPI_SendRequest,
	NetAPI_CancelRequest,
	NetAPI_CancelAllRequests,
	NetAPI_AdrToString,
	NetAPI_CompareAdr,
	NetAPI_StringToAdr,
	NetAPI_ValueForKey,
	NetAPI_RemoveKey,
	NetAPI_SetValueForKey,
};

/*
=================
NetworkApi implementation
=================
*/

/*
=================
NetAPI_InitNetworking
=================
*/
static void NetAPI_InitNetworking()
{
	NET_Config(true); // allow remote
};

/*
=================
NetAPI_InitNetworking
=================
*/
static void NetAPI_Status(net_status_t *status)
{
	ASSERT(status != NULL);

	status->connected = NET_IsLocalAddress(cls.netchan.remote_address) ? false : true;
	status->connection_time = host.realtime - cls.netchan.connect_time;
	status->remote_address = cls.netchan.remote_address;
	status->packet_loss = cls.packet_loss / 100; // percent
	status->latency = cl.frame.latency;
	status->local_address = net_local;
	status->rate = cls.netchan.rate;
};

/*
=================
NetAPI_SendRequest
=================
*/
static void NetAPI_SendRequest(int context, int request, int flags, double timeout, netadr_t *remote_address, net_api_response_func_t response)
{
	net_request_t	*nr = NULL;
	string		req;
	int		i;

	if(!response)
	{
		MsgDev(D_ERROR, "Net_SendRequest: no callbcak specified for request with context %i!", context);
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
		double	max_timeout = 0;

		// no free requests? use older
		for(i = 0, nr = NULL; i < MAX_REQUESTS; i++)
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
	};
};

/*
=================
NetAPI_CancelRequest
=================
*/
static void NetAPI_CancelRequest(int context)
{
	int	i;

	// find a specified request
	for(i = 0; i < MAX_REQUESTS; i++)
	{
		if(clgame.net_requests[i].resp.context == context)
		{
			MsgDev(D_NOTE, "Request with context %i cancelled", context);
			Q_memset(&clgame.net_requests[i], 0, sizeof(net_request_t));
			break;
		};
	};
};

/*
=================
NetAPI_CancelAllRequests
=================
*/
static void NetAPI_CancelAllRequests()
{
	Q_memset(clgame.net_requests, 0, sizeof(clgame.net_requests));
};

/*
=================
NetAPI_AdrToString
=================
*/
static char *NetAPI_AdrToString(netadr_t *a)
{
	return NET_AdrToString(*a);
};

/*
=================
NetAPI_CompareAdr
=================
*/
static int NetAPI_CompareAdr(netadr_t *a, netadr_t *b)
{
	return NET_CompareAdr(*a, *b);
};

/*
=================
NetAPI_StringToAdr
=================
*/
static int NetAPI_StringToAdr(char *s, netadr_t *a)
{
	return NET_StringToAdr(s, a);
};

/*
=================
NetAPI_ValueForKey
=================
*/
static const char *NetAPI_ValueForKey(const char *s, const char *key)
{
	return Info_ValueForKey(s, key);
};

/*
=================
NetAPI_RemoveKey
=================
*/
static void NetAPI_RemoveKey(char *s, const char *key)
{
	Info_RemoveKey(s, key);
};

/*
=================
NetAPI_SetValueForKey
=================
*/
static void NetAPI_SetValueForKey(char *s, const char *key, const char *value, int maxsize)
{
	if(key[0] == '*') return;
	Info_SetValueForStarKey(s, key, value, maxsize);
};