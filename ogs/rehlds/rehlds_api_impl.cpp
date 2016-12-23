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
#include "precompiled.h"

struct plugin_api_t
{
	char name[32];
	void *impl;
};

std::vector<plugin_api_t *> g_PluginApis;

plugin_api_t* FindPluginApiByName(const char *name) {
	for (auto pl : g_PluginApis) {
		if (!strcmp(pl->name, name)) {
			return pl;
		}
	}

	return NULL;
}

char* EXT_FUNC GetClientFallback_api() {
	return com_clientfallback;
}

int* EXT_FUNC GetAllowCheats_api() {
	return &allow_cheats;
}

bool EXT_FUNC GSBSecure_api() {
	return Steam_GSBSecure() != 0;
}

int EXT_FUNC GetBuildNumber_api() {
	return build_number();
}

double EXT_FUNC GetRealTime_api() {
	return realtime;
}

int* EXT_FUNC GetMsgBadRead_api() {
	return &msg_badread;
}

cmd_source_t* EXT_FUNC GetCmdSource_api() {
	return &cmd_source;
}

void EXT_FUNC Log_api(const char* prefix, const char* msg) {
#ifdef REHLDS_FLIGHT_REC
	FR_Log(prefix, msg);
#endif
}

DLL_FUNCTIONS* EXT_FUNC GetEntityInterface_api() {
	return &gEntityInterface;
}

void EXT_FUNC MSG_StartBitWriting_api(sizebuf_t *buf) {
	MSG_StartBitWriting(buf);
}

void EXT_FUNC MSG_WriteBits_api(uint32 data, int numbits) {
	MSG_WriteBits(data, numbits);
}

int EXT_FUNC MSG_ReadShort_api() {
	return MSG_ReadShort();
}

int EXT_FUNC MSG_ReadBuf_api(int iSize, void *pbuf) {
	return MSG_ReadBuf(iSize, pbuf);
}

void EXT_FUNC MSG_WriteBuf_api(sizebuf_t *sb, int iSize, void *buf) {
	MSG_WriteBuf(sb, iSize, buf);
}

void EXT_FUNC MSG_WriteByte_api(sizebuf_t *sb, int c) {
	MSG_WriteByte(sb, c);
}

void EXT_FUNC MSG_WriteShort_api(sizebuf_t *sb, int c) {
	MSG_WriteShort(sb, c);
}

void EXT_FUNC MSG_WriteString_api(sizebuf_t *sb, const char *s) {
	MSG_WriteString(sb, s);
}

void EXT_FUNC MSG_WriteBitVec3Coord_api(const vec3_t fa) {
	MSG_WriteBitVec3Coord(fa);
}

void EXT_FUNC MSG_EndBitWriting_api(sizebuf_t *buf) {
	MSG_EndBitWriting(buf);
}

cvar_t* EXT_FUNC GetCvarVars_api() {
	return cvar_vars;
}

void EXT_FUNC SV_StartSound_api(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int flags, int pitch) {
	SV_StartSound(recipients, entity, channel, sample, volume, attenuation, flags, pitch);
}

void EXT_FUNC SV_UpdateUserInfo_api(IGameClient *pGameClient) {
	SV_UpdateUserInfo(pGameClient->GetClient());
}

void* EXT_FUNC Rehlds_GetPluginApi(const char *name) {
	auto api = FindPluginApiByName(name);
	return api ? api->impl : NULL;
}

bool EXT_FUNC StripUnprintableAndSpace_api(char *pch) {
	return Q_StripUnprintableAndSpace(pch) != FALSE;
}

void EXT_FUNC Rehlds_RegisterPluginApi(const char *name, void *impl) {
	auto api = FindPluginApiByName(name);

	if (!api) {
		api = new plugin_api_t;
		Q_strncpy(api->name, name, sizeof api->name - 1);
		api->name[sizeof api->name - 1] = '\0';

		g_PluginApis.push_back(api);
	}

	api->impl = impl;
}

CRehldsServerStatic g_RehldsServerStatic;
CRehldsServerData g_RehldsServerData;
CRehldsHookchains g_RehldsHookchains;
RehldsFuncs_t g_RehldsApiFuncs = 
{
	&SV_DropClient_api,
	&SV_RejectConnection,
	&Steam_NotifyBotConnect_api,
	&GetNetMessage_api,
	&GetHostClient_api,
	&GetMsgReadCount_api,
	&SV_FilterUser,
	&NET_SendPacket_api,
	&Cmd_TokenizeString,
	&SV_CheckChallenge_api,
	&SV_SendUserReg,
	&SV_WriteDeltaDescriptionsToClient,
	&SV_SetMoveVars,
	&SV_WriteMovevarsToClient,
	&GetClientFallback_api,
	&GetAllowCheats_api,
	&GSBSecure_api,
	&GetBuildNumber_api,
	&GetRealTime_api,
	&GetMsgBadRead_api,
	&GetCmdSource_api,
	&Log_api,
	&GetEntityInterface_api,
	&EV_PlayReliableEvent_api,
	&SV_LookupSoundIndex,
	&MSG_StartBitWriting_api,
	&MSG_WriteBits_api,
	&MSG_WriteBitVec3Coord_api,
	&MSG_EndBitWriting_api,
	&SZ_GetSpace,
	&GetCvarVars_api,
	&SV_GetChallenge,
	&SV_AddResource,
	&MSG_ReadShort_api,
	&MSG_ReadBuf_api,
	&MSG_WriteBuf_api,
	&MSG_WriteByte_api,
	&MSG_WriteShort_api,
	&MSG_WriteString_api,
	&Rehlds_GetPluginApi,
	&Rehlds_RegisterPluginApi,
	&SV_FileInConsistencyList,
	&Steam_NotifyClientConnect_api,
	&Steam_NotifyClientDisconnect_api,
	&SV_StartSound_api,
	&SV_EmitSound2_api,
	&SV_UpdateUserInfo_api,
	&StripUnprintableAndSpace_api
};

bool EXT_FUNC SV_EmitSound2_internal(edict_t *entity, IGameClient *pReceiver, int channel, const char *sample, float volume, float attenuation, int flags, int pitch, int emitFlags, const float *pOrigin)
{
	bool bSendPAS = (channel != CHAN_STATIC && !(flags & SND_FL_STOP) && !(emitFlags & SND_EMIT2_NOPAS));
	vec3_t origin = {0, 0, 0};

	if (entity && entity != g_psv.edicts)
	{
		for (int i = 0; i < 3; ++i)
			origin[i] = (entity->v.maxs[i] + entity->v.mins[i]) * 0.5f + entity->v.origin[i];
	}
	// only worldspawn
	else if (pOrigin != nullptr)
	{
		for (int i = 0; i < 3; ++i)
			origin[i] = pOrigin[i];
	}

	// spawn sound of the entity for everyone clients
	if (!pReceiver)
	{
		if (!SV_BuildSoundMsg(entity, channel, sample, (int)(volume * 255.0f), attenuation, flags, pitch, origin, &g_psv.multicast))
			return false;

		int msg_destination = MSG_FL_NONE;
		if (emitFlags & SND_EMIT2_INVOKER) {
			msg_destination = MSG_FL_ONE;
		}

		if (bSendPAS)
		{
			SV_Multicast(entity, origin, msg_destination | MSG_FL_PAS, FALSE);
		}
		else
		{
			SV_Multicast(entity, origin, msg_destination | MSG_FL_BROADCAST, TRUE);
		}
		return true;
	}

	sizebuf_t *pBuffer;
	client_t *receiver = pReceiver->GetClient();

	if (bSendPAS && receiver->edict != entity) // if the sound for myself, then do not check PAS.
	{
		int leafnum = SV_PointLeafnum(origin);
		if (!SV_ValidClientMulticast(receiver, leafnum, MSG_FL_PAS))
			return false;

		pBuffer = &receiver->datagram;
	}
	else
	{
		// reliable
		pBuffer = &receiver->netchan.message;
	}

	// send to one client
	return SV_BuildSoundMsg(entity, channel, sample, (int)(volume * 255.0f), attenuation, flags, pitch, origin, pBuffer) != FALSE;
}

bool EXT_FUNC SV_EmitSound2_api(edict_t *entity, IGameClient *receiver, int channel, const char *sample, float volume, float attenuation, int flags, int pitch, int emitFlags, const float *pOrigin)
{
	return g_RehldsHookchains.m_SV_EmitSound2.callChain(SV_EmitSound2_internal, entity, receiver, channel, sample, volume, attenuation, flags, pitch, emitFlags, pOrigin);
}

sizebuf_t* EXT_FUNC GetNetMessage_api()
{
	return &net_message;
}

IGameClient* EXT_FUNC GetHostClient_api()
{
	if (host_client == NULL)
		return NULL;

	return GetRehldsApiClient(host_client);
}

extern int* EXT_FUNC GetMsgReadCount_api()
{
	return &msg_readcount;
}

IRehldsHookRegistry_Steam_NotifyClientConnect* CRehldsHookchains::Steam_NotifyClientConnect()
{
	return &m_Steam_NotifyClientConnect;
}

IRehldsHookRegistry_SV_ConnectClient* CRehldsHookchains::SV_ConnectClient()
{
	return &m_SV_ConnectClient;
}

IRehldsHookRegistry_SV_GetIDString* CRehldsHookchains::SV_GetIDString()
{
	return &m_SV_GetIDString;
}

IRehldsHookRegistry_SV_SendServerinfo* CRehldsHookchains::SV_SendServerinfo()
{
	return &m_SV_SendServerinfo;
}

IRehldsHookRegistry_SV_CheckProtocol* CRehldsHookchains::SV_CheckProtocol()
{
	return &m_SV_CheckProtocol;
}

IRehldsHookRegistry_SVC_GetChallenge_mod* CRehldsHookchains::SVC_GetChallenge_mod()
{
	return &m_SVC_GetChallenge_mod;
}

IRehldsHookRegistry_SV_CheckKeyInfo* CRehldsHookchains::SV_CheckKeyInfo()
{
	return &m_SV_CheckKeyInfo;
}

IRehldsHookRegistry_SV_CheckIPRestrictions* CRehldsHookchains::SV_CheckIPRestrictions()
{
	return &m_SV_CheckIPRestrictions;
}

IRehldsHookRegistry_SV_FinishCertificateCheck* CRehldsHookchains::SV_FinishCertificateCheck()
{
	return &m_SV_FinishCertificateCheck;
}

IRehldsHookRegistry_Steam_NotifyBotConnect* CRehldsHookchains::Steam_NotifyBotConnect()
{
	return &m_Steam_NotifyBotConnect;
}

IRehldsHookRegistry_SerializeSteamId* CRehldsHookchains::SerializeSteamId()
{
	return &m_SerializeSteamId;
}

IRehldsHookRegistry_SV_CompareUserID* CRehldsHookchains::SV_CompareUserID()
{
	return &m_SV_CompareUserID;
}

IRehldsHookRegistry_Steam_NotifyClientDisconnect* CRehldsHookchains::Steam_NotifyClientDisconnect()
{
	return &m_Steam_NotifyClientDisconnect;
}

IRehldsHookRegistry_PreprocessPacket* CRehldsHookchains::PreprocessPacket() {
	return &m_PreprocessPacket;
}

IRehldsHookRegistry_ValidateCommand* CRehldsHookchains::ValidateCommand() {
	return &m_ValidateCommand;
}

IRehldsHookRegistry_ClientConnected* CRehldsHookchains::ClientConnected() {
	return &m_ClientConnected;
}

IRehldsHookRegistry_HandleNetCommand* CRehldsHookchains::HandleNetCommand() {
	return &m_HandleNetCommand;
}

IRehldsHookRegistry_Mod_LoadBrushModel* CRehldsHookchains::Mod_LoadBrushModel() {
	return &m_Mod_LoadBrushModel;
}

IRehldsHookRegistry_Mod_LoadStudioModel* CRehldsHookchains::Mod_LoadStudioModel() {
	return &m_Mod_LoadStudioModel;
}

IRehldsHookRegistry_ExecuteServerStringCmd* CRehldsHookchains::ExecuteServerStringCmd() {
	return &m_ExecuteServerStringCmd;
}

IRehldsHookRegistry_SV_EmitEvents* CRehldsHookchains::SV_EmitEvents() {
	return &m_SV_EmitEvents;
}

IRehldsHookRegistry_EV_PlayReliableEvent* CRehldsHookchains::EV_PlayReliableEvent() {
	return &m_EV_PlayReliableEvent;
}

IRehldsHookRegistry_SV_StartSound* CRehldsHookchains::SV_StartSound() {
	return &m_SV_StartSound;
}

IRehldsHookRegistry_PF_Remove_I* CRehldsHookchains::PF_Remove_I() {
	return &m_PF_Remove_I;
}

IRehldsHookRegistry_PF_BuildSoundMsg_I* CRehldsHookchains::PF_BuildSoundMsg_I() {
	return &m_PF_BuildSoundMsg_I;
}

IRehldsHookRegistry_SV_WriteFullClientUpdate* CRehldsHookchains::SV_WriteFullClientUpdate() {
	return &m_SV_WriteFullClientUpdate;
}

IRehldsHookRegistry_SV_CheckConsistencyResponse* CRehldsHookchains::SV_CheckConsistencyResponse() {
	return &m_SV_CheckConsistencyResponse;
}

IRehldsHookRegistry_SV_DropClient* CRehldsHookchains::SV_DropClient() {
	return &m_SV_DropClient;
}

IRehldsHookRegistry_SV_ActivateServer* CRehldsHookchains::SV_ActivateServer() {
	return &m_SV_ActivateServer;
}

IRehldsHookRegistry_SV_WriteVoiceCodec* CRehldsHookchains::SV_WriteVoiceCodec() {
	return &m_SV_WriteVoiceCodec;
}

IRehldsHookRegistry_Steam_GSGetSteamID* CRehldsHookchains::Steam_GSGetSteamID() {
	return &m_Steam_GSGetSteamID;
}

IRehldsHookRegistry_SV_TransferConsistencyInfo* CRehldsHookchains::SV_TransferConsistencyInfo() {
	return &m_SV_TransferConsistencyInfo;
}

IRehldsHookRegistry_Steam_GSBUpdateUserData* CRehldsHookchains::Steam_GSBUpdateUserData() {
	return &m_Steam_GSBUpdateUserData;
}

IRehldsHookRegistry_Cvar_DirectSet* CRehldsHookchains::Cvar_DirectSet() {
	return &m_Cvar_DirectSet;
}

IRehldsHookRegistry_SV_EstablishTimeBase* CRehldsHookchains::SV_EstablishTimeBase() {
	return &m_SV_EstablishTimeBase;
}

IRehldsHookRegistry_SV_Spawn_f* CRehldsHookchains::SV_Spawn_f() {
	return &m_SV_Spawn_f;
}

IRehldsHookRegistry_SV_CreatePacketEntities* CRehldsHookchains::SV_CreatePacketEntities() {
	return &m_SV_CreatePacketEntities;
}

IRehldsHookRegistry_SV_EmitSound2* CRehldsHookchains::SV_EmitSound2() {
	return &m_SV_EmitSound2;
}

int EXT_FUNC CRehldsApi::GetMajorVersion()
{
	return REHLDS_API_VERSION_MAJOR;
}

int EXT_FUNC CRehldsApi::GetMinorVersion()
{
	return REHLDS_API_VERSION_MINOR;
}

const RehldsFuncs_t* EXT_FUNC CRehldsApi::GetFuncs()
{
	return &g_RehldsApiFuncs;
}

IRehldsHookchains* EXT_FUNC CRehldsApi::GetHookchains()
{
	return &g_RehldsHookchains;
}

IRehldsServerStatic* EXT_FUNC CRehldsApi::GetServerStatic() {
	return &g_RehldsServerStatic;
}

IRehldsServerData* EXT_FUNC CRehldsApi::GetServerData() {
	return &g_RehldsServerData;
}

IRehldsFlightRecorder* EXT_FUNC CRehldsApi::GetFlightRecorder() {
	return g_FlightRecorder;
}

EXPOSE_SINGLE_INTERFACE(CRehldsApi, IRehldsApi, VREHLDS_HLDS_API_VERSION);