

#include "maintypes.h"
#include "common.h"
#include "custom.h"
#include "cl_entity.h"
#include "consistency.h"
#include "delta_packet.h"
#include "dlight.h"
#include "entity_state.h"
#include "event.h"
#include "info.h"
#include "net.h"
#include "keys.h"
#include "sound.h"
#include "screenfade.h"
#include "usercmd.h"
#include "model.h"
#include "kbutton.h"

#define MAX_SCOREBOARDNAME	32
#define MAX_DEMOS		32

typedef enum cactive_e
{
	ca_dedicated,
	ca_disconnected,
	ca_connecting,
	ca_connected,
	ca_uninitialized,
	ca_active,
} cactive_t;

#ifdef HOOK_ENGINE
#define g_pcls (*pcls)
#define g_pcl (*pcl)
#define key_dest (*pkey_dest)

#define g_clmove (*pg_clmove)
#define cl_inmovie (*pcl_inmovie)

#define cl_name (*pcl_name)
#define rate_ (*prate)
#define console (*pconsole)
#endif // HOOK_ENGINE

extern keydest_t key_dest;
extern client_static_t g_pcls;
extern client_state_t g_pcl;

extern playermove_t g_clmove;
extern qboolean cl_inmovie;

extern cvar_t cl_name;
extern cvar_t rate_;
extern cvar_t console;

void CL_RecordHUDCommand(char *cmdname);
void R_DecalRemoveAll(int textureIndex);
void CL_CheckForResend(void);
qboolean CL_CheckFile(sizebuf_t *msg, char *filename);
void CL_ClearClientState(void);

void CL_DecayLights(void);

void CL_EmitEntities(void);
void CL_InitClosest(void);
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel);
void CL_PredictMove(qboolean repredicting);
void CL_PrintLogos(void);
qboolean CL_RequestMissingResources(void);
void CL_Move(void);

void CL_StopPlayback(void);
void CL_UpdateSoundFade(void);
void CL_AdjustClock(void);
void CL_Save(const char *name);
void CL_HudMessage(const char *pMessage);

int Key_CountBindings(void);
void Key_WriteBindings(FileHandle_t f);
extern "C" void ClientDLL_UpdateClientData(void);
extern "C" void ClientDLL_HudVidInit(void);
void Chase_Init(void);
void Key_Init(void);
extern "C" void ClientDLL_Init(void);

int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf);
void CL_ShutDownUsrMessages(void);
void CL_ShutDownClientStatic(void);

extern "C" void ClientDLL_MoveClient(struct playermove_s *ppmove);

extern "C" void ClientDLL_Frame(double time);
extern "C" void ClientDLL_CAM_Think(void);
void CL_InitEventSystem(void);
void CL_CheckClientState(void);
void CL_RedoPrediction(void);
void CL_SetLastUpdate(void);




void CL_WriteMessageHistory(int starting_count, int cmd);

void CL_MoveSpectatorCamera(void);
void CL_AddVoiceToDatagram(qboolean bFinal);
void CL_VoiceIdle(void);

void PollDInputDevices(void);

void CL_KeepConnectionActive(void);

void CL_UpdateModuleC(void);

int VGuiWrap2_IsInCareerMatch(void);

void VguiWrap2_GetCareerUI(void);

int VGuiWrap2_GetLocalizedStringLength(const char *label);
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName);

void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength);
void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength);
int GetTimesTutorMessageShown(int id);
void RegisterTutorMessageShown(int mid);
void ResetTutorMessageDecayData(void);
void SetCareerAudioState(int state);