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

// EngClient.h - Local client representation, primary header for engine client

#pragma once

enum cactive_t
{
	ca_dedicated,
	ca_disconnected, 	// full screen console with no connection
	ca_demostart,		// starting up a demo
	ca_connected,		// netchan_t established, waiting for svc_serverdata
	ca_onserver,		// processing data lists, donwloading, etc
	ca_active			// everything is in, so frames can be rendered
};

class CEngClient // CEngineClient
{
public:
	CEngClient();
	~CEngClient();
	
	void Init();
	void Shutdown();
	
	void Frame(int msec);
	
	void EstablishConnection(char *host); //BeginServerConnect();
	void Disconnect(); // Drop
	
	void SendCmd();
	void SendMove(usercmd_t *cmd);
	
	/*bool*/ int GetMessage();
	void ParseServerMessage();
	
	int ReadFromServer();
	void WriteToServer(usercmd_t *cmd);
	
	void SetState(cactive_t eState);
	cactive_t GetState();
private:
	// connection information
	cactive_t state; // Current state type
	std::vector<IClientState*> mvStates;

// personalization data sent to server	
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart a level

// demo loop control
	int			demonum;		// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];		// when not playing

// demo recording info must be here, because record is started before
// entering a map (and clearing client_state_t)
	bool demorecording;
	bool demoplayback;
	bool timedemo;
	
	int			forcetrack;			// -1 = use normal cd track
	FILE		*demofile;
	
	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo

	// connection information
	int			signon;			// 0 to SIGNONS
	sizebuf_t	message;		// writing buffer to send to server
	
	int challenge; // from the server to use for connecting
	
	int userid;
	
	// network stuff
	CNetChannel *netchan;

	// private userinfo for sending to masterless servers
	char userinfo[MAX_INFO_STRING];
	
	char servername[MAX_OSPATH];	// name of server from original connect

	int qport;

	FILE		*download;		// file transfer from server
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int			downloadnumber;
	dltype_t	downloadtype;
	int			downloadpercent;

	float		latency;		// rolling average
};