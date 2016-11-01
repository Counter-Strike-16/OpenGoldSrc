// C API for server world (game & physics)

void World_RemoveEntity(edict_t *e)
{
};

edict_t *World_CreateNamedEntity(int className)
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
};

void World_TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
};

void World_TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
};

const char *World_TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
};

void World_TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
};