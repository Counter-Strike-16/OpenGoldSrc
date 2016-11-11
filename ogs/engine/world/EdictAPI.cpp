// C API for edict

#include "world/Edict.hpp"

// Use normal impl by default with the ability to use null impl to test
#ifndef OGS_NULL_EDICT_API_IMPL
bool ED_IsValid(edict_t *pEdict)
{
	if(pEdict)
		return true;
	
	return false;
};

struct entvars_s *ED_GetVars(edict_t *pEdict)
{
	if(!ED_IsValid(pEdict))
		return nullptr;
	
	return nullptr;
};

void ED_MakeStatic(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

int ED_IsOnFloor(edict_t *e)
{
	if(!ED_IsValid(e))
		return 0;
};

int ED_DropToFloor(edict_t *e)
{
	if(!ED_IsValid(e))
		return 0;
};

int ED_WalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	if(!ED_IsValid(ent))
		return 0;
};

void ED_AnimationAutomove(const edict_t *pEdict, float flTime)
{
	if(!ED_IsValid(pEdict))
		return;
};

void ED_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
	if(!ED_IsValid(pEdict))
		return;
};

void ED_SetModel(edict_t *e, const char *m)
{
	if(!ED_IsValid(e))
		return;
};

void ED_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	if(!ED_IsValid(e))
		return;
};

void ED_SetOrigin(edict_t *e, const float *rgflOrigin)
{
	if(!ED_IsValid(e))
		return;
};

int ED_GetIllum(edict_t *pEnt)
{
	if(!ED_IsValid(pEnt))
		return 0;
};

void ED_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_ChangeYaw(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_ChangePitch(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_GetSpawnParms(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_SaveSpawnParms(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void *ED_GetModelPtr(edict_t *pEdict)
{
	if(!ED_IsValid(pEdict))
		return nullptr;
};

void ED_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
	if(!ED_IsValid(pEdict))
		return;
};
#else // OGS_NULL_EDICT_API_IMPL
bool ED_IsValid(edict_t *pEdict)
{
	if(pEdict)
		return true;
	
	return false;
};

bool ED_IsFree(edict_t *pEdict)
{
	if(!pEdict)
		return false;
	
	if(!pEdict->free)
		return false;
	
	return true;
};

int ED_GetOffset(const edict_t *pEdict)
{
};

int ED_GetIndex(const edict_t *pEdict)
{
};

struct entvars_s *ED_GetVars(edict_t *pEdict)
{
	if(!ED_IsValid(pEdict))
		return nullptr;
};

void *ED_GetPrivateData(edict_t *pEdict)
{
};

void ED_FreePrivateData(edict_t *pEdict)
{
};

void ED_MakeStatic(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

int ED_IsOnFloor(edict_t *e)
{
	if(!ED_IsValid(e))
		return 0;
};

int ED_DropToFloor(edict_t *e)
{
	if(!ED_IsValid(e))
		return 0;
};

int ED_WalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	if(!ED_IsValid(ent))
		return 0;
};

void ED_AnimationAutomove(const edict_t *pEdict, float flTime)
{
	if(!ED_IsValid(pEdict))
		return;
};

void ED_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
	if(!ED_IsValid(pEdict))
		return;
};

void ED_SetModel(edict_t *e, const char *m)
{
	if(!ED_IsValid(e))
		return;
};

void ED_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	if(!ED_IsValid(e))
		return;
};

void ED_SetOrigin(edict_t *e, const float *rgflOrigin)
{
	if(!ED_IsValid(e))
		return;
};

int ED_GetIllum(edict_t *pEnt)
{
	if(!ED_IsValid(pEnt))
		return 0;
};

void ED_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_ChangeYaw(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_ChangePitch(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_GetSpawnParms(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void ED_SaveSpawnParms(edict_t *ent)
{
	if(!ED_IsValid(ent))
		return;
};

void *ED_GetModelPtr(edict_t *pEdict)
{
	if(!ED_IsValid(pEdict))
		return nullptr;
};

void ED_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
	if(!ED_IsValid(pEdict))
		return;
};
#endif // OGS_NULL_EDICT_API_IMPL