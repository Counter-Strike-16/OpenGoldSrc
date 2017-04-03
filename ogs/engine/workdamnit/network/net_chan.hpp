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
/// @brief network connection channel

#pragma once

#include "common/commontypes.h"
#include "common/enums.h"
#include "common/maintypes.h"
#include "network/net.hpp"

#ifdef HOOK_ENGINE

#define gDownloadFile (*pgDownloadFile)

#define net_drop (*pnet_drop)
#define net_log (*pnet_log)
#define net_showpackets (*pnet_showpackets)
#define net_showdrop (*pnet_showdrop)
#define net_drawslider (*pnet_drawslider)
#define net_chokeloopback (*pnet_chokeloopback)
#define sv_filetransfercompression (*psv_filetransfercompression)
#define sv_filetransfermaxsize (*psv_filetransfermaxsize)

#endif // HOOK_ENGINE

typedef struct cvar_s cvar_t;
typedef struct fragbuf_s fragbuf_t;
typedef struct sizebuf_s sizebuf_t;
typedef struct fragbuf_s fragbuf_t;
typedef struct fragbufwaiting_s fragbufwaiting_t;
typedef struct netadr_s netadr_t;

extern char gDownloadFile[256];

// Packets dropped before this one
extern int net_drop;

extern cvar_t net_log;
extern cvar_t net_showpackets;
extern cvar_t net_showdrop;
extern cvar_t net_drawslider;
extern cvar_t net_chokeloopback;
extern cvar_t sv_filetransfercompression;
extern cvar_t sv_filetransfermaxsize;

// Network Connection Channel
typedef struct netchan_s
{
	// NS_SERVER or NS_CLIENT, depending on channel.
	netsrc_t sock;

	// Address this channel is talking to.
	netadr_t remote_address;

	int player_slot;
	// For timeouts.  Time last message was received.
	float last_received;
	// Time when channel was connected.
	float connect_time;

	// Bandwidth choke
	// Bytes per second
	double rate;
	// If realtime > cleartime, free to send next packet
	double cleartime;

	// Sequencing variables
	//
	// Increasing count of sequence numbers
	int incoming_sequence;
	// # of last outgoing message that has been ack'd.
	int incoming_acknowledged;
	// Toggles T/F as reliable messages are received.
	int incoming_reliable_acknowledged;
	// single bit, maintained local
	int incoming_reliable_sequence;
	// Message we are sending to remote
	int outgoing_sequence;
	// Whether the message contains reliable payload, single bit
	int reliable_sequence;
	// Outgoing sequence number of last send that had reliable data
	int last_reliable_sequence;

	void *connection_status;
	int (*pfnNetchan_Blocksize)(void *);

	// Staging and holding areas
	sizebuf_t message;
	byte message_buf[MAX_MSGLEN];

	// Reliable message buffer. We keep adding to it until reliable is
	// acknowledged. Then we clear it.
	int reliable_length;
	byte reliable_buf[MAX_MSGLEN];

	// Waiting list of buffered fragments to go onto queue. Multiple outgoing
	// buffers can be queued in succession.
	fragbufwaiting_t *waitlist[MAX_STREAMS];

	// Is reliable waiting buf a fragment?
	int reliable_fragment[MAX_STREAMS];
	// Buffer id for each waiting fragment
	unsigned int reliable_fragid[MAX_STREAMS];

	// The current fragment being set
	fragbuf_t *fragbufs[MAX_STREAMS];
	// The total number of fragments in this stream
	int fragbufcount[MAX_STREAMS];

	// Position in outgoing buffer where frag data starts
	short int frag_startpos[MAX_STREAMS];
	// Length of frag data in the buffer
	short int frag_length[MAX_STREAMS];

	// Incoming fragments are stored here
	fragbuf_t *incomingbufs[MAX_STREAMS];
	// Set to true when incoming data is ready
	qboolean incomingready[MAX_STREAMS];

	// Only referenced by the FRAG_FILE_STREAM component
	// Name of file being downloaded
	char incomingfilename[MAX_PATH];

	void *tempbuffer;
	int tempbuffersize;

	// Incoming and outgoing flow metrics
	flow_t flow[MAX_FLOWS];
} netchan_t;

void Netchan_UnlinkFragment(fragbuf_t *buf, fragbuf_t **list);

void Netchan_OutOfBand(netsrc_t sock, netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint(netsrc_t sock, netadr_t adr, char *format, ...);

void Netchan_ClearFragbufs(fragbuf_t **ppbuf);
void Netchan_ClearFragments(netchan_t *chan);
void Netchan_Clear(netchan_t *chan);

void Netchan_Setup(netsrc_t socketnumber, netchan_t *chan, netadr_t adr, int player_slot, void *connection_status, qboolean (*pfnNetchan_Blocksize)(void *));

qboolean Netchan_CanPacket(netchan_t *chan);

void Netchan_UpdateFlow(netchan_t *chan);

void Netchan_Transmit(netchan_t *chan, int length, byte *data);

fragbuf_t *Netchan_FindBufferById(fragbuf_t **pplist, int id, qboolean allocate);
void Netchan_CheckForCompletion(netchan_t *chan, int stream, int intotalbuffers);
qboolean Netchan_Validate(netchan_t *chan, qboolean *frag_message, unsigned int *fragid, int *frag_offset, int *frag_length);
qboolean Netchan_Process(netchan_t *chan);
void Netchan_FragSend(netchan_t *chan);
void Netchan_AddBufferToList(fragbuf_t **pplist, fragbuf_t *pbuf);

fragbuf_t *Netchan_AllocFragbuf(void);
void Netchan_AddFragbufToTail(fragbufwaiting_t *wait, fragbuf_t *buf);

void Netchan_CreateFragments_(qboolean server, netchan_t *chan, sizebuf_t *msg);
void Netchan_CreateFragments(qboolean server, netchan_t *chan, sizebuf_t *msg);

void Netchan_CreateFileFragmentsFromBuffer(qboolean server, netchan_t *chan, const char *filename, unsigned char *uncompressed_pbuf, int uncompressed_size);
int Netchan_CreateFileFragments(qboolean server, netchan_t *chan, const char *filename);

#ifdef REHLDS_FIXES
int Netchan_CreateFileFragments_(qboolean server, netchan_t *chan, const char *filename);
#endif // REHLDS_FIXES

void Netchan_FlushIncoming(netchan_t *chan, int stream);

qboolean Netchan_CopyNormalFragments(netchan_t *chan);
qboolean Netchan_CopyFileFragments(netchan_t *chan);

NOXREF qboolean Netchan_IsSending(netchan_t *chan);
NOXREF qboolean Netchan_IsReceiving(netchan_t *chan);

qboolean Netchan_IncomingReady(netchan_t *chan);

NOXREF void Netchan_UpdateProgress(netchan_t *chan);

void Netchan_Init(void);

NOXREF qboolean Netchan_CompressPacket(sizebuf_t *chan);
NOXREF qboolean Netchan_DecompressPacket(sizebuf_t *chan);

class CNetChan
{
public:
	CNetChan(netchan_t *apData);
private:
	netchan_t *mpData{nullptr};
};