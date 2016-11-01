/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "common/net_api.h"
#include "network/Network.h"
#include <cstddef>

namespace
{
CNetwork *gpNetwork = NULL;

void NetAPI_InitNetworking()
{
	gpNetwork->Init();
};

void NetAPI_Status(struct net_status_s *status)
{
	status = gpNetwork->GetStatus(); // not so elegant but will work
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

char *NetAPI_AdrToString(struct netadr_s *a) // const ref in arg
{
	static char s[64];
	
	Q_memset(s, 0, sizeof(s));
	
	if(a.type == NA_LOOPBACK)
		Q_snprintf(s, sizeof(s), "loopback");
	else if(a.type == NA_IP)
		Q_snprintf(s, sizeof(s), "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
#ifdef _WIN32
	else
		Q_snprintf(s, sizeof(s), "%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%i", a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9], ntohs(a.port));
#endif // _WIN32
	
	return s;
};

/*qboolean*/ int NetAPI_CompareAdr(struct netadr_s *a, struct netadr_s *b) // arg: ref, ref
{
	if(a.type != b.type)
		return FALSE;
	
	if(a.type == NA_LOOPBACK)
		return TRUE;
	
	if(a.type == NA_IP)
	{
		if(a.ip[0] == b.ip[0] &&
			a.ip[1] == b.ip[1] &&
			a.ip[2] == b.ip[2] &&
			a.ip[3] == b.ip[3] &&
			a.port == b.port)
			return TRUE;
	}
#ifdef _WIN32
	else if(a.type == NA_IPX)
	{
		if(Q_memcmp(a.ipx, b.ipx, 10) == 0 && a.port == b.port)
			return TRUE;
	};
#endif // _WIN32
	
	return FALSE;
};

/*qboolean*/ int NetAPI_StringToAdr(/*const*/ char *s, struct netadr_s *a)
{
	gpNetwork->StringToAdr(s, a);
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