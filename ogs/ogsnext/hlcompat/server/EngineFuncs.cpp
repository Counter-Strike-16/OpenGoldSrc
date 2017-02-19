/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2015-2017 OGS Dev Team
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

// ServerEngineFuncs.cpp - source of engine functions set for exposing to game dll

#include "game/ServerEngineFuncs.hpp"
#include "resources/IModelManager.hpp"
#include "resources/ISoundManager.hpp"
#include "EngineServer.hpp"
#include "game/GameClientHandler.hpp"
#include "world/EntityHandler.hpp"
#include "world/GameWorld.hpp"
#include "console/Console.hpp"
#include "console/CmdBuffer.hpp"
#include "math/Math.hpp"
#include "debug/Debug.hpp"
#include "engine/eiface.h"

IModelManager *gpModelManager = nullptr;
ISoundManager *gpSoundManager = nullptr;
CEngineServer *gpEngineServer = nullptr;
CClientHandler *gpClientHandler = nullptr;
CEntityHandler *gpEntityHandler = nullptr;
CGameWorld *gpGameWorld = nullptr;
CConsole *gpConsole = nullptr;
CCRC32Handler *gpCRC32Handler = nullptr;
CCmdBuffer *gpCmdBuffer = nullptr;
globalvars_t *gpGlobals = nullptr;

// Engine callbacks
static enginefuncs_t gEngFuncs = 
{
	EngFunc_PrecacheModel,
	EngFunc_PrecacheSound,
	EngFunc_SetModel,
	EngFunc_GetModelIndex,
	EngFunc_GetModelFrames,
	EngFunc_SetSize,	
	EngFunc_ChangeLevel,
	EngFunc_GetSpawnParms,
	EngFunc_SaveSpawnParms,
	EngFunc_VecToYaw,
	EngFunc_VectorAngles,
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
	EngFunc_AllocEdict,
	EngFunc_RemoveEntity,
	EngFunc_CreateNamedEntity,
	EngFunc_MakeStatic,
	EngFunc_EntIsOnFloor,
	EngFunc_DropToFloor,
	EngFunc_WalkMove,
	EngFunc_SetOrigin,
	EngFunc_StartSound,
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
	EngFunc_ExecCmdBuffer,
	EngFunc_ClientCommand,
	EngFunc_ParticleEffect,
	EngFunc_LightStyle,
	EngFunc_GetDecalIndex,
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
	EngFunc_RegisterVariable,
	EngFunc_VariableValue,
	EngFunc_VariableString,
	EngFunc_SetFloat,
	EngFunc_Set,
	EngFunc_AlertMessage,
	EngFunc_EngineFprintf,
	EngFunc_PvAllocEntPrivateData,
	EngFunc_PvEntPrivateData,
	EngFunc_FreeEntPrivateData,
	EngFunc_StringFromIndex,
	EngFunc_AllocString,
	EngFunc_GetVarsOfEnt,
	EngFunc_PEntityOfEntOffset,
	EngFunc_GetEntityOffset,
	EngFunc_GetEntityIndex,
	EngFunc_PEntityOfEntIndex,
	EngFunc_FindEntityByVars,
	EngFunc_GetModelPtr,
	EngFunc_RegUserMsg,
	EngFunc_AnimationAutomove,
	EngFunc_GetBonePosition,
	EngFunc_FunctionFromName,
	EngFunc_NameForFunction,
	EngFunc_ClientPrintf,
	EngFunc_BroadcastPrint,
	EngFunc_GetCmdArgs,
	EngFunc_GetCmdArgv,
	EngFunc_GetCmdArgc,
	EngFunc_GetAttachment,
	EngFunc_CRC32Init,
	EngFunc_CRC32ProcessBuffer,
	EngFunc_CRC32ProcessByte,
	EngFunc_CRC32_Final,
	EngFunc_RandomLong,
	EngFunc_RandomFloat,
	EngFunc_SetView,
	EngFunc_GetTime,
	EngFunc_CrosshairAngle,
	EngFunc_LoadFileForMe,
	EngFunc_FreeFile,
	EngFunc_EndSection,
	EngFunc_CompareFileTime,
	EngFunc_GetGameDir,
	EngFunc_RegisterVariable,
	EngFunc_FadeClientVolume,
	EngFunc_SetClientMaxspeed,
	EngFunc_FakeConnect,
	EngFunc_RunPlayerMove,
	EngFunc_NumberOfEntities,
	EngFunc_GetInfoKeyBuffer,
	EngFunc_GetKeyValue,
	EngFunc_SetKeyValue,
	EngFunc_SetClientKeyValue,
	EngFunc_IsMapValid,
	EngFunc_StaticDecal,
	EngFunc_GetGenericIndex,
	EngFunc_GetPlayerUserId,
	EngFunc_BuildSoundMsg,
	EngFunc_IsDedicatedServer,
	EngFunc_GetCVarPointer,
	EngFunc_GetPlayerWONId,
	EngFunc_RemoveKey,
	EngFunc_GetPhysicsKeyValue,
	EngFunc_SetPhysicsKeyValue,
	EngFunc_GetPhysicsInfoString,
	EngFunc_PrecacheEvent,
	EngFunc_PlaybackEvent,
	EngFunc_SetFatPVS,
	EngFunc_SetFatPAS,
	EngFunc_CheckVisibility,
	EngFunc_SetDeltaField,
	EngFunc_UnsetDeltaField,
	EngFunc_AddDeltaEncoder,
	EngFunc_GetCurrentPlayer,
	EngFunc_CanSkipPlayer,
	EngFunc_FindDeltaField,
	EngFunc_SetDeltaFieldByIndex,
	EngFunc_UnsetDeltaFieldByIndex,
	EngFunc_SetGroupMask,	
	EngFunc_CreateInstancedBaseline,
	EngFunc_DirectSet,
	EngFunc_ForceUnmodified,
	EngFunc_GetPlayerStats,
	EngFunc_AddGameCommand,
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

enginefuncs_t *gpEngFuncs = &gEngFuncs;

CEngineFuncs::CEngineFuncs()
{
	LogMsg("Constructing the server engine functions component...");
};

CEngineFuncs::~CEdictHandler()
{
	LogMsg("Destructing the server engine functions component...");
};

void CEngineFuncs::Init(IModelManager *apModelManager, ISoundManager *apSoundManager, CEngineServer *apEngineServer, CEdictHandler *apEdictHandler, CGameWorld *apGameWorld)
{
	LogMsg("Initializing the server engine functions component...");
	
	gpModelManager = apModelManager;
	gpSoundManager = apSoundManager;
	gpEngineServer = apEngineServer;
	gpEdictHandler = apEdictHandler;
	gpGameWorld = apGameWorld;
	gpPhysicsWorld = apPhysicsWorld;
};

static int EngFunc_PrecacheModel(char *asName)
{
	int nModelIndex = SV_ModelIndex(asName);
	
	Mod_RegisterModel(asName, nModelIndex);
	
	//return modelIndex;
	return gpModelManager->PrecacheModel(asName);
};

static int EngFunc_PrecacheSound(char *asName)
{
	return gpSoundManager->PrecacheSound(asName);
};

static void EngFunc_SetModel(edict_t *apEntity, const char *asName)
{
	if(!SV_IsValidEdict(e))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(e));
		return;
	};
	
	if(!asName || asName[0] <= ' ')
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": null name");
		return;
	};
	
	gpEdictHandler->ToEdictClass(apEntity)->SetModel(asName);
};

static int EngFunc_GetModelIndex(const char *asName)
{
	if(!asName || !asName[0])
		return 0;
	
	for(int i = 1; i < MAX_MODELS && sv.model_precache[i][0]; i++)
		if(!stricmp(sv.model_precache[i], asName))
			return i;
	
	DevMsg(eMsgType_Error, __FUNCTION__ ": %s is not precached!", asName);
	//return 0;
	
	return gpModelManager->GetModelIndex(asName);
};

static int EngFunc_GetModelFrames(int anModelIndex)
{
	int	nFrames = 0;
	
	Mod_GetFrames(anModelIndex, &nFrames);
	
	//return numFrames;
	return gpModelManager->GetModelByIndex(anModelIndex)->GetFrames();
};

static void EngFunc_SetSize(edict_t *apEntity, const float *avMin, const float *avMax)
{
	if(!SV_IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEntity));
		return;
	};
	
	gpEdictHandler->ToEdictClass(apEntity)->SetSize(avMin, avMax);
};

static void EngFunc_ChangeLevel(char *asMapName, char *asStartSpot)
{
	static uint	last_spawncount = 0;
	
	if(!asMapName || asMapName[0] <= ' ' || sv.background)
		return;
	
	// Make sure we don't issue two changelevels at one time
	if(svs.changelevel_next_time > host.realtime)
		return;
	
	// Rest for 0.5f secs if failed
	svs.changelevel_next_time = host.realtime + 0.5f;
	
	// Make sure we don't issue two changelevels
	if(svs.spawncount == last_spawncount)
		return;
	
	last_spawncount = svs.spawncount;
	
	gpEngineServer->SkipUpdates();
	
	if(!asStartSpot)
		gpCmdBuffer->AddText(va("changelevel %s\n", asMapName)); // Quake style changelevel
	else
		gpCmdBuffer->AddText(va("changelevel %s %s\n", asMapName, asStartSpot)); // Half-Life style changelevel
	
	gpGameWorld->ChangeLevel(asMapName, asStartSpot);
};

/*
=================
pfnGetSpawnParms

obsolete
=================
*/
static void EngFunc_GetSpawnParms(edict_t *apEntity)
{
	Host_Error(__FUNCTION__ ": %s [%i]\n", SV_ClassName(apEntity), NUM_FOR_EDICT(apEntity));
};

/*
=================
pfnSaveSpawnParms

obsolete
=================
*/
static void EngFunc_SaveSpawnParms(edict_t *apEntity)
{
	Host_Error(__FUNCTION__ ": %s [%i]\n", SV_ClassName(apEntity), NUM_FOR_EDICT(apEntity));
};

static float EngFunc_VecToYaw(const float *avVector)
{
	return CMath::ToYaw(avVector);
};

static void EngFunc_VecToAngles(const float *avVectorIn, float *avVectorOut)
{
};

static void EngFunc_MoveToOrigin(edict_t *apEntity, const float *avGoalPos, float afDist, int anMoveType)
{
	if(!IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEntity));
		return;
	};
	
	if(!avGoalPos)
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid goal pos");
		return;
	};
	
	gpEdictHandler->ToEdictClass(apEntity)->MoveToOrigin(avGoalPos, afDist, anMoveType);
};

static void EngFunc_ChangeYaw(edict_t *apEntity)
{
	if(!IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEntity));
		return;
	};
	
	apEntity->v.angles[YAW] = SV_AngleMod(apEntity->v.ideal_yaw, apEntity->v.angles[YAW], apEntity->v.yaw_speed);
};

static void EngFunc_ChangePitch(edict_t *apEntity)
{
	if(!IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEntity));
		return;
	};
	
	apEntity->v.angles[PITCH] = SV_AngleMod(apEntity->v.idealpitch, apEntity->v.angles[PITCH], apEntity->v.pitch_speed);
};

static edict_t *EngFunc_FindEntityByString(edict_t *apEdictStartSearchAfter, const char *asField, const char *asValue)
{
	return nullptr;
};

/*
==============
pfnGetEntityIllum

returns weighted lightvalue for entity position
==============
*/
static int EngFunc_GetEntityIllum(edict_t *apEntity)
{
	if(!IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEntity));
		return 0;
	};
	
	//return SV_LightForEntity(pEnt);
	return gpEdictHandler->ToEdictClass(apEntity)->GetIllum();
};

/*
=================
pfnFindEntityInSphere

return NULL instead of world!
=================
*/
static edict_t *EngFunc_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	edict_t *ent = nullptr;
	float distSquared;
	float eorg;
	int	j, e = 0;
	
	flRadius *= flRadius;
	
	if(IsValidEdict(pStartEdict))
		e = NUM_FOR_EDICT(pStartEdict);
	
	for(e++; e < svgame.numEntities; e++)
	{
		ent = EDICT_NUM(e);
		
		if(!IsValidEdict(ent))
			continue;
		
		// Ignore clients that not in a game
		if(e <= sv_maxplayers->GetInt() && !gpClientHandler->ClientFromEdict(ent, true))
			continue;
		
		distSquared = 0.0f;
		
		for(j = 0; j < 3 && distSquared <= flRadius; j++)
		{
			if(org[j] < ent->v.absmin[j])
				eorg = org[j] - ent->v.absmin[j];
			else if(org[j] > ent->v.absmax[j])
				eorg = org[j] - ent->v.absmax[j];
			else
				eorg = 0;
			
			distSquared += eorg * eorg;
		};
		
		if(distSquared > flRadius)
			continue;
		
		return ent;
	};
	
	return svgame.edicts[0];
};

static edict_t *EngFunc_FindClientInPVS(edict_t *apEntity)
{
	if(!IsValidEdict(pEdict))
		return svgame.edicts;
	
	float fDelta = sv.time - sv.lastchecktime;
	
	// Don't merge visibility for portal entity, only for monsters
	bool bMergePVS = (pEdict->v.flags & FL_MONSTER) ? true : false;
	
	// Find a new check if on a new frame
	if(fDelta < 0.0f || fDelta >= 0.1f)
	{
		sv.lastcheck = SV_CheckClientPVS(sv.lastcheck, bMergePVS);
		sv.lastchecktime = sv.time;
	};
	
	// Return check if it might be visible	
	edict_t	*pClient = EDICT_NUM(sv.lastcheck);
	
	if(!gpClientHandler->ClientFromEdict(pClient, true))
		return svgame.edicts;
	
	model_t *mod = Mod_Handle(pEdict->v.modelindex);
	
	vec3_t view;
	
	VectorAdd(pEdict->v.origin, pEdict->v.view_ofs, view);
	
	// HACKHACK for barnacle
	if(pEdict->v.effects & EF_INVLIGHT)
		view[2] -= 1.0f;
	
	int i = Mod_PointLeafnum(view) - 1;
	
	if(i < 0 || !((clientpvs[i>>3]) & (1 << (i & 7))))
		return svgame.edicts;
	
	// Client which currently in PVS
	return pClient;
};

static edict_t *EngFunc_EntitiesInPVS(edict_t *pplayer)
{
	edict_t	*pEdict = nullptr, *pEdict2 = nullptr;
	vec3_t viewpoint;
	
	if(!IsValidEdict(pview))
		return nullptr;
	
	VectorAdd(pview->v.origin, pview->v.view_ofs, viewpoint);
	
	edict_t	*chain = nullptr;
	
	for(chain = EDICT_NUM(0), int i = 1; i < svgame.numEntities; i++)
	{
		pEdict = EDICT_NUM(i);
		
		if(!SV_IsValidEdict(pEdict))
			continue;
		
		if(pEdict->v.movetype == MOVETYPE_FOLLOW && SV_IsValidEdict(pEdict->v.aiment))
		{
			// Force all items across level even it is not visible
			if(pEdict->v.aiment == EDICT_NUM(1))
			{
				pEdict->v.chain = chain;
				chain = pEdict;
				continue;
			};
			
			pEdict2 = pEdict->v.aiment;
		}
		else
			pEdict2 = pEdict;
		
		if(SV_BoxInPVS(viewpoint, pEdict2->v.absmin, pEdict2->v.absmax))
		{
			pEdict->v.chain = chain;
			chain = pEdict;
		};
	};
	
	return chain;
};

static void EngFunc_MakeVectors(const float *avVector)
{
	AngleVectors(avVector, gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up);
};

static void EngFunc_AngleVectors(const float *avVector, float *avForward, float *avRight, float *avUp)
{
};

static edict_t *EngFunc_CreateEntity()
{
	return gpEdictHandler->CreateEntity();
};

/*
==============
pfnRemoveEntity

free edict private mem, unlink physics etc
==============
*/
static void EngFunc_RemoveEntity(edict_t *apEntity)
{
	if(!SV_IsValidEdict(apEntity))
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": entity is already free");
		return;
	};
	
	// Never free client or world entity
	if(NUM_FOR_EDICT(apEntity) < (svgame.globals->maxClients + 1))
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": can't delete %s", (apEntity == EDICT_NUM(0)) ? "world" : "client");
		return;
	};
	
	//gpGameWorld->FreeEdict();
	//apEntity->Free();
	gpEdictHandler->FreeEntity(apEntity);
};

static edict_t *EngFunc_CreateNamedEntity(int anClassName)
{
	//return SV_AllocPrivateData(NULL, className);
	return gpEdictHandler->CreateNamedEntity(anClassName);
};

static void EngFunc_MakeStatic(edict_t *apEntity)
{
	if(!apEntity)
		return;
	
	gpEdictHandler->ToEdictClass(apEntity)->MakeStatic();
};

static int EngFunc_EntIsOnFloor(edict_t *apEntity)
{
	if(!apEntity)
		return 0;
	
	return gpEdictHandler->ToEdictClass(apEntity)->IsOnFloor();
};

static int EngFunc_DropToFloor(edict_t *apEntity)
{
	if(!apEntity)
		return 0;
	
	return gpEdictHandler->ToEdictClass(apEntity)->DropToFloor();
};

static int EngFunc_WalkMove(edict_t *apEdict, float afYaw, float afDist, int anMode)
{
	if(!apEdict)
		return 0;
	
	return gpEdictHandler->ToEdictClass(apEdict)->WalkMove(afYaw, afDist, anMode);
};

static void EngFunc_SetOrigin(edict_t *apEdict, const float *avOrigin)
{
	if(!apEdict)
		return;
	
	gpEdictHandler->ToEdictClass(apEdict)->SetOrigin(avOrigin);
};

static void EngFunc_EmitSound(edict_t *apEntity, int anChannel, const char *asSample, float afVolume, float afAttenuation, int anFlags, int anPitch)
{
};

static void EngFunc_EmitAmbientSound(edict_t *apEntity, float *avPos, const char *asSample, float afVolume, float afAttenuation, int anFlags, int anPitch)
{
	gpGameWorld->EmitAmbientSound();
};

static void EngFunc_TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *apTraceResult)
{
	gpPhysicsWorld->TraceLine(v1, v2, fNoMonsters, pentToSkip, apTraceResult);
};

static void EngFunc_TraceToss(edict_t *apEnt, edict_t *apEntToIgnore, TraceResult *apTraceResult)
{
	gpPhysicsWorld->TraceToss(apEnt, apEntToIgnore, apTraceResult);
};

static int EngFunc_TraceMonsterHull(edict_t *apEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *apTraceResult)
{
	return gpPhysicsWorld->TraceMonsterHull();
};

static void EngFunc_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *apTraceResult)
{
	gpPhysicsWorld->TraceHull(v1, v2, fNoMonsters, hullNumber, pentToSkip, apTraceResult);
};

static void EngFunc_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *apTraceResult)
{
	gpPhysicsWorld->TraceModel(v1, v2, hullNumber, pent, apTraceResult);
};

static const char *EngFunc_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	return gpPhysicsWorld->TraceTexture();
};

static void EngFunc_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float afRadius, edict_t *pentToSkip, TraceResult *apTraceResult)
{
	gpPhysicsWorld->TraceSphere(v1, v2, fNoMonsters, afRadius, pentToSkip, apTraceResult);
};

/*
=============
pfnGetAimVector

NOTE: speed is unused
=============
*/
static void EngFunc_GetAimVector(edict_t *ent, float afSpeed, float *avResult)
{
	vec3_t start, dir, end, bestdir;
	float dist;
	int i, j;
	
	// Assume failure if it returns early
	VectorCopy(svgame.globals->v_forward, avResult);
	
	if(!SV_IsValidEdict(ent) || (ent->v.flags & FL_FAKECLIENT))
		return;
	
	VectorCopy(ent->v.origin, start);
	VectorAdd(start, ent->v.view_ofs, start);
	
	// Try sending a trace straight
	VectorCopy(svgame.globals->v_forward, dir);
	VectorMA(start, 2048, dir, end);
	
	trace_t tr = gpGameWorld->Move(start, vec3_origin, vec3_origin, end, MOVE_NORMAL, ent);
	
	if(tr.ent && (tr.ent->v.takedamage == DAMAGE_AIM || ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
		return;
	
	// Try all possible entities
	float bestdir;
	VectorCopy(dir, bestdir);
	bestdist = gpConsole->GetVar("sv_aim")->GetFloat();
	
	// Start at first client
	edict_t *check = EDICT_NUM(1);
	
	for(i = 1; i < svgame.numEntities; i++, check++)
	{
		if(check->v.takedamage != DAMAGE_AIM)
			continue;
		
      	if(check->v.flags & FL_FAKECLIENT)
			continue;
		
		if(ent->v.team > 0 && ent->v.team == check->v.team)
			continue;
		
		if(check == ent)
			continue;
		
		for(j = 0; j < 3; j++)
			end[j] = check->v.origin[j] + 0.5f * (check->v.mins[j] + check->v.maxs[j]);
		
		VectorSubtract(end, start, dir);
		dir.Normalize();
		dist = DotProduct(dir, svgame.globals->v_forward);
		
		// Too far to turn
		if(dist < bestdist)
			continue;
		
		tr = gpGameWorld->Move(start, vec3_origin, vec3_origin, end, MOVE_NORMAL, ent);
		
		if(tr.ent == check)
		{	
			bestdist = dist;
			VectorCopy(dir, bestdir);
		};
	};
	
	VectorCopy(bestdir, avResult);
};

static void EngFunc_ServerCommand(char *asCmd)
{
	if(SV_IsValidCmd(asCmd))
		gpCmdBuffer->AddText(asCmd);
	else
		DevMsg(eMsgType_Error, "bad server command %s", asCmd);
};

static void EngFunc_ServerExecute()
{
	gpCmdBuffer->Execute();
};

static void EngFunc_ClientCommand(edict_t *apClient, char *asCmd, ...)
{
	if(gpEngineServer->GetState() != eServerState_Active)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": server is not active!");
		return;
	};
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not spawned!");
		return;
	};
	
	if(pClient->IsFakeClient())
		return;
	
	string buffer;
	va_list	ArgList;
	
	va_start(ArgList, asCmd);
	vsnprintf(buffer, MAX_STRING, asCmd, ArgList);
	va_end(ArgList);
	
	if(SV_IsValidCmd(buffer))
	{
		pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_stufftext);
		pClient->GetNetChan()->GetMsgBuffer()->WriteString(buffer);
	}
	else
		DevMsg(eMsgType_Error, __FUNCTION__ ": Tried to stuff bad command %s", buffer);
};

/*
=================
pfnParticleEffect

Make sure the event gets sent to all clients
=================
*/
static void EngFunc_ParticleEffect(const float *avOrigin, const float *avDir, float afColor, float afCount)
{
	gpGameWorld->ParticleEffect(avOrigin, avDir, afColor, afCount);
};

static void EngFunc_SetLightStyle(int anStyle, char *asValue)
{
	gpGameWorld->SetLightStyle(anStyle, asValue);
};

/*
=================
pfnDecalIndex

register decal shader on client
=================
*/
static int EngFunc_GetDecalIndex(const char *asName)
{
	if(!asName || !asName[0])
		return 0;
	
	for(auto i = 1; i < MAX_DECALS && host.draw_decals[i][0]; i++)
		if(!stricmp(host.draw_decals[i], asName))
			return i;
	
	// Throw a warning (this can happens if decal is not present in decals.wad)
	DevMsg(eMsgType_Warning, __FUNCTION__ ": Can't find decal %s", asName);
	return 0;	
};

static int EngFunc_GetPointContents(const float *avPoint)
{
	return gpGameWorld->GetPointContents(avPoint);
};

static void EngFunc_MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if(svgame.msg_started)
		Host_Error(__FUNCTION__ ": New message started when msg '%s' has not been sent yet\n", svgame.msg_name);
	
	svgame.msg_started = true;
	
	// Check range
	msg_num = clamp(svc_bad, msg_num, 255);
	
	int	nMsgSize = 0;
	
	if(msg_num < svc_lastmsg)
	{
		// This is a system message
		svgame.msg_index = -msg_num;
		svgame.msg_name = svc_strings[msg_num];
		
		if(msg_num == svc_temp_entity)
			nMsgSize = -1; // Temp entity has variable size
		else
			nMsgSize = 0;
	}
	else
	{
		int i = 0;
		
		// Check for existance
		for(i = 0; i < MAX_USER_MESSAGES && svgame.msg[i].name[0]; i++)
			if(svgame.msg[i].number == msg_num)
				break; // Found
		
		if(i == MAX_USER_MESSAGES)
		{
			Host_Error(__FUNCTION__ ": tried to send unregistered message %i\n", msg_num);
			return;
		};
		
		svgame.msg_name = svgame.msg[i].name;
		nMsgSize = svgame.msg[i].size;
		svgame.msg_index = i;
	};
	
	gpGameServer->GetMulticast()->WriteByte(msg_num);
	
	// Save message destination
	if(pOrigin)
		VectorCopy(pOrigin, svgame.msg_org);
	else
		VectorClear(svgame.msg_org);
	
	if(nMsgSize == -1)
	{
		// Variable sized messages sent size as first byte
		svgame.msg_size_index = gpGameServer->GetMulticast()->GetNumBytesWritten();
		
		// Reserve space for now
		gpGameServer->GetMulticast()->WriteByte(0);
	}
	else
		svgame.msg_size_index = -1; // Message has a constant size
	
	svgame.msg_realsize = 0;
	svgame.msg_dest = msg_dest;
	svgame.msg_ent = ed;
};

static void EngFunc_MessageEnd()
{
	gpEngineServer->MessageEnd();
};

static void EngFunc_WriteByte(int anValue)
{
	// Convert char to byte 
	if(anValue == -1)
		anValue = 0xFF;
	
	gpGameServer->GetMulticast()->WriteByte((byte)anValue);
	svgame.msg_realsize++;
};

static void EngFunc_WriteChar(int anValue)
{
	gpEngineServer->GetMulticast()->WriteChar((char)anValue);
	svgame.msg_realsize++;
};

static void EngFunc_WriteShort(int anValue)
{
	gpEngineServer->GetMulticast()->WriteShort((short)anValue);
	svgame.msg_realsize += 2;
};

static void EngFunc_WriteLong(int anValue)
{
	gpEngineServer->GetMulticast()->WriteLong(anValue);
	svgame.msg_realsize += 4;
};

/*
=============
pfnWriteAngle

this is low-res angle
=============
*/
static void EngFunc_WriteAngle(float afValue)
{
	int	nAngle = ((int)((afValue) * 256 / 360) & 255);
	
	gpEngineServer->GetMulticast()->WriteChar(afAngle);
	svgame.msg_realsize += 1;
};

static void EngFunc_WriteCoord(float afValue)
{
	gpEngineServer->GetMulticast()->WriteCoord(afValue);
	svgame.msg_realsize += 2;
};

static void EngFunc_WriteString(const char *asValue)
{
	char *dst, string[MAX_SYSPATH];
	int	len = strlen(src) + 1;
	int	rem = (255 - svgame.msg_realsize);
	
	if(len >= rem)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": exceeds %i symbols", rem);
		gpEngineServer->GetMulticast()->WriteChar(0);
		svgame.msg_realsize += 1;
		return;
	};
	
	// Prepare string for sending
	dst = string;
	
	while(1)
	{
		// Some escaped chars parsed as two symbols - merge it here
		if(src[0] == '\\' && src[1] == 'n')
		{
			*dst++ = '\n';
			src += 2;
			len -= 1;
		}
		else if(src[0] == '\\' && src[1] == 'r')
		{
			*dst++ = '\r';
			src += 2;
			len -= 1;
		}
		else if(src[0] == '\\' && src[1] == 't')
		{
			*dst++ = '\t';
			src += 2;
			len -= 1;
		}
		else if((*dst++ = *src++) == 0)
			break;
	};
	
	// String end (not included in count)
	*dst = '\0';
	
	gpEngineServer->GetMulticast()->WriteString(string);
	
	// NOTE: some messages with constant string length can be marked as known sized
	svgame.msg_realsize += len;
};

static void EngFunc_WriteEntity(int anValue)
{
	if(anValue < 0 || anValue >= svgame.numEntities)
		Host_Error(__FUNCTION__ ": invalid entnumber %i\n", anValue);
	
	gpGameServer->GetMulticast()->WriteShort((short)anValue);
	svgame.msg_realsize += 2;
};

static void EngFunc_CVarRegister(cvar_t *apCvar)
{
	gpConsole->AddConVar(apCvar);
};

// gpConsole->GetVar(); will return nullptr if cvar is not found
// it could return null impl of cvar

static void EngFunc_CVarSetFloat(const char *asVarName, float afValue)
{
	IConVar *pConVar = gpConsole->GetVar(asVarName);
	
	if(!pConVar)
		return;
	
	return pConVar->SetFloat(afValue);
};

static float EngFunc_CVarGetFloat(const char *asVarName)
{
	IConVar *pConVar = gpConsole->GetVar(asVarName);
	
	if(!pConVar)
		return 0.0f;
	
	return pConVar->GetFloat();
};

static void EngFunc_CVarSetString(const char *asVarName, const char *asValue)
{
	IConVar *pConVar = gpConsole->GetVar(asVarName);
	
	if(!pConVar)
		return;
	
	pConVar->SetString(asValue);
};

const char *EngFunc_CVarGetString(const char *asVarName)
{
	IConVar *pConVar = gpConsole->GetVar(asVarName);
	
	if(!pConVar)
		return "";
	
	return pConVar->GetString();
};

static void EngFunc_AlertMessage(ALERT_TYPE aeType, char *asMsg, ...)
{
	char buffer[2048]; // Must support > 1k messages
	va_list	args;
	
	// Check message for pass
	switch(level)
	{
	case at_notice:
		break; // Passed always
	case at_console:
		if(host.developer < eMsgType_Info)
			return;
		break;
	case at_aiconsole:
		if(host.developer < eMsgType_AIConsole)
			return;
		break;
	case at_warning:
		if(host.developer < eMsgType_Warning)
			return;
		break;
	case at_error:
		if(host.developer < eMsgType_Error)
			return;
		break;
	};
	
	va_start(args, szFmt);
	vsnprintf(buffer, 2048, szFmt, args);
	va_end(args);
	
	if(level == at_warning)
		Sys_Print(va("^3Warning:^7 %s", buffer));
	else if(level == at_error )
		Sys_Print(va("^1Error:^7 %s", buffer));
	else
		Sys_Print(buffer);
};

/*
=============
pfnEngineFprintf

legacy. probably was a part of early save\restore system
=============
*/
static void EngFunc_EngineFprintf(FILE *apFile, char *asMsg, ...)
{
	char sBuffer[2048];
	va_list	ArgList;
	
	va_start(ArgList, asMsg);
	vsnprintf(sBuffer, sizeof(sBuffer), asMsg, ArgList);
	va_end(ArgList);
	
	fprintf(apfile, sBuffer);
};

/*
=============
pfnPvAllocEntPrivateData
=============
*/
static void *EngFunc_PvAllocEntPrivateData(edict_t *apEdict, long cb)
{
	ASSERT(apEdict);
	
	CEdict *pEdict = gpEdictHandler->ToEdictClass(apEdict)->FreePrivateData();
	
	// Poke646 has a memory corruption in somewhere - this is trashed last four bytes :(
	if(cb > 0)
		pEdict->pvPrivateData = Mem_Alloc(svgame.mempool, (cb + 15) & ~15);
	
	return pEdict->GetPrivateData();
};

static void *EngFunc_PvEntPrivateData(edict_t *apEdict)
{
	return nullptr;
};

static void EngFunc_FreeEntPrivateData(edict_t *apEdict)
{
};

static const char *EngFunc_StringFromIndex(int anString)
{
	return "";
};

static int EngFunc_AllocString(const char *asValue)
{
	return 0;
};

static entvars_t *EngFunc_GetVarsOfEnt(edict_t *apEdict)
{
	if(apEdict)
		return &apEdict->v;
	
	return nullptr;
};

static edict_t *EngFunc_PEntityOfEntOffset(int anOffset)
{
	return (&((edict_t*)svgame.vp)[anOffset]);
};

/*
=============
pfnEntOffsetOfPEntity
=============
*/
static int EngFunc_GetEntityOffset(const edict_t *apEdict)
{
	return ((byte*)pEdict - (byte*)svgame.vp);
};

/*
=============
pfnIndexOfEdict
=============
*/
static int EngFunc_GetEntityIndex(const edict_t *apEdict)
{
	int	nIndex = NUM_FOR_EDICT(apEdict);
	
	// Out of range
	if(nIndex < 0 || nIndex >= svgame.numEntities)
		return 0;
	
	return nIndex;
};

/*
=============
pfnPEntityOfEntIndex
=============
*/
static edict_t *EngFunc_PEntityOfEntIndex(int anEntIndex)
{
	// Out of range
	if(anEntIndex < 0 || anEntIndex >= svgame.numEntities)
		return nullptr;
	
	return EDICT_NUM(anEntIndex);
};

/*
=============
pfnFindEntityByVars

debug routine
=============
*/
static edict_t *EngFunc_FindEntityByVars(struct entvars_s *apVars)
{
	// Don't pass invalid arguments
	if(!apVars)
		return nullptr;
	
	edict_t *e = nullptr;
	
	for(auto i = 0; i < svgame.numEntities; i++)
	{
		e = EDICT_NUM(i);
		
		if(&e->v == apVars)
		{
			if(e->v.pContainingEntity != e)
			{
				DevMsg(eMsgType_Note, __FUNCTION__ ": fixing pContainingEntity for %s", SV_ClassName(e));
				e->v.pContainingEntity = e;
			};
			
			return e; // Found it
		};
	};
	
	return nullptr;
};

/*
=============
pfnGetModelPtr

returns pointer to a studiomodel
=============
*/
static void *EngFunc_GetModelPtr(edict_t *apEdict)
{
	if(!SV_IsValidEdict(apEdict))
		return nullptr;
	
	CModel *pModel = gpModelManager->GetModelByIndex(apEdict->v.modelindex);
	return pModel->GetExtraData();
};

static int EngFunc_RegUserMsg(const char *asName, int anSize)
{
	int	i;
	
	if(!asName || !asName[0])
		return svc_bad;
	
	if(strlen(asName) >= sizeof(svgame.msg[0].name))
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": too long name %s", asName);
		return svc_bad; // Force error
	};
	
	if(anSize > 255)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": %s has too big size %i", asName, anSize);
		return svc_bad; // Force error
	};
	
	// Make sure what size inrange
	anSize = clamp(-1, anSize, 255);
	
	// Message 0 is reserved for svc_bad
	for(i = 0; i < MAX_USER_MESSAGES && svgame.msg[i].name[0]; i++)
	{
		// See if already registered
		if(!strcmp(svgame.msg[i].name, asName))
			return svc_lastmsg + i; // offset
	};
	
	if(i == MAX_USER_MESSAGES) 
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": user messages limit exceeded");
		return svc_bad;
	};
	
	// Register new message
	strncpy(svgame.msg[i].name, pszName, sizeof(svgame.msg[i].name));
	svgame.msg[i].number = svc_lastmsg + i;
	svgame.msg[i].size = iSize;
	
	// Catch some user messages
	if(!strcmp(pszName, "HudText"))
		svgame.gmsgHudText = svc_lastmsg + i;
	
	if(gpEngineServer->GetState() == eServerState_Active)
	{
		// Tell the client about new user message
		gpEngineServer->GetMulticast()->WriteByte(svc_usermessage);
		gpEngineServer->GetMulticast()->WriteByte(svgame.msg[i].number);
		gpEngineServer->GetMulticast()->WriteByte((byte)iSize);
		gpEngineServer->GetMulticast()->WriteString(svgame.msg[i].name);
		
		gpEngineServer->SendMsg(MSG_ALL, NULL, NULL);
	};
	
	return svgame.msg[i].number;
};

/*
=============
pfnAnimationAutomove

animating studiomodel
=============
*/
static void EngFunc_AnimationAutomove(const edict_t *apEdict, float afTime)
{
	// It's empty in the original HL
};

static void EngFunc_GetBonePosition(const edict_t *apEdict, int anBone, float *avOrigin, float *avAngles)
{
	if(!SV_IsValidEdict(apEdict))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(apEdict));
		return;
	};
	
	Mod_GetBonePosition(apEdict, anBone, avOrigin, avAngles);
};

static unsigned long EngFunc_GetFunctionFromName(const char *asName)
{
	return Com_FunctionFromName(svgame.hInstance, asName);
};

static const char *EngFunc_GetNameForFunction(unsigned long function)
{
	return Com_NameForFunction(svgame.hInstance, function);
};

/*
=============
pfnClientPrintf

// JOHN: engine callbacks so game DLL can print messages to individual clients
=============
*/
static void EngFunc_ClientPrintf(edict_t *apClient, PRINT_TYPE aeMsgType, const char *asMsg)
{
	// Send this message to console during loading
	if(gpEngineServer->GetState() != eServerState_Active)
	{
		DevMsg(eMsgType_Info, asMsg);
		return;
	};
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not spawned!");
		return;
	};
	
	switch(aeMsgType)
	{
	case print_console:
		if(pClient->IsFakeClient())
			DevMsg(eMsgType_Info, asMsg);
		else
			pClient->Printf(PRINT_HIGH, "%s", asMsg);
		
		break;
	case print_chat:
		if(pClient->IsFakeClient())
			return;
		
		pClient->Printf(PRINT_CHAT, "%s", asMsg);
		break;
	case print_center:
		if(pClient->IsFakeClient())
			return;
		
		pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_centerprint);
		pClient->GetNetChan()->GetMsgBuffer()->WriteString(asMsg);
		break;
	};
};

/*
=============
pfnServerPrint
=============
*/
static void EngFunc_BroadcastPrint(const char *asMsg)
{
	// While loading in-progress we can send message only for local client
	if(gpGameServer->GetState() != eServerState_Active)
		DevMsg(eMsgType_Info, asMsg);	
	else
		gpGameServer->BroadcastPrintf(PRINT_HIGH, asMsg);
};

// these 3 added
// so game dll can easily
// access client 'cmd' strings

static const char *EngFunc_GetCmdArgs()
{
	return "";
};

static const char *EngFunc_GetCmdArgv(int argc)
{
	return "";
};

static int EngFunc_GetCmdArgc()
{
	return 0;
};

static void EngFunc_GetAttachment(const edict_t *apEdict, int anAttachment, float *avOrigin, float *avAngles)
{
	if(!SV_IsValidEdict(apEdict))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(pEdict));
		return;
	};
	
	Mod_StudioGetAttachment(apEdict, anAttachment, avOrigin, avAngles);
};

static void EngFunc_CRC32_Init(CRC32_t *pulCRC)
{
	gpCRC32Handler->Init(pulCRC);
};

static void EngFunc_CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
	gpCRC32Handler->ProcessBuffer(pulCRC, p, len);
};

static void EngFunc_CRC32_ProcessByte(CRC32_t *pulCRC, byte ch)
{
	gpCRC32Handler->ProcessByte(pulCRC, ch);
};

/*
=============
pfnCRC32_Final
=============
*/
static CRC32_t EngFunc_CRC32_Final(CRC32_t pulCRC)
{
	gpCRC32Handler->Finalize(&pulCRC);
	return pulCRC;
};

static long EngFunc_RandomLong(long anMin, long anMax)
{
	return 0;
};

static float EngFunc_RandomFloat(float afMin, float afMax)
{
	return 0.0f;
};

static void EngFunc_SetView(const edict_t *apClient, const edict_t *apViewEnt)
{
	if(!apClient || apClient->IsFree())
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": invalid client!");
		return;
	};
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": not a client!");
		return;
	};
	
	if(!SV_IsValidEdict(apViewEnt))
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": invalid viewent!");
		return;
	};
	
	if(pClient == apViewEnt)
		pClient->SetViewEntity(nullptr);
	else
		pClient->SetViewEntity((edict_t*)apViewEnt);
	
	// Fakeclients ignores to send client message (but can see into the trigger_camera through the PVS)
	if(pClient->IsFakeClient())
		return;
	
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_setview);
	pClient->GetNetChan()->GetMsgBuffer()->WriteWord(NUM_FOR_EDICT(apViewEnt));
};

static float EngFunc_GetTime()
{
	return static_cast<float>(Sys_DoubleTime());
};

/*
=============
pfnCrosshairAngle
=============
*/
static void EngFunc_CrosshairAngle(const edict_t *apClient, float afPitch, float afYaw)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": invalid client!");
		return;
	};
	
	// Fakeclients ignores it silently
	if(pClient->IsFakeClient())
		return;
	
	if(afPitch > 180.0f)
		afPitch -= 360;
	
	if(afPitch < -180.0f)
		afPitch += 360;
	
	if(afYaw > 180.0f)
		afYaw -= 360;
	
	if(afYaw < -180.0f)
		afYaw += 360;
	
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_crosshairangle);
	pClient->GetNetChan()->GetMsgBuffer()->WriteChar(afPitch * 5);
	pClient->GetNetChan()->GetMsgBuffer()->WriteChar(afYaw * 5);
};

static byte *EngFunc_LoadFileForMe(char *asFileName, int *anLength)
{
	return "";
};

static void EngFunc_FreeFile(void *apBuffer)
{
};

/*
=============
pfnEndSection

trigger_endsection
=============
*/
static void EngFunc_EndSection(const char *asSectionName)
{
	if(!stricmp("oem_end_credits", asSectionName))
		Host_Credits();
	else
		gpCmdBuffer->AddText(va("endgame \"%s\"\n", asSectionName));
};

static int EngFunc_CompareFileTime(char *asFileNameA, char *asFileNameB, int *iCompare)
{
	return 0;
};

static void EngFunc_GetGameDir(char *asGameDir)
{
	if(!asGameDir)
		return;
	
	sprintf(asGameDir, "%s", gpGameInfo->GetGameDir());
};

static void EngFunc_Cvar_RegisterVariable(cvar_t *apVariable)
{
};

static void EngFunc_FadeClientVolume(const edict_t *apClient, int anFadePercent, int anFadeOutSeconds, int anHoldTime, int anFadeInSeconds)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not spawned!");
		return;
	};
	
	if(pClient->IsFakeClient())
		return;
	
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_soundfade);
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(anFadePercent);
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(anHoldTime);
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(anFadeOutSeconds);
	pClient->GetNetChan()->GetMsgBuffer()->WriteByte(anFadeInSeconds);
};

/*
=============
pfnSetClientMaxspeed

fakeclients can be changed speed to
=============
*/
static void EngFunc_SetClientMaxspeed(const edict_t *apClient, float afNewSpeed)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	// Not spawned clients allowed
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not active!");
		return;
	};
	
	pClient->SetMaxSpeed(afNewSpeed);
};

// Returns nullptr if fake client can't be created
static edict_t *EngFunc_CreateFakeClient(const char *asNetName)
{
	return nullptr;
};

static void EngFunc_RunPlayerMove(edict_t *apFakeClient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec)
{
	if(gpEngineServer->IsPaused())
		return;
	
	CGameClient *cl = gpClientHandler->ClientFromEdict(apFakeClient, true);
	
	if(!cl)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": fakeclient is not spawned!");
		return;
	};
	
	// Only fakeclients are allowed
	if(!cl->IsFakeClient())
		return;
	
	CGameClient *oldcl = svs.currentPlayer;
	
	svs.currentPlayer = gpClientHandler->ClientFromEdict(pClient, true);
	svs.currentPlayerNum = (svs.currentPlayer - svs.clients);
	svs.currentPlayer->timebase = (sv.time + host.frametime) - (msec / 1000.0f);
	
	usercmd_t cmd;
	memset(&cmd, 0, sizeof(cmd));
	
	if(v_angle)
		VectorCopy(v_angle, cmd.viewangles);
	
	cmd.forwardmove = fmove;
	cmd.sidemove = smove;
	cmd.upmove = upmove;
	cmd.buttons = buttons;
	cmd.impulse = impulse;
	cmd.msec = msec;
	
	uint seed = Com_RandomLong(0, 0x7fffffff); // Full range
	
	cl->RunCmd(&cmd, seed);
	
	cl->SetLastCmd(&cmd);
	cl->GetLastCmd()->buttons = 0; // Avoid multiple fires on lag
	
	svs.currentPlayer = oldcl;
	svs.currentPlayerNum = (svs.currentPlayer - svs.clients);
};

/*
=============
pfnNumberOfEntities

returns actual entity count
=============
*/
static int EngFunc_GetNumberOfEntities()
{
	int	nTotal = 0;
	
	for(auto nIt = 0; nIt < svgame.numEntities; nIt++)
	{
		if(svgame.edicts[nIt].free)
			continue;
		
		nTotal++;
	};
	
	return nTotal;
};

/*
=============
pfnGetInfoKeyBuffer

passing in NULL gets the serverinfo
=============
*/
static char *EngFunc_GetInfoKeyBuffer(edict_t *apClient)
{
	// Otherwise return ServerInfo
	if(!SV_IsValidEdict(apClient))
		return Cvar_Serverinfo();
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	// pfnUserInfoChanged passed
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not connected!");
		return Cvar_ServerInfo(); // Otherwise return ServerInfo
	};
	
	return pClient->GetUserInfo();
};

static char *EngFunc_InfoGetKeyValue(char *asInfoBuffer, char *asKey)
{
	return "";
};

static void EngFunc_SetKeyValue(char *asInfoBuffer, char *asKey, char *asValue)
{
};

static void EngFunc_SetClientKeyValue(int anClientIndex, char *asInfoBuffer, char *asKey, char *asValue)
{
	anClientIndex -= 1;
	
	if(anClientIndex < 0 || anClientIndex >= sv_maxplayers->GetInt())
		return;
	
	if(svs.clients[anClientIndex].state < eClientState_Spawned || asInfoBuffer == NULL)
		return;
	
	Info_SetValueForKey(asInfoBuffer, asKey, asValue);
	svs.clients[anClientIndex].sendinfo = true;
};

/*
=============
pfnIsMapValid

vaild map must contain one info_player_deatchmatch
=============
*/
static int EngFunc_IsMapValid(char *asFileName)
{
	char *sSpawnEntity = "";
	
	// Determine spawn entity classname
	if(sv_maxplayers->GetInt() == 1)
		sSpawnEntity = gpGameInfo->GetSPEntName();
	else
		sSpawnEntity = gpGameInfo->GetMPEntName();
	
	int nFlags = SV_MapIsValid(asFileName, sSpawnEntity, NULL);
	
	if((nFlags & MAP_IS_EXIST) && (nFlags & MAP_HAS_SPAWNPOINT))
		return 1;
	
	return 0;
};

static void EngFunc_StaticDecal(const float *avOrigin, int anDecalIndex, int anEntityIndex, int anModelIndex)
{
	if(!avOrigin)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": NULL origin. Ignored");
		return;
	};
	
	//SV_CreateDecal(&sv.signon, avOrigin, anDecalIndex, anEntityIndex, anModelIndex, FDECAL_PERMANENT, 1.0f);
	gpGameWorld->CreateDecal(avOrigin, anDecalIndex, anEntityIndex, anModelIndex);
};

static int EngFunc_PrecacheGeneric(char *asName)
{
	return 0;
};

/*
=============
pfnGetPlayerUserId

returns the server assigned userid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
=============
*/
static int EngFunc_GetPlayerUserId(edict_t *apPlayer)
{
	if(gpEngineServer->GetState() != eServerState_Active)
		return -1;
	
	if(!gpClientHandler->ClientFromEdict(apPlayer, false))
		return -1;
	
	for(auto i = 0, CGameClient *pClient = svs.clients; i < sv_maxplayers->GetInt(); i++, pClient++)
		if(pClient->GetEdict() == apPlayer)
			return pClient->GetUserID();
	
	// Couldn't find it
	return -1;
};

/*
=============
pfnBuildSoundMsg

Customizable sound message
=============
*/
static void EngFunc_BuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	EngFunc_MessageBegin(msg_dest, msg_type, pOrigin, pSend);
	gpEngineServer->BuildSoundMsg(pSource, chan, samp, fvol * 255, attn, fFlags, pitch, pOrigin);
	EngFunc_MessageEnd();
};

/*
=============
pfnIsDedicatedServer

Is this a dedicated server?
=============
*/
static int EngFunc_IsDedicatedServer()
{
	return gpEngineServer->IsDedicated() ? 1 : 0;
};

/*
=============
pfnCVarGetPointer

can return NULL
=============
*/
static cvar_t *EngFunc_GetCVarPointer(const char *asVarName)
{
	IConVar *pConVar = gpConsole->GetConVar(asVarName);
	
	if(!pConVar)
		return nullptr;
	
	return pConVar;
};

/*
=============
pfnGetPlayerWONId

returns the server assigned WONid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
=============
*/
static uint EngFunc_GetPlayerWONId(edict_t *apClient)
{
	if(gpEngineServer->GetState() != eServerState_Active)
		return -1;
	
	if(!gpClientHandler->ClientFromEdict(apClient, false))
		return -1;
	
	for(auto i = 0, CGameClient *pClient = svs.clients; i < sv_maxplayers->GetInt(); i++, pClient++)
		if(pClient->GetEdict() == apClient && pClient->GetAuthMethod() == 0)
			return pClient->GetWonID();
	
	return -1;
};

static void EngFunc_Info_RemoveKey(char *asInfo, const char *asKey)
{
};

static const char *EngFunc_GetPhysicsKeyValue(const edict_t *apClient, const char *asKey)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	// pfnUserInfoChanged passed
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not connected!");
		return "";
	};
	
	return Info_ValueForKey(pClient->GetPhysInfo(), asKey);
};

static void EngFunc_SetPhysicsKeyValue(const edict_t *apClient, const char *asKey, const char *asValue)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	// pfnUserInfoChanged passed
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not connected!");
		return;
	};
	
	Info_SetValueForKey(pClient->GetPhysInfo(), asKey, asValue);
};

static const char *EngFunc_GetPhysicsInfoString(const edict_t *apClient)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	// pfnUserInfoChanged passed
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not connected!");
		return "";
	};
	
	return pClient->GetPhysInfo();
};

/*
=============
pfnPrecacheEvent

register or returns already registered event id
a type of event is ignored at this moment
=============
*/
static unsigned short EngFunc_PrecacheEvent(int type, const char*psz)
{
	return (word)SV_GetEventIndex(psz);
};

static void EngFunc_PlaybackEvent(int anFlags, const edict_t *apInvoker, unsigned short anEventIndex, float afDelay, float *avOrigin, float *avAngles, float afParam1, float afParam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	gpGameWorld->PlaybackEvent(anFlags, apInvoker, anEventIndex, afDelay, avOrigin, avAngles, afParam1, afParam2,);
};

/*
=============
pfnSetFatPVS

The client will interpolate the view position,
so we can't use a single PVS point
=============
*/
static byte *EngFunc_SetFatPVS(float *org)
{
	if(!sv.worldmodel->visdata || sv_novis->GetInt() || !org || CL_DisableVisibility())
		return Mod_DecompressVis(NULL);
	
	ASSERT(svs.currentPlayerNum >= 0 && svs.currentPlayerNum < MAX_CLIENTS);
	
	fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
	bitvector = fatpvs;
	
	// Portals can't change viewpoint!
	if(!(sv.hostflags & SVF_PORTALPASS))
	{
		vec3_t viewPos, offset;
		
		// see code from client.cpp for understanding:
		// org = pView->v.origin + pView->v.view_ofs;
		// if (pView->v.flags & FL_DUCKING)
		// {
		//	org = org + (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
		// }
		// so we have unneeded duck calculations who have affect when player
		// is ducked into water. Remove offset to restore right PVS position
		if(svs.currentPlayer->GetEdict()->v.flags & FL_DUCKING)
		{
			VectorSubtract(svgame.pmove->player_mins[0], svgame.pmove->player_mins[1], offset);
			VectorSubtract(org, offset, viewPos);
		}
		else
			VectorCopy(org, viewPos);
		
		// Build a new PVS frame
		memset(bitvector, 0, fatbytes);
		
		SV_AddToFatPVS(viewPos, DVIS_PVS, sv.worldmodel->nodes);
		VectorCopy(viewPos, viewPoint[svs.currentPlayerNum]);
	}
	else
		SV_AddToFatPVS(org, DVIS_PVS, sv.worldmodel->nodes);
	
	return bitvector;
};

/*
=============
pfnSetFatPHS

The client will interpolate the hear position,
so we can't use a single PHS point
=============
*/
static byte *EngFunc_SetFatPAS(float *org)
{
	if(!sv.worldmodel->visdata || sv_novis->GetInt() || !org || CL_DisableVisibility())
		return Mod_DecompressVis(NULL);
	
	ASSERT(svs.currentPlayerNum >= 0 && svs.currentPlayerNum < MAX_CLIENTS);
	
	fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
	bitvector = fatphs;
	
	// Portals can't change viewpoint!
	if(!(sv.hostflags & SVF_PORTALPASS))
	{
		vec3_t viewPos, offset;
		
		// see code from client.cpp for understanding:
		// org = pView->v.origin + pView->v.view_ofs;
		// if (pView->v.flags & FL_DUCKING)
		// {
		//	org = org + (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
		// }
		// so we have unneeded duck calculations who have affect when player
		// is ducked into water. Remove offset to restore right PVS position
		if(svs.currentPlayer->GetEdict()->v.flags & FL_DUCKING)
		{
			VectorSubtract(svgame.pmove->player_mins[0], svgame.pmove->player_mins[1], offset);
			VectorSubtract(org, offset, viewPos);
		}
		else
			VectorCopy(org, viewPos);
		
		// Build a new PHS frame
		memset(bitvector, 0, fatbytes);
		
		SV_AddToFatPVS(viewPos, DVIS_PHS, sv.worldmodel->nodes);
	}
	else
		SV_AddToFatPVS(org, DVIS_PHS, sv.worldmodel->nodes); // merge PVS
	
	return bitvector;
};

static int EngFunc_CheckVisibility(const edict_t *entity, unsigned char *pset)
{
	if(!IsValidEdict(ent))
	{
		DevMsg(eMsgType_Warning, __FUNCTION__ ": invalid entity %s", SV_ClassName(ent));
		return 0;
	};
	
	// Vis is not set - fullvis enabled
	if(!pset)
		return 1;
	
	if(ent->v.flags & FL_CUSTOMENTITY && ent->v.owner && ent->v.owner->v.flags & FL_CLIENT)
		ent = ent->v.owner;	// upcast beams to my owner
	
	size_t i;
	
	if(ent->headnode < 0)
	{
		// Check individual leafs
		for(i = 0; i < ent->num_leafs; i++)
		{
			// Visible passed by leaf
			if(pset[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
				return 1;
		};
		
		return 0;
	}
	else
	{
		int	leafnum = 0;
		
		for(i = 0; i < MAX_ENT_LEAFS; i++)
		{
			leafnum = ent->leafnums[i];
			
			if(leafnum == -1)
				break;
			
			// Visible passed by leaf
			if(pset[leafnum >> 3] & (1 << (leafnum & 7)))
				return 1;
		};
		
		// Too many leafs for individual check, go by headnode
		if(!SV_HeadnodeVisible(&sv.worldmodel->nodes[ent->headnode], pset, &leafnum))
			return 0;
		
		((edict_t*)ent)->leafnums[ent->num_leafs] = leafnum;
		((edict_t*)ent)->num_leafs = (ent->num_leafs + 1) % MAX_ENT_LEAFS;
		
		// Visible passed by headnode
		return 2;
	};
};

static void EngFunc_DeltaSetField(struct delta_s *apFields, const char *asFieldName)
{
};

static void EngFunc_DeltaUnsetField(struct delta_s *apFields, const char *asFieldName)
{
};

static void EngFunc_DeltaAddEncoder(char *asName, void (*conditionalencode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

static int EngFunc_GetCurrentPlayer()
{
	if(svs.currentPlayer)
		return svs.currentPlayer - svs.clients;
	
	return -1;
};

static int EngFunc_CanSkipPlayer(const edict_t *apPlayer)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apPlayer, false);
	
	if(!pClient)
		return 0;
	
	return pClient->HasLocalWeaponsActive() ? 1 : 0;
};

static int EngFunc_DeltaFindField(struct delta_s *apFields, const char *asFieldName)
{
	return 0;
};

static void EngFunc_DeltaSetFieldByIndex(struct delta_s *apFields, int anFieldNumber)
{
};

static void EngFunc_DeltaUnsetFieldByIndex(struct delta_s *apFields, int anFieldNumber)
{
};

static void EngFunc_SetGroupMask(int anMask, int op)
{
	//svs.groupmask = anMask;
	//svs.groupop = op;
	gpServer->SetGroupMask(anMask, op);
};

static int EngFunc_CreateInstancedBaseline(int anClassName, struct entity_state_s *apBaseline)
{
	return gpGameWorld->CreateInstancedBaseline(anClassName, apBaseline);
};

static void EngFunc_Cvar_DirectSet(struct cvar_s *apCvar, char *asValue)
{
	gpConsole->FindVar(apCvar->name)->SetString(asValue);
};

static void EngFunc_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	if(!filename || !*filename)
		Host_Error(__FUNCTION__ ": bad filename string\n");
	
	sv_consistency_t *pData = nullptr;
	int i;
	
	if(gpEngineServer->GetState() == eServerState_Loading)
	{
		for(i = 0, pData = sv.consistency_files; i < MAX_MODELS; i++, pData++)
		{
			if(!pData->name)
			{
				pData->name = filename;
				pData->force_state = type;
				
				if(mins)
					VectorCopy(mins, pData->mins);
				
				if(maxs)
					VectorCopy(maxs, pData->maxs);
				
				return;
			}
			else if(!strcmp(filename, pData->name))
				return;
		};
		
		Host_Error(__FUNCTION__ ": MAX_MODELS limit exceeded\n");
	}
	else
	{
		for(i = 0, pData = sv.consistency_files; i < MAX_MODELS; i++, pData++)
			if(pData->name && !strcmp(filename, pData->name))
				return;
		
		Host_Error(__FUNCTION__ ": can only be done during precache\n");
	};
};

static void EngFunc_GetPlayerStats(const edict_t *apClient, int *anPing, int *anPacketLoss)
{
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, false);
	
	if(!pClient)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": client is not connected!");
		return;
	};
	
	// This should be cl->latency not ping!
	if(anPing)
		*anPing = pClient->GetPing() * 1000;
	
	if(anPacketLoss)
		*anPacketLoss = pClient->GetPacketLoss();
};

static void EngFunc_AddServerCommand(char *asName, void (*afnCallback)())
{
};

static qboolean EngFunc_Voice_GetClientListening(int anReceiver, int anSender)
{
	if(!gpEngineServer->IsInitialized())
		return false;
	
	int	nMaxClients = sv_maxplayers->GetInt();
	
	if(anReceiver <= 0 || anReceiver > nMaxClients || anSender <= 0 || anSender > nMaxClients)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": invalid client indexes (%i, %i)", anReceiver, anSender);
		return false;
	};
	
	return ((svs.clients[anSender - 1].listeners & (1 << anReceiver)) != 0);
};

static qboolean EngFunc_Voice_SetClientListening(int anReceiverID, int anSenderID, qboolean anListen)
{
	if(!gpEngineServer->IsInitialized())
		return false;
	
	int	nMaxClients = sv_maxplayers->GetInt();
	
	if(anReceiverID <= 0 || anReceiverID > nMaxClients || anSenderID <= 0 || anSenderID > nMaxClients)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": invalid client indexes (%i, %i)", anReceiverID, anSenderID);
		return false;
	};
	
	if(anListen)
		svs.clients[anSenderID - 1].listeners |= (1 << anReceiverID);
	else
		svs.clients[anSenderID - 1].listeners &= ~(1 << anReceiverID);
	
	return true;
};

/*
=============
pfnGetPlayerAuthId

These function must return cd-key hashed value
but Xash3D currently doesn't have any security checks
return nullstring for now
=============
*/
static const char *EngFunc_GetPlayerAuthId(edict_t *apPlayer)
{
	static tString sResult = "";
	
	if(gpEngineServer->GetState() != eServerState_Active || !IsValidEdict(apPlayer))
		return sResult.c_str();
	
	for(auto i = 0, CGameClient *pClient = svs.clients; i < sv_maxplayers->GetInt(); i++, pClient++)
	{
		if(pClient->GetEdict() == apPlayer)
		{
			if(pClient->IsFakeClient())
				sResult = "BOT";
			else if(pClient->GetAuthMethod() == 0)
				sResult = (uint)pClient->GetWonID();
			else
				sResult = pClient->GetIDString();
			
			return sResult.c_str();
		};
	};
	
	return sResult.c_str();
};

/*
=============
pfnSequenceGet

used by CS:CZ
=============
*/
static void *EngFunc_SequenceGet(const char *asFileName, const char *asEntryName)
{
	Msg(__FUNCTION__ ": file %s, entry %s", asFileName, asEntryName);
	return nullptr;
};

/*
=============
pfnSequencePickSentence

used by CS:CZ
=============
*/
static void *EngFunc_SequencePickSentence(const char *asGroupName, int anPickMethod, int *anPicked)
{
	Msg(__FUNCTION__ ": group %s, pickMethod %i", asGroupName, anPickMethod);
	*anPicked = 0;
	
	return nullptr;
};

/*
=============
pfnGetFileSize

returns the filesize in bytes
=============
*/
static int EngFunc_GetFileSize(char *asFileName)
{
	return gpFileSystem->GetFileSize(asFileName, false);
};

static uint EngFunc_GetApproxWavePlayLen(const char *asFilePath)
{
	return 0;
};

/*
=============
pfnIsCareerMatch

used by CS:CZ (client stub)
=============
*/
static int EngFunc_IsCareerMatch()
{
	return 0;
};

/*
=============
pfnGetLocalizedStringLength

used by CS:CZ (client stub)
=============
*/
static int EngFunc_GetLocalizedStringLength(const char *asLabel)
{
	return 0;
};

/*
=============
pfnRegisterTutorMessageShown

only exists in PlayStation version
=============
*/
static void EngFunc_RegisterTutorMessageShown(int mid)
{
};

/*
=============
pfnGetTimesTutorMessageShown

only exists in PlayStation version
=============
*/
static int EngFunc_GetTimesTutorMessageShown(int mid)
{
	return 0;
};

/*
=============
pfnProcessTutorMessageDecayBuffer

only exists in PlayStation version
=============
*/
static void EngFunc_ProcessTutorMessageDecayBuffer(int *apBuffer, int anBufferLength)
{
};

/*
=============
pfnConstructTutorMessageDecayBuffer

only exists in PlayStation version
=============
*/
static void EngFunc_ConstructTutorMessageDecayBuffer(int *apBuffer, int anBufferLength)
{
};

/*
=============
pfnResetTutorMessageDecayData

only exists in PlayStation version
=============
*/
static void EngFunc_ResetTutorMessageDecayData()
{
};

/*
=============
pfnQueryClientCvarValue

request client cvar value
=============
*/
static void EngFunc_QueryClientCvarValue(const edict_t *apClient, const char *asCvarName)
{
	if(!asCvarName || !*asCvarName)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": NULL cvar name!");
		return;
	};
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(pClient)
	{
		pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_querycvarvalue);
		pClient->GetNetChan()->GetMsgBuffer()->WriteString(asCvarName);
	}
	else
	{
		gpGameClientBroadcaster->QueryClientCvarValue(apClient, "Bad Player");
		
		DevMsg(eMsgType_Error, __FUNCTION__ ": tried to send to a non-client!");
	};
};

/*
=============
pfnQueryClientCvarValue2

request client cvar value (bugfixed)
=============
*/
static void EngFunc_QueryClientCvarValue2(const edict_t *apClient, const char *asCvarName, int anRequestID)
{
	if(!asCvarName || !*asCvarName)
	{
		DevMsg(eMsgType_Error, __FUNCTION__ ": NULL cvar name!");
		return;
	};
	
	CGameClient *pClient = gpClientHandler->ClientFromEdict(apClient, true);
	
	if(pClient)
	{
		pClient->GetNetChan()->GetMsgBuffer()->WriteByte(svc_querycvarvalue2);
		pClient->GetNetChan()->GetMsgBuffer()->WriteLong(requestID);
		pClient->GetNetChan()->GetMsgBuffer()->WriteString(asCvarName);
	}
	else
	{
		gpGameClientBroadcaster->QueryClientCvarValueEx(apClient, anRequestID, asCvarName, "Bad Player");
		
		DevMsg(eMsgType_Error, __FUNCTION__ ": tried to send to a non-client!");
	};
};

static int EngFunc_CheckParm(char *asParm, char **ppnext)
{
	static char	str[64];
	
	if(Sys_GetParmFromCmdLine(asParm, str))
	{
		// Get the pointer to cmdline param
		if(ppnext)
			*ppnext = str;
		
		return 1;
	};
	
	return 0;
};