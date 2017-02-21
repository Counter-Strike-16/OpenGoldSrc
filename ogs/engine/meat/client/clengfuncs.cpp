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
/// @brief export engine functions for client side

#include "precompiled.hpp"
#include "system/system.hpp"
#include "system/common.hpp"
#include "client/cl_demo.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "engine/cdll_int.h"
#include "graphics/spriteapi.hpp"
#include "graphics/view.hpp"
#include "input/keys.hpp"
#include "voice/voicetweak.hpp"
#include "common/net_api.h"

#define OGS_CLENGFUNCS_NULL_IMPL

extern triangleapi_t gTriAPI;
extern efx_api_t gEfxAPI;
extern event_api_t gEventAPI;
extern net_api_t gNetAPI;

namespace
{

#ifndef OGS_CLENGFUNCS_NULL_IMPL

void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
	float x1 = x, y1 = y, w1 = width, h1 = height;

	r = bound(0, r, 255);
	g = bound(0, g, 255);
	b = bound(0, b, 255);
	a = bound(0, a, 255);

	pglColor4ub(r, g, b, a);

	SPR_AdjustSize(&x1, &y1, &w1, &h1);

	GL_SetRenderMode(kRenderTransAdd);
	R_DrawStretchPic(x1, y1, w1, h1, 0, 0, 1, 1, cls.fillImage);
	pglColor4ub(255, 255, 255, 255);
};

// get actual screen info
int EngFunc_GetScreenInfo(SCREENINFO *pscrinfo)
{
	// setup screen info
	float scale_factor = hud_scale->value;

	clgame.scrInfo.iSize = sizeof(clgame.scrInfo);

	if(scale_factor && scale_factor != 1.0f)
	{
		clgame.scrInfo.iWidth = scr_width->integer / scale_factor;
		clgame.scrInfo.iHeight = scr_height->integer / scale_factor;
		clgame.scrInfo.iFlags |= SCRINFO_STRETCHED;
	}
	else
	{
		clgame.scrInfo.iWidth = scr_width->integer;
		clgame.scrInfo.iHeight = scr_height->integer;
		clgame.scrInfo.iFlags &= ~SCRINFO_STRETCHED;
	};

	if(!pscrinfo)
		return 0;

	if(pscrinfo->iSize != clgame.scrInfo.iSize)
		clgame.scrInfo.iSize = pscrinfo->iSize;

	// copy screeninfo out
	Q_memcpy(pscrinfo, &clgame.scrInfo, clgame.scrInfo.iSize);
	return 1;
};

void EngFunc_SetCrosshair(HSPRITE_ hspr, wrect_t rc, int r, int g, int b)
{
	clgame.ds.rgbaCrosshair[0] = (byte)r;
	clgame.ds.rgbaCrosshair[1] = (byte)g;
	clgame.ds.rgbaCrosshair[2] = (byte)b;
	clgame.ds.rgbaCrosshair[3] = (byte)0xFF;

	clgame.ds.pCrosshair = CL_GetSpritePointer(hspr);
	clgame.ds.rcCrosshair = rc;
};

int EngFunc_HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	int i;

	// ignore blank names or invalid callbacks
	if(!pszName || !*pszName || !pfn)
		return 0;

	for(i = 0; i < MAX_USER_MESSAGES && clgame.msg[i].name[0]; i++)
	{
		// see if already hooked
		if(!Q_strcmp(clgame.msg[i].name, pszName))
			return 1;
	};

	if(i == MAX_USER_MESSAGES)
	{
		Host_Error("HookUserMsg: MAX_USER_MESSAGES hit!\n");
		return 0;
	};

	// hook new message
	Q_strncpy(clgame.msg[i].name, pszName, sizeof(clgame.msg[i].name));
	clgame.msg[i].func = pfn;

	return 1;
};

int EngFunc_ServerCmd(char *szCmdString)
{
	string buf;

	if(!szCmdString || !szCmdString[0])
		return 0;

	// just like the client typed "cmd xxxxx" at the console
	Q_snprintf(buf, sizeof(buf) - 1, "cmd %s\n", szCmdString);
	Cbuf_AddText(buf);

	return 1;
};

int EngFunc_ClientCmd(char *szCmdString)
{
	if(!szCmdString || !szCmdString[0])
		return 0;

	Cbuf_AddText(szCmdString);
	Cbuf_AddText("\n");
	return 1;
};

void EngFunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
	player_info_t *player;
	cl_entity_t *ent;
	qboolean spec = false;

	ent = CL_GetEntityByIndex(ent_num);
	ent_num -= 1; // player list if offset by 1 from ents

	if(ent_num >= cl.maxclients || ent_num < 0 || !cl.players[ent_num].name[0])
	{
		Q_memset(pinfo, 0, sizeof(*pinfo));
		return;
	}

	player = &cl.players[ent_num];
	pinfo->thisplayer = (ent_num == cl.playernum) ? true : false;
	if(ent)
		spec = ent->curstate.spectator;

	pinfo->name = player->name;
	pinfo->model = player->model;

	pinfo->spectator = spec;
	pinfo->ping = player->ping;
	pinfo->packetloss = player->packet_loss;
	pinfo->topcolor = Q_atoi(Info_ValueForKey(player->userinfo, "topcolor"));
	pinfo->bottomcolor =
	Q_atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));
};

void EngFunc_PlaySoundByName(char *szSound, float volume)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_ITEM, hSound, volume, ATTN_NORM, PITCH_NORM, SND_STOP_LOOPING);
};

void EngFunc_PlaySoundByIndex(int iSound, float volume)
{
	int hSound;

	// make sure what we in-bounds
	iSound = bound(0, iSound, MAX_SOUNDS);
	hSound = cl.sound_index[iSound];

	if(!hSound)
	{
		MsgDev(D_ERROR, "CL_PlaySoundByIndex: invalid sound handle %i\n", iSound);
		return;
	};

	S_StartSound(NULL, cl.refdef.viewentity, CHAN_ITEM, hSound, volume, ATTN_NORM, PITCH_NORM, SND_STOP_LOOPING);
};

// returns specified message from titles.txt
client_textmessage_t *EngFunc_TextMessageGet(const char *pName)
{
	int i;

	// first check internal messages
	for(i = 0; i < MAX_TEXTCHANNELS; ++i)
	{
		if(!Q_strcmp(pName, va(TEXT_MSGNAME, i)))
			return cl_textmessage + i;
	};

	// find desired message
	for(i = 0; i < clgame.numTitles; ++i)
	{
		if(!Q_stricmp(pName, clgame.titles[i].pName))
			return clgame.titles + i;
	};

	return NULL; // found nothing
};

// returns drawed chachter width (in real screen pixels)
int EngFunc_DrawCharacter(int x, int y, int number, int r, int g, int b)
{
	if(!cls.creditsFont.valid)
		return 0;

	number &= 255;
	number = Con_UtfProcessChar(number);

	if(number < 32)
		return 0;

	if(y < -clgame.scrInfo.iCharHeight)
		return 0;

	clgame.ds.adjust_size = true;
	pfnPIC_Set(cls.creditsFont.hFontTexture, r, g, b, 255);
	pfnPIC_DrawAdditive(x, y, -1, -1, &cls.creditsFont.fontRc[number]);
	clgame.ds.adjust_size = false;

	return clgame.scrInfo.charWidths[number];
};

int EngFunc_DrawConsoleString(int x, int y, char *string)
{
	if(!string || !*string)
		return 0; // silent ignore

	clgame.ds.adjust_size = true;
	Con_SetFont(con_fontsize->integer);

	int drawLen = Con_DrawString(x, y, string, clgame.ds.textColor);

	MakeRGBA(clgame.ds.textColor, 255, 255, 255, 255);
	clgame.ds.adjust_size = false;
	Con_RestoreFont();

	return x + drawLen; // exclude color prexfixes
};

void EngFunc_DrawSetTextColor(float r, float g, float b)
{
	// bound color and convert to byte
	clgame.ds.textColor[0] = (byte)bound(0, r * 255, 255);
	clgame.ds.textColor[1] = (byte)bound(0, g * 255, 255);
	clgame.ds.textColor[2] = (byte)bound(0, b * 255, 255);
	clgame.ds.textColor[3] = (byte)0xFF;
};

void EngFunc_DrawConsoleStringLen(const char *string, int *length, int *height)
{
	Con_SetFont(con_fontsize->integer);
	Con_DrawStringLen(string, length, height);
	Con_RestoreFont();
};

// prints directly into console (can skip notify)
void EngFunc_ConsolePrint(const char *string)
{
	if(!string || !*string)
		return;

	if(*string != 1)
		Msg("%s", string); // show notify
	else
		Con_NPrintf(0, "%s", (char *)string + 1); // skip notify
};

/*
=============
pfnCenterPrint

holds and fade message at center of screen
like trigger_multiple message in q1
=============
*/
void EngFunc_CenterPrint(const char *string)
{
	// someone's stupid joke
	if(!string || !*string)
		return;

	CL_CenterPrint(string, 0.25f);
};

int EngFunc_GetWindowCenterX(){};

int EngFunc_GetWindowCenterY(){};

// return interpolated angles from previous frame
void EngFunc_GetViewAngles(float *angles)
{
	if(angles)
		VectorCopy(cl.refdef.cl_viewangles, angles);
};

void EngFunc_SetViewAngles(float *angles)
{
	if(angles)
		VectorCopy(angles, cl.refdef.cl_viewangles);
};

/*
====================
CL_GetMaxClients

Render callback for studio models
====================
*/
int EngFunc_GetMaxClients() // CL_
{
	return cl.maxclients;
};

const char *EngFunc_PhysInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(cl.frame.client.physinfo, key);
};

const char *EngFunc_ServerInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(cl.serverinfo, key);
};

float EngFunc_GetClientMaxspeed()
{
	return cl.frame.client.maxspeed;
};

int EngFunc_CheckParm(char *parm, char **ppnext)
{
	static char str[64];

	if(Sys_GetParmFromCmdLine(parm, str))
	{
		// get the pointer on cmdline param
		if(ppnext)
			*ppnext = str;

		return 1;
	};

	return 0;
};

void EngFunc_GetMousePosition(int *mx, int *my){};

int EngFunc_IsNoClipping()
{
	cl_entity_t *pl = CL_GetLocalPlayer();

	if(!pl)
		return false;

	return pl->curstate.movetype == MOVETYPE_NOCLIP;
};

struct cl_entity_s *EngFunc_GetLocalPlayer(){};

struct cl_entity_s *EngFunc_GetViewModel()
{
	return &clgame.viewent;
};

struct cl_entity_s *EngFunc_GetEntityByIndex(int idx){};

float EngFunc_GetClientTime()
{
	return cl.time;
};

int PM_PointContents(float *point, int *truecontents)
{
	int cont, truecont;

	truecont = cont = CL_TruePointContents(p);

	if(truecontents)
		*truecontents = truecont;

	if(cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
		cont = CONTENTS_WATER;

	return cont;
};

int PM_WaterEntity(float *p){};

struct pmtrace_s *PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe){};

struct model_s *CL_LoadModel(const char *modelname, int *index)
{
	int idx = CL_FindModelIndex(modelname);

	if(!idx)
		return NULL;

	if(index)
		*index = idx;

	return Mod_Handle(idx);
};

int CL_CreateVisibleEntity(int type, struct cl_entity_s *ent){};

void EngFunc_PlaySoundByNameAtLocation(char *szSound, float volume, float *origin)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(origin, 0, CHAN_AUTO, hSound, volume, ATTN_NORM, PITCH_NORM, 0);
};

unsigned short EngFunc_PrecacheEvent(int type, const char *psz){};

void EngFunc_PlaybackEvent(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2){};

void EngFunc_WeaponAnim(int iAnim, int body){};

long EngFunc_RandomLong(long lLow, long lHigh){};

// pfnHookEvent( const char *filename, pfnEventHook pfn )
void EngFunc_HookEvent(char *name,
                       void (*pfnEvent)(struct event_args_s *args))
{
	char name[64];
	cl_user_event_t *ev;
	int i;

	// ignore blank names
	if(!filename || !*filename)
		return;

	Q_strncpy(name, filename, sizeof(name));
	COM_FixSlashes(name);

	// find an empty slot
	for(i = 0; i < MAX_EVENTS; i++)
	{
		ev = clgame.events[i];
		if(!ev)
			break;

		if(!Q_stricmp(name, ev->name) && ev->func != NULL)
		{
			MsgDev(D_WARN, "CL_HookEvent: %s already hooked!\n", name);
			return;
		};
	};

	CL_RegisterEvent(i, name, pfn);
};

const char *EngFunc_GetGameDirectory()
{
	static char sGameDir[MAX_SYSPATH];

	Q_sprintf(sGameDir, "%s", GI->gamedir);
	return sGameDir;
};

const char *EngFunc_GetLevelName()
{
	static char mapname[64];
	mapname[0] = '\0';

	if(cls.state >= ca_connected)
		Q_snprintf(mapname, sizeof(mapname), "maps/%s.bsp", clgame.mapname);

	return mapname;
};

void EngFunc_GetScreenFade(struct screenfade_s *fade)
{
	if(fade)
		*fade = clgame.fade;
};

void EngFunc_SetScreenFade(struct screenfade_s *fade)
{
	if(fade)
		clgame.fade = *fade;
};

void *VGui_GetPanel()
{
	// stub
	return nullptr;
};

void VGui_ViewportPaintBackground(int extents[4])
{
	// stub
};

int EngFunc_IsSpectateOnly()
{
	cl_entity_t *pPlayer = CL_GetLocalPlayer();
	return pPlayer ? (pPlayer->curstate.spectator != 0) : 0;
};

struct model_s *EngFunc_LoadMapSprite(const char *filename)
{
	char name[64];
	int i;
	int texFlags = TF_NOPICMIP;

	if(cl_sprite_nearest->value)
		texFlags |= TF_NEAREST;

	if(!filename || !*filename)
	{
		MsgDev(D_ERROR, "CL_LoadMapSprite: bad name!\n");
		return NULL;
	}

	Q_strncpy(name, filename, sizeof(name));
	COM_FixSlashes(name);

	// slot 0 isn't used
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!Q_stricmp(clgame.sprites[i].name, name))
		{
			// prolonge registration
			clgame.sprites[i].needload = clgame.load_sequence;
			return &clgame.sprites[i];
		}
	}

	// find a free model slot spot
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!clgame.sprites[i].name[0])
			break; // this is a valid spot
	}

	if(i == MAX_IMAGES)
	{
		MsgDev(D_ERROR,
		       "LoadMapSprite: can't load %s, MAX_HSPRITES limit exceeded\n",
		       filename);
		return NULL;
	}

	// load new map sprite
	if(CL_LoadHudSprite(name, &clgame.sprites[i], true, texFlags))
	{
		clgame.sprites[i].needload = clgame.load_sequence;
		return &clgame.sprites[i];
	};

	return NULL;
};

const char *EngFunc_PlayerInfo_ValueForKey(int playerNum, const char *key)
{
	// find the player
	if((playerNum > cl.maxclients) || (playerNum < 1))
		return NULL;

	if(!cl.players[playerNum - 1].name[0])
		return NULL;

	return Info_ValueForKey(cl.players[playerNum - 1].userinfo, key);
};

void EngFunc_PlayerInfo_SetValueForKey(const char *key, const char *value)
{
	cvar_t *var = Cvar_FindVar(key);

	if(!var || !(var->flags & CVAR_USERINFO))
		return;

	Cvar_DirectSet(var, value);
};

qboolean EngFunc_GetPlayerUniqueID(int iPlayer, char playerID[16])
{
	// TODO: implement

	playerID[0] = '\0';
	return false;
};

int EngFunc_GetTrackerIDForPlayer(int playerSlot)
{
	playerSlot -= 1; // make into a client index

	if(!cl.players[playerSlot].userinfo[0] || !cl.players[playerSlot].name[0])
		return 0;

	return Q_atoi(Info_ValueForKey(cl.players[playerSlot].userinfo, "*tracker"));
};

int EngFunc_GetPlayerForTrackerID(int trackerID)
{
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(!cl.players[i].userinfo[0] || !cl.players[i].name[0])
			continue;

		if(Q_atoi(Info_ValueForKey(cl.players[i].userinfo, "*tracker")) ==
		   trackerID)
		{
			// make into a player slot
			return (i + 1);
		};
	};

	return 0;
};

int EngFunc_ServerCmdUnreliable(char *szCmdString)
{
	if(!szCmdString || !szCmdString[0])
		return 0;

	MSG_WriteByte(&cls.datagram, clc_stringcmd);
	MSG_WriteString(&cls.datagram, szCmdString);

	return 1;
};

void EngFunc_GetMousePos(struct tagPOINT *ppt){};

void EngFunc_SetMousePos(int x, int y){};

void EngFunc_SetMouseEnable(qboolean fEnable)
{
	if(fEnable)
		IN_ActivateMouse(false);
	else
		IN_DeactivateMouse();
};

float EngFunc_GetClientOldTime()
{
	return cl.oldtime;
};

float EngFunc_GetGravity()
{
	return g_clmove.gravity;
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

void *EngFunc_SequencePickSentence(const char *groupName, int pickMethod, int *entryPicked)
{
	return nullptr;
};

int EngFunc_DrawString(int x, int y, const char *str, int r, int g, int b)
{
	Con_UtfProcessChar(0);

	// draw the string until we hit the null character or a newline character
	for(; *str != 0 && *str != '\n'; str++)
		x += pfnDrawCharacter(x, y, (unsigned char)*str, r, g, b);

	return x;
};

int EngFunc_DrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
	// find the end of the string
	for(char *szIt = (char *)str; *szIt != 0; szIt++)
		x -= clgame.scrInfo.charWidths[(unsigned char)*szIt];

	pfnDrawString(x, y, str, r, g, b);
	return x;
};

const char *EngFunc_LocalPlayerInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(Cvar_Userinfo(), key);
};

unsigned int EngFunc_GetApproxWavePlayLen(char *filename)
{
	return 0;
};

void *EngFunc_GetCareerGameUI()
{
	Con_DPrintf("^1Career GameInterface called!\n");
	return NULL;
};

int EngFunc_IsPlayingCareerMatch()
{
	return 0;
};

void EngFunc_PlaySoundVoiceByName(char *szSound, float volume, int pitch)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_AUTO, hSound, volume, ATTN_NORM, pitch, SND_STOP_LOOPING);
};

void EngFunc_PrimeMusicStream(char *filename, int looping){};

void EngFunc_PlaySoundByNameAtPitch(char *szSound, float volume, int pitch)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_STATIC, hSound, volume, ATTN_NORM, pitch, SND_STOP_LOOPING);
};

void EngFunc_FillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a)
{
	float x1 = x, y1 = y, w1 = width, h1 = height;

	r = bound(0, r, 255);
	g = bound(0, g, 255);
	b = bound(0, b, 255);
	a = bound(0, a, 255);

	pglColor4ub(r, g, b, a);

	SPR_AdjustSize(&x1, &y1, &w1, &h1);

	pglEnable(GL_BLEND);
	pglDisable(GL_ALPHA_TEST);
	pglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	R_DrawStretchPic(x1, y1, w1, h1, 0, 0, 1, 1, cls.fillImage);
	pglColor4ub(255, 255, 255, 255);
};

int EngFunc_GetAppID()
{
	return 130; // return 220; // standard Valve value
};

void EngFunc_VguiWrap2_GetMouseDelta(int *x, int *y)
{
	// TODO: implement
};

#else // if defined(OGS_CLENGFUNCS_NULL_IMPL)

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

#endif // OGS_CLENGFUNCS_NULL_IMPL

}; // namespace

cl_enginefunc_t gClEngFuncs =
{
  SPR_Load,

  SPR_Frames,

  SPR_Height,
  SPR_Width,

  SPR_Set,

  SPR_Draw,
  SPR_DrawHoles,
  SPR_DrawAdditive,

  SPR_EnableScissor,
  SPR_DisableScissor,

  SPR_GetList,

  pfnFillRGBA,

  pfnGetScreenInfo,

  pfnSetCrosshair,

  (void *)pfnCvar_RegisterVariable,
  (void *)Cvar_VariableValue,
  (void *)Cvar_VariableString,

  (void *)pfnAddClientCommand,

  (void *)pfnHookUserMsg,

  (void *)pfnServerCmd,
  (void *)pfnClientCmd,

  pfnGetPlayerInfo,

  (void *)pfnPlaySoundByName,
  pfnPlaySoundByIndex,

  AngleVectors,

  CL_TextMessageGet,

  pfnDrawCharacter,
  pfnDrawConsoleString,
  pfnDrawSetTextColor,
  pfnDrawConsoleStringLen,

  pfnConsolePrint,
  pfnCenterPrint,

  pfnGetWindowCenterX,
  pfnGetWindowCenterY,

  pfnGetViewAngles,
  pfnSetViewAngles,

  CL_GetMaxClients,

  (void *)Cvar_SetFloat,

  Cmd_Argc,
  Cmd_Argv,

  Con_Printf,
  Con_DPrintf,
  Con_NPrintf,
  Con_NXPrintf,

  pfnPhysInfo_ValueForKey,
  pfnServerInfo_ValueForKey,

  pfnGetClientMaxspeed,
  pfnCheckParm,
  (void *)Key_Event,
  CL_GetMousePosition,
  pfnIsNoClipping,
  CL_GetLocalPlayer,
  pfnGetViewModel,
  CL_GetEntityByIndex,
  pfnGetClientTime,
  
  V_CalcShake,
  V_ApplyShake,
  
  (void *)pfnPointContents,
  (void *)CL_WaterEntity,
  pfnTraceLine,
  CL_LoadModel,
  CL_AddEntity,
  CL_GetSpritePointer,
  pfnPlaySoundByNameAtLocation,

  pfnPrecacheEvent,
  CL_PlaybackEvent,

  CL_WeaponAnim,

  Com_RandomFloat,
  Com_RandomLong,

  (void *)pfnHookEvent,
  (void *)Con_Visible,
  pfnGetGameDirectory,
  pfnCVarGetPointer,
  Key_LookupBinding,
  pfnGetLevelName,

  pfnGetScreenFade,
  pfnSetScreenFade,

  VGui_GetPanel,
  VGui_ViewportPaintBackground,

  (void *)COM_LoadFile,
  COM_ParseFile,
  COM_FreeFile,

  &gTriAPI,
  &gEfxAPI,
  &gEventAPI,
  &gDemoAPI,
  &gNetAPI,
  &gVoiceAPI,

  pfnIsSpectateOnly,
  pfnLoadMapSprite,
  COM_AddAppDirectoryToSearchPath,
  COM_ExpandFilename,
  PlayerInfo_ValueForKey,
  PlayerInfo_SetValueForKey,
  pfnGetPlayerUniqueID,
  pfnGetTrackerIDForPlayer,
  pfnGetPlayerForTrackerID,
  pfnServerCmdUnreliable,

  pfnGetMousePos,
  pfnSetMousePos,

  pfnSetMouseEnable,

  Cvar_GetList,
  (void *)Cmd_GetFirstFunctionHandle,
  (void *)Cmd_GetNextFunctionHandle,
  (void *)Cmd_GetName,
  pfnGetClientOldTime,
  pfnGetGravity,
  Mod_Handle,
  pfnEnableTexSort,
  pfnSetLightmapColor,
  pfnSetLightmapScale,
  pfnSequenceGet,
  pfnSPR_DrawGeneric,
  pfnSequencePickSentence,
  pfnDrawString,
  pfnDrawStringReverse,
  LocalPlayerInfo_ValueForKey,
  pfnVGUI2DrawCharacter,
  pfnVGUI2DrawCharacterAdditive,
  (void *)Sound_GetApproxWavePlayLen,
  GetCareerGameInterface,
  (void *)Cvar_Set,
  pfnIsCareerMatch,
  pfnPlaySoundVoiceByName,
  pfnMP3_InitStream,
  Sys_DoubleTime,
  pfnProcessTutorMessageDecayBuffer,
  pfnConstructTutorMessageDecayBuffer,
  pfnResetTutorMessageDecayData,
  pfnPlaySoundByNameAtPitch,
  pfnFillRGBABlend,
  pfnGetAppID,
  Cmd_AliasGetList,
  pfnVguiWrap2_GetMouseDelta
};