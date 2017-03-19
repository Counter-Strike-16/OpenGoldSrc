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

#pragma once

#include "public/archtypes.h"
#include "rehlds/rehlds_api.h"
#include "rehlds_interfaces_impl.h"
#include "hookchains_impl.h"

//Steam_NotifyClientConnect
typedef IHookChainImpl<qboolean, IGameClient*, const void*, unsigned int> CRehldsHook_Steam_NotifyClientConnect;
typedef IHookChainRegistryImpl<qboolean, IGameClient*, const void*, unsigned int> CRehldsHookRegistry_Steam_NotifyClientConnect;

//SV_ConnectClient hook
typedef IVoidHookChainImpl<> CRehldsHook_SV_ConnectClient;
typedef IVoidHookChainRegistryImpl<> CRehldsHookRegistry_SV_ConnectClient;

//SV_GetIDString hook
typedef IHookChainImpl<char*, USERID_t*> CRehldsHook_SV_GetIDString;
typedef IHookChainRegistryImpl<char*, USERID_t*> CRehldsHookRegistry_SV_GetIDString;

//SV_SendServerinfo hook
typedef IVoidHookChainImpl<sizebuf_t*, IGameClient*> CRehldsHook_SV_SendServerinfo;
typedef IVoidHookChainRegistryImpl<sizebuf_t*, IGameClient*> CRehldsHookRegistry_SV_SendServerinfo;

//SV_CheckProtocol hook
typedef IHookChainImpl<int, netadr_t*, int> CRehldsHook_SV_CheckProtocol;
typedef IHookChainRegistryImpl<int, netadr_t*, int> CRehldsHookRegistry_SV_CheckProtocol;

//SVC_GetChallenge_mod hook
typedef IVoidHookChainImpl<char*, int> CRehldsHook_SVC_GetChallenge_mod;
typedef IVoidHookChainRegistryImpl<char*, int> CRehldsHookRegistry_SVC_GetChallenge_mod;

//SV_CheckKeyInfo hook
typedef IHookChainImpl<int, netadr_t*, char*, uint16*, int*, char*, char*> CRehldsHook_SV_CheckKeyInfo;
typedef IHookChainRegistryImpl<int, netadr_t*, char*, uint16*, int*, char*, char*> CRehldsHookRegistry_SV_CheckKeyInfo;

//SV_CheckIPRestrictions hook
typedef IHookChainImpl<int, netadr_t*, int> CRehldsHook_SV_CheckIPRestrictions;
typedef IHookChainRegistryImpl<int, netadr_t*, int> CRehldsHookRegistry_SV_CheckIPRestrictions;

//SV_FinishCertificateCheck hook
typedef IHookChainImpl<int, netadr_t*, int, char*, char*> CRehldsHook_SV_FinishCertificateCheck;
typedef IHookChainRegistryImpl<int, netadr_t*, int, char*, char*> CRehldsHookRegistry_SV_FinishCertificateCheck;

//Steam_NotifyBotConnect hook
typedef IHookChainImpl<qboolean, IGameClient*> CRehldsHook_Steam_NotifyBotConnect;
typedef IHookChainRegistryImpl<qboolean, IGameClient*> CRehldsHookRegistry_Steam_NotifyBotConnect;

//SerializeSteamId
typedef IVoidHookChainImpl<USERID_t*, USERID_t*> CRehldsHook_SerializeSteamId;
typedef IVoidHookChainRegistryImpl<USERID_t*, USERID_t*> CRehldsHookRegistry_SerializeSteamId;

//SV_CompareUserID hook
typedef IHookChainImpl<qboolean, USERID_t*, USERID_t*> CRehldsHook_SV_CompareUserID;
typedef IHookChainRegistryImpl<qboolean, USERID_t*, USERID_t*> CRehldsHookRegistry_SV_CompareUserID;

//Steam_NotifyClientDisconnect
typedef IVoidHookChainImpl<IGameClient*> CRehldsHook_Steam_NotifyClientDisconnect;
typedef IVoidHookChainRegistryImpl<IGameClient*> CRehldsHookRegistry_Steam_NotifyClientDisconnect;

//PreProcessPacket
typedef IHookChainImpl<bool, uint8*, unsigned int, const netadr_t&> CRehldsHook_PreprocessPacket;
typedef IHookChainRegistryImpl<bool, uint8*, unsigned int, const netadr_t&> CRehldsHookRegistry_PreprocessPacket;

//ValidateCommand
typedef IHookChainImpl<bool, const char*, cmd_source_t, IGameClient*> CRehldsHook_ValidateCommand;
typedef IHookChainRegistryImpl<bool, const char*, cmd_source_t, IGameClient*> CRehldsHookRegistry_ValidateCommand;

//ExecuteServerStringCmd
typedef IVoidHookChainImpl<const char*, cmd_source_t, IGameClient*> CRehldsHook_ExecuteServerStringCmd;
typedef IVoidHookChainRegistryImpl<const char*, cmd_source_t, IGameClient*> CRehldsHookRegistry_ExecuteServerStringCmd;

//ClientConnected
typedef IVoidHookChainImpl<IGameClient*> CRehldsHook_ClientConnected;
typedef IVoidHookChainRegistryImpl<IGameClient*> CRehldsHookRegistry_ClientConnected;

//HandleNetCommand
typedef IVoidHookChainImpl<IGameClient*, int8> CRehldsHook_HandleNetCommand;
typedef IVoidHookChainRegistryImpl<IGameClient*, int8> CRehldsHookRegistry_HandleNetCommand;

//Mod_LoadBrushModel
typedef IVoidHookChainImpl<model_t*, void*> CRehldsHook_Mod_LoadBrushModel;
typedef IVoidHookChainRegistryImpl<model_t*, void*> CRehldsHookRegistry_Mod_LoadBrushModel;

//Mod_LoadStudioModel
typedef IVoidHookChainImpl<model_t*, void*> CRehldsHook_Mod_LoadStudioModel;
typedef IVoidHookChainRegistryImpl<model_t*, void*> CRehldsHookRegistry_Mod_LoadStudioModel;

//SV_EmitEvents hook
typedef IVoidHookChainImpl<IGameClient *, packet_entities_t *, sizebuf_t *> CRehldsHook_SV_EmitEvents;
typedef IVoidHookChainRegistryImpl<IGameClient *, packet_entities_t *, sizebuf_t *> CRehldsHookRegistry_SV_EmitEvents;

//EV_PlayReliableEvent hook
typedef IVoidHookChainImpl<IGameClient *, int, short unsigned int, float, event_args_t *> CRehldsHook_EV_PlayReliableEvent;
typedef IVoidHookChainRegistryImpl<IGameClient *, int, short unsigned int, float, event_args_t *> CRehldsHookRegistry_EV_PlayReliableEvent;

//SV_StartSound hook
typedef IVoidHookChainImpl<int , edict_t *, int, const char *, int, float, int, int> CRehldsHook_SV_StartSound;
typedef IVoidHookChainRegistryImpl<int , edict_t *, int, const char *, int, float, int, int> CRehldsHookRegistry_SV_StartSound;

//PF_Remove_I hook
typedef IVoidHookChainImpl<edict_t *> CRehldsHook_PF_Remove_I;
typedef IVoidHookChainRegistryImpl<edict_t *> CRehldsHookRegistry_PF_Remove_I;

//PF_BuildSoundMsg_I hook
typedef IVoidHookChainImpl<edict_t *, int, const char *, float, float, int, int, int, int, const float *, edict_t *> CRehldsHook_PF_BuildSoundMsg_I;
typedef IVoidHookChainRegistryImpl<edict_t *, int, const char *, float, float, int, int, int, int, const float *, edict_t *> CRehldsHookRegistry_PF_BuildSoundMsg_I;

//SV_WriteFullClientUpdate hook
typedef IVoidHookChainImpl<IGameClient *, char *, size_t, sizebuf_t *, IGameClient *> CRehldsHook_SV_WriteFullClientUpdate;
typedef IVoidHookChainRegistryImpl<IGameClient *, char *, size_t, sizebuf_t *, IGameClient *> CRehldsHookRegistry_SV_WriteFullClientUpdate;

//SV_CheckConsistencyResponse hook
typedef IHookChainImpl<bool, IGameClient *, resource_t *, uint32> CRehldsHook_SV_CheckConsistencyResponse;
typedef IHookChainRegistryImpl<bool, IGameClient *, resource_t *, uint32> CRehldsHookRegistry_SV_CheckConsistencyResponse;

//SV_DropClient hook
typedef IVoidHookChainImpl<IGameClient*, bool, const char*> CRehldsHook_SV_DropClient;
typedef IVoidHookChainRegistryImpl<IGameClient*, bool, const char*> CRehldsHookRegistry_SV_DropClient;

//SV_ActivateServer hook
typedef IVoidHookChainImpl<int> CRehldsHook_SV_ActivateServer;
typedef IVoidHookChainRegistryImpl<int> CRehldsHookRegistry_SV_ActivateServer;

//SV_WriteVoiceCodec hook
typedef IVoidHookChainImpl<sizebuf_t *> CRehldsHook_SV_WriteVoiceCodec;
typedef IVoidHookChainRegistryImpl<sizebuf_t *> CRehldsHookRegistry_SV_WriteVoiceCodec;

//Steam_GSGetSteamID hook
typedef IHookChainImpl<uint64> CRehldsHook_Steam_GSGetSteamID;
typedef IHookChainRegistryImpl<uint64> CRehldsHookRegistry_Steam_GSGetSteamID;

//SV_TransferConsistencyInfo hook
typedef IHookChainImpl<int> CRehldsHook_SV_TransferConsistencyInfo;
typedef IHookChainRegistryImpl<int> CRehldsHookRegistry_SV_TransferConsistencyInfo;

//Steam_GSBUpdateUserData hook
typedef IHookChainImpl<bool, uint64, const char *, uint32> CRehldsHook_Steam_GSBUpdateUserData;
typedef IHookChainRegistryImpl<bool, uint64, const char *, uint32> CRehldsHookRegistry_Steam_GSBUpdateUserData;

//Cvar_DirectSet hook
typedef IVoidHookChainImpl<struct cvar_s *, const char *> CRehldsHook_Cvar_DirectSet;
typedef IVoidHookChainRegistryImpl<struct cvar_s *, const char *> CRehldsHookRegistry_Cvar_DirectSet;

//SV_EstablishTimeBase hook
typedef IVoidHookChainImpl<IGameClient *, usercmd_t *, int, int, int> CRehldsHook_SV_EstablishTimeBase;
typedef IVoidHookChainRegistryImpl<IGameClient *, usercmd_t *, int, int, int> CRehldsHookRegistry_SV_EstablishTimeBase;

//SV_Spawn_f hook
typedef IVoidHookChainImpl<> CRehldsHook_SV_Spawn_f;
typedef IVoidHookChainRegistryImpl<> CRehldsHookRegistry_SV_Spawn_f;

//SV_CreatePacketEntities hook
typedef IHookChainImpl<int, sv_delta_t, IGameClient *, packet_entities_t *, sizebuf_t *> CRehldsHook_SV_CreatePacketEntities;
typedef IHookChainRegistryImpl<int, sv_delta_t, IGameClient *, packet_entities_t *, sizebuf_t *> CRehldsHookRegistry_SV_CreatePacketEntities;

//SV_EmitSound2 hook
typedef IHookChainImpl<bool, edict_t *, IGameClient *, int, const char*, float, float, int, int, int, const float*> CRehldsHook_SV_EmitSound2;
typedef IHookChainRegistryImpl<bool, edict_t *, IGameClient *, int, const char*, float, float, int, int, int, const float*> CRehldsHookRegistry_SV_EmitSound2;

class CRehldsHookchains : public IRehldsHookchains {
public:
	CRehldsHookRegistry_Steam_NotifyClientConnect m_Steam_NotifyClientConnect;
	CRehldsHookRegistry_SV_ConnectClient m_SV_ConnectClient;
	CRehldsHookRegistry_SV_GetIDString m_SV_GetIDString;
	CRehldsHookRegistry_SV_SendServerinfo m_SV_SendServerinfo;
	CRehldsHookRegistry_SV_CheckProtocol m_SV_CheckProtocol;
	CRehldsHookRegistry_SVC_GetChallenge_mod m_SVC_GetChallenge_mod;
	CRehldsHookRegistry_SV_CheckKeyInfo m_SV_CheckKeyInfo;
	CRehldsHookRegistry_SV_CheckIPRestrictions m_SV_CheckIPRestrictions;
	CRehldsHookRegistry_SV_FinishCertificateCheck m_SV_FinishCertificateCheck;
	CRehldsHookRegistry_Steam_NotifyBotConnect m_Steam_NotifyBotConnect;
	CRehldsHookRegistry_SerializeSteamId m_SerializeSteamId;
	CRehldsHookRegistry_SV_CompareUserID m_SV_CompareUserID;
	CRehldsHookRegistry_Steam_NotifyClientDisconnect m_Steam_NotifyClientDisconnect;
	CRehldsHookRegistry_PreprocessPacket m_PreprocessPacket;
	CRehldsHookRegistry_ValidateCommand m_ValidateCommand;
	CRehldsHookRegistry_ClientConnected m_ClientConnected;
	CRehldsHookRegistry_HandleNetCommand m_HandleNetCommand;
	CRehldsHookRegistry_Mod_LoadBrushModel m_Mod_LoadBrushModel;
	CRehldsHookRegistry_Mod_LoadStudioModel m_Mod_LoadStudioModel;
	CRehldsHookRegistry_ExecuteServerStringCmd m_ExecuteServerStringCmd;
	CRehldsHookRegistry_SV_EmitEvents m_SV_EmitEvents;
	CRehldsHookRegistry_EV_PlayReliableEvent m_EV_PlayReliableEvent;
	CRehldsHookRegistry_SV_StartSound m_SV_StartSound;
	CRehldsHookRegistry_PF_Remove_I m_PF_Remove_I;
	CRehldsHookRegistry_PF_BuildSoundMsg_I m_PF_BuildSoundMsg_I;
	CRehldsHookRegistry_SV_WriteFullClientUpdate m_SV_WriteFullClientUpdate;
	CRehldsHookRegistry_SV_CheckConsistencyResponse m_SV_CheckConsistencyResponse;
	CRehldsHookRegistry_SV_DropClient m_SV_DropClient;
	CRehldsHookRegistry_SV_ActivateServer m_SV_ActivateServer;
	CRehldsHookRegistry_SV_WriteVoiceCodec m_SV_WriteVoiceCodec;
	CRehldsHookRegistry_Steam_GSGetSteamID m_Steam_GSGetSteamID;
	CRehldsHookRegistry_SV_TransferConsistencyInfo m_SV_TransferConsistencyInfo;
	CRehldsHookRegistry_Steam_GSBUpdateUserData m_Steam_GSBUpdateUserData;
	CRehldsHookRegistry_Cvar_DirectSet m_Cvar_DirectSet;
	CRehldsHookRegistry_SV_EstablishTimeBase m_SV_EstablishTimeBase;
	CRehldsHookRegistry_SV_Spawn_f m_SV_Spawn_f;
	CRehldsHookRegistry_SV_CreatePacketEntities m_SV_CreatePacketEntities;
	CRehldsHookRegistry_SV_EmitSound2 m_SV_EmitSound2;

public:
	virtual IRehldsHookRegistry_Steam_NotifyClientConnect* Steam_NotifyClientConnect();
	virtual IRehldsHookRegistry_SV_ConnectClient* SV_ConnectClient();
	virtual IRehldsHookRegistry_SV_GetIDString* SV_GetIDString();
	virtual IRehldsHookRegistry_SV_SendServerinfo* SV_SendServerinfo();
	virtual IRehldsHookRegistry_SV_CheckProtocol* SV_CheckProtocol();
	virtual IRehldsHookRegistry_SVC_GetChallenge_mod* SVC_GetChallenge_mod();
	virtual IRehldsHookRegistry_SV_CheckKeyInfo* SV_CheckKeyInfo();
	virtual IRehldsHookRegistry_SV_CheckIPRestrictions* SV_CheckIPRestrictions();
	virtual IRehldsHookRegistry_SV_FinishCertificateCheck* SV_FinishCertificateCheck();
	virtual IRehldsHookRegistry_Steam_NotifyBotConnect* Steam_NotifyBotConnect();
	virtual IRehldsHookRegistry_SerializeSteamId* SerializeSteamId();
	virtual IRehldsHookRegistry_SV_CompareUserID* SV_CompareUserID();
	virtual IRehldsHookRegistry_Steam_NotifyClientDisconnect* Steam_NotifyClientDisconnect();
	virtual IRehldsHookRegistry_PreprocessPacket* PreprocessPacket();
	virtual IRehldsHookRegistry_ValidateCommand* ValidateCommand();
	virtual IRehldsHookRegistry_ClientConnected* ClientConnected();
	virtual IRehldsHookRegistry_HandleNetCommand* HandleNetCommand();
	virtual IRehldsHookRegistry_Mod_LoadBrushModel* Mod_LoadBrushModel();
	virtual IRehldsHookRegistry_Mod_LoadStudioModel* Mod_LoadStudioModel();
	virtual IRehldsHookRegistry_ExecuteServerStringCmd* ExecuteServerStringCmd();
	virtual IRehldsHookRegistry_SV_EmitEvents* SV_EmitEvents();
	virtual IRehldsHookRegistry_EV_PlayReliableEvent* EV_PlayReliableEvent();
	virtual IRehldsHookRegistry_SV_StartSound* SV_StartSound();
	virtual IRehldsHookRegistry_PF_Remove_I* PF_Remove_I();
	virtual IRehldsHookRegistry_PF_BuildSoundMsg_I* PF_BuildSoundMsg_I();
	virtual IRehldsHookRegistry_SV_WriteFullClientUpdate* SV_WriteFullClientUpdate();
	virtual IRehldsHookRegistry_SV_CheckConsistencyResponse* SV_CheckConsistencyResponse();
	virtual	IRehldsHookRegistry_SV_DropClient* SV_DropClient();
	virtual	IRehldsHookRegistry_SV_ActivateServer* SV_ActivateServer();
	virtual	IRehldsHookRegistry_SV_WriteVoiceCodec* SV_WriteVoiceCodec();
	virtual IRehldsHookRegistry_Steam_GSGetSteamID* Steam_GSGetSteamID();
	virtual IRehldsHookRegistry_SV_TransferConsistencyInfo* SV_TransferConsistencyInfo();
	virtual IRehldsHookRegistry_Steam_GSBUpdateUserData* Steam_GSBUpdateUserData();
	virtual IRehldsHookRegistry_Cvar_DirectSet* Cvar_DirectSet();
	virtual IRehldsHookRegistry_SV_EstablishTimeBase* SV_EstablishTimeBase();
	virtual	IRehldsHookRegistry_SV_Spawn_f* SV_Spawn_f();
	virtual	IRehldsHookRegistry_SV_CreatePacketEntities* SV_CreatePacketEntities();
	virtual	IRehldsHookRegistry_SV_EmitSound2* SV_EmitSound2();
};

extern CRehldsHookchains g_RehldsHookchains;
extern RehldsFuncs_t g_RehldsApiFuncs;
extern CRehldsServerStatic g_RehldsServerStatic;
extern CRehldsServerData g_RehldsServerData;


class CRehldsApi : public IRehldsApi {
public:
	virtual int GetMajorVersion();
	virtual int GetMinorVersion();
	virtual const RehldsFuncs_t* GetFuncs();
	virtual IRehldsHookchains* GetHookchains();
	virtual IRehldsServerStatic* GetServerStatic();
	virtual IRehldsServerData* GetServerData();
	virtual IRehldsFlightRecorder* GetFlightRecorder();
};

extern sizebuf_t* GetNetMessage_api();
extern IGameClient* GetHostClient_api();
extern int* GetMsgReadCount_api();

bool SV_EmitSound2_api(edict_t *entity, IGameClient *receiver, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int emitFlags, const float *pOrigin);
