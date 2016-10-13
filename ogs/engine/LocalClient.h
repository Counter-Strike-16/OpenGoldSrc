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

// LocalClient.h - Local client representation

#pragma once

class CLocalClient // i dunno how to call this class
{
public:
	CLocalClient();
	~CLocalClient();
	
	void Init();
	
	void EstablishConnection(char *host);
	void Disconnect();
	
	int GetMessage();
	void ParseServerMessage();
	
	int ReadFromServer();
	void WriteToServer(usercmd_t *cmd);
private:
//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	// connection information
	cactive_t state; // Current state type
	//std::vector<IClientState*> mvStates; // can be used to represent current client state as an object (ie curstate->update())

// personalization data sent to server	
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart a level

// demo loop control
	int			demonum;		// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];		// when not playing

// demo recording info must be here, because record is started before
// entering a map (and clearing client_state_t)
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;
	int			forcetrack;			// -1 = use normal cd track
	FILE		*demofile;
	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo

// connection information
	int			signon;			// 0 to SIGNONS
	struct qsocket_s	*netcon;
	sizebuf_t	message;		// writing buffer to send to server
	
	int challenge;
	
} client_static_t;
};