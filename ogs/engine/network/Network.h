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

// Network.h -- interface to the networking layer

#pragma once

const int PORT_ANY = -1;

class CNetChannel;

class CNetwork
{
public:
	CNetwork();
	~CNetwork();
	
	void Init(/*int port*/);
	void Shutdown();
	
	//void Frame();
	
	// INetServer *StartServer(int port);
	// INetClient *StartClient();
	
	void Sleep(int msec);
	
	void Config(bool multiplayer);
	
	bool GetPacket(netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message);
	void SendPacket(netsrc_t sock, int length, void *data, netadr_t to);
	
	// Returns a new connection number if there is one pending, else -1
	CNetChannel *CheckNewConnections();
	
	// Called by client to connect to a host. Returns -1 if not able to
	CNetChannel *ConnectTo(char *host);
	
	// This is a reliable *blocking* send to all attached clients
	int SendToAll(CNetBuffer *data, int blocktime);
	
	// If a dead connection is returned by a get or send function, 
	// this function should be called when it is convenient
	void CloseSocket(qsocket_t *sock); // problem?
	
	// Server calls when a client is kicked off for a game related misbehavior
	// like an illegal protocal conversation.  Client calls when disconnecting
	// from a server.
	// A netcon_t number will not be reused until this function is called for it
	
	void Poll();
	
	net_status_t *GetStatus(){return mpStatus;}
	
	qboolean CompareAdr(netadr_t a, netadr_t b);
	qboolean CompareBaseAdr(netadr_t a, netadr_t b);
	
	char *BaseAdrToString(netadr_t a);
	qboolean StringToAdr(char *s, netadr_t *a);
	qboolean IsClientLegal(netadr_t *adr);
private:
	net_status_t *mpStatus;
};