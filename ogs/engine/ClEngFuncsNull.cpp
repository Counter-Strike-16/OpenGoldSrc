#include "engine/cdll_int.h"

namespace
{
HSPRITE_ EngFunc_SPR_Load(const char *szPicName)
{
};

int EngFunc_SPR_Frames(HSPRITE_ hPic)
{
};

int EngFunc_SPR_Height(HSPRITE_ hPic, int frame)
{
};

int EngFunc_SPR_Width(HSPRITE_ hPic, int frame)
{
};

void EngFunc_SPR_Set(HSPRITE_ hPic, int r, int g, int b)
{
};

void EngFunc_SPR_Draw(int frame, int x, int y, const wrect_t *prc)
{
};

void EngFunc_SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc)
{
};

void EngFunc_SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc)
{
};

void EngFunc_SPR_EnableScissor(int x, int y, int width, int height)
{
};

void EngFunc_SPR_DisableScissor()
{
};

client_sprite_t *EngFunc_SPR_GetList(char *psz, int *piCount)
{
};

void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
};

int EngFunc_GetScreenInfo(SCREENINFO *pscrinfo)
{
};

void EngFunc_SetCrosshair(HSPRITE_ hspr, wrect_t rc, int r, int g, int b)
{
};

struct cvar_s *EngFunc_RegisterVariable(char *szName, char *szValue, int flags)
{
};

float EngFunc_GetCvarFloat(char *szName)
{
};

char *EngFunc_GetCvarString(char *szName)
{
};

int EngFunc_AddCommand(char *cmd_name, void (*function)())
{
};

int EngFunc_HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
};

int EngFunc_ServerCmd(char *szCmdString)
{
};

int EngFunc_ClientCmd(char *szCmdString)
{
};

void EngFunc_GetPlayerInfo(int ent_num, hud_player_info_t *pinfo)
{
};

void EngFunc_PlaySoundByName(char *szSound, float volume)
{
};

void EngFunc_PlaySoundByIndex(int iSound, float volume)
{
};

void EngFunc_AngleVectors(const float *vecAngles, float *forward, float *right, float *up)
{
};

client_textmessage_t *EngFunc_TextMessageGet(const char *pName)
{
};

int EngFunc_DrawCharacter(int x, int y, int number, int r, int g, int b)
{
};

int EngFunc_DrawConsoleString(int x, int y, char *string)
{
};

void EngFunc_DrawSetTextColor(float r, float g, float b)
{
};

void EngFunc_DrawConsoleStringLen(const char *string, int *length, int *height)
{
};

void EngFunc_ConsolePrint(const char *string)
{
};

void EngFunc_CenterPrint(const char *string)
{
};

int EngFunc_GetWindowCenterX()
{
};

int EngFunc_GetWindowCenterY()
{
};

void EngFunc_GetViewAngles(float *)
{
};

void EngFunc_SetViewAngles(float *)
{
};

int EngFunc_GetMaxClients()
{
};

void EngFunc_Cvar_SetValue(char *cvar, float value)
{
};

int EngFunc_Cmd_Argc()
{
};

char *EngFunc_Cmd_Argv(int arg)
{
};

void EngFunc_Con_Printf(char *fmt, ...)
{
};

void EngFunc_Con_DPrintf(char *fmt, ...)
{
};

void EngFunc_Con_NPrintf(int pos, char *fmt, ...)
{
};

void EngFunc_Con_NXPrintf(struct con_nprint_s *info, char *fmt, ...)
{
};

const char *EngFunc_PhysInfo_ValueForKey(const char *key)
{
};

const char *EngFunc_ServerInfo_ValueForKey(const char *key)
{
};

float EngFunc_GetClientMaxspeed()
{
};

int EngFunc_CheckParm(char *parm, char **ppnext)
{
};

void EngFunc_Key_Event(int key, int down)
{
};

void EngFunc_GetMousePosition(int *mx, int *my)
{
};

int EngFunc_IsNoClipping()
{
};

struct cl_entity_s *EngFunc_GetLocalPlayer()
{
};

struct cl_entity_s *EngFunc_GetViewModel()
{
};

struct cl_entity_s *EngFunc_GetEntityByIndex(int idx)
{
};

float EngFunc_GetClientTime()
{
};

void EngFunc_V_CalcShake()
{
};

void EngFunc_V_ApplyShake(float *origin, float *angles, float factor)
{
};

int EngFunc_PM_PointContents(float *point, int *truecontents)
{
};

int EngFunc_PM_WaterEntity(float *p)
{
};

struct pmtrace_s *EngFunc_PM_TraceLine(float *start, float *end, int flags, int usehull, int ignore_pe)
{
};

struct model_s *EngFunc_CL_LoadModel(const char *modelname, int *index)
{
};

int EngFunc_CL_CreateVisibleEntity(int type, struct cl_entity_s *ent)
{
};

const struct model_s *EngFunc_GetSpritePointer(HSPRITE hSprite)
{
};

void EngFunc_PlaySoundByNameAtLocation(char *szSound, float volume, float *origin)
{
};

unsigned short EngFunc_PrecacheEvent(int type, const char* psz)
{
};

void EngFunc_PlaybackEvent(int flags,
							const struct edict_s *pInvoker,
							unsigned short eventindex,
							float delay,
							float *origin,
							float *angles,
							float fparam1,
							float fparam2,
							int iparam1,
							int iparam2,
							int bparam1,
							int bparam2)
{
};

void EngFunc_WeaponAnim(int iAnim, int body)
{
};

float EngFunc_RandomFloat(float flLow, float flHigh)
{
};

long EngFunc_RandomLong(long lLow, long lHigh)
{
};

void EngFunc_HookEvent(char *name, void (*pfnEvent)(struct event_args_s *args))
{
};

int EngFunc_Con_IsVisible()
{
};

const char *EngFunc_GetGameDirectory()
{
};

struct cvar_s *EngFunc_GetCvarPointer(const char *szName)
{
};

const char *EngFunc_Key_LookupBinding(const char *pBinding)
{
};

const char *EngFunc_GetLevelName()
{
};

void EngFunc_GetScreenFade(struct screenfade_s *fade)
{
};

void EngFunc_SetScreenFade(struct screenfade_s *fade)
{
};

void *EngFunc_VGui_GetPanel()
{
};

void EngFunc_VGui_ViewportPaintBackground(int extents[4])
{
};

byte *EngFunc_COM_LoadFile(char *path, int usehunk, int *pLength)
{
};

char *EngFunc_COM_ParseFile(char *data, char *token)
{
};

void EngFunc_COM_FreeFile(void *buffer)
{
};

struct triangleapi_s*pTriAPI;
struct efx_api_s*pEfxAPI;
struct event_api_s*pEventAPI;
struct demo_api_s*pDemoAPI;
struct net_api_s*pNetAPI;
struct IVoiceTweak_s*pVoiceTweak;

int EngFunc_IsSpectateOnly()
{
};

struct model_s *EngFunc_LoadMapSprite(const char *filename)
{
};

void EngFunc_COM_AddAppDirectoryToSearchPath(const char *pszBaseDir, const char *appName)
{
};

int EngFunc_COM_ExpandFilename(const char *fileName, char *nameOutBuffer, int nameOutBufferSize)
{
};

const char *EngFunc_PlayerInfo_ValueForKey(int playerNum, const char *key)
{
};

void EngFunc_PlayerInfo_SetValueForKey(const char *key, const char *value)
{
};

qboolean EngFunc_GetPlayerUniqueID(int iPlayer, char playerID[16])
{
};

int EngFunc_GetTrackerIDForPlayer(int playerSlot)
{
};

int EngFunc_GetPlayerForTrackerID(int trackerID)
{
};

int EngFunc_ServerCmdUnreliable(char *szCmdString)
{
};

void EngFunc_GetMousePos(struct tagPOINT *ppt)
{
};

void EngFunc_SetMousePos(int x, int y)
{
};

void EngFunc_SetMouseEnable(qboolean fEnable)
{
};

struct cvar_s *EngFunc_GetFirstCvarPtr()
{
};

void *EngFunc_GetFirstCmdFunctionHandle()
{
};

void *EngFunc_GetNextCmdFunctionHandle(void *cmdhandle)
{
};

const char *EngFunc_GetCmdFunctionName(void *cmdhandle)
{
};

float EngFunc_GetClientOldTime()
{
};

float EngFunc_GetGravity()
{
};

struct model_s *EngFunc_GetModelByIndex(int index)
{
};

void EngFunc_SetFilterMode(int mode)
{
};

void EngFunc_SetFilterColor(float red, float green, float blue)
{
};

void EngFunc_SetFilterBrightness(float brightness)
{
};

void *EngFunc_SequenceGet(const char *fileName, const char *entryName)
{
};

void EngFunc_SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height)
{
};

void *EngFunc_SequencePickSentence(const char *groupName, int pickMethod, int *entryPicked)
{
};

int EngFunc_DrawString(int x, int y, const char *str, int r, int g, int b)
{
};

int EngFunc_DrawStringReverse(int x, int y, const char *str, int r, int g, int b)
{
};

const char *EngFunc_LocalPlayerInfo_ValueForKey(const char* key)
{
};

int EngFunc_VGUI2DrawCharacter(int x, int y, int ch, unsigned int font)
{
};

int EngFunc_VGUI2DrawCharacterAdditive(int x, int y, int ch, int r, int g, int b, unsigned int font)
{
};

unsigned int EngFunc_GetApproxWavePlayLen(char *filename)
{
};

void *EngFunc_GetCareerGameUI()
{
};

void EngFunc_Cvar_Set(char *name, char *value)
{
};

int EngFunc_IsPlayingCareerMatch()
{
};

void EngFunc_PlaySoundVoiceByName(char *szSound, float volume, int pitch)
{
};

void EngFunc_PrimeMusicStream(char *filename, int looping)
{
};

double EngFunc_Sys_FloatTime()
{
};

void EngFunc_ProcessTutorMessageDecayBuffer(int *buffer, int buflen)
{
};

void EngFunc_ConstructTutorMessageDecayBuffer(int *buffer, int buflen)
{
};

void EngFunc_ResetTutorMessageDecayData()
{
};

void EngFunc_PlaySoundByNameAtPitch(char *szSound, float volume, int pitch)
{
};

void EngFunc_FillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a)
{
};

int EngFunc_GetAppID()
{
};

cmdalias_t *EngFunc_GetAliases()
{
};

void EngFunc_VguiWrap2_GetMouseDelta(int *x, int *y)
{
};

}; // namespace

cl_enginefunc_t gClEngFuncsNull =
{
};