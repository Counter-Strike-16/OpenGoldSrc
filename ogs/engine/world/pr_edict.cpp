/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

// C API for edict

#include "world/pr_edict.h"

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