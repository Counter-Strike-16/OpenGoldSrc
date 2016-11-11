#include "server/GameClientAPI.h"

#ifndef OGS_NULL_GAMECLIENT_API_IMPL
void GameClient_SetView(const edict_t *pClient, const edict_t *pViewent)
{
};

void GameClient_CrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
};

void GameClient_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
};

void GameClient_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
};

const char *GameClient_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
};

const char *GameClient_GetPhysicsInfoString(const edict_t *pClient)
{
};

void GameClient_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
};

const char *GameClient_GetPlayerAuthId(edict_t *e)
{
};

int GameClient_GetPlayerUserId(edict_t *e)
{
};

unsigned int GameClient_GetPlayerWONId(edict_t *e)
{
};

void GameClient_QueryClientCvarValue(const edict_t *player, const char *cvarName)
{
};

void GameClient_QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
};
#else // OGS_NULL_GAMECLIENT_API_IMPL
edict_t *EngFunc_CreateFakeClient(const char *netname)
{
};

void GameClient_SetView(const edict_t *pClient, const edict_t *pViewent)
{
};

void GameClient_CrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
};

void GameClient_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
};

void GameClient_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
};

const char *GameClient_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
};

const char *GameClient_GetPhysicsInfoString(const edict_t *pClient)
{
};

void GameClient_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
};

const char *GameClient_GetPlayerAuthId(edict_t *e)
{
};

int GameClient_GetPlayerUserId(edict_t *e)
{
};

unsigned int GameClient_GetPlayerWONId(edict_t *e)
{
};

void GameClient_QueryClientCvarValue(const edict_t *player, const char *cvarName)
{
};

void GameClient_QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
};

void EngFunc_SetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
};

int EngFunc_CanSkipPlayer(const edict_t *player)
{
};

void EngFunc_SetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
};

void EngFunc_ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
};

void EngFunc_ClientCommand(edict_t *pEdict, char *szFmt, ...)
{
};

qboolean EngFunc_Voice_GetClientListening(int iReceiver, int iSender)
{
};

qboolean EngFunc_Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
};
#endif // OGS_NULL_GAMECLIENT_API_IMPL