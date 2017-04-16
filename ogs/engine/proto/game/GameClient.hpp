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
/// @brief representation of a client connected to server

#pragma once

//=============================================================================

// a client can leave the server in one of four ways:
// dropping properly by quiting or disconnecting
// timing out if no valid messages are received for timeout.value seconds
// getting kicked off by the server operator
// a program error, like an overflowed reliable buffer

//=============================================================================

enum client_state_t
{
	cs_free,   // can be reused for a new connection
	cs_zombie, // client has been disconnected, but don't reuse connection for a couple seconds
	cs_connected, // has been assigned to a client_t, but not in game yet
	cs_spawned    // client is fully in game
};

/* <2eb23> ../engine/server.h:177 */
// Note: we can't change these structs because some of them are exported to "hidden" GS api's
// So we need binary compat for them
typedef struct client_s
{
	qboolean active;
	qboolean spawned;
	qboolean fully_connected;
	qboolean connected;
	qboolean uploading;
	qboolean hasusrmsgs;
	qboolean has_force_unmodified;

	netchan_t netchan;

	int chokecount;
	int delta_sequence;

	qboolean fakeclient; // Client is a bot
	qboolean proxy;      // Client is a HLTV proxy

	usercmd_t lastcmd;

	double connecttime;
	double cmdtime;
	double ignorecmdtime;

	float latency;
	float packet_loss;

	double localtime;
	double nextping;
	double svtimebase;

	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];

	double connection_started;
	double next_messagetime;
	double next_messageinterval;

	qboolean send_message;
	qboolean skip_message;

	client_frame_t *frames;

	event_state_t events;

	edict_t *edict;
	const edict_t *pViewEntity;

	int userid;
	USERID_t network_userid;

	char userinfo[MAX_INFO_STRING];

	qboolean sendinfo;
	float sendinfo_time;

	char hashedcdkey[64];
	char name[32];

	int topcolor;
	int bottomcolor;

	int entityId;

	resource_t resourcesonhand;
	resource_t resourcesneeded;

	FileHandle_t upload;
	qboolean uploaddoneregistering;

	customization_t customdata;

	int crcValue;

	int lw;
	int lc;

	char physinfo[MAX_INFO_STRING];

	qboolean m_bLoopback;

	uint32 m_VoiceStreams[2];
	double m_lastvoicetime;

	int m_sendrescount;
} client_t;

// ReHLDS provides some OOP interfaces from itself
// But we want to use OOP inside the engine code too
class CGameClient : public IGameClient
{
public:
	CGameClient(int id, client_t *apClientData);
	~CGameClient();

	void Disconnect(const char *asReason, ...);
	void Reconnect(const char *message);
	void Drop(bool crash, const char *fmt, ...);
	
	void Kick(const char *asReason, ...);
	void Ban();
	void BanEx(const char *asReason, ...);
	void TimeBan(uint anTimeInMins);

	virtual void SetConnected(bool connected);
	virtual bool IsConnected();

	virtual void SetActive(bool active);
	virtual bool IsActive();
	
	virtual void SetSpawned(bool spawned);
	virtual bool IsSpawned();
	
	bool IsFakeClient() const; // IsBot
	
	void Inactivate();

	void UpdateUserInfo();
	void ExtractFromUserinfo();

	int CalcPing();
	bool ShouldUpdatePing();
	
	float CalcTime();
	void ComputeLatency();

	void CheckRate();

	void BuildFrame();

	void WriteEntities(sizebuf_t *msg);
	void WriteFrame(sizebuf_t *msg);

	void ProcessFile(char *filename);

	bool SendDatagram();
	void SendServerInfo();

	void ExtractFromUserInfo();

	void SendFullUpdate(sizebuf_t *sb);
	
	void ClearResourceLists(client_t *cl);
	void CreateCustomizationList(client_t *pHost);
	void Customization(client_t *pPlayer, resource_t *pResource, qboolean bSkipPlayer);

	bool UploadComplete(client_t *cl);
	void BatchUploadRequest(client_t *cl);

	void ParseResourceList(client_t *pSenderClient);
	
	virtual uint32 GetVoiceStream(int stream_id);
	
	virtual void SetLastVoiceTime(double time);
	virtual double GetLastVoiceTime();
	
	virtual bool GetLoopback();
	
	char *GetIDString();
	
	virtual USERID_t *GetNetworkUserID();
	virtual sizebuf_t *GetDatagram();
	
	virtual INetChan* GetNetChan();
	
	virtual struct usercmd_s *GetLastCmd();
	
	virtual const char *GetName(){return mpClientData->name;}
	virtual int GetId(){return mpClientData->userid;}
	virtual edict_t *GetEdict(){return mpClientData->edict;}
	
	virtual client_t *GetClient(){return mpClientData;}
	
	bool GetSpawnedOnce() const {return m_bSpawnedOnce;}
	void SetSpawnedOnce(bool spawned){m_bSpawnedOnce = spawned;}
	
#ifdef REHLDS_FIXES
	uint8_t* GetExtendedMessageBuffer(){return m_NetChan.GetExtendedMessageBuffer(); };
#endif
	
#ifdef REHLDS_FIXES
	void SetupLocalGameTime(){m_localGameTimeBase = g_psv.time;}
	double GetLocalGameTime() const {return g_psv.time - m_localGameTimeBase;}
	double GetLocalGameTimeBase() const {return m_localGameTimeBase;}
#endif
private:
	void Drop_internal(bool crash, const char *string);
	void ReplaceSpecialCharactersInName(char *newname, const char *oldname);
	
	void WriteClientdata(sizebuf_t *msg);
	
	CNetChan m_NetChan; // INetChan *
	
#ifdef REHLDS_FIXES
	double m_localGameTimeBase{0.0f};
#endif
	
	client_t *mpClientData{nullptr}; // m_pClient
	CGameServer *mpServer{nullptr};
	
	int m_Id{0};
	
	// added this field to handle a bug with hanging clients in scoreboard after a map change.
	// we would also use the field client_t:connected, but actually can't because there is a bug in CS client when server sends TeamScore
	// and client is not yet initialized ScoreBoard which leads to memory corruption in the client.
	bool m_bSpawnedOnce{false};
};

/*
struct oldclient
{
	client_state_t	state;
	
	bool active;				// false = client is free
	bool spawned;			// false = don't send datagrams
	bool dropasap;			// has been told to go to another level
	bool sendsignon;			// only valid before spawned
	bool fakeclient;
	bool hltvproxy;
	
	double last_message;		// reliable messages must be sent
										// periodically

	struct qsocket_s *netconnection;	// communications handle
	
	usercmd_t lastcmd; // for filling in big drops and partial predictions
	//usercmd_t cmd; // movement
	vec3_t wishdir;			// intended motion calced from cmd

	sizebuf_t message;			// can be added to at any time,
										// copied and clear once per frame
	byte msgbuf[MAX_MSGLEN];
	
	edict_t *edict; // Associated entity in world; EDICT_NUM(clientnum+1)
	const edict_t *pViewEntity;
	
	int userid; // identifying number
	
	char userinfo[MAX_INFO_STRING]; // infostring
	char physinfo[MAX_INFO_STRING];
	
	int lw;
	int lc;
	
	char name[32]; // for printing to other people (extracted from userinfo, high bits masked)
	
	int topcolor;
	int bottomcolor;
		
	float ping_times[NUM_PING_TIMES];
	int num_pings;			// ping_times[num_pings%NUM_PING_TIMES]

// spawn parms are carried from level to level
	float spawn_parms[NUM_SPAWN_PARMS];

// client known data for deltas	
	int old_frags;
	
	int				spectator;			// non-interactive

	qboolean		sendinfo;			// at end of frame, send info to all
										// this prevents malicious multiple broadcasts
	float			lastnametime;		// time of last name change
	int				lastnamecount;		// time of last name change
	unsigned		checksum;			// checksum for calcs
	bool drop;				// lose this guy next opportunity
	int				lossage;			// loss percentage	
			
	double			localtime;			// of last message
	int				oldbuttons;

	float			maxspeed;			// localized maxspeed
	float			entgravity;			// localized ent gravity
	
	int				messagelevel;		// for filtering printed messages

	// the datagram is written to after every frame, but only cleared
	// when it is sent out to the client.  overflow is tolerated.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM]; // MAX_MSGLEN

	// back buffers for client reliable data
	sizebuf_t	backbuf;
	int			num_backbuf;
	int			backbuf_size[MAX_BACK_BUFFERS];
	byte		backbuf_data[MAX_BACK_BUFFERS][MAX_MSGLEN];

	double connection_started;	// or time of disconnect for zombies
	bool send_message;		// set on frames a datagram arived on
	
	int				stats[MAX_CL_STATS];

	client_frame_t	frames[UPDATE_BACKUP];	// updates can be delta'd from here

	FILE			*download;			// file being downloaded
	int				downloadsize;		// total bytes (can't use EOF because of paks)
	int				downloadcount;		// bytes sent

	int				spec_track;			// entnum of player tracking

	double			whensaid[10];       // JACK: For floodprots
 	int			whensaidhead;       // Head value for floodprots
 	double			lockedtill;

	bool upgradewarn;		// did we warn him?

	FILE			*upload;
	char			uploadfn[MAX_QPATH];
	netadr_t		snap_from;
	bool remote_snap;
 
//===== NETWORK ============
	int				chokecount;
	int				delta_sequence;		// -1 = no compression
	netchan_t		netchan;
};
*/