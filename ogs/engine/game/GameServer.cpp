/*
Copyright (C) 1996-1997 Id Software, Inc.

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
	int		i;
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

	for (i=0 ; i<MAX_MODELS ; i++)
		sprintf (localmodels[i], "*%i", i);
};

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
SV_ClientPrintf

Sends text across to be displayed 
FIXME: make this just a stuffed echo?
=================
*/
void CGameServer::ClientPrintf(CGameClient *apClient, /*int level,*/ char *fmt, ...)
{
	va_list		argptr;
	char		string[1024];
	
	va_start (argptr,fmt);
	vsprintf (string, fmt,argptr);
	va_end (argptr);
	
	MSG_WriteByte (&host_client->message, svc_print);
	MSG_WriteString (&host_client->message, string);
}

/*
=================
SV_BroadcastPrintf

Sends text to all active clients
=================
*/
void CGameServer::BroadcastPrintf(/*int level,*/ char *fmt, ...)
{
	va_list		argptr;
	char		string[1024];
	int			i;
	
	va_start (argptr,fmt);
	vsprintf (string, fmt,argptr);
	va_end (argptr);
	
	for (i=0 ; i<svs.maxclients ; i++)
		if (svs.clients[i].active && svs.clients[i].spawned)
		{
			MSG_WriteByte (&svs.clients[i].message, svc_print);
			MSG_WriteString (&svs.clients[i].message, string);
		}
}

/*
=====================
SV_DropClient

Called when the player is getting totally kicked off the host
if (crash = true), don't bother sending signofs
=====================
*/
void CGameServer::DropClient(CGameClient *apClient, bool crash)
{
	int		saveSelf;
	int		i;
	client_t *client;

	if (!crash)
	{
		// send any final messages (don't check for errors)
		if (NET_CanSendMessage (host_client->netconnection))
		{
			MSG_WriteByte (&host_client->message, svc_disconnect);
			NET_SendMessage (host_client->netconnection, &host_client->message);
		}
	
		if (host_client->edict && host_client->spawned)
		{
		// call the prog function for removing a client
		// this will set the body to a dead frame, among other things
			saveSelf = pr_global_struct->self;
			pr_global_struct->self = EDICT_TO_PROG(host_client->edict);
			PR_ExecuteProgram (pr_global_struct->ClientDisconnect);
			pr_global_struct->self = saveSelf;
		}

		Sys_Printf ("Client %s removed\n",host_client->name);
	}

// break the net connection
	NET_Close (host_client->netconnection);
	host_client->netconnection = NULL;

// free the client (the body stays around)
	host_client->active = false;
	host_client->name[0] = 0;
	host_client->old_frags = -999999;
	net_activeconnections--;

// send notification to all clients
	for (i=0, client = svs.clients ; i<svs.maxclients ; i++, client++)
	{
		if (!client->active)
			continue;
		MSG_WriteByte (&client->message, svc_updatename);
		MSG_WriteByte (&client->message, host_client - svs.clients);
		MSG_WriteString (&client->message, "");
		MSG_WriteByte (&client->message, svc_updatefrags);
		MSG_WriteByte (&client->message, host_client - svs.clients);
		MSG_WriteShort (&client->message, 0);
		MSG_WriteByte (&client->message, svc_updatecolors);
		MSG_WriteByte (&client->message, host_client - svs.clients);
		MSG_WriteByte (&client->message, 0);
	}
}

/*
================
SV_SendReconnect

Tell all the clients that the server is changing levels
================
*/
void CGameServer::ReconnectAll()
{
	char	data[128];
	sizebuf_t	msg;

	msg.data = data;
	msg.cursize = 0;
	msg.maxsize = sizeof(data);

	MSG_WriteChar (&msg, svc_stufftext);
	MSG_WriteString (&msg, "reconnect\n");
	NET_SendToAll (&msg, 5);
	
	if (cls.state != ca_dedicated)
#ifdef QUAKE2
		Cbuf_InsertText ("reconnect\n");
#else
		Cmd_ExecuteString ("reconnect\n", src_command);
#endif
}