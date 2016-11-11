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

void EngFunc_ServerCommand(char *str)
{
};

void EngFunc_ServerExecute()
{
};

int EngFunc_DecalIndex(const char *name)
{
};

const char *EngFunc_SzFromIndex(int iString)
{
};

int EngFunc_AllocString(const char *szValue)
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

float EngFunc_Time()
{
};

void EngFunc_EndSection(const char *pszSectionName)
{
};

void EngFunc_GetGameDir(char *szGetGameDir)
{
};

int EngFunc_IsMapValid(char *filename)
{
};

int EngFunc_PrecacheGeneric(char *s)
{
};

int EngFunc_IsDedicatedServer()
{
};

unsigned short EngFunc_PrecacheEvent(int type, const char *psz)
{
};

int EngFunc_GetCurrentPlayer()
{
};

void EngFunc_SetGroupMask(int mask, int op)
{
};

void EngFunc_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
};

void EngFunc_AddServerCommand(char *cmd_name, void (*function)())
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

unsigned int EngFunc_GetApproxWavePlayLen(const char *filepath)
{
};

int EngFunc_IsCareerMatch()
{
};

int EngFunc_GetLocalizedStringLength(const char *label)
{
};

int EngFunc_CheckParm(const char *pchCmdLineToken, char **ppnext)
{
};
}; // namespace

enginefuncs_t gSvEngFuncsNull =
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