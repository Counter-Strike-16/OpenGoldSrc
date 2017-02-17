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
/// @brief null implementation for export client engine funcs

#include "precompiled.hpp"
#include "engine/cdll_int.h"

namespace
{
void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a){};

int EngFunc_GetScreenInfo(SCREENINFO *pscrinfo)
{
	return 0;
};

void EngFunc_SetCrosshair(HSPRITE_ hspr, wrect_t rc, int r, int g, int b){};

int EngFunc_HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	return 0;
};

int EngFunc_ServerCmd(char *szCmdString)
{
	return 0;
};

int EngFunc_ClientCmd(char *szCmdString)
{
	return 0;
};

void EngFunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo){};

void EngFunc_PlaySoundByName(char *szSound, float volume){};

void EngFunc_PlaySoundByIndex(int iSound, float volume){};

client_textmessage_t *EngFunc_TextMessageGet(const char *pName)
{
	return nullptr;
};

int EngFunc_DrawCharacter(int x, int y, int number, int r, int g, int b)
{
	return 0;
};

int EngFunc_DrawConsoleString(int x, int y, char *string)
{
	return 0;
};

void EngFunc_DrawSetTextColor(float r, float g, float b){};

void EngFunc_DrawConsoleStringLen(const char *string, int *length, int *height){};

void EngFunc_ConsolePrint(const char *string){};

void EngFunc_CenterPrint(const char *string){};

int EngFunc_GetWindowCenterX()
{
	return 0;
};

int EngFunc_GetWindowCenterY()
{
	return 0;
};

void EngFunc_GetViewAngles(float *){};

void EngFunc_SetViewAngles(float *){};

int EngFunc_GetMaxClients()
{
	return 0;
};

const char *EngFunc_PhysInfo_ValueForKey(const char *key)
{
	return "";
};

const char *EngFunc_ServerInfo_ValueForKey(const char *key)
{
	return "";
};

float EngFunc_GetClientMaxspeed()
{
	return 0.0f;
};

int EngFunc_CheckParm(char *parm, char **ppnext)
{
	return 0;
};

void EngFunc_Key_Event(int key, int down){};

void EngFunc_GetMousePosition(int *mx, int *my){};

int EngFunc_IsNoClipping()
{
	return 0;
};

struct cl_entity_s *EngFunc_GetLocalPlayer()
{
	return nullptr;
};

struct cl_entity_s *EngFunc_GetViewModel()
{
	return nullptr;
};

struct cl_entity_s *EngFunc_GetEntityByIndex(int idx)
{
	return nullptr;
};

float EngFunc_GetClientTime()
{
	return 0.0f;
};

void EngFunc_V_CalcShake(){};

void EngFunc_V_ApplyShake(float *origin, float *angles, float factor){};

int EngFunc_PM_PointContents(float *point, int *truecontents)
{
	return 0;
};

int EngFunc_PM_WaterEntity(float *p)
{
	return 0;
};

struct pmtrace_s *EngFunc_PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe)
{
	return nullptr;
};

struct model_s *EngFunc_CL_LoadModel(const char *modelname, int *index)
{
	return nullptr;
};

int EngFunc_CL_CreateVisibleEntity(int type, struct cl_entity_s *ent)
{
	return 0;
};

void EngFunc_PlaySoundByNameAtLocation(char *szSound, float volume, float *origin){};

unsigned short EngFunc_PrecacheEvent(int type, const char *psz)
{
	return 0;
};

void EngFunc_PlaybackEvent(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2){};

void EngFunc_WeaponAnim(int iAnim, int body){};

long EngFunc_RandomLong(long lLow, long lHigh)
{
	return 0;
};

void EngFunc_HookEvent(char *name,
                       void (*pfnEvent)(struct event_args_s *args)){};

const char *EngFunc_GetGameDirectory()
{
	return "";
};

const char *EngFunc_Key_LookupBinding(const char *pBinding)
{
	return "";
};

const char *EngFunc_GetLevelName()
{
	return "";
};

void EngFunc_GetScreenFade(struct screenfade_s *fade){};

void EngFunc_SetScreenFade(struct screenfade_s *fade){};

int EngFunc_IsSpectateOnly()
{
	return 0;
};

struct model_s *EngFunc_LoadMapSprite(const char *filename)
{
	return nullptr;
};

const char *EngFunc_PlayerInfo_ValueForKey(int playerNum, const char *key)
{
	return "";
};

void EngFunc_PlayerInfo_SetValueForKey(const char *key, const char *value){};

qboolean EngFunc_GetPlayerUniqueID(int iPlayer, char playerID[16])
{
	return false;
};

int EngFunc_GetTrackerIDForPlayer(int playerSlot)
{
	return 0;
};

int EngFunc_GetPlayerForTrackerID(int trackerID)
{
	return 0;
};

int EngFunc_ServerCmdUnreliable(char *szCmdString)
{
	return 0;
};

void EngFunc_GetMousePos(struct tagPOINT *ppt){};

void EngFunc_SetMousePos(int x, int y){};

void EngFunc_SetMouseEnable(qboolean fEnable){};

float EngFunc_GetClientOldTime()
{
	return 0.0f;
};

float EngFunc_GetGravity()
{
	return 0.0f;
};

struct model_s *EngFunc_GetModelByIndex(int index)
{
	return nullptr;
};

void EngFunc_SetFilterMode(int mode){};

void EngFunc_SetFilterColor(float red, float green, float blue){};

void EngFunc_SetFilterBrightness(float brightness){};

void *EngFunc_SequenceGet(const char *fileName, const char *entryName)
{
	return nullptr;
};

void EngFunc_SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height){};

void *EngFunc_SequencePickSentence(const char *groupName, int pickMethod, int *entryPicked)
{
	return nullptr;
};

int EngFunc_DrawString(int x, int y, const char *str, int r, int g, int b)
{
	return 0;
};

int EngFunc_DrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
	return 0;
};

const char *EngFunc_LocalPlayerInfo_ValueForKey(const char *key)
{
	return "";
};

unsigned int EngFunc_GetApproxWavePlayLen(char *filename)
{
	return 0;
};

void *EngFunc_GetCareerGameUI()
{
	return nullptr;
};

int EngFunc_IsPlayingCareerMatch()
{
	return 0;
};

void EngFunc_PlaySoundVoiceByName(char *szSound, float volume, int pitch){};

void EngFunc_PrimeMusicStream(char *filename, int looping){};

double EngFunc_Sys_FloatTime()
{
	return 0.0f;
};

void EngFunc_PlaySoundByNameAtPitch(char *szSound, float volume, int pitch){};

void EngFunc_FillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a){};

int EngFunc_GetAppID()
{
	return 0;
};

void EngFunc_VguiWrap2_GetMouseDelta(int *x, int *y){};

}; // namespace

// clang-format off

cl_enginefunc_t gClEngFuncsNull =
{
};

// clang-format on