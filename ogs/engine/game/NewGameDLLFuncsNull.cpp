// Null implementation for NEW_DLL_FUNCTIONS (this struct can be optionally present in gamedll)

#include "eiface.h"

namespace
{
void GameDLL_OnFreeEntPrivateData(edict_t *pEnt)
{
};

void GameDLL_GameShutdown()
{
};

int GameDLL_ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
};

void GameDLL_CvarValue(const edict_t *pEnt, const char *value)
{
};

void GameDLL_CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value)
{
};
}; // namespace

static NEW_DLL_FUNCTIONS *gpNewGameDLLFuncsNull =
{
	GameDLL_OnFreeEntPrivateData,
	GameDLL_GameShutdown,
	GameDLL_ShouldCollide,
	GameDLL_CvarValue,
	GameDLL_CvarValue2
};