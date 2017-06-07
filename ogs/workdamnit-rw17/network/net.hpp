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

#include "common/commontypes.h"
#include "common/enums.h"
#include "common/netadr.h"
#include "common/maintypes.h"
#include "public/rehlds/common_rehlds.h"
#include "network/networktypes.hpp"

#ifndef _WIN32

#define WSAEWOULDBLOCK EWOULDBLOCK     // Operation would block EAGAIN (11)
#define WSAEMSGSIZE EMSGSIZE           // Message too long (90)
#define WSAEADDRNOTAVAIL EADDRNOTAVAIL // Cannot assign requested address (99)
#define WSAEAFNOSUPPORT EAFNOSUPPORT   // Address family not supported by protocol (97)
#define WSAECONNRESET ECONNRESET     // Connection reset by peer (104)
#define WSAECONNREFUSED ECONNREFUSED // Connection refused (111)
#define WSAEADDRINUSE EADDRINUSE     // Address already in use (98)
#define WSAENOBUFS ENOBUFS           // No buffer space available (105)

#endif // _WIN32

const int MAX_ROUTEABLE_PACKET = 1400;

#define SPLIT_SIZE (MAX_ROUTEABLE_PACKET - sizeof(SPLITPACKET))

// Create general message queues
const int NUM_MSG_QUEUES = 40;
const int MSG_QUEUE_SIZE = 1536;

typedef struct sizebuf_s sizebuf_t;
typedef struct cvar_s cvar_t;

// Message data
typedef struct flowstats_s
{
	// Size of message sent/received
	int size;
	// Time that message was sent/received
	double time;
} flowstats_t;

typedef struct flow_s
{
	// Data for last MAX_LATENT messages
	flowstats_t stats[MAX_LATENT];
	// Current message position
	int current;
	// Time when we should recompute k/sec data
	double nextcompute;
	// Average data
	float kbytespersec;
	float avgkbytespersec;
} flow_t;

// Generic fragment structure
typedef struct fragbuf_s
{
	// Next buffer in chain
	fragbuf_s *next;
	// Id of this buffer
	int bufferid;
	// Message buffer where raw data is stored
	sizebuf_t frag_message;
	// The actual data sits here
	byte frag_message_buf[FRAGMENT_MAX_SIZE];
	// Is this a file buffer?
	qboolean isfile;
	// Is this file buffer from memory ( custom decal, etc. ).
	qboolean isbuffer;
	qboolean iscompressed;
	// Name of the file to save out on remote host
	char filename[MAX_PATH];
	// Offset in file from which to read data
	int foffset;
	// Size of data to read at that offset
	int size;
} fragbuf_t;

// Waiting list of fragbuf chains
typedef struct fragbufwaiting_s
{
	// Next chain in waiting list
	fragbufwaiting_s *next;
	// Number of buffers in this chain
	int fragbufcount;
	// The actual buffers
	fragbuf_t *fragbufs;
} fragbufwaiting_t;

typedef struct loopmsg_s
{
	unsigned char data[NET_MAX_MESSAGE];
	int datalen;
} loopmsg_t;

const int MAX_LOOPBACK = 4;

typedef struct loopback_s
{
	loopmsg_t msgs[MAX_LOOPBACK];
	int get;
	int send;
} loopback_t;

typedef struct packetlag_s
{
	unsigned char *pPacketData; // Raw stream data is stored.
	int nSize;
	netadr_t net_from_;
	float receivedTime;
	struct packetlag_s *pNext;
	struct packetlag_s *pPrev;
} packetlag_t;

typedef struct net_messages_s
{
	struct net_messages_s *next;
	qboolean preallocated;
	unsigned char *buffer;
	netadr_t from;
	int buffersize;
} net_messages_t;

// Split long packets. Anything over 1460 is failing on some routers.
typedef struct LONGPACKET_t
{
	int currentSequence;
	int splitCount;
	int totalSize;
	// TODO: It should be NET_MAX_MESSAGE, but value differs
	char buffer[MAX_UDP_PACKET]; // This has to be big enough to hold the largest
	                             // message
} LONGPACKET;

// Use this to pick apart the network stream, must be packed
#pragma pack(push, 1)
typedef struct SPLITPACKET_t
{
	int netID;
	int sequenceNumber;
	unsigned char packetID;
} SPLITPACKET;
#pragma pack(pop)

const int NET_WS_MAX_FRAGMENTS = 5;

#ifdef HOOK_ENGINE
#define net_thread_initialized (*pnet_thread_initialized)
#define net_address (*pnet_address)
#define ipname (*pipname)
#define defport (*pdefport)
#define ip_clientport (*pip_clientport)
#define clientport (*pclientport)
#define net_sleepforever (*pnet_sleepforever)
#define loopbacks (*ploopbacks)
#define g_pLagData (*pg_pLagData)
#define gFakeLag (*pgFakeLag)
#define net_configured (*pnet_configured)
#define net_message (*pnet_message)
#ifdef _WIN32
#define net_local_ipx_adr (*pnet_local_ipx_adr)
#endif // _WIN32
#define net_local_adr (*pnet_local_adr)
#define net_from (*pnet_from)
#define noip (*pnoip)
#ifdef _WIN32
#define noipx (*pnoipx)
#endif // _WIN32
#define clockwindow (*pclockwindow)
#define use_thread (*puse_thread)
#define iphostport (*piphostport)
#define hostport (*phostport)
#define multicastport (*pmulticastport)
#ifdef _WIN32
#define ipx_hostport (*pipx_hostport)
#define ipx_clientport (*pipx_clientport)
#endif // _WIN32
#define fakelag (*pfakelag)
#define fakeloss (*pfakeloss)

#define net_graph (*pnet_graph)
#define net_graphwidth (*pnet_graphwidth)
#define net_scale (*pnet_scale)
#define net_graphpos (*pnet_graphpos)
#define net_message_buffer (*pnet_message_buffer)
#define in_message_buf (*pin_message_buf)

#define in_message (*pin_message)
#define in_from (*pin_from)
#define ip_sockets (*pip_sockets)
#ifdef _WIN32
#define ipx_sockets (*pipx_sockets)
#endif // _WIN32
#define gNetSplit (*pgNetSplit)
#define messages (*pmessages)
#define normalqueue (*pnormalqueue)
#endif // HOOK_ENGINE

extern cvar_t net_address;
extern cvar_t ipname;
extern cvar_t defport;
extern cvar_t ip_clientport;
extern cvar_t clientport;
extern int net_sleepforever;
extern loopback_t loopbacks[2];
extern packetlag_t g_pLagData[3];
extern float gFakeLag;

#ifdef _WIN32
extern netadr_t net_local_ipx_adr;
#endif // _WIN32
extern netadr_t net_local_adr;
extern netadr_t net_from;
extern qboolean noip;
#ifdef _WIN32
extern qboolean noipx;
#endif // _WIN32
extern sizebuf_t net_message;
extern cvar_t clockwindow;
extern cvar_t iphostport;
extern cvar_t hostport;
#ifdef _WIN32
extern cvar_t ipx_hostport;
extern cvar_t ipx_clientport;
#endif // _WIN32
extern cvar_t multicastport;
extern cvar_t fakelag;
extern cvar_t fakeloss;
extern cvar_t net_graph;
extern cvar_t net_graphwidth;
extern cvar_t net_scale;
extern cvar_t net_graphpos;
extern unsigned char net_message_buffer[NET_MAX_PAYLOAD];
extern unsigned char in_message_buf[NET_MAX_PAYLOAD];
extern sizebuf_t in_message;
extern netadr_t in_from;
extern int ip_sockets[3];
#ifdef _WIN32
extern int ipx_sockets[3];
#endif // _WIN32
extern LONGPACKET gNetSplit;
extern net_messages_t *messages[3];
extern net_messages_t *normalqueue;

unsigned short Q_ntohs(unsigned short netshort);

void NetadrToSockadr(const netadr_t *a, struct sockaddr *s);
void SockadrToNetadr(const struct sockaddr *s, netadr_t *a);

NOXREF unsigned short NET_HostToNetShort(unsigned short us_in);

qboolean NET_CompareAdr(netadr_t &a, netadr_t &b);
qboolean NET_CompareClassBAdr(netadr_t &a, netadr_t &b);

qboolean NET_IsReservedAdr(netadr_t &a);

qboolean NET_CompareBaseAdr(const netadr_t &a, const netadr_t &b);

char *NET_AdrToString(const netadr_t &a);
char *NET_BaseAdrToString(netadr_t &a);

qboolean NET_StringToSockaddr(const char *s, struct sockaddr *sadr);
qboolean NET_StringToAdr(const char *s, netadr_t *a);

qboolean NET_IsLocalAddress(netadr_t &adr);

char *NET_ErrorString(int code);

void NET_TransferRawData(sizebuf_t *msg, unsigned char *pStart, int nSize);
qboolean NET_GetLoopPacket(netsrc_t sock, netadr_t *in_from_, sizebuf_t *msg);
void NET_SendLoopPacket(netsrc_t sock, int length, void *data, const netadr_t &to);
void NET_RemoveFromPacketList(packetlag_t *pPacket);
NOXREF int NET_CountLaggedList(packetlag_t *pList);
void NET_ClearLaggedList(packetlag_t *pList);
void NET_AddToLagged(netsrc_t sock, packetlag_t *pList, packetlag_t *pPacket, netadr_t *net_from_, sizebuf_t messagedata, float timestamp);
void NET_AdjustLag();
qboolean NET_LagPacket(qboolean newdata, netsrc_t sock, netadr_t *from, sizebuf_t *data);
void NET_FlushSocket(netsrc_t sock);
qboolean NET_GetLong(unsigned char *pData, int size, int *outSize);
qboolean NET_QueuePacket(netsrc_t sock);

int NET_Sleep();

void *net_malloc(size_t size);

net_messages_t *NET_AllocMsg(int size);
void NET_FreeMsg(net_messages_t *pmsg);

qboolean NET_GetPacket(netsrc_t sock);

int NET_SendLong(netsrc_t sock, int s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);

void NET_SendPacket_api(unsigned int length, void *data, const netadr_t &to);
void NET_SendPacket(netsrc_t sock, int length, void *data, const netadr_t &to);

int NET_IPSocket(char *net_interface, int port, qboolean multicast);


int NET_IPXSocket(int hostshort);

void MaxPlayers_f();

void NET_ClearLagData(qboolean bClient, qboolean bServer);

qboolean NET_JoinGroup(netsrc_t sock, netadr_t &addr);
qboolean NET_LeaveGroup(netsrc_t sock, netadr_t &addr);