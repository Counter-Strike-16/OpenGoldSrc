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

#include "common/enums.h"
#include "common/netadr.h"
#include "maintypes.h"
#include "rehlds/common_rehlds.h"

// MAX_CHALLENGES is made large to prevent a denial
//  of service attack that could cycle all of them
//  out before legitimate users connected
#ifdef REHLDS_OPT_PEDANTIC
#define MAX_CHALLENGES 64
#else
#define MAX_CHALLENGES 1024
#endif // REHLDS_OPT_PEDANTIC

// 0 == regular, 1 == file stream
#define MAX_STREAMS 2

// Flow control bytes per second limits
#define MAX_RATE 100000.0f
#define MIN_RATE 1000.0f

// Default data rate
#define DEFAULT_RATE (9999.0f)

// NETWORKING INFO

// Max size of udp packet payload
#define MAX_UDP_PACKET 4010 // 9 bytes SPLITHEADER + 4000 payload?

// Max length of a reliable message
#define MAX_MSGLEN 3990 // 10 reserved for fragheader?

// Max length of unreliable message
#define MAX_DATAGRAM 4000

// This is the packet payload without any header bytes (which are attached for
// actual sending)
#define NET_MAX_PAYLOAD 65536

// This is the payload plus any header info (excluding UDP header)

// Packet header is:
//  4 bytes of outgoing seq
//  4 bytes of incoming seq
//  and for each stream
// {
//  byte (on/off)
//  int (fragment id)
//  short (startpos)
//  short (length)
// }
#define HEADER_BYTES (8 + MAX_STREAMS * 9)

// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER(number, boundary) \
	(((number) + ((boundary)-1)) / (boundary)) * (boundary)

// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before
// processing the header
//  bytes will be stripped by the networking channel layer
//#define NET_MAX_MESSAGE PAD_NUMBER( ( MAX_MSGLEN + HEADER_BYTES ), 16 )
// This is currently used value in the engine. TODO: define above gives 4016,
// check it why.
#define NET_MAX_MESSAGE 4037

enum
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING,

	MAX_FLOWS
};

// Message data
typedef struct flowstats_s
{
	// Size of message sent/received
	int size;
	// Time that message was sent/received
	double time;
} flowstats_t;

const int MAX_LATENT = 32;

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

const int FRAGMENT_C2S_MIN_SIZE = 16;
const int FRAGMENT_S2C_MIN_SIZE = 256;
const int FRAGMENT_S2C_MAX_SIZE = 1024;
const int CLIENT_FRAGMENT_SIZE_ONCONNECT = 128;
const int CUSTOMIZATION_MAX_SIZE = 20480;

#ifndef REHLDS_FIXES
// Size of fragmentation buffer internal buffers
const int FRAGMENT_MAX_SIZE = 1400;

const int MAX_FRAGMENTS = 25000;
#else
const int FRAGMENT_MAX_SIZE = 1024;

// Client sends normal fragments only while connecting
#define MAX_NORMAL_FRAGMENTS (NET_MAX_PAYLOAD / CLIENT_FRAGMENT_SIZE_ONCONNECT)

// While client is connecting it sending fragments with minimal size, also it
// transfers sprays with minimal fragments...
// But with sv_delayed_spray_upload it sends with cl_dlmax fragment size
#define MAX_FILE_FRAGMENTS (CUSTOMIZATION_MAX_SIZE / FRAGMENT_C2S_MIN_SIZE)
#endif

#define UDP_HEADER_SIZE 28
#define MAX_RELIABLE_PAYLOAD 1200

#define FRAG_NORMAL_STREAM 0
#define FRAG_FILE_STREAM 1

#define MAKE_FRAGID(id, count) (((id & 0xffff) << 16) | (count & 0xffff))
#define FRAG_GETID(fragid) ((fragid >> 16) & 0xffff)
#define FRAG_GETCOUNT(fragid) (fragid & 0xffff)

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

#ifdef REHLDS_FIXES
#define Con_NetPrintf Con_DPrintf
#else // REHLDS_FIXES
#define Con_NetPrintf Con_Printf
#endif // REHLDS_FIXES