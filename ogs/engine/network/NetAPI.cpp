#include "common/net_api.h"

namespace
{
void NetAPI_InitNetworking()
{
};

void NetAPI_Status(struct net_status_s *status)
{
};

void NetAPI_SendRequest(int context, int request, int flags, double timeout, struct netadr_s *remote_address, net_api_response_func_t response)
{
};

void NetAPI_CancelRequest(int context)
{
};

void NetAPI_CancelAllRequests()
{
};

char *NetAPI_AdrToString(struct netadr_s *a)
{
	return "";
};

int NetAPI_CompareAdr(struct netadr_s *a, struct netadr_s *b)
{
	return 0;
};

int NetAPI_StringToAdr(char *s, struct netadr_s *a)
{
	return 0;
};

const char *NetAPI_ValueForKey(const char *s, const char *key)
{
	return "";
};

void NetAPI_RemoveKey(char *s, const char *key)
{
};

void NetAPI_SetValueForKey(char *s, const char *key, const char *value, int maxsize)
{
};
}; // namespace

net_api_t gNetAPI =
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
	NetAPI_SetValueForKey
};