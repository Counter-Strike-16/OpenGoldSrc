#include "client/hlcompatclientdll/ClientEngineFuncs.hpp"
#include "debug/Debug.hpp"

// Engine callbacks for client dll
cl_enginefunc_t gEngfuncs = 
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
	//pfnCvar_RegisterVariable,
	//Cvar_VariableValue,
	//Cvar_VariableString,
	//pfnAddClientCommand,
	pfnHookUserMsg,
	pfnServerCmd,
	pfnClientCmd,
	pfnGetPlayerInfo,
	pfnPlaySoundByName,
	pfnPlaySoundByIndex,
	//AngleVectors,
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
	//CL_GetMaxClients,
	//Cvar_SetFloat,
	//Cmd_Argc,
	//Cmd_Argv,
	//Con_Printf,
	//Con_DPrintf,
	//Con_NPrintf,
	//Con_NXPrintf,
	pfnPhysInfo_ValueForKey,
	pfnServerInfo_ValueForKey,
	pfnGetClientMaxspeed,
	pfnCheckParm,
	//Key_Event,
	pfnGetMousePosition,
	pfnIsNoClipping,
	//CL_GetLocalPlayer,
	pfnGetViewModel,
	//CL_GetEntityByIndex,
	pfnGetClientTime,
	pfnCalcShake,
	pfnApplyShake,
	pfnPointContents,
	//CL_WaterEntity,
	pfnTraceLine,
	//CL_LoadModel,
	//CL_AddEntity,
	CL_GetSpritePointer,
	pfnPlaySoundByNameAtLocation,
	pfnPrecacheEvent,
	//CL_PlaybackEvent,
	//CL_WeaponAnim,
	//Com_RandomFloat,
	//Com_RandomLong,
	pfnHookEvent,
	//Con_Visible,
	pfnGetGameDirectory,
	//pfnCVarGetPointer,
	Key_LookupBinding,
	pfnGetLevelName,
	pfnGetScreenFade,
	pfnSetScreenFade,
	//VGui_GetPanel,
	//VGui_ViewportPaintBackground,
	//COM_LoadFile,
	//COM_ParseFile,
	//COM_FreeFile,
	//&gTriApi,
	//&gEfxApi,
	//&gEventApi,
	//&gDemoApi,
	//&gNetApi,
	//&gVoiceApi,
	pfnIsSpectateOnly,
	pfnLoadMapSprite,
	//COM_AddAppDirectoryToSearchPath,
	//COM_ExpandFilename,
	PlayerInfo_ValueForKey,
	PlayerInfo_SetValueForKey,
	pfnGetPlayerUniqueID,
	pfnGetTrackerIDForPlayer,
	pfnGetPlayerForTrackerID,
	pfnServerCmdUnreliable,
	pfnGetMousePos,
	pfnSetMousePos,
	pfnSetMouseEnable,
	//Cvar_GetList,
	//Cmd_GetFirstFunctionHandle,
	//Cmd_GetNextFunctionHandle,
	//Cmd_GetName,
	pfnGetClientOldTime,
	pfnGetGravity,
	//Mod_Handle,
	pfnEnableTexSort,
	pfnSetLightmapColor,
	pfnSetLightmapScale,
	//pfnSequenceGet,
	pfnSPR_DrawGeneric,
	//pfnSequencePickSentence,
	pfnDrawString,
	pfnDrawStringReverse,
	LocalPlayerInfo_ValueForKey,
	pfnVGUI2DrawCharacter,
	pfnVGUI2DrawCharacterAdditive,
	//Sound_GetApproxWavePlayLen,
	GetCareerGameInterface,
	//Cvar_Set,
	//pfnIsCareerMatch,
	pfnPlaySoundVoiceByName,
	pfnMP3_InitStream,
	//Sys_DoubleTime,
	//pfnProcessTutorMessageDecayBuffer,
	//pfnConstructTutorMessageDecayBuffer,
	//pfnResetTutorMessageDecayData,
	pfnPlaySoundByNameAtPitch,
	pfnFillRGBABlend,
	pfnGetAppID,
	//Cmd_AliasGetList,
	pfnVguiWrap2_GetMouseDelta
};

CClientEngineFuncs::CClientEngineFuncs()
{
	LogMsg(eMsgType_Info, "Constructing the hl-compat client engine functions component...");
};

CClientEngineFuncs::~CClientEngineFuncs()
{
	LogMsg(eMsgType_Info, "Destructing the hl-compat client engine functions component...");
};

void CClientEngineFuncs::Init()
{
	LogMsg(eMsgType_Info, "Initializing the hl-compat client engine functions component...");
};

/*
=========
pfnSPR_Load
=========
*/
static HSPRITE_ EngFunc_SPR_Load(const char *szPicName)
{
	return pfnSPR_LoadExt(szPicName, 0);
};

/*
=========
pfnSPR_Frames
=========
*/
static int EngFunc_SPR_Frames(HSPRITE_ hPic)
{
	int	numFrames;
	R_GetSpriteParms(NULL, NULL, &numFrames, 0, CL_GetSpritePointer(hPic));
	return numFrames;
};

/*
=========
pfnSPR_Height
=========
*/
static int EngFunc_SPR_Height(HSPRITE_ hPic, int frame)
{
	int	sprHeight;
	R_GetSpriteParms(NULL, &sprHeight, NULL, frame, CL_GetSpritePointer(hPic));
	return sprHeight;
};

/*
=========
pfnSPR_Width
=========
*/
static int EngFunc_SPR_Width(HSPRITE_ hPic, int frame)
{
	int	sprWidth;
	R_GetSpriteParms(&sprWidth, NULL, NULL, frame, CL_GetSpritePointer(hPic));
	return sprWidth;
};

/*
=========
pfnSPR_Set
=========
*/
static void EngFunc_SPR_Set(HSPRITE_ hPic, int r, int g, int b)
{
	clgame.ds.pSprite = CL_GetSpritePointer(hPic);
	clgame.ds.spriteColor[0] = bound(0, r, 255);
	clgame.ds.spriteColor[1] = bound(0, g, 255);
	clgame.ds.spriteColor[2] = bound(0, b, 255);
	clgame.ds.spriteColor[3] = 255;
	
	// Set default state
	pglDisable(GL_BLEND);
	pglDisable(GL_ALPHA_TEST);
	pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
};

/*
=========
pfnSPR_Draw
=========
*/
static void EngFunc_SPR_Draw(int frame, int x, int y, const wrect_t *prc)
{
	pglEnable(GL_ALPHA_TEST);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

/*
=========
pfnSPR_DrawHoles
=========
*/
static void EngFunc_SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc)
{
	GL_SetRenderMode(kRenderTransAlpha);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

/*
=========
pfnSPR_DrawAdditive
=========
*/
static void EngFunc_SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc)
{
	GL_SetRenderMode(kRenderTransAdd);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

/*
=========
SPR_EnableScissor
=========
*/
static void EngFunc_SPR_EnableScissor(int x, int y, int width, int height)
{
	// Check bounds
	x = clamp(0, x, clgame.scrInfo.iWidth);
	y = clamp(0, y, clgame.scrInfo.iHeight);
	width = clamp(0, width, clgame.scrInfo.iWidth - x);
	height = clamp(0, height, clgame.scrInfo.iHeight - y);
	
	clgame.ds.scissor_x = x;
	clgame.ds.scissor_width = width;
	clgame.ds.scissor_y = y;
	clgame.ds.scissor_height = height;
	clgame.ds.scissor_test = true;
};

/*
=========
SPR_DisableScissor
=========
*/
static void EngFunc_SPR_DisableScissor()
{
	clgame.ds.scissor_x = 0;
	clgame.ds.scissor_width = 0;
	clgame.ds.scissor_y = 0;
	clgame.ds.scissor_height = 0;
	clgame.ds.scissor_test = false;
};

/*
=========
pfnSPR_GetList

for parsing half-life scripts - hud.txt etc
=========
*/
static client_sprite_t *EngFunc_SPR_GetList(char *psz, int *piCount)
{
	client_sprite_t	*pList;
	int index, numSprites = 0;
	char *afile, *pfile;
	string token;
	byte *pool;
	
	if(piCount)
		*piCount = 0;
	
	if(!clgame.itemspath[0])	// typically it's sprites\*.txt
		FS_ExtractFilePath(psz, clgame.itemspath);
	
	afile = FS_LoadFile(psz, NULL, false);
	
	if(!afile)
		return nullptr;
	
	pfile = afile;
	pfile = COM_ParseFile(pfile, token);          
	numSprites = Q_atoi(token);
	
	if(!cl.video_prepped)
		pool = cls.mempool; // static memory
	else
		pool = com_studiocache; // temporary
	
	// name, res, pic, x, y, w, h
	// NOTE: we must use com_studiocache because it will be purge on next restart or change map
	pList = Mem_Alloc(pool, sizeof(client_sprite_t) * numSprites);
	
	for(index = 0; index < numSprites; index++)
	{
		if((pfile = COM_ParseFile(pfile, token)) == NULL)
			break;
		
		Q_strncpy(pList[index].szName, token, sizeof(pList[index].szName));
		
		// Read resolution
		pfile = COM_ParseFile(pfile, token);
		pList[index].iRes = Q_atoi(token);
		
		// Read spritename
		pfile = COM_ParseFile(pfile, token);
		Q_strncpy(pList[index].szSprite, token, sizeof(pList[index].szSprite));
		
		// Parse rectangle
		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.left = Q_atoi(token);
		
		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.top = Q_atoi(token);
		
		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.right = pList[index].rc.left + Q_atoi(token);
		
		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.bottom = pList[index].rc.top + Q_atoi(token);
		
		if(piCount)
			(*piCount)++;
	};
	
	if(index < numSprites)
		DevMsg(eMsgType_Warning, "SPR_GetList: unexpected end of %s(%i should be %i)", psz, numSprites, index);
	
	Mem_Free(afile);
	return pList;
};

/*
=============
pfnFillRGBA
=============
*/
static void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
	r = bound(0, r, 255);
	g = bound(0, g, 255);
	b = bound(0, b, 255);
	a = bound(0, a, 255);
	pglColor4ub(r, g, b, a);
	
	SPR_AdjustSize((float *)&x,(float *)&y,(float *)&width,(float *)&height);
	
	GL_SetRenderMode(kRenderTransAdd);
	R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, cls.fillImage);
	pglColor4ub(255, 255, 255, 255);
};

/*
=============
pfnGetScreenInfo

get actual screen info
=============
*/
static int EngFunc_GetScreenInfo(SCREENINFO *pscrinfo)
{
	// Setup screen info
	clgame.scrInfo.iSize = sizeof(clgame.scrInfo);
	
	if(Cvar_VariableInteger("hud_scale"))
	{
		if(scr_width->integer < 640)
		{
			// Virtual screen space 320x200
			clgame.scrInfo.iWidth = 320;
			clgame.scrInfo.iHeight = 200;
		}
		else
		{
			// Virtual screen space 640x480
			clgame.scrInfo.iWidth = 640;
			clgame.scrInfo.iHeight = 480;
		};
		
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
	
	// Copy screeninfo out
	Q_memcpy(pscrinfo, &clgame.scrInfo, clgame.scrInfo.iSize);
	
	return 1;
};

/*
=============
pfnSetCrosshair

setup crosshair
=============
*/
static void EngFunc_SetCrosshair(HSPRITE_ hspr, wrect_t rc, int r, int g, int b)
{
	clgame.ds.rgbaCrosshair[0] =(byte)r;
	clgame.ds.rgbaCrosshair[1] =(byte)g;
	clgame.ds.rgbaCrosshair[2] =(byte)b;
	clgame.ds.rgbaCrosshair[3] =(byte)0xFF;
	clgame.ds.pCrosshair = CL_GetSpritePointer(hspr);
	clgame.ds.rcCrosshair = rc;
};

/*
=============
pfnHookUserMsg
=============
*/
static int EngFunc_HookUserMsg(const char *pszName, pfnUserMsgHook pfn)
{
	int	i;
	
	// Ignore blank names or invalid callbacks
	if(!pszName || !*pszName || !pfn)
		return 0;
	
	for(i = 0; i < MAX_USER_MESSAGES && clgame.msg[i].name[0]; i++)
	{
		// See if already hooked
		if(!Q_strcmp(clgame.msg[i].name, pszName))
			return 1;
	};
	
	if(i == MAX_USER_MESSAGES) 
	{
		Host_Error("HookUserMsg: MAX_USER_MESSAGES hit!");
		return 0;
	};
	
	// Hook new message
	Q_strncpy(clgame.msg[i].name, pszName, sizeof(clgame.msg[i].name));
	clgame.msg[i].func = pfn;
	
	return 1;
};

/*
=============
pfnServerCmd
=============
*/
static int EngFunc_ServerCmd(const char *szCmdString)
{
	string buf;
	
	if(!szCmdString || !szCmdString[0])
		return 0;
	
	// Just like the client typed "cmd xxxxx" to the console
	Q_snprintf(buf, sizeof(buf) - 1, "cmd %s\n", szCmdString);
	Cbuf_AddText(buf);
	
	return 1;
};

/*
=============
pfnClientCmd
=============
*/
static int EngFunc_ClientCmd(const char *szCmdString)
{
	if(!szCmdString || !szCmdString[0])
		return 0;
	
	Cbuf_AddText(szCmdString);
	Cbuf_AddText("\n");
	return 1;
};

/*
=============
pfnGetPlayerInfo
=============
*/
static void EngFunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
	player_info_t	*player;
	cl_entity_t	*ent;
	bool spec = false;
	
	ent = CL_GetEntityByIndex(ent_num);
	ent_num -= 1; // player list if offset by 1 from ents
	
	if(ent_num >= cl.maxclients || ent_num < 0 || !cl.players[ent_num].name[0])
	{
		Q_memset(pinfo, 0, sizeof(*pinfo));
		return;
	};
	
	player = &cl.players[ent_num];
	pinfo->thisplayer =(ent_num == cl.playernum) ? true : false;
	
	if(ent)
		spec = ent->curstate.spectator;
	
	pinfo->name = player->name;
	pinfo->model = player->model;
	
	pinfo->spectator = spec;		
	pinfo->ping = player->ping;
	pinfo->packetloss = player->packet_loss;
	pinfo->topcolor = Q_atoi(Info_ValueForKey(player->userinfo, "topcolor"));
	pinfo->bottomcolor = Q_atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));
};

/*
=============
pfnPlaySoundByName
=============
*/
static void EngFunc_PlaySoundByName(const char *szSound, float volume)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_ITEM, hSound, volume, ATTN_NORM, PITCH_NORM, SND_STOP_LOOPING);
};

/*
=============
pfnPlaySoundByIndex
=============
*/
static void EngFunc_PlaySoundByIndex(int iSound, float volume)
{
	// Make sure what we in-bounds
	iSound = clamp(0, iSound, MAX_SOUNDS);
	
	int hSound = cl.sound_index[iSound];
	
	if(!hSound)
	{
		DevMsg(eMsgType_Error, "CL_PlaySoundByIndex: invalid sound handle %i", iSound);
		return;
	};
	
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_ITEM, hSound, volume, ATTN_NORM, PITCH_NORM, SND_STOP_LOOPING);
};

/*
=============
pfnTextMessageGet

returns specified message from titles.txt
=============
*/
static client_textmessage_t *EngFunc_CL_TextMessageGet(const char *pName)
{
	int	i;
	
	// First check internal messages
	for(i = 0; i < MAX_TEXTCHANNELS; i++)
	{
		if(!Q_strcmp(pName, va(TEXT_MSGNAME, i)))
			return cl_textmessage + i;
	};
	
	// Find desired message
	for(i = 0; i < clgame.numTitles; i++)
	{
		if(!Q_stricmp(pName, clgame.titles[i].pName))
			return clgame.titles + i;
	};
	
	return NULL; // found nothing
};

/*
=============
pfnDrawCharacter

returns drawed chachter width(in real screen pixels)
=============
*/
static int EngFunc_DrawCharacter(int x, int y, int number, int r, int g, int b)
{
	if(!cls.creditsFont.valid)
		return 0;
	
	number &= 255;
	
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

/*
=============
pfnDrawConsoleString

drawing string like a console string 
=============
*/
static int EngFunc_DrawConsoleString(int x, int y, char *string)
{
	// Silent ignore
	if(!string || !*string)
		return 0;
	
	clgame.ds.adjust_size = true;
	Con_SetFont(con_fontsize->integer);
	
	int drawLen = Con_DrawString(x, y, string, clgame.ds.textColor);
	
	MakeRGBA(clgame.ds.textColor, 255, 255, 255, 255);
	clgame.ds.adjust_size = false;
	Con_RestoreFont();
	
	return (x + drawLen); // exclude color prexfixes
};

/*
=============
pfnDrawSetTextColor

set color for anything
=============
*/
static void EngFunc_DrawSetTextColor(float r, float g, float b)
{
	// Bound color and convert to byte
	clgame.ds.textColor[0] = (byte)bound(0, r * 255, 255);
	clgame.ds.textColor[1] = (byte)bound(0, g * 255, 255);
	clgame.ds.textColor[2] = (byte)bound(0, b * 255, 255);
	clgame.ds.textColor[3] = (byte)0xFF;
};

/*
=============
pfnDrawConsoleStringLen

compute string length in screen pixels
=============
*/
static void EngFunc_DrawConsoleStringLen(const char *pText, int *length, int *height)
{
	Con_SetFont(con_fontsize->integer);
	Con_DrawStringLen(pText, length, height);
	Con_RestoreFont();
};

/*
=============
pfnConsolePrint

prints directly into console(can skip notify)
=============
*/
static void EngFunc_ConsolePrint(const char *string)
{
	if(!string || !*string)
		return;
	
	if(*string != 1)
		Con_Print((char*)string); // show notify
	else
		Con_NPrintf(0, (char*)string + 1); // skip notify
};

/*
=============
pfnCenterPrint

holds and fade message at center of screen
like trigger_multiple message in q1
=============
*/
static void EngFunc_CenterPrint(const char *string)
{
	// Someone stupid joke
	if(!string || !*string)
		return;
	
	CL_CenterPrint(string, 0.25f);
};

/*
=========
GetWindowCenterX
=========
*/
static int EngFunc_GetWindowCenterX()
{
	return host.window_center_x;
};

/*
=========
GetWindowCenterY
=========
*/
static int EngFunc_GetWindowCenterY()
{
	return host.window_center_y;
};

/*
=============
pfnGetViewAngles

return interpolated angles from previous frame
=============
*/
static void EngFunc_GetViewAngles(float *angles)
{
	if(angles)
		VectorCopy(cl.refdef.cl_viewangles, angles);
};

/*
=============
pfnSetViewAngles

return interpolated angles from previous frame
=============
*/
static void EngFunc_SetViewAngles(float *angles)
{
	if(angles)
		VectorCopy(angles, cl.refdef.cl_viewangles);
};

/*
=============
pfnPhysInfo_ValueForKey
=============
*/
static const char *EngFunc_PhysInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(cl.frame.local.client.physinfo, key);
};

/*
=============
pfnServerInfo_ValueForKey
=============
*/
static const char *EngFunc_ServerInfo_ValueForKey(const char *key)
{
	return Info_ValueForKey(cl.serverinfo, key);
};

/*
=============
pfnGetClientMaxspeed

value that come from server
=============
*/
static float EngFunc_GetClientMaxspeed()
{
	return cl.frame.local.client.maxspeed;
};

/*
=============
pfnCheckParm
=============
*/
static int EngFunc_CheckParm(char *parm, char **ppnext)
{
	static char	str[64];
	
	if(Sys_GetParmFromCmdLine(parm, str))
	{
		// Get the pointer on cmdline param
		if(ppnext)
			*ppnext = str;
		
		return 1;
	};
	
	return 0;
};

/*
=============
pfnGetMousePosition
=============
*/
static void EngFunc_GetMousePosition(int *mx, int *my)
{
	POINT curpos;
	
	GetCursorPos(&curpos);
	ScreenToClient(host.hWnd, &curpos);
	
	if(mx)
		*mx = curpos.x;
	
	if(my)
		*my = curpos.y;
};

/*
=============
pfnIsNoClipping
=============
*/
static int EngFunc_IsNoClipping()
{
	cl_entity_t *pl = CL_GetLocalPlayer();
	
	if(!pl)
		return false;
	
	return pl->curstate.movetype == MOVETYPE_NOCLIP;
};

/*
=============
pfnGetViewModel
=============
*/
static cl_entity_t* EngFunc_GetViewModel()
{
	return &clgame.viewent;
};

/*
=============
pfnGetClientTime
=============
*/
static float EngFunc_GetClientTime()
{
	return cl.time;
};

/*
=============
pfnCalcShake
=============
*/
static void EngFunc_CalcShake()
{
	int	i;
	float	fraction, freq;
	float	localAmp;
	
	if(clgame.shake.time == 0)
		return;
	
	if((cl.time > clgame.shake.time) || clgame.shake.amplitude <= 0 || clgame.shake.frequency <= 0)
	{
		Q_memset(&clgame.shake, 0, sizeof(clgame.shake));
		return;
	};
	
	if(cl.time > clgame.shake.next_shake)
	{
		// Higher frequency means we recalc the extents more often and perturb the display again
		clgame.shake.next_shake = cl.time +(1.0f / clgame.shake.frequency);
		
		// Compute random shake extents(the shake will settle down from this)
		for(i = 0; i < 3; i++)
			clgame.shake.offset[i] = Com_RandomFloat(-clgame.shake.amplitude, clgame.shake.amplitude);
		
		clgame.shake.angle = Com_RandomFloat(-clgame.shake.amplitude * 0.25f, clgame.shake.amplitude * 0.25f);
	};
	
	// Ramp down amplitude over duration(fraction goes from 1 to 0 linearly with slope 1/duration)
	fraction =(clgame.shake.time - cl.time) / clgame.shake.duration;
	
	// Ramp up frequency over duration
	if(fraction)
		freq =(clgame.shake.frequency / fraction);
	else
		freq = 0;
	
	// Square fraction to approach zero more quickly
	fraction *= fraction;
	
	// Sine wave that slowly settles to zero
	fraction = fraction * sin(cl.time * freq);
	
	// Add to view origin
	VectorScale(clgame.shake.offset, fraction, clgame.shake.applied_offset);
	
	// Add to roll
	clgame.shake.applied_angle = clgame.shake.angle * fraction;
	
	// Drop amplitude a bit, less for higher frequency shakes
	localAmp = clgame.shake.amplitude *(host.frametime /(clgame.shake.duration * clgame.shake.frequency));
	clgame.shake.amplitude -= localAmp;
};

/*
=============
pfnApplyShake
=============
*/
static void EngFunc_ApplyShake(float *origin, float *angles, float factor)
{
	if(origin)
		VectorMA(origin, factor, clgame.shake.applied_offset, origin);
	
	if(angles)
		angles[ROLL] += clgame.shake.applied_angle * factor;
};

/*
=============
pfnPointContents
=============
*/
static int EngFunc_PointContents(const float *p, int *truecontents)
{
	int	cont, truecont;
	
	truecont = cont = CL_TruePointContents(p);
	
	if(truecontents)
		*truecontents = truecont;

	if(cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
		cont = CONTENTS_WATER;
	
	return cont;
};

/*
=============
pfnTraceLine
=============
*/
static pmtrace_t *EngFunc_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe)
{
	static pmtrace_t tr;
	int old_usehull = clgame.pmove->usehull;
	
	clgame.pmove->usehull = usehull;
	
	switch(flags)
	{
	case PM_TRACELINE_PHYSENTSONLY:
		tr = PM_PlayerTraceExt(clgame.pmove, start, end, 0, clgame.pmove->numphysent, clgame.pmove->physents, ignore_pe, NULL);
		break;
	case PM_TRACELINE_ANYVISIBLE:
		tr = PM_PlayerTraceExt(clgame.pmove, start, end, 0, clgame.pmove->numvisent, clgame.pmove->visents, ignore_pe, NULL);
		break;
	};
	
	clgame.pmove->usehull = old_usehull;
	return &tr;
};

/*
=============
CL_GetSpritePointer
=============
*/
static const model_t *EngFunc_CL_GetSpritePointer(HSPRITE_ hSprite)
{
	// Bad image
	if(hSprite <= 0 || hSprite >(MAX_IMAGES - 1))
		return nullptr;
	
	return &clgame.sprites[hSprite];
};

static void EngFunc_PlaySoundByNameAtLocation(char *szSound, float volume, float *origin)
{
	int hSound = S_RegisterSound(szSound);
	S_StartSound(origin, 0, CHAN_AUTO, hSound, volume, ATTN_NORM, PITCH_NORM, 0);
};

/*
=============
pfnPrecacheEvent
=============
*/
static word EngFunc_PrecacheEvent(int type, const char* psz)
{
	return CL_EventIndex(psz);
};

/*
=============
pfnHookEvent
=============
*/
static void EngFunc_HookEvent(const char *filename, pfnEventHook pfn)
{
	char name[64];
	cl_user_event_t	*ev;
	int i;
	
	// Ignore blank names
	if(!filename || !*filename)
		return;	
	
	Q_strncpy(name, filename, sizeof(name));
	COM_FixSlashes(name);
	
	// Find an empty slot
	for(i = 0; i < MAX_EVENTS; i++)
	{
		ev = clgame.events[i];		
		if(!ev)
			break;
		
		if(!Q_stricmp(name, ev->name) && ev->func != NULL)
		{
			DevMsg(eMsgType_Warning, "CL_HookEvent: %s already hooked!");
			return;
		};
	};
	
	CL_RegisterEvent(i, name, pfn);
};

/*
=============
pfnGetGameDirectory
=============
*/
static const char *EngFunc_GetGameDirectory()
{
	static char	szGetGameDir[MAX_SYSPATH];
	
	Q_sprintf(szGetGameDir, "%s", GI->gamedir);
	return szGetGameDir;
};

/*
=============
Key_LookupBinding
=============
*/
static const char *EngFunc_Key_LookupBinding(const char *pBinding)
{
	return Key_KeynumToString(Key_GetKey(pBinding));
};

/*
=============
pfnGetLevelName
=============
*/
static const char *EngFunc_GetLevelName()
{
	static char	mapname[64];
	
	if(cls.state >= ca_connected)
		Q_snprintf(mapname, sizeof(mapname), "maps/%s.bsp", clgame.mapname);
	else
		mapname[0] = '\0'; // not in game
	
	return mapname;
};

/*
=============
pfnGetScreenFade
=============
*/
static void EngFunc_GetScreenFade(struct screenfade_s *fade)
{
	if(fade)
		*fade = clgame.fade;
};

/*
=============
pfnSetScreenFade
=============
*/
static void EngFunc_SetScreenFade(struct screenfade_s *fade)
{
	if(fade)
		clgame.fade = *fade;
};

/*
=============
pfnIsSpectateOnly
=============
*/
static int EngFunc_IsSpectateOnly()
{
	cl_entity_t *pPlayer = CL_GetLocalPlayer();
	return pPlayer ?(pPlayer->curstate.spectator != 0) : 0;
};

/*
=============
pfnLoadMapSprite
=============
*/
static model_t *EngFunc_LoadMapSprite(const char *filename)
{
	char name[64];
	int	i;
	
	if(!filename || !*filename)
	{
		DevMsg(eMsgType_Error, "CL_LoadMapSprite: bad name!");
		return nullptr;
	};
	
	Q_strncpy(name, filename, sizeof(name));
	COM_FixSlashes(name);
	
	// Slot 0 isn't used
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!Q_stricmp(clgame.sprites[i].name, name))
		{
			// Prolonge registration
			clgame.sprites[i].needload = clgame.load_sequence;
			return &clgame.sprites[i];
		};
	};
	
	// Find a free model slot spot
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!clgame.sprites[i].name[0])
			break; // This is a valid spot
	};
	
	if(i == MAX_IMAGES) 
	{
		DevMsg(eMsgType_Error, "LoadMapSprite: can't load %s, MAX_HSPRITES limit exceeded", filename);
		return nullptr;
	};
	
	// Load new map sprite
	if(CL_LoadHudSprite(name, &clgame.sprites[i], true, 0))
	{
		clgame.sprites[i].needload = clgame.load_sequence;
		return &clgame.sprites[i];
	};
	
	return nullptr;
};

/*
=============
PlayerInfo_ValueForKey
=============
*/
static const char *EngFunc_PlayerInfo_ValueForKey(int playerNum, const char *key)
{
	// Find the player
	if((playerNum > cl.maxclients) ||(playerNum < 1))
		return "";
	
	if((cl.players[playerNum-1].name == NULL) ||(*(cl.players[playerNum-1].name) == 0))
		return "";
	
	return Info_ValueForKey(cl.players[playerNum-1].userinfo, key);
};

/*
=============
PlayerInfo_SetValueForKey
=============
*/
static void EngFunc_PlayerInfo_SetValueForKey(const char *key, const char *value)
{
	cvar_t *var = (cvar_t*)Cvar_FindVar(key);
	
	if(!var || !(var->flags & CVAR_USERINFO))
		return;
	
	Cvar_DirectSet(var, value);
};

/*
=============
pfnGetPlayerUniqueID
=============
*/
static qboolean EngFunc_GetPlayerUniqueID(int iPlayer, char playerID[16])
{
	// TODO: implement
	playerID[0] = '\0';
	return 0;
};

/*
=============
pfnGetTrackerIDForPlayer
=============
*/
static int EngFunc_GetTrackerIDForPlayer(int playerSlot)
{
	playerSlot -= 1; // Make into a client index
	
	if(!cl.players[playerSlot].userinfo[0] || !cl.players[playerSlot].name[0])
			return 0;
	
	return Q_atoi(Info_ValueForKey(cl.players[playerSlot].userinfo, "*tracker"));
};

/*
=============
pfnGetPlayerForTrackerID
=============
*/
static int EngFunc_GetPlayerForTrackerID(int trackerID)
{
	for(auto i = 0; i < MAX_CLIENTS; i++)
	{
		if(!cl.players[i].userinfo[0] || !cl.players[i].name[0])
			continue;
		
		if(Q_atoi(Info_ValueForKey(cl.players[i].userinfo, "*tracker")) == trackerID)
		{
			// Make into a player slot
			return (i+1);
		};
	};
	
	return 0;
};

/*
=============
pfnServerCmdUnreliable
=============
*/
static int EngFunc_ServerCmdUnreliable(char *szCmdString)
{
	if(!szCmdString || !szCmdString[0])
		return 0;
	
	BF_WriteByte(&cls.datagram, clc_stringcmd);
	BF_WriteString(&cls.datagram, szCmdString);
	
	return 1;
};

/*
=============
pfnGetMousePos
=============
*/
static void EngFunc_GetMousePos(struct tagPOINT *ppt)
{
	ASSERT(ppt != nullptr);
	
	// Find mouse movement
	GetCursorPos(ppt);
	ScreenToClient(host.hWnd, ppt);
};

/*
=============
pfnSetMousePos
=============
*/
static void EngFunc_SetMousePos(int mx, int my)
{
	POINT pt;
	
	pt.x = mx;
	pt.y = my;
	
	ClientToScreen(host.hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
};

/*
=============
pfnSetMouseEnable
=============
*/
static void EngFunc_SetMouseEnable(int anEnable)
{
	if(anEnable)
		IN_ActivateMouse(false);
	else
		IN_DeactivateMouse();
};

/*
=============
pfnGetServerTime
=============
*/
static float EngFunc_GetClientOldTime()
{
	return cl.oldtime;
};

/*
=============
pfnGetGravity
=============
*/
static float EngFunc_GetGravity()
{
	return clgame.movevars.gravity;
};

/*
=============
pfnEnableTexSort

TODO: implement
=============
*/
static void EngFunc_EnableTexSort(int anEnable)
{
};

/*
=============
pfnSetLightmapColor

TODO: implement
=============
*/
static void EngFunc_SetLightmapColor(float afRed, float afGreen, float afBlue)
{
};

/*
=============
pfnSetLightmapScale

TODO: implement
=============
*/
static void EngFunc_SetLightmapScale(float afScale)
{
};

/*
=============
pfnSPR_DrawGeneric
=============
*/
static void EngFunc_SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height)
{
	pglEnable(GL_BLEND);
	pglBlendFunc(blendsrc, blenddst); // g-cont: Are params valid?
	SPR_DrawGeneric(frame, x, y, width, height, prc);
};

/*
=============
pfnDrawString

TODO: implement
=============
*/
static int EngFunc_DrawString(int x, int y, const char *str, int r, int g, int b)
{
	return 0;
};

/*
=============
pfnDrawStringReverse

TODO: implement
=============
*/
static int EngFunc_DrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
	return 0;
};

/*
=============
LocalPlayerInfo_ValueForKey
=============
*/
static const char *EngFunc_LocalPlayerInfo_ValueForKey(const char *asKey)
{
	return Info_ValueForKey(Cvar_Userinfo(), asKey);
};

/*
=============
pfnVGUI2DrawCharacter

TODO: implement
=============
*/
static int EngFunc_VGUI2DrawCharacter(int x, int y, int ch, unsigned int font)
{
	return 0;
};

/*
=============
pfnVGUI2DrawCharacterAdditive

TODO: implement
=============
*/
static int EngFunc_VGUI2DrawCharacterAdditive(int x, int y, int ch, int r, int g, int b, unsigned int font)
{
	return 0;
};

/*
=============
GetCareerGameInterface
=============
*/
static void *EngFunc_GetCareerGameInterface()
{
	Msg("^1Career GameInterface called!");
	return nullptr;
};

/*
=============
pfnPlaySoundVoiceByName
=============
*/
static void EngFunc_PlaySoundVoiceByName(char *asFileName, float afVolume, int anPitch)
{
	int hSound = S_RegisterSound(asFileName);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_AUTO, hSound, afVolume, ATTN_NORM, anPitch, SND_STOP_LOOPING);
};

/*
=============
pfnMP3_InitStream
=============
*/
static void EngFunc_MP3_InitStream(char *asFileName, int anLooping)
{
	if(!asFileName)
	{
		S_StopBackgroundTrack();
		return;
	};
	
	if(anLooping)
		S_StartBackgroundTrack(asFileName, asFileName, 0);
	else
		S_StartBackgroundTrack(asFileName, "", 0);
};

/*
=============
pfnPlaySoundByNameAtPitch
=============
*/
static void EngFunc_PlaySoundByNameAtPitch(char *asFileName, float afVolume, int anPitch)
{
	int hSound = S_RegisterSound(asFileName);
	S_StartSound(NULL, cl.refdef.viewentity, CHAN_STATIC, hSound, afVolume, ATTN_NORM, anPitch, SND_STOP_LOOPING);
};

/*
=============
pfnFillRGBABlend
=============
*/
static void EngFunc_FillRGBABlend(int anX, int anY, int anWidth, int anHeight, int anColorRed, int anColorGreen, int anColorBlue, int anColorAlpha)
{
	anColorRed = clamp(0, r, 255);
	anColorGreen = clamp(0, g, 255);
	anColorBlue = clamp(0, b, 255);
	anColorAlpha = clamp(0, a, 255);
	pglColor4ub(anColorRed, anColorGreen, anColorBlue, anColorAlpha);
	
	SPR_AdjustSize((float *)&anX,(float *)&anY,(float *)&anWidth,(float *)&anHeight);
	
	pglEnable(GL_BLEND);
	pglDisable(GL_ALPHA_TEST);
	pglBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
	pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	R_DrawStretchPic(anX, anY, anWidth, anHeight, 0, 0, 1, 1, cls.fillImage);
	pglColor4ub(255, 255, 255, 255);
};

/*
=============
pfnGetAppID
=============
*/
static int EngFunc_GetAppID()
{
	return 220; // Standard Valve value
};

/*
=============
pfnVguiWrap2_GetMouseDelta

TODO: implement
=============
*/
static void EngFunc_VguiWrap2_GetMouseDelta(int *anX, int *anY)
{
};