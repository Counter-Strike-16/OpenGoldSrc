#include "engine/eiface.h"

namespace
{
int EngFunc_PrecacheModel(char *s)
{
};

int EngFunc_PrecacheSound(char *s)
{
};

int EngFunc_ModelIndex(const char *m)
{
};

int EngFunc_ModelFrames(int modelIndex)
{
};

void EngFunc_ChangeLevel(char *s1, char *s2)
{
};

float EngFunc_VecToYaw(const float *rgflVector)
{
};

void EngFunc_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
};

edict_t *EngFunc_FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
};

edict_t *EngFunc_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
};

edict_t *EngFunc_FindClientInPVS(edict_t *pEdict)
{
};

edict_t *EngFunc_EntitiesInPVS(edict_t *pplayer)
{
};

void EngFunc_MakeVectors(const float *rgflVector)
{
};

void EngFunc_AngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
};

edict_t *EngFunc_CreateEntity()
{
};

void EngFunc_GetAimVector(edict_t *ent, float speed, float *rgflReturn)
{
};

void EngFunc_ServerCommand(char *str)
{
};

void EngFunc_ServerExecute()
{
};

void EngFunc_ClientCommand(edict_t *pEdict, char *szFmt, ...)
{
};

void EngFunc_ParticleEffect(const float *org, const float *dir, float color, float count)
{
};

void EngFunc_LightStyle(int style, char *val)
{
};

int EngFunc_DecalIndex(const char *name)
{
};

int EngFunc_PointContents(const float *rgflVector)
{
};

void EngFunc_MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
};

void EngFunc_MessageEnd()
{
};

void EngFunc_WriteByte(int iValue)
{
};

void EngFunc_WriteChar(int iValue)
{
};

void EngFunc_WriteShort(int iValue)
{
};

void EngFunc_WriteLong(int iValue)
{
};

void EngFunc_WriteAngle(float flValue)
{
};

void EngFunc_WriteCoord(float flValue)
{
};

void EngFunc_WriteString(const char *sz)
{
};

void EngFunc_WriteEntity(int iValue)
{
};

void EngFunc_CVarRegister(cvar_t *pCvar)
{
};

float EngFunc_CVarGetFloat(const char *szVarName)
{
};

const char *EngFunc_CVarGetString(const char *szVarName)
{
};

void EngFunc_CVarSetFloat(const char *szVarName, float flValue)
{
};

void EngFunc_CVarSetString(const char *szVarName, const char *szValue)
{
};

void EngFunc_AlertMessage(ALERT_TYPE atype, char *szFmt, ...)
{
};

void EngFunc_EngineFprintf(void *pfile, char *szFmt, ...)
{
};

void *EngFunc_PvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
};

void *EngFunc_PvEntPrivateData(edict_t *pEdict)
{
};

void EngFunc_FreeEntPrivateData(edict_t *pEdict)
{
};

const char *EngFunc_SzFromIndex(int iString)
{
};

int EngFunc_AllocString(const char *szValue)
{
};

edict_t *EngFunc_PEntityOfEntOffset(int iEntOffset)
{
};

int EngFunc_EntOffsetOfPEntity(const edict_t *pEdict)
{
};

int EngFunc_IndexOfEdict(const edict_t *pEdict)
{
};

edict_t *EngFunc_PEntityOfEntIndex(int iEntIndex)
{
};

edict_t *EngFunc_FindEntityByVars(struct entvars_s *pvars)
{
};

int EngFunc_RegUserMsg(const char *pszName, int iSize)
{
};

uint32 EngFunc_FunctionFromName(const char *pName)
{
};

const char *EngFunc_NameForFunction(uint32 function)
{
};

void EngFunc_ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
};

void EngFunc_ServerPrint(const char *szMsg)
{
};

const char *EngFunc_Cmd_Args()
{
};

const char *EngFunc_Cmd_Argv(int argc)
{
};

int EngFunc_Cmd_Argc()
{
};

void EngFunc_CRC32_Init(CRC32_t *pulCRC)
{
};

void EngFunc_CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
};

void EngFunc_CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
};

CRC32_t EngFunc_CRC32_Final(CRC32_t pulCRC)
{
};

int32 EngFunc_RandomLong(int32lLow,int32lHigh)
{
};

float EngFunc_RandomFloat(float flLow, float flHigh)
{
};

void EngFunc_SetView(const edict_t *pClient, const edict_t *pViewent)
{
};

float EngFunc_Time()
{
};

void EngFunc_CrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
};

byte *EngFunc_LoadFileForMe(char *filename, int *pLength)
{
};

void EngFunc_FreeFile(void *buffer)
{
};

void EngFunc_EndSection(const char *pszSectionName)
{
};

int EngFunc_CompareFileTime(char *filename1, char *filename2, int *iCompare)
{
};

void EngFunc_GetGameDir(char *szGetGameDir)
{
};

void EngFunc_Cvar_RegisterVariable(cvar_t *variable)
{
};

void EngFunc_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
};

void EngFunc_SetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
};

edict_t *EngFunc_CreateFakeClient(const char *netname)
{
};

void EngFunc_RunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec)
{
};

int EngFunc_NumberOfEntities()
{
};

char *EngFunc_GetInfoKeyBuffer(edict_t *e)
{
};

char *EngFunc_InfoKeyValue(char *infobuffer, char *key)
{
};

void EngFunc_SetKeyValue(char *infobuffer, char *key, char *value)
{
};

void EngFunc_SetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
};

int EngFunc_IsMapValid(char *filename)
{
};

void EngFunc_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
};

int EngFunc_PrecacheGeneric(char *s)
{
};

int EngFunc_GetPlayerUserId(edict_t *e)
{
};

void EngFunc_BuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
};

int EngFunc_IsDedicatedServer()
{
};

cvar_t *EngFunc_CVarGetPointer(const char *szVarName)
{
};

unsigned int EngFunc_GetPlayerWONId(edict_t *e)
{
};

void EngFunc_Info_RemoveKey(char *s, const char *key)
{
};

const char *EngFunc_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
};

void EngFunc_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
};

const char *EngFunc_GetPhysicsInfoString(const edict_t *pClient)
{
};

unsigned short EngFunc_PrecacheEvent(int type, const char*psz)
{
};

void EngFunc_PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
};

unsigned char *EngFunc_SetFatPVS(float *org)
{
};

unsigned char *EngFunc_SetFatPAS(float *org)
{
};

int EngFunc_CheckVisibility(const edict_t *entity, unsigned char *pset)
{
};

void EngFunc_DeltaSetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaAddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

int EngFunc_GetCurrentPlayer()
{
};

int EngFunc_CanSkipPlayer(const edict_t *player)
{
};

int EngFunc_DeltaFindField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void EngFunc_DeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void EngFunc_SetGroupMask(int mask, int op)
{
};

int EngFunc_CreateInstancedBaseline( int classname, struct entity_state_s *baseline)
{
};

void EngFunc_Cvar_DirectSet(struct cvar_s *var, char *value)
{
};

void EngFunc_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
};

void EngFunc_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
};

void EngFunc_AddServerCommand(char *cmd_name, void (*function)())
{
};

qboolean EngFunc_Voice_GetClientListening(int iReceiver, int iSender)
{
};

qboolean EngFunc_Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
};

const char *EngFunc_GetPlayerAuthId(edict_t *e)
{
};

//const char *EngFunc_KeyNameForBinding(const char *pBinding)
//{
//};

sequenceEntry_s *EngFunc_SequenceGet(const char *fileName, const char *entryName)
{
};

sentenceEntry_s *EngFunc_SequencePickSentence(const char *groupName, int pickMethod, int *picked)
{
};

int EngFunc_GetFileSize(char *filename)
{
};

unsigned int EngFunc_GetApproxWavePlayLen(const char *filepath)
{
};

int EngFunc_IsCareerMatch()
{
};

int EngFunc_GetLocalizedStringLength(const char *label)
{
};

void EngFunc_RegisterTutorMessageShown(int mid)
{
};

int EngFunc_GetTimesTutorMessageShown(int mid)
{
};

void EngFunc_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ResetTutorMessageDecayData()
{
};

void EngFunc_QueryClientCvarValue(const edict_t *player, const char *cvarName)
{
};

void EngFunc_QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
};

int EngFunc_CheckParm(const char *pchCmdLineToken, char **ppnext)
{
};
}; // namespace

enginefuncs_t gSvEngFuncs =
{
	EngFunc_
	EngFunc_PrecacheModel,
	EngFunc_PrecacheSound,
	
	EngFunc_SetModel,
	
	EngFunc_ModelIndex,
	EngFunc_ModelFrames,
	
	EngFunc_SetSize,
	
	EngFunc_ChangeLevel,
	
	EngFunc_GetSpawnParms,
	EngFunc_SaveSpawnParms,
	
	EngFunc_VecToYaw,
	EngFunc_VecToAngles,
	
	EngFunc_MoveToOrigin,
	
	EngFunc_ChangeYaw,
	EngFunc_ChangePitch,
	
	EngFunc_FindEntityByString,
	
	EngFunc_GetEntityIllum,
	
	EngFunc_FindEntityInSphere,
	EngFunc_FindClientInPVS,
	EngFunc_EntitiesInPVS,
	
	EngFunc_MakeVectors,
	EngFunc_AngleVectors,
	
	EngFunc_CreateEntity,
	EngFunc_RemoveEntity,
	EngFunc_CreateNamedEntity,
	
	EngFunc_MakeStatic,
	EngFunc_EntIsOnFloor,
	EngFunc_DropToFloor,
	EngFunc_WalkMove,
	EngFunc_SetOrigin,
	EngFunc_EmitSound,
	EngFunc_EmitAmbientSound,
	
	EngFunc_TraceLine,
	EngFunc_TraceToss,
	EngFunc_TraceMonsterHull,
	EngFunc_TraceHull,
	EngFunc_TraceModel,
	EngFunc_TraceTexture,
	EngFunc_TraceSphere,
	
	EngFunc_GetAimVector,
	
	EngFunc_ServerCommand,
	EngFunc_ServerExecute,
	EngFunc_ClientCommand,
	
	EngFunc_ParticleEffect,
	EngFunc_LightStyle,
	
	EngFunc_DecalIndex,
	
	EngFunc_PointContents,
	
	EngFunc_MessageBegin,
	EngFunc_MessageEnd,
	
	EngFunc_WriteByte,
	EngFunc_WriteChar,
	EngFunc_WriteShort,
	EngFunc_WriteLong,
	EngFunc_WriteAngle,
	EngFunc_WriteCoord,
	EngFunc_WriteString,
	EngFunc_WriteEntity,
	
	EngFunc_CVarRegister
	EngFunc_CVarGetFloat,
	EngFunc_CVarGetString,
	EngFunc_CVarSetFloat,
	EngFunc_CVarSetString,
	
	EngFunc_AlertMessage,
	EngFunc_EngineFprintf,
	
	EngFunc_PvAllocEntPrivateData,
	EngFunc_PvEntPrivateData,
	EngFunc_FreeEntPrivateData,
	
	EngFunc_SzFromIndex,
	EngFunc_AllocString,
	
	EngFunc_GetVarsOfEnt,
	
	EngFunc_PEntityOfEntOffset,
	EngFunc_EntOffsetOfPEntity,
	EngFunc_IndexOfEdict,
	EngFunc_PEntityOfEntIndex,
	EngFunc_FindEntityByVars,
	
	EngFunc_GetModelPtr,
	
	EngFunc_RegUserMsg,
	
	EngFunc_AnimationAutomove,
	EngFunc_GetBonePosition,
	
	EngFunc_FunctionFromName,
	EngFunc_NameForFunction,
	
	EngFunc_ClientPrintf,
	EngFunc_ServerPrint,
	
	EngFunc_Cmd_Args,
	EngFunc_Cmd_Argv,
	EngFunc_Cmd_Argc,
	
	EngFunc_GetAttachment,
	
	EngFunc_CRC32_Init,
	EngFunc_CRC32_ProcessBuffer,
	EngFunc_CRC32_ProcessByte,
	EngFunc_CRC32_Final,
	
	EngFunc_RandomLong,
	EngFunc_RandomFloat,
	
	EngFunc_SetView,
	
	EngFunc_Time,
	
	EngFunc_CrosshairAngle,
	
	EngFunc_LoadFileForMe,
	EngFunc_FreeFile,
	
	EngFunc_EndSection,
	
	EngFunc_CompareFileTime,
	
	EngFunc_GetGameDir,
	
	EngFunc_Cvar_RegisterVariable,
	
	EngFunc_FadeClientVolume,
	EngFunc_SetClientMaxspeed,
	
	EngFunc_CreateFakeClient,
	
	EngFunc_RunPlayerMove,
	
	EngFunc_NumberOfEntities,
	
	EngFunc_GetInfoKeyBuffer,
	EngFunc_InfoKeyValue,
	EngFunc_SetKeyValue,
	EngFunc_SetClientKeyValue,
	
	EngFunc_IsMapValid,
	
	EngFunc_StaticDecal,
	
	EngFunc_PrecacheGeneric,
	
	EngFunc_GetPlayerUserId,
	
	EngFunc_BuildSoundMsg,
	
	EngFunc_IsDedicatedServer,
	
	EngFunc_CVarGetPointer,
	
	EngFunc_GetPlayerWONId,
	
	EngFunc_Info_RemoveKey,
	
	EngFunc_GetPhysicsKeyValue,
	EngFunc_SetPhysicsKeyValue,
	EngFunc_GetPhysicsInfoString,
	
	EngFunc_PrecacheEvent,
	EngFunc_PlaybackEvent,
	
	EngFunc_SetFatPVS,
	EngFunc_SetFatPAS,
	
	EngFunc_CheckVisibility,
	
	EngFunc_DeltaSetField,
	EngFunc_DeltaUnsetField,
	EngFunc_DeltaAddEncoder,
	
	EngFunc_GetCurrentPlayer,
	
	EngFunc_CanSkipPlayer,
	
	EngFunc_DeltaFindField,
	EngFunc_DeltaSetFieldByIndex,
	EngFunc_DeltaUnsetFieldByIndex,
	
	EngFunc_SetGroupMask,
	
	EngFunc_CreateInstancedBaseline,
	
	EngFunc_Cvar_DirectSet,
	
	EngFunc_ForceUnmodified,
	
	EngFunc_GetPlayerStats,
	
	EngFunc_AddServerCommand,
	
	EngFunc_Voice_GetClientListening,
	EngFunc_Voice_SetClientListening,
	
	EngFunc_GetPlayerAuthId,
	
	EngFunc_SequenceGet,
	EngFunc_SequencePickSentence,
	
	EngFunc_GetFileSize,
	
	EngFunc_GetApproxWavePlayLen,
	
	EngFunc_IsCareerMatch,
	
	EngFunc_GetLocalizedStringLength,
	
	EngFunc_RegisterTutorMessageShown,
	EngFunc_GetTimesTutorMessageShown,
	EngFunc_ProcessTutorMessageDecayBuffer,
	EngFunc_ConstructTutorMessageDecayBuffer,
	EngFunc_ResetTutorMessageDecayData,
	
	EngFunc_QueryClientCvarValue,
	EngFunc_QueryClientCvarValue2,
	
	EngFunc_CheckParm
};