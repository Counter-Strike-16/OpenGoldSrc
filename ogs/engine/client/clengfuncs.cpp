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

#include "engine/cdll_int.h"
#include "system/system.hpp"
#include "client/view.hpp"
#include "input/keys.hpp"

namespace
{
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
	float scale_factor   = hud_scale->value;
	clgame.scrInfo.iSize = sizeof(clgame.scrInfo);

	if(scale_factor && scale_factor != 1.0f)
	{
		clgame.scrInfo.iWidth  = scr_width->integer / scale_factor;
		clgame.scrInfo.iHeight = scr_height->integer / scale_factor;
		clgame.scrInfo.iFlags |= SCRINFO_STRETCHED;
	}
	else
	{
		clgame.scrInfo.iWidth  = scr_width->integer;
		clgame.scrInfo.iHeight = scr_height->integer;
		clgame.scrInfo.iFlags &= ~SCRINFO_STRETCHED;
	}

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
	clgame.ds.pCrosshair       = CL_GetSpritePointer(hspr);
	clgame.ds.rcCrosshair      = rc;
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

void EngFunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo){};

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
	for(i = 0; i < MAX_TEXTCHANNELS; i++)
	{
		if(!Q_strcmp(pName, va(TEXT_MSGNAME, i)))
			return cl_textmessage + i;
	};

	// find desired message
	for(i = 0; i < clgame.numTitles; i++)
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
	int drawLen;

	if(!string || !*string)
		return 0; // silent ignore

	clgame.ds.adjust_size = true;
	Con_SetFont(con_fontsize->integer);
	drawLen = Con_DrawString(x, y, string, clgame.ds.textColor);
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

int EngFunc_GetMaxClients(){};

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

int EngFunc_PM_PointContents(float *point, int *truecontents){};

int EngFunc_PM_WaterEntity(float *p){};

struct pmtrace_s *EngFunc_PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe){};

struct model_s *EngFunc_CL_LoadModel(const char *modelname, int *index){};

int EngFunc_CL_CreateVisibleEntity(int type, struct cl_entity_s *ent){};

void EngFunc_PlaySoundByNameAtLocation(char *szSound, float volume, float *origin){};

unsigned short EngFunc_PrecacheEvent(int type, const char *psz){};

void EngFunc_PlaybackEvent(int                   flags,
                           const struct edict_s *pInvoker,
                           unsigned short        eventindex,
                           float                 delay,
                           float *               origin,
                           float *               angles,
                           float                 fparam1,
                           float                 fparam2,
                           int                   iparam1,
                           int                   iparam2,
                           int                   bparam1,
                           int                   bparam2){};

void EngFunc_WeaponAnim(int iAnim, int body){};

long EngFunc_RandomLong(long lLow, long lHigh){};

void EngFunc_HookEvent(char *name, void (*pfnEvent)(struct event_args_s *args)){};

const char *EngFunc_GetGameDirectory()
{
	static char sGameDir[MAX_SYSPATH];

	Q_sprintf(sGameDir, "%s", GI->gamedir);
	return sGameDir;
};

const char *EngFunc_GetLevelName()
{
	static char mapname[64];

	if(cls.state >= ca_connected)
		Q_snprintf(mapname, sizeof(mapname), "maps/%s.bsp", clgame.mapname);
	else
		mapname[0] = '\0'; // not in game

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

int EngFunc_IsSpectateOnly(){};

struct model_s *EngFunc_LoadMapSprite(const char *filename){};

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
	cvar_t *var;

	var = (cvar_t *)Cvar_FindVar(key);
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
	int i;

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(!cl.players[i].userinfo[0] || !cl.players[i].name[0])
			continue;

		if(Q_atoi(Info_ValueForKey(cl.players[i].userinfo, "*tracker")) == trackerID)
		{
			// make into a player slot
			return (i + 1);
		}
	};

	return 0;
};

int EngFunc_ServerCmdUnreliable(char *szCmdString)
{
	if(!szCmdString || !szCmdString[0])
		return 0;

	BF_WriteByte(&cls.datagram, clc_stringcmd);
	BF_WriteString(&cls.datagram, szCmdString);

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
	return clmove.gravity;
};

struct model_s *EngFunc_GetModelByIndex(int index){};

void EngFunc_SetFilterMode(int mode){};

void EngFunc_SetFilterColor(float red, float green, float blue){};

void EngFunc_SetFilterBrightness(float brightness){};

void *EngFunc_SequenceGet(const char *fileName, const char *entryName){};

void EngFunc_SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height)
{
	pglEnable(GL_BLEND);
	pglBlendFunc(blendsrc, blenddst); // g-cont. are params is valid?
	SPR_DrawGeneric(frame, x, y, width, height, prc);
};

void *EngFunc_SequencePickSentence(const char *groupName, int pickMethod, int *entryPicked){};

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
	char *szIt;

	for(szIt = (char *)str; *szIt != 0; szIt++)
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
	return 130; //return 220; // standard Valve value
};

void EngFunc_VguiWrap2_GetMouseDelta(int *x, int *y){
    // TODO: implement
};

}; // namespace

cl_enginefunc_t gClEngFuncs =
    {
        pfnSPR_Load,
        pfnSPR_Frames,
        pfnSPR_Height,
        pfnSPR_Width,
        pfnSPR_Set,
        pfnSPR_Draw,
        pfnSPR_DrawHoles,
        pfnSPR_DrawAdditive,
        SPR_EnableScissor,
        SPR_DisableScissor,
        pfnSPR_GetList,

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
        pfnCalcShake,
        pfnApplyShake,
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
        &gTriApi,
        &gEfxApi,
        &gEventApi,
        &gDemoApi,
        &gNetApi,
        &gVoiceApi,
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
        pfnVguiWrap2_GetMouseDelta};