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
	int (*pfnPrecacheModel)(char *s);
	int (*pfnPrecacheSound)(char *s);
	
	int (*pfnModelIndex)(const char *m);
	int (*pfnModelFrames)(int modelIndex);
	
	void (*pfnChangeLevel)(char *s1, char *s2);
	
	float (*pfnVecToYaw)(const float *rgflVector);
	void (*pfnVecToAngles)(const float *rgflVectorIn, float *rgflVectorOut);
	
	edict_t *(*pfnFindEntityByString)(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue);
	
	edict_t *(*pfnFindEntityInSphere)(edict_t *pEdictStartSearchAfter, const float *org, float rad);
	edict_t *(*pfnFindClientInPVS)(edict_t *pEdict);
	edict_t *(*pfnEntitiesInPVS)(edict_t *pplayer);
	
	void (*pfnMakeVectors)(const float *rgflVector);
	void (*pfnAngleVectors)(const float *rgflVector, float *forward, float *right, float *up);
	
	edict_t *(*pfnCreateEntity)();
	void (*pfnRemoveEntity)(edict_t *e);
	edict_t *(*pfnCreateNamedEntity)(int className);
	
	void (*pfnMakeStatic)(edict_t *ent);
	int (*pfnEntIsOnFloor)(edict_t *e);
	int (*pfnDropToFloor)(edict_t *e);
	int (*pfnWalkMove)(edict_t *ent, float yaw, float dist, int iMode);
	void (*pfnSetOrigin)(edict_t *e, const float *rgflOrigin);
	void (*pfnEmitSound)(edict_t *entity, int channel, const char *sample, /*int*/ float volume, float attenuation, int fFlags, int pitch);
	void (*pfnEmitAmbientSound)(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch);
	
	void (*pfnTraceLine)(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
	void (*pfnTraceToss)(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr);
	int (*pfnTraceMonsterHull)(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
	void (*pfnTraceHull)(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr);
	void (*pfnTraceModel)(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr);
	const char *(*pfnTraceTexture)(edict_t *pTextureEntity, const float *v1, const float *v2);
	void (*pfnTraceSphere)(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr);
	
	void (*pfnGetAimVector)(edict_t *ent, float speed, float *rgflReturn);
	
	void (*pfnServerCommand)(char *str);
	void (*pfnServerExecute)();
	
	void (*pfnClientCommand)(edict_t *pEdict, char *szFmt, ...);
	
	void (*pfnParticleEffect)(const float *org, const float *dir, float color, float count);
	void (*pfnLightStyle)(int style, char *val);
	int (*pfnDecalIndex)(const char *name);
	int (*pfnPointContents)(const float *rgflVector);
	
	virtual INetMsg &MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) = 0;
	virtual void MessageEnd(INetMsg &apMsg) = 0;
	
	void (*pfnCVarRegister)(cvar_t *pCvar);
	float (*pfnCVarGetFloat)(const char *szVarName);
	const char *(*pfnCVarGetString)(const char *szVarName);
	void (*pfnCVarSetFloat)(const char *szVarName, float flValue);
	void (*pfnCVarSetString)(const char *szVarName, const char *szValue);
	
	void (*pfnAlertMessage)(ALERT_TYPE atype, char *szFmt, ...);
	
	void (*pfnEngineFprintf)(void *pfile, char *szFmt, ...);
	
	void *(*pfnPvAllocEntPrivateData)(edict_t *pEdict, int32 cb);
	void *(*pfnPvEntPrivateData)(edict_t *pEdict);
	void (*pfnFreeEntPrivateData)(edict_t *pEdict);
	
	const char *(*pfnSzFromIndex)(int iString);
	int (*pfnAllocString)(const char *szValue);
	
	struct entvars_s *(*pfnGetVarsOfEnt)(edict_t *pEdict);
	edict_t *(*pfnPEntityOfEntOffset)(int iEntOffset);
	int (*pfnEntOffsetOfPEntity)(const edict_t *pEdict);
	int (*pfnIndexOfEdict)(const edict_t *pEdict);
	edict_t *(*pfnPEntityOfEntIndex)(int iEntIndex);
	edict_t *(*pfnFindEntityByVars)(struct entvars_s *pvars);
	void *(*pfnGetModelPtr)(edict_t *pEdict);
	int (*pfnRegUserMsg)(const char *pszName, int iSize);
	void (*pfnAnimationAutomove)(const edict_t *pEdict, float flTime);
	void (*pfnGetBonePosition)(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles);
	uint32 (*pfnFunctionFromName)(const char *pName);
	const char *(*pfnNameForFunction)(uint32 function);
	void (*pfnClientPrintf)(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg); // JOHN: engine callbacks so game DLL can print messages to individual clients
	void (*pfnServerPrint)(const char *szMsg);
	const char *(*pfnCmd_Args)();     // these 3 added
	const char *(*pfnCmd_Argv)(int argc); // so game DLL can easily
	int (*pfnCmd_Argc)();             // access client 'cmd' strings
	void (*pfnGetAttachment)(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles);
	
	void (*pfnCRC32_Init)(CRC32_t *pulCRC);
	void (*pfnCRC32_ProcessBuffer)(CRC32_t *pulCRC, void *p, int len);
	void (*pfnCRC32_ProcessByte)(CRC32_t *pulCRC, unsigned char ch);
	CRC32_t (*pfnCRC32_Final)(CRC32_t pulCRC);
	
	int32 (*pfnRandomLong)(int32 lLow, int32 lHigh);
	float (*pfnRandomFloat)(float flLow, float flHigh);
	
	void (*pfnSetView)(const edict_t *pClient, const edict_t *pViewent);
	float (*pfnTime)();
	void (*pfnCrosshairAngle)(const edict_t *pClient, float pitch, float yaw);
	byte *(*pfnLoadFileForMe)(char *filename, int *pLength);
	void (*pfnFreeFile)(void *buffer);
	void (*pfnEndSection)(const char *pszSectionName); // trigger_endsection
	int (*pfnCompareFileTime)(char *filename1, char *filename2, int *iCompare);
	void (*pfnGetGameDir)(char *szGetGameDir);
	void (*pfnCvar_RegisterVariable)(cvar_t *variable);
	void (*pfnFadeClientVolume)(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds);
	void (*pfnSetClientMaxspeed)(const edict_t *pEdict, float fNewMaxspeed);
	edict_t *(*pfnCreateFakeClient)(const char *netname); // returns NULL if fake client can't be created
	void (*pfnRunPlayerMove)(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec);
	int (*pfnNumberOfEntities)();
	char *(*pfnGetInfoKeyBuffer)(edict_t *e); // passing in NULL gets the serverinfo
	char *(*pfnInfoKeyValue)(char *infobuffer, char *key);
	void (*pfnSetKeyValue)(char *infobuffer, char *key, char *value);
	void (*pfnSetClientKeyValue)(int clientIndex, char *infobuffer, char *key, char *value);
	int (*pfnIsMapValid)(char *filename);
	void (*pfnStaticDecal)(const float *origin, int decalIndex, int entityIndex, int modelIndex);
	int (*pfnPrecacheGeneric)(char *s);
	int (*pfnGetPlayerUserId)(edict_t *e); // returns the server assigned userid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
	void (*pfnBuildSoundMsg)(edict_t *entity, int channel, const char *sample, /*int*/ float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
	int (*pfnIsDedicatedServer)(); // is this a dedicated server?
	cvar_t *(*pfnCVarGetPointer)(const char *szVarName);
	unsigned int (*pfnGetPlayerWONId)(edict_t *e); // returns the server assigned WONid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients

	void (*pfnInfo_RemoveKey)(char *s, const char *key);
	const char *(*pfnGetPhysicsKeyValue)(const edict_t *pClient, const char *key);
	void (*pfnSetPhysicsKeyValue)(const edict_t *pClient, const char *key, const char *value);
	const char *(*pfnGetPhysicsInfoString)(const edict_t *pClient);
	unsigned short (*pfnPrecacheEvent)(int type, const char *psz);
	void (*pfnPlaybackEvent)(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);

	unsigned char *(*pfnSetFatPVS)(float *org);
	unsigned char *(*pfnSetFatPAS)(float *org);

	int (*pfnCheckVisibility)(const edict_t *entity, unsigned char *pset);

	void (*pfnDeltaSetField)(struct delta_s *pFields, const char *fieldname);
	void (*pfnDeltaUnsetField)(struct delta_s *pFields, const char *fieldname);
	void (*pfnDeltaAddEncoder)(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to));
	int (*pfnGetCurrentPlayer)();
	int (*pfnCanSkipPlayer)(const edict_t *player);
	int (*pfnDeltaFindField)(struct delta_s *pFields, const char *fieldname);
	void (*pfnDeltaSetFieldByIndex)(struct delta_s *pFields, int fieldNumber);
	void (*pfnDeltaUnsetFieldByIndex)(struct delta_s *pFields, int fieldNumber);

	void (*pfnSetGroupMask)(int mask, int op);

	int (*pfnCreateInstancedBaseline)(int classname, struct entity_state_s *baseline);
	void (*pfnCvar_DirectSet)(struct cvar_s *var, char *value);

	void (*pfnForceUnmodified)(FORCE_TYPE type, float *mins, float *maxs, const char *filename);

	void (*pfnGetPlayerStats)(const edict_t *pClient, int *ping, int *packet_loss);

	void (*pfnAddServerCommand)(char *cmd_name, void (*function)());

	// For voice communications, set which clients hear eachother.
	// NOTE: these functions take player entity indices (starting at 1).
	qboolean (*pfnVoice_GetClientListening)(int iReceiver, int iSender);
	qboolean (*pfnVoice_SetClientListening)(int iReceiver, int iSender, qboolean bListen);

	const char *(*pfnGetPlayerAuthId)(edict_t *e);

	// PSV: Added for CZ training map
	//	const char *(*pfnKeyNameForBinding)		( const char* pBinding );

	/*void*/ sequenceEntry_s *(*pfnSequenceGet)(const char *fileName, const char *entryName);
	/*void*/ sentenceEntry_s *(*pfnSequencePickSentence)(const char *groupName, int pickMethod, int *picked);

	// LH: Give access to filesize via filesystem
	int (*pfnGetFileSize)(char *filename);

	unsigned int (*pfnGetApproxWavePlayLen)(const char *filepath);
	
	int (*pfnIsCareerMatch)();

	int (*pfnGetLocalizedStringLength)(const char *label);

	void (*pfnRegisterTutorMessageShown)(int mid);
	int (*pfnGetTimesTutorMessageShown)(int mid);
	void (*pfnProcessTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	void (*pfnConstructTutorMessageDecayBuffer)(int *buffer, int bufferLength);
	void (*pfnResetTutorMessageDecayData)();

	void (*pfnQueryClientCvarValue)(const edict_t *player, const char *cvarName);
	void (*pfnQueryClientCvarValue2)(const edict_t *player, const char *cvarName, int requestID);

	int (*pfnCheckParm)(const char *pchCmdLineToken, char **ppnext);
};