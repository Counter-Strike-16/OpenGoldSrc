#include "cdll_exp.h"

namespace
{
int ClientDLL_Initialize(cl_enginefunc_t *pEnginefuncs, int iVersion)
{
	return 0;
};

void ClientDLL_Init()
{
};

int ClientDLL_VidInit()
{
	return 0;
};

int ClientDLL_Redraw(float flTime, int intermission)
{
	return 0;
};

int ClientDLL_UpdateClientData(client_data_t *cdata, float flTime)
{
	return 0;
};

void ClientDLL_Reset()
{
};

void ClientDLL_PlayerMove(struct playermove_s *ppmove, int server)
{
};

void ClientDLL_PlayerMoveInit(struct playermove_s *ppmove)
{
};

char ClientDLL_PlayerMoveTexture(char *name)
{
	return '';
};

void ClientDLL_IN_ActivateMouse()
{
};

void ClientDLL_IN_DeactivateMouse()
{
};

void ClientDLL_IN_MouseEvent(int mstate)
{
};

void ClientDLL_IN_ClearStates()
{
};

void ClientDLL_IN_Accumulate()
{
};

void ClientDLL_CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
};

int ClientDLL_CL_IsThirdPerson()
{
	return 0;
};

void ClientDLL_CL_CameraOffset(float *ofs)
{
};

void *ClientDLL_KB_Find(const char *name)
{
	return NULL;
};

void ClientDLL_CAM_Think()
{
};

void ClientDLL_CalcRefdef(ref_params_t *pparams)
{
};

int ClientDLL_AddEntity(int type, cl_entity_t *ent, const char *modelname)
{
	return 0;
};

void ClientDLL_CreateEntities()
{
};

void ClientDLL_DrawNormalTriangles()
{
};

void ClientDLL_DrawTransparentTriangles()
{
};

void ClientDLL_StudioEvent(const struct mstudioevent_s *event, const cl_entity_t *entity)
{
};

void ClientDLL_PostRunCmd(struct local_state_s *from, struct local_state_s *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed)
{
};

void ClientDLL_Shutdown()
{
};

void ClientDLL_TxferLocalOverrides(entity_state_t *state, const clientdata_t *client)
{
};

void ClientDLL_ProcessPlayerState(entity_state_t *dst, const entity_state_t *src)
{
};

void ClientDLL_TxferPredictionData(entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd)
{
};

void ClientDLL_Demo_ReadBuffer(int size, byte *buffer)
{
};

int ClientDLL_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *buffer, int *size)
{
	return 0;
};

int ClientDLL_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return 0;
};

void ClientDLL_Frame(double time)
{
};

int ClientDLL_Key_Event(int eventcode, int keynum, const char *pszCurrentBinding)
{
	return 0;
};

void ClientDLL_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int ( *Callback_AddVisibleEntity)( cl_entity_t *pEntity), void ( *Callback_TempEntPlaySound)( struct tempent_s *pTemp, float damp))
{
};

cl_entity_t *ClientDLL_GetUserEntity(int index)
{
	return NULL;
};

void ClientDLL_VoiceStatus(int entindex, qboolean bTalking)
{
};

void ClientDLL_DirectorMessage(int iSize, void *pbuf)
{
};

int ClientDLL_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio)
{
	return 0;
};

void ClientDLL_ChatInputPosition(int *x, int *y)
{
};

int ClientDLL_GetPlayerTeam(int playerIndex)
{
	return 0;
};

void *ClientDLL_GetClientFactory()
{
	return NULL;
};
}; // namespace

static cdll_func_t *gpClientDLLFuncsNull =
{
	ClientDLL_Initialize,
	ClientDLL_Init,
	ClientDLL_VidInit,
	
	ClientDLL_Redraw,
	ClientDLL_UpdateClientData,
	ClientDLL_Reset,
	
	ClientDLL_PlayerMove,
	ClientDLL_PlayerMoveInit,
	ClientDLL_PlayerMoveTexture,
	
	ClientDLL_IN_ActivateMouse,
	ClientDLL_IN_DeactivateMouse,
	ClientDLL_IN_MouseEvent,
	ClientDLL_IN_ClearStates,
	ClientDLL_IN_Accumulate,
	
	ClientDLL_CL_CreateMove,
	ClientDLL_CL_IsThirdPerson,
	ClientDLL_CL_CameraOffset,
	
	ClientDLL_KB_Find,
	
	ClientDLL_CAM_Think,
	ClientDLL_CalcRefdef,
	
	ClientDLL_AddEntity,
	ClientDLL_CreateEntities,
	
	ClientDLL_DrawNormalTriangles,
	ClientDLL_DrawTransparentTriangles,
	
	ClientDLL_StudioEvent,
	
	ClientDLL_PostRunCmd,
	
	ClientDLL_Shutdown,
	
	ClientDLL_TxferLocalOverrides,
	ClientDLL_ProcessPlayerState,
	ClientDLL_TxferPredictionData,
	
	ClientDLL_Demo_ReadBuffer,
	
	ClientDLL_ConnectionlessPacket,
	
	ClientDLL_GetHullBounds,
	
	ClientDLL_Frame,
	
	ClientDLL_Key_Event,
	
	ClientDLL_TempEntUpdate,
	ClientDLL_GetUserEntity,
	
	ClientDLL_VoiceStatus,
	
	ClientDLL_DirectorMessage,
	
	ClientDLL_GetStudioModelInterface,
	
	ClientDLL_ChatInputPosition,
	ClientDLL_GetPlayerTeam,
	ClientDLL_GetClientFactory
};