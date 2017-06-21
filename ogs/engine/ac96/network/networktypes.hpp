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
/// @brief network module types

#pragma once

enum
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING,

	MAX_FLOWS
};

// 0 == regular, 1 == file stream
enum
{
	FRAG_NORMAL_STREAM = 0,
	FRAG_FILE_STREAM,
	
	MAX_STREAMS
};

// MAX_CHALLENGES is made large to prevent a denial
//  of service attack that could cycle all of them
//  out before legitimate users connected
#ifdef REHLDS_OPT_PEDANTIC
const int MAX_CHALLENGES = 64;
#else
const int MAX_CHALLENGES = 1024;
#endif // REHLDS_OPT_PEDANTIC

// Flow control bytes per second limits
const float MAX_RATE = 100000.0f;
const float MIN_RATE = 1000.0f;

// Default data rate
const float DEFAULT_RATE = 9999.0f;

// NETWORKING INFO

// Max size of udp packet payload
const int MAX_UDP_PACKET = 4010; // 9 bytes SPLITHEADER + 4000 payload?

// Max length of a reliable message
const int MAX_MSGLEN = 3990; // 10 reserved for fragheader?

// Max length of unreliable message
const int MAX_DATAGRAM = 4000;

// This is the packet payload without any header bytes (which are attached for
// actual sending)
const int NET_MAX_PAYLOAD = 65536;

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
const int HEADER_BYTES = (8 + MAX_STREAMS * 9);

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
const int NET_MAX_MESSAGE = 4037;

const int MAX_LATENT = 32;

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
	const int MAX_NORMAL_FRAGMENTS = (NET_MAX_PAYLOAD / CLIENT_FRAGMENT_SIZE_ONCONNECT);

	// While client is connecting it sending fragments with minimal size, also it
	// transfers sprays with minimal fragments...
	// But with sv_delayed_spray_upload it sends with cl_dlmax fragment size
	const int MAX_FILE_FRAGMENTS = (CUSTOMIZATION_MAX_SIZE / FRAGMENT_C2S_MIN_SIZE);
#endif

const int UDP_HEADER_SIZE = 28;
const int MAX_RELIABLE_PAYLOAD = 1200;

#define MAKE_FRAGID(id, count) (((id & 0xffff) << 16) | (count & 0xffff))
#define FRAG_GETID(fragid) ((fragid >> 16) & 0xffff)
#define FRAG_GETCOUNT(fragid) (fragid & 0xffff)

#ifdef REHLDS_FIXES
#define Con_NetPrintf Con_DPrintf
#else // REHLDS_FIXES
#define Con_NetPrintf Con_Printf
#endif // REHLDS_FIXES