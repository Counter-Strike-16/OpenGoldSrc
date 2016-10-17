/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// GameServer.cpp -- server main program

#include "game/GameServer.h"
#include "game/GameClient.h"

CGameServer gsv;
CGameServer *sv = &gsv; // temp (sv->HelloWorld)

//============================================================================

/*
===============
SV_Init
===============
*/
void CGameServer::Init()
{
	InitOperatorCommands();
	
	// physics cvars
	extern	cvar_t	sv_maxvelocity;
	extern	cvar_t	sv_gravity;
	extern	cvar_t	sv_nostep;
	extern	cvar_t	sv_friction;
	extern	cvar_t	sv_edgefriction;
	extern	cvar_t	sv_stopspeed;
	extern	cvar_t	sv_maxspeed;
	extern	cvar_t	sv_accelerate;
	extern	cvar_t	sv_idealpitchscale;
	extern	cvar_t	sv_aim;

	Cvar_RegisterVariable (&sv_maxvelocity);
	Cvar_RegisterVariable (&sv_gravity);
	Cvar_RegisterVariable (&sv_friction);
	Cvar_RegisterVariable (&sv_edgefriction);
	Cvar_RegisterVariable (&sv_stopspeed);
	Cvar_RegisterVariable (&sv_maxspeed);
	Cvar_RegisterVariable (&sv_accelerate);
	Cvar_RegisterVariable (&sv_idealpitchscale);
	Cvar_RegisterVariable (&sv_aim);
	Cvar_RegisterVariable (&sv_nostep);

	for(int i = 0; i < MAX_MODELS; i++)
		sprintf (localmodels[i], "*%i", i);
	
	Con_DPrintf("Server module initialized.\n");
};

/*
================
SV_Shutdown

Called when each game quits,
before Sys_Quit or Sys_Error
================
*/
void CGameServer::Shutdown(char *finalmsg, bool reconnect)
{
	Con_DPrintf("Server module shutdown.\n");
	
	if(svs.clients)
		SV_FinalMessage (finalmsg, reconnect);

	Master_Shutdown ();
	SV_ShutdownGameProgs ();

	// free current level
	if (sv.demofile)
		fclose (sv.demofile);
	memset (&sv, 0, sizeof(sv));
	Com_SetServerState (sv.state);

	// free server static data
	if (svs.clients)
		Z_Free (svs.clients);
	if (svs.client_entities)
		Z_Free (svs.client_entities);
	if (svs.demofile)
		fclose (svs.demofile);
	memset (&svs, 0, sizeof(svs));
};

/*
=================
SV_ConnectionlessPacket

A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
=================
*/
void CGameServer::ConnectionlessPacket()
{
	char	*s;
	char	*c;

	MSG_BeginReading (&net_message);
	MSG_ReadLong (&net_message);		// skip the -1 marker

	s = MSG_ReadStringLine (&net_message);

	Cmd_TokenizeString (s, false);

	c = Cmd_Argv(0);
	Com_DPrintf ("Packet %s : %s\n", NET_AdrToString(net_from), c);

	if (!strcmp(c, "ping"))
		SVC_Ping ();
	else if (!strcmp(c, "ack"))
		SVC_Ack ();
	else if (!strcmp(c,"status"))
		SVC_Status ();
	else if (!strcmp(c,"info"))
		SVC_Info ();
	else if (!strcmp(c,"getchallenge"))
		SVC_GetChallenge ();
	else if (!strcmp(c,"connect"))
		SVC_DirectConnect ();
	else if (!strcmp(c, "rcon"))
		SVC_RemoteCommand ();
	else
		Com_Printf ("bad connectionless packet from %s:\n%s\n"
		, NET_AdrToString (net_from), s);
};

/*
===================
SV_CalcPings

Updates the cl->ping variables
===================
*/
void CGameServer::CalcPings()
{
	int			i, j;
	client_t	*cl;
	int			total, count;

	for (i=0 ; i<maxclients->value ; i++)
	{
		cl = &svs.clients[i];
		if (cl->state != cs_spawned )
			continue;

#if 0
		if (cl->lastframe > 0)
			cl->frame_latency[sv.framenum&(LATENCY_COUNTS-1)] = sv.framenum - cl->lastframe + 1;
		else
			cl->frame_latency[sv.framenum&(LATENCY_COUNTS-1)] = 0;
#endif

		total = 0;
		count = 0;
		for (j=0 ; j<LATENCY_COUNTS ; j++)
		{
			if (cl->frame_latency[j] > 0)
			{
				count++;
				total += cl->frame_latency[j];
			}
		}
		if (!count)
			cl->ping = 0;
		else
#if 0
			cl->ping = total*100/count - 100;
#else
			cl->ping = total / count;
#endif

		// let the game dll know about the ping
		cl->edict->client->ping = cl->ping;
	}
}

/*
==================
SV_CheckTimeouts

If a packet has not been received from a client for timeout->value
seconds, drop the conneciton.  Server frames are used instead of
realtime to avoid dropping the local client while debugging.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void CGameServer::CheckTimeouts()
{
	int		i;
	client_t	*cl;
	int			droppoint;
	int			zombiepoint;

	droppoint = svs.realtime - 1000*timeout->value;
	zombiepoint = svs.realtime - 1000*zombietime->value;

	for (i=0,cl=svs.clients ; i<maxclients->value ; i++,cl++)
	{
		// message times may be wrong across a changelevel
		if (cl->lastmessage > svs.realtime)
			cl->lastmessage = svs.realtime;

		if (cl->state == cs_zombie && cl->lastmessage < zombiepoint)
		{
			cl->state = cs_free;	// can now be reused
			continue;
		};
		
		if ( (cl->state == cs_connected || cl->state == cs_spawned) 
			&& cl->lastmessage < droppoint)
		{
			BroadcastPrintf (PRINT_HIGH, "%s timed out\n", cl->name);
			cl->Drop(); 
			cl->state = cs_free;	// don't bother with zombie state
		}
	}
}

/*
===================
SV_CheckForNewClients

===================
*/
void CGameServer::CheckForNewClients(void)
{
	struct qsocket_s	*ret;
	int				i;
		
//
// check for new connections
//
	while (1)
	{
		ret = NET_CheckNewConnections ();
		if (!ret)
			break;

	// 
	// init a new client structure
	//	
		for (i=0 ; i<svs.maxclients ; i++)
			if (!svs.clients[i].active)
				break;
		if (i == svs.maxclients)
			Sys_Error ("Host_CheckForNewClients: no free clients");
		
		svs.clients[i].netconnection = ret;
		SV_ConnectClient (i);	
	
		net_activeconnections++;
	}
};

/*
=================
SV_ReadPackets
=================
*/
void CGameServer::ReadPackets()
{
	int			i;
	client_t	*cl;
	int			qport;

	while (NET_GetPacket (NS_SERVER, &net_from, &net_message))
	{
		// filter packet here
		
		// check for connectionless packet (0xffffffff) first
		if(*(int *)net_message.data == -1)
		{
			HandleConnectionlessPacket();
			continue;
		};

		// read the qport out of the message so we can fix up
		// stupid address translating routers
		MSG_BeginReading (&net_message);
		MSG_ReadLong (&net_message);		// sequence number
		MSG_ReadLong (&net_message);		// sequence number
		qport = MSG_ReadShort (&net_message) & 0xffff;

		// check for packets from connected clients
		for (i=0, cl=svs.clients ; i<maxclients->value ; i++,cl++)
		{
			if (cl->state == cs_free)
				continue;
			if (!NET_CompareBaseAdr (net_from, cl->netchan.remote_address))
				continue;
			if (cl->netchan.qport != qport)
				continue;
			if (cl->netchan.remote_address.port != net_from.port)
			{
				Com_Printf ("SV_ReadPackets: fixing up a translated port\n");
				cl->netchan.remote_address.port = net_from.port;
			}

			if (Netchan_Process(&cl->netchan, &net_message))
			{	// this is a valid, sequenced packet, so process it
				if (cl->state != cs_zombie)
				{
					cl->lastmessage = svs.realtime;	// don't timeout
					SV_ExecuteClientMessage (cl);
				}
			}
			break;
		}
		
		if (i != maxclients->value)
			continue;
	}
}

/*
=======================
SV_SendClientMessages
=======================
*/
void CGameServer::SendClientMessages(void)
{
	int			i;
	
// update frags, names, etc
	SV_UpdateToReliableMessages ();

// build individual updates
	for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
	{
		if (!host_client->active)
			continue;

		if (host_client->spawned)
		{
			if (!SV_SendClientDatagram (host_client))
				continue;
		}
		else
		{
		// the player isn't totally in the game yet
		// send small keepalive messages if too much time has passed
		// send a full message when the next signon stage has been requested
		// some other message data (name changes, etc) may accumulate 
		// between signon stages
			if (!host_client->sendsignon)
			{
				if (realtime - host_client->last_message > 5)
					SV_SendNop (host_client);
				continue;	// don't send out non-signon messages
			}
		}

		// check for an overflowed message.  Should only happen
		// on a very fucked up connection that backs up a lot, then
		// changes level
		if (host_client->message.overflowed)
		{
			SV_DropClient (true);
			host_client->message.overflowed = false;
			continue;
		}
			
		if (host_client->message.cursize || host_client->dropasap)
		{
			if (!NET_CanSendMessage (host_client->netconnection))
			{
//				I_Printf ("can't write\n");
				continue;
			}

			if (host_client->dropasap)
				SV_DropClient (false);	// went to another level
			else
			{
				if (NET_SendMessage (host_client->netconnection
				, &host_client->message) == -1)
					SV_DropClient (true);	// if the message couldn't send, kick off
				SZ_Clear (&host_client->message);
				host_client->last_message = realtime;
				host_client->sendsignon = false;
			}
		}
	}
	
	
// clear muzzle flashes
	SV_CleanupEnts ();
}

/*
=================
SV_BroadcastPrintf

Sends text to all active clients
=================
*/
void CGameServer::BroadcastPrintf(/*int level,*/ char *fmt, ...)
{
	va_list argptr;
	char string[1024]; // 2048
	
	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);
	
	for(int i = 0; i < svs.maxclients; i++)
	{
		if(svs.clients[i].active && svs.clients[i].spawned)
		{
			MSG_WriteByte(&svs.clients[i].message, svc_print);
			MSG_WriteString(&svs.clients[i].message, string);
		};
	};
};

/*
=================
SV_BroadcastCommand

Sends text to all active clients
=================
*/
void CGameServer::BroadcastCommand(char *fmt, ...)
{
	if(!sv.state)
		return;
	
	va_list		argptr;
	char		string[1024];
	
	va_start (argptr,fmt);
	vsprintf (string, fmt,argptr);
	va_end (argptr);

	MSG_WriteByte (&sv.multicast, svc_stufftext);
	MSG_WriteString (&sv.multicast, string);
	SV_Multicast (NULL, MULTICAST_ALL_R);
};

/*
================
SV_SendReconnect

Tell all the clients that the server is changing levels
================
*/
void CGameServer::ReconnectAll()
{
	//
	char	data[128];
	sizebuf_t	msg;

	msg.data = data;
	msg.cursize = 0;
	msg.maxsize = sizeof(data);

	MSG_WriteChar (&msg, svc_stufftext);
	MSG_WriteString (&msg, "reconnect\n");
	NET_SendToAll (&msg, 5);
	//
//	BroadcastCommand("reconnect"); // voila
	//
	
	if (cls.state != ca_dedicated)
#ifdef QUAKE2
		Cbuf_InsertText ("reconnect\n");
#else
		Cmd_ExecuteString ("reconnect\n", src_command);
#endif
}