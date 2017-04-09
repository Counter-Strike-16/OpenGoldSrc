/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#pragma once

#include "common/maintypes.h"
#include "public/archtypes.h"

typedef struct edict_s edict_t;
typedef struct entvars_s entvars_t;
typedef struct cvar_s cvar_t;

// commented things was conflicting with progs.h

void ED_ClearEdict(edict_t *e);
//edict_t *ED_Alloc(void);
//void ED_Free(edict_t *ed);
NOXREF void ED_Count(void);
char *ED_NewString(const char *string);
char *ED_ParseEdict(char *data, edict_t *ent);
//void ED_LoadFromFile(char *data);
NOXREF void PR_Init(void);
//edict_t *EDICT_NUM(int n);
//int NUM_FOR_EDICT(const edict_t *e);
bool SuckOutClassname(char *szInputStream, edict_t *pEdict);
void ReleaseEntityDLLFields(edict_t *pEdict);
void InitEntityDLLFields(edict_t *pEdict);
void *PvAllocEntPrivateData(edict_t *pEdict, int32 cb);
void *PvEntPrivateData(edict_t *pEdict);
void FreeEntPrivateData(edict_t *pEdict);
void FreeAllEntPrivateData(void);
edict_t *PEntityOfEntOffset(int iEntOffset);
int EntOffsetOfPEntity(const edict_t *pEdict);
int IndexOfEdict(const edict_t *pEdict);
edict_t *PEntityOfEntIndex(int iEntIndex);
const char *SzFromIndex(int iString);
entvars_t *GetVarsOfEnt(edict_t *pEdict);
edict_t *FindEntityByVars(entvars_t *pvars);

int AllocEngineString(const char *szValue);

void SaveSpawnParms(edict_t *pEdict);
void *GetModelPtr(edict_t *pEdict);