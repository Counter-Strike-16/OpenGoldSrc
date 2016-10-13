// Null implementation for game dll exports (in case we couldn't load that dll instead of check for func presence)

#include "eiface.h"

namespace
{
void GameDLL_GameInit()
{
};

int GameDLL_Spawn(edict_t *pent)
{
};

void GameDLL_Think(edict_t *pent)
{
};

void GameDLL_Use(edict_t *pentUsed, edict_t *pentOther)
{
};

void GameDLL_Touch(edict_t *pentTouched, edict_t *pentOther)
{
};

void GameDLL_Blocked(edict_t *pentBlocked, edict_t *pentOther)
{
};

void GameDLL_KeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd)
{
};

void GameDLL_Save(edict_t *pent, SAVERESTOREDATA *pSaveData)
{
};

int GameDLL_Restore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
};

void GameDLL_SetAbsBox(edict_t *pent)
{
};

void GameDLL_SaveWriteFields(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int)
{
};

void GameDLL_SaveReadFields(SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int)
{
};

void GameDLL_SaveGlobalState(SAVERESTOREDATA *)
{
};

void GameDLL_RestoreGlobalState(SAVERESTOREDATA *)
{
};

void GameDLL_ResetGlobalState()
{
};

qboolean GameDLL_ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ])
{
};

void GameDLL_ClientDisconnect(edict_t *pEntity)
{
};

void GameDLL_ClientKill(edict_t *pEntity)
{
};

void GameDLL_ClientPutInServer(edict_t *pEntity)
{
};

void GameDLL_ClientCommand(edict_t *pEntity)
{
};

void GameDLL_ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
};

void GameDLL_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
};

void GameDLL_ServerDeactivate()
{
};

void GameDLL_PlayerPreThink(edict_t *pEntity)
{
};

void GameDLL_PlayerPostThink(edict_t *pEntity)
{
};

void GameDLL_StartFrame()
{
};

void GameDLL_ParmsNewLevel()
{
};

void GameDLL_ParmsChangeLevel()
{
};

const char *GameDLL_GetGameDescription()
{
};

void GameDLL_PlayerCustomization(edict_t *pEntity, customization_t *pCustom)
{
};

void GameDLL_SpectatorConnect(edict_t *pEntity)
{
};

void GameDLL_SpectatorDisconnect(edict_t *pEntity)
{
};

void GameDLL_SpectatorThink(edict_t *pEntity)
{
};

void GameDLL_Sys_Error(const char *error_string)
{
};

void GameDLL_PM_Move(struct playermove_s *ppmove, qboolean server)
{
};

void GameDLL_PM_Init(struct playermove_s *ppmove)
{
};

char GameDLL_PM_FindTextureType(char *name)
{
};

void GameDLL_SetupVisibility(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas)
{
};

void GameDLL_UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd)
{
};

int GameDLL_AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
};

void GameDLL_CreateBaseline(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs)
{
};

void GameDLL_RegisterEncoders()
{
};

int GameDLL_GetWeaponData(struct edict_s *player, struct weapon_data_s *info)
{
};

void GameDLL_CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed)
{
};

void GameDLL_CmdEnd(const edict_t *player)
{
};

int GameDLL_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
};

int GameDLL_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
};

void GameDLL_CreateInstancedBaselines()
{
};

int GameDLL_InconsistentFile(const struct edict_s *player, const char *filename, char *disconnect_message)
{
};

int GameDLL_AllowLagCompensation()
{
};
}; // namespace

static DLL_FUNCTIONS *gpNullGameDLL =
{
	GameDLL_GameInit,
	
	GameDLL_Spawn,
	GameDLL_Think,
	GameDLL_Use,
	GameDLL_Touch,
	GameDLL_Blocked,
	GameDLL_KeyValue,
	GameDLL_Save,
	GameDLL_Restore,
	GameDLL_SetAbsBox,
	
	GameDLL_SaveWriteFields,
	GameDLL_SaveReadFields,
	
	GameDLL_SaveGlobalState,
	GameDLL_ResetGlobalState,
	GameDLL_ResetGlobalState,
	
	GameDLL_ClientConnect,
	GameDLL_ClientDisconnect,
	GameDLL_ClientKill,
	GameDLL_ClientPutInServer,
	GameDLL_ClientCommand,
	GameDLL_ClientUserInfoChanged,
	
	GameDLL_ServerActivate,
	GameDLL_ServerDeactivate,
	
	GameDLL_PlayerPreThink,
	GameDLL_PlayerPostThink,
	
	GameDLL_StartFrame,
	
	GameDLL_ParmsNewLevel,
	GameDLL_ParmsChangeLevel,
	
	GameDLL_GetGameDescription,
	
	GameDLL_PlayerCustomization,
	
	GameDLL_SpectatorConnect,
	GameDLL_SpectatorDisconnect,
	GameDLL_SpectatorThink,
	
	GameDLL_Sys_Error,
	
	GameDLL_PM_Move,
	GameDLL_PM_Init,
	GameDLL_PM_FindTextureType,
	
	GameDLL_SetupVisibility,
	GameDLL_UpdateClientData,
	
	GameDLL_AddToFullPack,
	
	GameDLL_CreateBaseline,
	
	GameDLL_RegisterEncoders,
	
	GameDLL_GetWeaponData,
	
	GameDLL_CmdStart,
	GameDLL_CmdEnd,
	
	GameDLL_ConnectionlessPacket,
	
	GameDLL_GetHullBounds,
	
	GameDLL_CreateInstancedBaselines,
	
	GameDLL_InconsistentFile,
	GameDLL_AllowLagCompensation
};