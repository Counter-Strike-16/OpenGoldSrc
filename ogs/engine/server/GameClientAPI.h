#pragma once
#ifndef OGS_GAMECLIENTAPI_HPP
#define OGS_GAMECLIENTAPI_HPP

extern "C"
{

void GameClient_SetView(const edict_t *pClient, const edict_t *pViewent);
void GameClient_CrosshairAngle(const edict_t *pClient, float pitch, float yaw);

void GameClient_FadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds);

void GameClient_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value);
const char *GameClient_GetPhysicsKeyValue(const edict_t *pClient, const char *key);
const char *GameClient_GetPhysicsInfoString(const edict_t *pClient);

void GameClient_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss);

const char *GameClient_GetPlayerAuthId(edict_t *e);
int GameClient_GetPlayerUserId(edict_t *e);
unsigned int GameClient_GetPlayerWONId(edict_t *e);

void GameClient_QueryClientCvarValue(const edict_t *player, const char *cvarName);
void GameClient_QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID);

};

#endif // OGS_GAMECLIENTAPI_HPP