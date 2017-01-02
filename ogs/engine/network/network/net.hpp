/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

typedef struct
{
	byte           ip[4];
	unsigned short port;
	unsigned short pad;
} netadr_t;

extern netadr_t  net_local_adr;
extern netadr_t  net_from; // address of who sent the packet
extern sizebuf_t net_message;

extern cvar_t hostname;

extern int net_socket;

qboolean NET_GetPacket(void);
void NET_SendPacket(int length, void *data, netadr_t to);

//============================================================================

#define OLD_AVG 0.99 // total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

#define MAX_LATENT 32

typedef struct
{
	qboolean fatal_error;

	float last_received; // for timeouts

	// the statistics are cleared at each client begin, because
	// the server connecting process gives a bogus picture of the data
	float frame_latency; // rolling average
	float frame_rate;

	int drop_count; // dropped packets, cleared each level
	int good_count; // cleared each level

	int qport;

	// bandwidth estimator
	double cleartime; // if realtime > nc->cleartime, free to go
	double rate;      // seconds / byte

	// sequencing variables
	int incoming_sequence;
	int incoming_acknowledged;
	int incoming_reliable_acknowledged; // single bit

	int incoming_reliable_sequence; // single bit, maintained local

	int outgoing_sequence;
	int reliable_sequence;      // single bit
	int last_reliable_sequence; // sequence number of last send

	// reliable staging and holding areas
	sizebuf_t message; // writing buffer to send to server
	byte      message_buf[MAX_MSGLEN];

	int  reliable_length;
	byte reliable_buf[MAX_MSGLEN]; // unacked reliable message

	// time and size data to calculate bandwidth
	int    outgoing_size[MAX_LATENT];
	double outgoing_time[MAX_LATENT];
} netchan_t;

extern int net_drop; // packets dropped before this one

void Netchan_Transmit(netchan_t *chan, int length, byte *data);
void Netchan_OutOfBand(netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint(netadr_t adr, char *format, ...);
qboolean Netchan_Process(netchan_t *chan);
void Netchan_Setup(netchan_t *chan, netadr_t adr, int qport);

qboolean Netchan_CanReliable(netchan_t *chan);