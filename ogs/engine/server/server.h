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
#ifndef OGS_SERVER_H
#define OGS_SERVER_H

class CGameClient;

/* <7588> ../engine/server.h:65 */
typedef enum server_state_e
{
	ss_dead		= 0,	// no map loaded
	ss_loading	= 1,	// spawning level edicts
	ss_active	= 2,	// actively running
} server_state_t;

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

/* <75f3> ../engine/server.h:75 */
typedef struct server_s
{
	qboolean active;
	qboolean paused;
	qboolean loadgame;
	
	double time;
	double oldtime;
	
	int lastcheck;
	double lastchecktime;
	
	char name[64];
	char oldname[64];
	char startspot[64];
	char modelname[64];
	
	struct model_s *worldmodel;
	
	CRC32_t worldmapCRC;
	unsigned char clientdllmd5[16];
	
	resource_t resourcelist[MAX_RESOURCE_LIST];
	int num_resources;
	
	consistency_t consistency_list[MAX_CONSISTENCY_LIST];
	int num_consistency;
	
	const char *model_precache[HL_MODEL_MAX];
	struct model_s *models[HL_MODEL_MAX];
	unsigned char model_precache_flags[HL_MODEL_MAX];
	
	struct event_s event_precache[HL_EVENT_MAX];
	
	const char *sound_precache[HL_SOUND_MAX];
	short int sound_precache_hashedlookup[HL_SOUND_HASHLOOKUP_SIZE];
	qboolean sound_precache_hashedlookup_built;
	
	const char *generic_precache[HL_GENERIC_MAX];
	char generic_precache_names[HL_GENERIC_MAX][64];
	int num_generic_names;
	
	char *lightstyles[MAX_LIGHTSTYLES];
	
	int num_edicts;
	int max_edicts;
	edict_t *edicts;
	struct entity_state_s *baselines;
	extra_baselines_t *instance_baselines;
	
	server_state_t state;
	
	sizebuf_t datagram;
	unsigned char datagram_buf[MAX_DATAGRAM];
	
	sizebuf_t reliable_datagram;
	unsigned char reliable_datagram_buf[MAX_DATAGRAM];
	
	sizebuf_t multicast;
	unsigned char multicast_buf[1024];
	
	sizebuf_t spectator;
	unsigned char spectator_buf[1024];
	
	sizebuf_t signon;
	unsigned char signon_data[32768];
} server_t;

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
	
	void ClientPrintf(CGameClient *apClient, char *fmt, ...);
	void BroadcastPrintf(const char *fmt, ...);
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
	bool mbInitialized; // sv_init has completed
	
	server_state_t state; // some actions are only valid during load
	//tGameServerStateVec mvStates; // mvStates[state]->Frame();
	
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
	
	/*int*/ double last_heartbeat;
	int heartbeat_sequence;
	
	struct model_s *models[MAX_MODELS];
	
	char *model_precache[MAX_MODELS];	// NULL terminated
	char *sound_precache[MAX_SOUNDS];	// NULL terminated
	
	char info[MAX_SERVERINFO_STRING];
	
	int flags; // episode completion information
	
	bool changelevel_issued; // cleared when at SV_SpawnServer
	
	challenge_t	challenges[MAX_CHALLENGES];	// to prevent invalid IPs from connecting
};

delta_t *SV_LookupDelta(char *name);
NOXREF void SV_DownloadingModules(void);
void SV_GatherStatistics(void);
void SV_DeallocateDynamicData(void);
void SV_ReallocateDynamicData(void);
void SV_AllocClientFrames(void);
qboolean SV_IsPlayerIndex(int index);
qboolean SV_IsPlayerIndex_wrapped(int index);
void SV_ClearPacketEntities(client_frame_t *frame);
void SV_AllocPacketEntities(client_frame_t *frame, int numents);
void SV_ClearFrames(client_frame_t ** frames);
void SV_Serverinfo_f(void);
void SV_Localinfo_f(void);
void SV_User_f(void);
void SV_Users_f(void);
void SV_CountPlayers(int *clients);
void SV_CountProxies(int *proxies);
void SV_FindModelNumbers(void);
void SV_StartParticle(const vec_t *org, const vec_t *dir, int color, int count);
void SV_StartSound(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
void SV_StartSound_internal(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
qboolean SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer);
int SV_HashString(const char *string, int iBounds);
int SV_LookupSoundIndex(const char *sample);
void SV_BuildHashedSoundLookupTable(void);
void SV_AddSampleToHashedLookupTable(const char *pszSample, int iSampleIndex);
qboolean SV_ValidClientMulticast(client_t *client, int soundLeaf, int to);
void SV_Multicast(edict_t *ent, vec_t *origin, int to, qboolean reliable);
void SV_WriteMovevarsToClient(sizebuf_t *message);
void SV_WriteDeltaDescriptionsToClient(sizebuf_t *msg);
void SV_SetMoveVars(void);
void SV_QueryMovevarsChanged(void);
void SV_SendServerinfo(sizebuf_t *msg, client_t *client);
void SV_SendServerinfo_internal(sizebuf_t *msg, client_t *client);
void SV_SendResources(sizebuf_t *msg);
void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg);
void SV_WriteSpawn(sizebuf_t *msg);
void SV_SendUserReg(sizebuf_t *msg);
void SV_New_f(void);
void SV_SendRes_f(void);
void SV_Spawn_f(void);
void SV_Spawn_f_internal(void);
void SV_CheckUpdateRate(double *rate);
void SV_RejectConnection(netadr_t *adr, char *fmt, ...);
void SV_RejectConnectionForPassword(netadr_t *adr);
int SV_GetFragmentSize(void *state);
qboolean SV_FilterUser(USERID_t *userid);
int SV_CheckProtocol(netadr_t *adr, int nProtocol);
int SV_CheckProtocol_internal(netadr_t *adr, int nProtocol);
bool SV_CheckChallenge_api(const netadr_t &adr, int nChallengeValue);
int SV_CheckChallenge(netadr_t *adr, int nChallengeValue);
int SV_CheckIPRestrictions(netadr_t *adr, int nAuthProtocol);
int SV_CheckIPRestrictions_internal(netadr_t *adr, int nAuthProtocol);
int SV_CheckIPConnectionReuse(netadr_t *adr);
int SV_FinishCertificateCheck(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_FinishCertificateCheck_internal(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_CheckKeyInfo(netadr_t *adr, char *protinfo, short unsigned int *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckKeyInfo_internal(netadr_t *adr, char *protinfo, short unsigned int *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckForDuplicateSteamID(client_t *client);
int SV_CheckForDuplicateNames(char *userinfo, qboolean bIsReconnecting, int nExcludeSlot);
int SV_CheckUserInfo(netadr_t *adr, char *userinfo, qboolean bIsReconnecting, int nReconnectSlot, char *name);
int SV_FindEmptySlot(netadr_t *adr, int *pslot, client_t ** ppClient);
void SV_ConnectClient(void);
void SV_ConnectClient_internal(void);
void SVC_Ping(void);
int SV_GetChallenge(const netadr_t& adr);
void SVC_GetChallenge(void);
void SVC_ServiceChallenge(void);
void SV_ResetModInfo(void);
int SV_GetFakeClientCount(void);
NOXREF qboolean SV_GetModInfo(char *pszInfo, char *pszDL, int *version, int *size, qboolean *svonly, qboolean *cldll, char *pszHLVersion);
NOXREF qboolean RequireValidChallenge(netadr_t *adr);
NOXREF qboolean ValidInfoChallenge(netadr_t *adr, const char *nugget);
NOXREF int GetChallengeNr(netadr_t *adr);
NOXREF qboolean CheckChallengeNr(netadr_t *adr, int nChallengeValue);
NOXREF void ReplyServerChallenge(netadr_t *adr);
NOXREF qboolean ValidChallenge(netadr_t *adr, int challengeNr);
NOXREF void SVC_InfoString(void);
NOXREF void SVC_Info(qboolean bDetailed);
NOXREF void SVC_PlayerInfo(void);
NOXREF void SVC_RuleInfo(void);
int SVC_GameDllQuery(const char *s);
void SV_FlushRedirect(void);
void SV_EndRedirect(void);
void SV_BeginRedirect(redirect_t rd, netadr_t *addr);
void SV_ResetRcon_f(void);
void SV_AddFailedRcon(netadr_t *adr);
qboolean SV_CheckRconFailure(netadr_t *adr);
int SV_Rcon_Validate(void);
void SV_Rcon(netadr_t *net_from_);
void SV_ConnectionlessPacket(void);
void SV_CheckRate(client_t *cl);
void SV_ProcessFile(client_t *cl, char *filename);
qboolean SV_FilterPacket(void);
void SV_SendBan(void);
void SV_ReadPackets(void);
//NOBODY int ntohl(void);
//NOBODY int htons(void);
void SV_CheckTimeouts(void);
int SV_CalcPing(client_t *cl);
void SV_FullClientUpdate(client_t *cl, sizebuf_t *sb);
void SV_EmitEvents(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_EmitEvents_internal(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_AddToFatPVS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPVS(float *org);
void SV_AddToFatPAS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPAS(float *org);
int SV_PointLeafnum(vec_t *p);
void TRACE_DELTA(char *fmt, ...);
void SV_SetCallback(int num, qboolean remove, qboolean custom, int *numbase, qboolean full, int offset);
void SV_SetNewInfo(int newblindex);
void SV_WriteDeltaHeader(int num, qboolean remove, qboolean custom, int *numbase, qboolean newbl, int newblindex, qboolean full, int offset);
void SV_InvokeCallback(void);
int SV_FindBestBaseline(int index, entity_state_t ** baseline, entity_state_t *to, int num, qboolean custom);
int SV_CreatePacketEntities(sv_delta_t type, client_t *client, packet_entities_t *to, sizebuf_t *msg);
int SV_CreatePacketEntities_internal(sv_delta_t type, client_t *client, packet_entities_t *to, sizebuf_t *msg);
void SV_EmitPacketEntities(client_t *client, packet_entities_t *to, sizebuf_t *msg);
qboolean SV_ShouldUpdatePing(client_t *client);
NOXREF qboolean SV_HasEventsInQueue(client_t *client);
void SV_GetNetInfo(client_t *client, int *ping, int *packet_loss);
int SV_CheckVisibility(edict_t *entity, unsigned char *pset);
void SV_EmitPings(client_t *client, sizebuf_t *msg);
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg);
void SV_CleanupEnts(void);
qboolean SV_SendClientDatagram(client_t *client);
void SV_UpdateUserInfo(client_t *pClient);
void SV_UpdateToReliableMessages(void);
void SV_SkipUpdates(void);
void SV_SendClientMessages(void);
void SV_ExtractFromUserinfo(client_t *cl);
int SV_ModelIndex(const char *name);
void SV_AddResource(resourcetype_t type, const char *name, int size, unsigned char flags, int index);
void SV_CreateGenericResources(void);
void SV_CreateResourceList(void);
void SV_ClearCaches(void);
void SV_PropagateCustomizations(void);
void SV_WriteVoiceCodec(sizebuf_t *pBuf);
void SV_WriteVoiceCodec_internal(sizebuf_t *pBuf);
void SV_CreateBaseline(void);
void SV_BroadcastCommand(char *fmt, ...);
void SV_BuildReconnect(sizebuf_t *msg);
NOXREF void SV_ReconnectAllClients(void);
void SetCStrikeFlags(void);
void SV_ActivateServer(int runPhysics);
void SV_ActivateServer_internal(int runPhysics);
void SV_ServerShutdown(void);
int SV_SpawnServer(qboolean bIsDemo, char *server, char *startspot);
void SV_LoadEntities(void);
void SV_ClearEntities(void);
int RegUserMsg(const char *pszName, int iSize);
qboolean StringToFilter(const char *s, ipfilter_t *f);
USERID_t *SV_StringToUserID(const char *str);
void SV_BanId_f(void);
void Host_Kick_f(void);
void SV_RemoveId_f(void);
void SV_WriteId_f(void);
void SV_ListId_f(void);
void SV_AddIP_f(void);
void SV_RemoveIP_f(void);
void SV_ListIP_f(void);
void SV_WriteIP_f(void);
void SV_KickPlayer(int nPlayerSlot, int nReason);
void SV_InactivateClients(void);
void SV_FailDownload(const char *filename);
const char *Q_stristr(const char *pStr, const char *pSearch);
qboolean IsSafeFileToDownload(const char *filename);
void SV_BeginFileDownload_f(void);
void SV_SetMaxclients(void);
void SV_HandleRconPacket(void);
void SV_CheckCmdTimes(void);
void SV_CheckForRcon(void);
qboolean SV_IsSimulating(void);
void SV_CheckMapDifferences(void);
void SV_Frame(void);
void SV_Drop_f(void);
void SV_RegisterDelta(char *name, char *loadfile);
void SV_InitDeltas(void);
void SV_InitEncoders(void);
void SV_Init(void);
void SV_Shutdown(void);
qboolean SV_CompareUserID(USERID_t *id1, USERID_t *id2);
qboolean SV_CompareUserID_internal(USERID_t *id1, USERID_t *id2);
char *SV_GetIDString(USERID_t *id);
char *SV_GetIDString_internal(USERID_t *id);
char *SV_GetClientIDString(client_t *client);
int GetGameAppID(void);
qboolean IsGameSubscribed(const char *gameName);
NOXREF qboolean BIsValveGame(void);

void SV_ClientPrintf(const char *fmt, ...);

void SV_DropClient_api(IGameClient* cl, bool crash, const char* fmt, ...);
void SV_DropClient(client_t *cl, qboolean crash, const char *fmt, ...);
void SV_DropClient_internal(client_t *cl, qboolean crash, const char *string);

void SV_ClearClientStates(void);

#endif // OGS_SERVER_H