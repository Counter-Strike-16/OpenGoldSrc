// C API for edict

#include "edict_local.hpp"

bool Edict_IsValid(edict_t *pEdict)
{
	if(pEdict)
		return true;
	
	return false;
};

struct entvars_s *Edict_GetVars(edict_t *pEdict)
{
	if(!Edict_IsValid(pEdict))
		return nullptr;
};

void Edict_MakeStatic(edict_t *ent)
{
	if(!Edict_IsValid(ent))
		return;
};

int Edict_IsOnFloor(edict_t *e)
{
	if(!Edict_IsValid(e))
		return 0;
};

int Edict_DropToFloor(edict_t *e)
{
	if(!Edict_IsValid(e))
		return 0;
};

int Edict_WalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	if(!Edict_IsValid(ent))
		return 0;
};

void Edict_AnimationAutomove(const edict_t *pEdict, float flTime)
{
	if(!Edict_IsValid(pEdict))
		return;
};

void Edict_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
	if(!Edict_IsValid(pEdict))
		return;
};

void Edict_SetModel(edict_t *e, const char *m)
{
	if(!Edict_IsValid(e))
		return;
};

void Edict_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	if(!Edict_IsValid(e))
		return;
};

void Edict_SetOrigin(edict_t *e, const float *rgflOrigin)
{
	if(!Edict_IsValid(e))
		return;
};

int Edict_GetIllum(edict_t *pEnt)
{
	if(!Edict_IsValid(pEnt))
		return 0;
};

void Edict_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	if(!Edict_IsValid(ent))
		return;
};

void Edict_ChangeYaw(edict_t *ent)
{
	if(!Edict_IsValid(ent))
		return;
};

void Edict_ChangePitch(edict_t *ent)
{
	if(!Edict_IsValid(ent))
		return;
};

void Edict_GetSpawnParms(edict_t *ent)
{
	if(!Edict_IsValid(ent))
		return;
};

void Edict_SaveSpawnParms(edict_t *ent)
{
	if(!Edict_IsValid(ent))
		return;
};

void *Edict_GetModelPtr(edict_t *pEdict)
{
	if(!Edict_IsValid(pEdict))
		return nullptr;
};

void Edict_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
	if(!Edict_IsValid(pEdict))
		return;
};