/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#pragma once

#include "public/interface.h"

const char OGS_ENGINESERVER_INTERFACE_VERSION[] = "OGSEngineServer001";

struct IEngineServer : public IBaseInterface
{
	virtual int PrecacheModel(const char *s) const = 0;
	virtual int PrecacheSound(const char *s) const = 0;
	
	virtual int GetModelIndex(const char *m) const = 0;
	virtual int GetModelFrames(int modelIndex) const = 0;
	
	virtual void ChangeLevel(const char *s1, const char *s2) = 0;
	
	virtual float VecToYaw(const float *rgflVector) const = 0;
	virtual void VecToAngles(const float *rgflVectorIn, float *rgflVectorOut) = 0;
	
	virtual edict_t *FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue) const = 0;
	
	virtual edict_t *FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad) const = 0;
	virtual edict_t *FindClientInPVS(edict_t *pEdict) const = 0;
	virtual edict_t *EntitiesInPVS(edict_t *pplayer) const = 0;
	
	virtual void MakeVectors(const float *rgflVector) = 0;
	virtual void AngleVectors(const float *rgflVector, float *forward, float *right, float *up) = 0;
	
	virtual edict_t *CreateEntity() const = 0;
	virtual void RemoveEntity(edict_t *e) = 0;
	virtual edict_t *CreateNamedEntity(int className) const = 0;
	
	virtual void EmitSound(edict_t *entity, int channel, const char *sample, /*int*/ float volume, float attenuation, int fFlags, int pitch) = 0;
	virtual void EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch) = 0;
	
	virtual void TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr) = 0;
	virtual void TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr) = 0;
	virtual int TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr) = 0;
	virtual void TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr) = 0;
	virtual void TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr) = 0;
	virtual const char *TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2) = 0;
	virtual void TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr) = 0;
	
	virtual void GetAimVector(edict_t *ent, float speed, float *rgflReturn) = 0;
	
	virtual void ServerCommand(char *str) = 0;
	virtual void ServerExecute() = 0;
	
	virtual void ClientCommand(edict_t *pEdict, char *szFmt, ...) = 0;
	
	virtual void SpawnParticleEffect(const float *org, const float *dir, float color, float count) = 0;
	virtual void SetLightStyle(int style, char *val) = 0;
	
	virtual int GetDecalIndex(const char *name) const = 0;
	virtual int GetPointContents(const float *rgflVector) const = 0;
	
	virtual INetMsg &MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) = 0;
	virtual void MessageEnd(INetMsg &apMsg) = 0;
	
	virtual void AlertMessage(ALERT_TYPE atype, char *szFmt, ...) = 0;
	
	virtual void EngineFprintf(void *pfile, char *szFmt, ...) = 0;
	
	virtual void *PvAllocEntPrivateData(edict_t *pEdict, int32 cb) = 0;
	virtual void *PvEntPrivateData(edict_t *pEdict) = 0;
	virtual void FreeEntPrivateData(edict_t *pEdict) = 0;
	
	virtual const char *SzFromIndex(int iString) = 0;
	virtual int AllocString(const char *szValue) = 0;
	
	virtual edict_t *PEntityOfEntOffset(int iEntOffset) = 0;
	virtual int EntOffsetOfPEntity(const edict_t *pEdict) = 0;
	
	virtual edict_t *PEntityOfEntIndex(int iEntIndex) const = 0;
	virtual edict_t *FindEntityByVars(struct entvars_s *pvars) const = 0;
	virtual void *GetModelPtr(edict_t *pEdict) const = 0;
	virtual int RegUserMsg(const char *pszName, int iSize) const = 0;
	virtual void AnimationAutomove(const edict_t *pEdict, float flTime) = 0;
	virtual void GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles) = 0;
	virtual uint32 FunctionFromName(const char *pName) = 0;
	virtual const char *NameForFunction(uint32 function) = 0;
	virtual void ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg) = 0; // JOHN: engine callbacks so game DLL can print messages to individual clients
	virtual void ServerPrint(const char *szMsg) = 0;
	virtual const char *Cmd_Args() = 0;     // these 3 added
	virtual const char *Cmd_Argv(int argc) = 0; // so game DLL can easily
	virtual int Cmd_Argc() = 0;             // access client 'cmd' strings
	virtual void GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles) = 0;
	
	virtual void CRC32_Init(CRC32_t *pulCRC) = 0;
	virtual void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len) = 0;
	virtual void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch) = 0;
	virtual CRC32_t CRC32_Final(CRC32_t pulCRC) = 0;
	
	virtual int32 GetRandomLong(int32 lLow, int32 lHigh) const = 0;
	virtual float GetRandomFloat(float flLow, float flHigh) const = 0;
	
	virtual void SetView(const edict_t *pClient, const edict_t *pViewent) = 0;
	
	virtual float GetTime() const = 0;
	
	virtual void SetCrosshairAngle(const edict_t *pClient, float pitch, float yaw) = 0;
	
	virtual byte *LoadFileForMe(char *filename, int *pLength) const = 0;
	virtual void FreeFile(void *buffer) = 0;
	
	virtual void EndSection(const char *pszSectionName) = 0; // trigger_endsection
	
	virtual int CompareFileTime(char *filename1, char *filename2, int *iCompare) = 0;
	
	virtual void GetGameDir(char *szGetGameDir) = 0;
	
	virtual void FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds) = 0;
	virtual void SetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed) = 0;
	virtual edict_t *CreateFakeClient(const char *netname) const = 0; // returns NULL if fake client can't be created
	virtual void RunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec) = 0;
	virtual int GetNumberOfEntities() const = 0;
	virtual char *GetInfoKeyBuffer(edict_t *e) const = 0; // passing in NULL gets the serverinfo
	virtual char *InfoKeyValue(char *infobuffer, char *key) const = 0;
	virtual void SetKeyValue(char *infobuffer, char *key, char *value) = 0;
	virtual void SetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value) = 0;
	virtual int IsMapValid(char *filename) const = 0;
	virtual void StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex) = 0;
	virtual int PrecacheGeneric(char *s) const = 0;
	virtual int GetPlayerUserId(edict_t *e) const = 0; // returns the server assigned userid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
	virtual void BuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/ float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) = 0;
	virtual int IsDedicatedServer() const = 0;
	
	virtual unsigned int GetPlayerWONId(edict_t *e) = 0; // returns the server assigned WONid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients

	virtual void Info_RemoveKey(char *s, const char *key) = 0;
	virtual const char *GetPhysicsKeyValue(const edict_t *pClient, const char *key) = 0;
	virtual void SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value) = 0;
	virtual const char *GetPhysicsInfoString(const edict_t *pClient) = 0;
	virtual unsigned short PrecacheEvent(int type, const char *psz) = 0;
	virtual void PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;

	virtual unsigned char *SetFatPVS(float *org) const = 0;
	virtual unsigned char *SetFatPAS(float *org) const = 0;

	virtual int CheckVisibility(const edict_t *entity, unsigned char *pset) = 0;

	virtual void DeltaSetField(struct delta_s *pFields, const char *fieldname) = 0;
	virtual void DeltaUnsetField(struct delta_s *pFields, const char *fieldname) = 0;
	virtual void DeltaAddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)) = 0;
	virtual int GetCurrentPlayer() = 0;
	virtual int CanSkipPlayer(const edict_t *player) = 0;
	virtual int DeltaFindField(struct delta_s *pFields, const char *fieldname) = 0;
	virtual void DeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber) = 0;
	virtual void DeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber) = 0;

	virtual void SetGroupMask(int mask, int op) = 0;

	virtual int CreateInstancedBaseline(int classname, struct entity_state_s *baseline) = 0;
	virtual void Cvar_DirectSet(struct cvar_s *var, char *value) = 0;

	virtual void ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename) = 0;

	virtual void GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss) = 0;

	virtual void AddServerCommand(char *cmd_name, void (*function)()) = 0;

	virtual const char *GetPlayerAuthId(edict_t *e) const = 0;

	// PSV: Added for CZ training map
	//	const char *KeyNameForBinding)		( const char* pBinding );

	virtual /*void*/ sequenceEntry_s *SequenceGet(const char *fileName, const char *entryName) = 0;
	virtual /*void*/ sentenceEntry_s *SequencePickSentence(const char *groupName, int pickMethod, int *picked) = 0;

	virtual unsigned int GetApproxWavePlayLen(const char *filepath) const = 0;
	
	virtual int IsCareerMatch() const = 0;

	virtual int GetLocalizedStringLength(const char *label) const = 0;

	virtual void RegisterTutorMessageShown(int mid) = 0;
	virtual int GetTimesTutorMessageShown(int mid) const = 0;
	virtual void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength) = 0;
	virtual void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength) = 0;
	virtual void ResetTutorMessageDecayData() = 0;

	virtual void QueryClientCvarValue(const edict_t *player, const char *cvarName) = 0;
	virtual void QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID) = 0;

	virtual int CheckParm(const char *pchCmdLineToken, char **ppnext) = 0;
};