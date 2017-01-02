// Null implementation for NEW_DLL_FUNCTIONS (this struct can be optionally present in gamedll)

#include "common/mathlib.h"
#include "engine/eiface.h"

namespace
{
void GameDLL_OnFreeEntPrivateData(edict_t *pEnt){};

void GameDLL_GameShutdown(){};

int GameDLL_ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	return 0;
};

void GameDLL_CvarValue(const edict_t *pEnt, const char *value){};

void GameDLL_CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value){};
}; // namespace

NEW_DLL_FUNCTIONS gNewGameDLLFuncsNull =
    {
        GameDLL_OnFreeEntPrivateData,
        GameDLL_GameShutdown,
        GameDLL_ShouldCollide,
        GameDLL_CvarValue,
        GameDLL_CvarValue2};