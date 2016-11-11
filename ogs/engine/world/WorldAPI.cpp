// C API for server world (game & physics)

#include "world/worldapi.h"

#ifndef OGS_NULL_WORLD_API_IMPL
edict_t *World_CreateEntity()
{
	return nullptr;
};

edict_t *World_CreateNamedEntity(int className)
{
	return nullptr;
};

void World_RemoveEntity(edict_t *e)
{
};

void World_EmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
};

void World_EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
};

void World_TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
};

int World_TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	return 0;
};

void World_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
};

const char *World_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	return "";
};

void World_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_CreateParticleEffect(const float *org, const float *dir, float color, float count)
{
};

void World_CreateStaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
};

void World_PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
};

void World_SetLightStyle(int style, char *val)
{
};

int World_GetPointContents(const float *rgflVector)
{
	return 0;
};

int World_GetNumberOfEntities()
{
	return 0;
};

unsigned char *World_SetFatPVS(float *org)
{
	return NULL;
};

unsigned char *World_SetFatPAS(float *org)
{
	return NULL;
};

int World_CheckEntityVisibility(const edict_t *entity, unsigned char *pset)
{
	return 0;
};

int World_CreateInstancedBaseline( int classname, struct entity_state_s *baseline)
{
	return 0;
};

char *EngFunc_GetInfoKeyBuffer(edict_t *e)
{
	return NULL;
};
#else // OGS_NULL_WORLD_API_IMPL
edict_t *World_CreateEntity()
{
	return nullptr;
};

edict_t *World_CreateNamedEntity(int className)
{
	return nullptr;
};

void World_RemoveEntity(edict_t *e)
{
};

void World_EmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
};

void World_EmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
};

void World_TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_TraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
};

int World_TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	return 0;
};

void World_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
};

const char *World_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	return "";
};

void World_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_CreateParticleEffect(const float *org, const float *dir, float color, float count)
{
};

void World_CreateStaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
};

void World_PlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
};

void World_SetLightStyle(int style, char *val)
{
};

int World_GetPointContents(const float *rgflVector)
{
	return 0;
};

int World_GetNumberOfEntities()
{
	return 0;
};

unsigned char *World_SetFatPVS(float *org)
{
	return NULL;
};

unsigned char *World_SetFatPAS(float *org)
{
	return NULL;
};

int World_CheckEntityVisibility(const edict_t *entity, unsigned char *pset)
{
	return 0;
};

int World_CreateInstancedBaseline( int classname, struct entity_state_s *baseline)
{
	return 0;
};

char *EngFunc_GetInfoKeyBuffer(edict_t *e)
{
	return NULL;
};

void EngFunc_ChangeLevel(char *s1, char *s2)
{
};

edict_t *EngFunc_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
};

edict_t *EngFunc_FindClientInPVS(edict_t *pEdict)
{
};

edict_t *EngFunc_EntitiesInPVS(edict_t *pplayer)
{
};
#endif // OGS_NULL_WORLD_API_IMPL