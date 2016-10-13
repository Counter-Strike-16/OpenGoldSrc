/*
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

// Network.h -- interface to the networking layer

#pragma once

class qsocket_t;

class CNetwork
{
public:
	CNetwork();
	~CNetwork();
	
	void Init();
	void Shutdown();
	
	// Returns a new connection number if there is one pending, else -1
	qsocket_t *CheckNewConnections();
	
	// Called by client to connect to a host. Returns -1 if not able to
	qsocket_t *ConnectTo(char *host);
	
	// This is a reliable *blocking* send to all attached clients
	int SendToAll(sizebuf_t *data, int blocktime);
	
	// If a dead connection is returned by a get or send function, 
	// this function should be called when it is convenient
	void CloseSocket(qsocket_t *sock);
	
	// Server calls when a client is kicked off for a game related misbehavior
	// like an illegal protocal conversation.  Client calls when disconnecting
	// from a server.
	// A netcon_t number will not be reused until this function is called for it
	
	void Poll();
};