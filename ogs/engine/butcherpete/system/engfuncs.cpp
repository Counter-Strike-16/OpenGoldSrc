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

#include "precompiled.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "system/host_cmd.hpp"
#include "math/mathlib_e.hpp"
#include "world/pr_cmds.hpp"
#include "world/pr_edict.hpp"
#include "physics/sv_move.hpp"
#include "network/delta.hpp"
#include "resources/studio_rehlds.hpp"
#include "voice/voiceserver.hpp"
#include "console/cmd.hpp"
#include "server/server.hpp"

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

enginefuncs_t g_engfuncsExportedToDlls = { PF_precache_model_I,
	                                       PF_precache_sound_I,
	                                       PF_setmodel_I,
	                                       PF_modelindex,
	                                       ModelFrames,
	                                       PF_setsize_I,
	                                       PF_changelevel_I,
	                                       PF_setspawnparms_I,
	                                       SaveSpawnParms,
	                                       PF_vectoyaw_I,
	                                       PF_vectoangles_I,
	                                       SV_MoveToOrigin_I,
	                                       PF_changeyaw_I,
	                                       PF_changepitch_I,
	                                       FindEntityByString,
	                                       GetEntityIllum,
	                                       FindEntityInSphere,
	                                       PF_checkclient_I,
	                                       PVSFindEntities,
	                                       PF_makevectors_I,
	                                       AngleVectors_ext,
	                                       PF_Spawn_I,
	                                       PF_Remove_I,
	                                       CreateNamedEntity,
	                                       PF_makestatic_I,
	                                       PF_checkbottom_I,
	                                       PF_droptofloor_I,
	                                       PF_walkmove_I,
	                                       PF_setorigin_I,
	                                       PF_sound_I,
	                                       PF_ambientsound_I,
	                                       PF_traceline_DLL,
	                                       PF_TraceToss_DLL,
	                                       TraceMonsterHull,
	                                       TraceHull,
	                                       TraceModel,
	                                       TraceTexture,
	                                       TraceSphere,
	                                       PF_aim_I,
	                                       PF_localcmd_I,
	                                       PF_localexec_I,
	                                       PF_stuffcmd_I,
	                                       PF_particle_I,
	                                       PF_lightstyle_I,
	                                       PF_DecalIndex,
	                                       PF_pointcontents_I,
	                                       PF_MessageBegin_I,
	                                       PF_MessageEnd_I,
	                                       PF_WriteByte_I,
	                                       PF_WriteChar_I,
	                                       PF_WriteShort_I,
	                                       PF_WriteLong_I,
	                                       PF_WriteAngle_I,
	                                       PF_WriteCoord_I,
	                                       PF_WriteString_I,
	                                       PF_WriteEntity_I,
	                                       CVarRegister,
	                                       CVarGetFloat,
	                                       CVarGetString,
	                                       CVarSetFloat,
	                                       CVarSetString,
	                                       AlertMessage,
	                                       EngineFprintf,
	                                       PvAllocEntPrivateData,
	                                       PvEntPrivateData,
	                                       FreeEntPrivateData,
	                                       SzFromIndex,
	                                       AllocEngineString,
	                                       GetVarsOfEnt,
	                                       PEntityOfEntOffset,
	                                       EntOffsetOfPEntity,
	                                       IndexOfEdict,
	                                       PEntityOfEntIndex,
	                                       FindEntityByVars,
	                                       GetModelPtr,
	                                       RegUserMsg,
	                                       AnimationAutomove,
	                                       GetBonePosition,
	                                       FunctionFromName,
	                                       NameForFunction,
	                                       ClientPrintf,
	                                       ServerPrint,
	                                       Cmd_Args,
	                                       Cmd_Argv,
	                                       Cmd_Argc,
	                                       GetAttachment,
	                                       CRC32_Init,
	                                       CRC32_ProcessBuffer,
	                                       CRC32_ProcessByte,
	                                       CRC32_Final,
	                                       RandomLong,
	                                       RandomFloat,
	                                       PF_setview_I,
	                                       PF_Time,
	                                       PF_crosshairangle_I,
	                                       COM_LoadFileForMe,
	                                       COM_FreeFile,
	                                       Host_EndSection,
	                                       COM_CompareFileTime,
	                                       COM_GetGameDir,
	                                       Cvar_RegisterVariable,
	                                       PF_FadeVolume,
	                                       PF_SetClientMaxspeed,
	                                       PF_CreateFakeClient_I,
	                                       PF_RunPlayerMove_I,
	                                       PF_NumberOfEntities_I,
	                                       PF_GetInfoKeyBuffer_I,
	                                       PF_InfoKeyValue_I,
	                                       PF_SetKeyValue_I,
	                                       PF_SetClientKeyValue_I,
	                                       PF_IsMapValid_I,
	                                       PF_StaticDecal,
	                                       PF_precache_generic_I,
	                                       PF_GetPlayerUserId,
	                                       PF_BuildSoundMsg_I,
	                                       PF_IsDedicatedServer,
	                                       CVarGetPointer,
	                                       PF_GetPlayerWONId,
	                                       PF_RemoveKey_I,
	                                       PF_GetPhysicsKeyValue,
	                                       PF_SetPhysicsKeyValue,
	                                       PF_GetPhysicsInfoString,
	                                       EV_Precache,
	                                       EV_Playback,
	                                       SV_FatPVS,
	                                       SV_FatPAS,
	                                       SV_CheckVisibility,
	                                       DELTA_SetField,
	                                       DELTA_UnsetField,
	                                       DELTA_AddEncoder,
	                                       PF_GetCurrentPlayer,
	                                       PF_CanSkipPlayer,
	                                       DELTA_FindFieldIndex,
	                                       DELTA_SetFieldByIndex,
	                                       DELTA_UnsetFieldByIndex,
	                                       PF_SetGroupMask,
	                                       PF_CreateInstancedBaseline,
	                                       PF_Cvar_DirectSet,
	                                       PF_ForceUnmodified,
	                                       PF_GetPlayerStats,
	                                       Cmd_AddGameCommand,
	                                       Voice_GetClientListening,
	                                       Voice_SetClientListening,
	                                       PF_GetPlayerAuthId,
	                                       NULL, // sequenceEntry_s *(*pfnSequenceGet)(const char *fileName, const char *entryName);
	                                       NULL, // sentenceEntry_s *(*pfnSequencePickSentence)(const char *groupName, int pickMethod, int *picked);
	                                       COM_FileSize,
	                                       COM_GetApproxWavePlayLength,
	                                       nullptr, //VGuiWrap2_IsInCareerMatch,
	                                       nullptr, //VGuiWrap2_GetLocalizedStringLength,
	                                       nullptr, //RegisterTutorMessageShown,
	                                       nullptr, //GetTimesTutorMessageShown,
	                                       nullptr, //ProcessTutorMessageDecayBuffer,
	                                       nullptr, //ConstructTutorMessageDecayBuffer,
	                                       nullptr, //ResetTutorMessageDecayData,
	                                       QueryClientCvarValue,
	                                       QueryClientCvarValue2,
	                                       EngCheckParm };

#else // HOOK_ENGINE

enginefuncs_t g_engfuncsExportedToDlls;

#endif // HOOK_ENGINE