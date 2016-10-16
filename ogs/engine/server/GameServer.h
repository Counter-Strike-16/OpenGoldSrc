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

// GameServer.h - Game server

//=============================================================================

#pragma once

class CGameClient;

enum server_state_t
{
	ss_dead = 0,	// no map loaded
	ss_loading,		// spawning level edicts
	ss_active		// actively running
};

// MAX_CHALLENGES is made large to prevent a denial
// of service attack that could cycle all of them
// out before legitimate users connected
const int MAX_CHALLENGES = 1024;

struct challenge_t
{
	netadr_t adr;
	int challenge;
	int time;
};

struct svstats_t
{
	double	active;
	double	idle;
	int		count;
	int		packets;

	double	latched_active;
	double	latched_idle;
	int		latched_packets;
};

class CGameServer
{
public:
	CGameServer();
	~CGameServer();
	
	void Init();
	void Shutdown();
	
	void Frame(float time); // int msec
	
	void ConnectionlessPacket();
	
	void CheckForNewClients();
	
	void ReadPackets();
	void SendClientMessages(); // SendPackets
	void CheckTimeouts();
	
	void RejectConnection(netadr_t *adr, char *fmt, ...);
	void RejectConnectionForPassword(netadr_t *adr);
	
	void ClientPrintf(CGameClient *apClient, /*int level,*/ char *fmt, ...);
	void BroadcastPrintf(/*int level,*/ const char *fmt, ...);
	void BroadcastCommand(char *fmt, ...);
	
	void KickPlayer(int nPlayerSlot, int nReason);
	// GetClient->Kick(reason);
	
	void InactivateClients();
	
	void BuildReconnect(sizebuf_t *msg);
	void ReconnectAllClients();
	
	void ClearDatagram();
	
	CGameClient *GetClientByName(const char *name);
	CGameClient *GetClientByIndex(int id);
private:
	server_state_t state; // some actions are only valid during load
	//tGameServerStateVec mvStates; // mvStates[state]->Frame();
	
	int spawncount; // number of servers spawned since start, used to check late spawns
	
	// added to every client's unreliable buffer each frame, then cleared
	sizebuf_t	datagram;
	byte		datagram_buf[MAX_DATAGRAM];

	// added to every client's reliable buffer each frame, then cleared
	sizebuf_t	reliable_datagram;
	byte		reliable_datagram_buf[MAX_MSGLEN];

	// the multicast buffer is used to send a message to a set of clients
	sizebuf_t	multicast;
	byte		multicast_buf[MAX_MSGLEN];

	// the master buffer is used for building log packets
	sizebuf_t	master;
	byte		master_buf[MAX_DATAGRAM];
	
	sizebuf_t	signon;
	byte		signon_buf[8192];
	
	double		time;
	
	int			lastcheck;			// used by PF_checkclient
	double		lastchecktime; // for monster ai
	
	svstats_t stats;
	
	int maxclients;
	int maxclientslimit;
	
	double last_heartbeat;
	int heartbeat_sequence;
	
	struct model_s	*models[MAX_MODELS];
	
	char *model_precache[MAX_MODELS];	// NULL terminated
	char *sound_precache[MAX_SOUNDS];	// NULL terminated
	
	GameClient *clients; // [maxclients->value]
	
	char info[MAX_SERVERINFO_STRING];
	
	int flags; // episode completion information
	
	bool changelevel_issued; // cleared when at SV_SpawnServer
	
	challenge_t	challenges[MAX_CHALLENGES];	// to prevent invalid IPs from connecting
};