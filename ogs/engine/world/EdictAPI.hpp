// C API for edict

#pragma once
#ifndef OGS_EDICTAPI_HPP
#define OGS_EDICTAPI_HPP

extern "C"
{

bool ED_IsValid(edict_t *pEdict);

struct entvars_s *ED_GetVars(edict_t *pEdict);

void ED_MakeStatic(edict_t *ent);

int ED_IsOnFloor(edict_t *e);
int ED_DropToFloor(edict_t *e);

int ED_WalkMove(edict_t *ent, float yaw, float dist, int iMode);

void ED_AnimationAutomove(const edict_t *pEdict, float flTime);

void ED_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles);

void ED_SetModel(edict_t *e, const char *m);
void ED_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax);
void ED_SetOrigin(edict_t *e, const float *rgflOrigin);

int ED_GetIllum(edict_t *pEnt);

void ED_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType);

void ED_ChangeYaw(edict_t *ent);
void ED_ChangePitch(edict_t *ent);

void ED_GetSpawnParms(edict_t *ent);
void ED_SaveSpawnParms(edict_t *ent);

void *ED_GetModelPtr(edict_t *pEdict);

void ED_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles);

};

#endif // OGS_EDICTAPI_HPP