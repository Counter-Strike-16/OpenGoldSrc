// C API for edict

#pragma once
#ifndef OGS_EDICT_LOCAL_HPP
#define OGS_EDICT_LOCAL_HPP

// We can use this

class CEdict : edict_t
{
	void SetSize(...){Edict_SetSize(this, ...)}
	void SetModel(...);
};

class CEdictHandle // reference to edict object; semantically it would work as EHANDLE in hlsdk
{
CEdictHandle(edict_t *aggr);
edict_t *privedict;
};

// And interact with it like this

// Signatures of these funcs are different but who cares, right?
// (It'll be just another func name printed into console, it's not matter)
// Or rewrite'em to "ED_" prefix if you really care...

bool Edict_IsValid(edict_t *pEdict);

struct entvars_s *Edict_GetVars(edict_t *pEdict);

void Edict_MakeStatic(edict_t *ent);

int Edict_IsOnFloor(edict_t *e);
int Edict_DropToFloor(edict_t *e);

int Edict_WalkMove(edict_t *ent, float yaw, float dist, int iMode);

void Edict_AnimationAutomove(const edict_t *pEdict, float flTime);

void Edict_GetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles);

void Edict_SetModel(edict_t *e, const char *m);
void Edict_SetSize(edict_t *e, const float *rgflMin, const float *rgflMax);
void Edict_SetOrigin(edict_t *e, const float *rgflOrigin);

int Edict_GetIllum(edict_t *pEnt);

void Edict_MoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType);

void Edict_ChangeYaw(edict_t *ent);
void Edict_ChangePitch(edict_t *ent);

void Edict_GetSpawnParms(edict_t *ent);
void Edict_SaveSpawnParms(edict_t *ent);

void *Edict_GetModelPtr(edict_t *pEdict);

void Edict_GetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles);

#endif // OGS_EDICT_LOCAL_HPP