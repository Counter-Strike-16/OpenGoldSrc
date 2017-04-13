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

#include "precompiled.hpp"
#include "world/pr_edict.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/host.hpp"
#include "system/system.hpp"
#include "console/console.hpp"
#include "console/cvar.hpp"
#include "system/server.hpp"
#include "world/world.hpp"
#include "resources/model_rehlds.hpp"

void ED_ClearEdict(edict_t *e)
{
	Q_memset(&e->v, 0, sizeof(e->v));
	e->free = FALSE;
	ReleaseEntityDLLFields(e);
	InitEntityDLLFields(e);
}

edict_t *ED_Alloc()
{
	int i;
	edict_t *e;

	// Search for free entity
	for(i = g_psvs.maxclients + 1; i < g_psv.num_edicts; i++)
	{
		e = &g_psv.edicts[i];
		if(e->free && (e->freetime <= 2.0f || g_psv.time - e->freetime >= 0.5))
		{
			ED_ClearEdict(e);
			return e;
		}
	}

	// Check if we are out of free edicts
	if(i >= g_psv.max_edicts)
	{
		if(!g_psv.max_edicts)
			Sys_Error("%s: no edicts yet", __FUNCTION__);

		Sys_Error("%s: no free edicts", __FUNCTION__);
	}

	// Use new one
	++g_psv.num_edicts;
	e = &g_psv.edicts[i];

	ED_ClearEdict(e);
	return e;
}

NOXREF void ED_Count()
{
	NOXREFCHECK;

	int i;
	edict_t *ent;
	int active = 0, models = 0, solid = 0, step = 0;

	for(i = 0; i < g_psv.num_edicts; i++)
	{
		ent = &g_psv.edicts[i];
		if(!ent->free)
		{
			++active;
			models += (ent->v.model) ? 1 : 0;
			solid += (ent->v.solid) ? 1 : 0;
			step += (ent->v.movetype == MOVETYPE_STEP) ? 1 : 0;
		}
	}

	Con_Printf("num_edicts:%3i\n", g_psv.num_edicts);
	Con_Printf("active    :%3i\n", active);
	Con_Printf("view      :%3i\n", models);
	Con_Printf("touch     :%3i\n", solid);
	Con_Printf("step      :%3i\n", step);
}

char *ED_NewString(const char *string)
{
	char *new_s;

// Engine string pooling
#ifdef REHLDS_FIXES

	// escaping is done inside Ed_StrPool_Alloc()
	new_s = Ed_StrPool_Alloc(string);

#else // REHLDS_FIXES

	int l = Q_strlen(string);
	new_s = (char *)Hunk_Alloc(l + 1);
	char *new_p = new_s;

	for(int i = 0; i < l; i++, new_p++)
	{
		if(string[i] == '\\')
		{
			if(string[i + 1] == 'n')
				*new_p = '\n';
			else
				*new_p = '\\';
			i++;
		}
		else
		{
			*new_p = string[i];
		}
	}
	*new_p = 0;

#endif // REHLDS_FIXES

	return new_s;
}

char *ED_ParseEdict(char *data, edict_t *ent)
{
	qboolean init = FALSE;
	char keyname[256];
	int n;
	ENTITYINIT pEntityInit;
	char *className;
	KeyValueData kvd;

	if(ent != g_psv.edicts)
	{
		Q_memset(&ent->v, 0, sizeof(ent->v));
	}

	InitEntityDLLFields(ent);

	if(SuckOutClassname(data, ent))
	{
		className = (char *)(pr_strings + ent->v.classname);

		pEntityInit = GetEntityInit(className);
		if(pEntityInit)
		{
			pEntityInit(&ent->v);
			init = TRUE;
		}
		else
		{
			pEntityInit = GetEntityInit("custom");
			if(pEntityInit)
			{
				pEntityInit(&ent->v);
				kvd.szClassName = "custom";
				kvd.szKeyName = "customclass";
				kvd.szValue = className;
				kvd.fHandled = FALSE;
				gEntityInterface.pfnKeyValue(ent, &kvd);
				init = TRUE;
			}
			else
			{
				Con_DPrintf("Can't init %s\n", className);
				init = FALSE;
			}
		}

		while(1)
		{
			data = COM_Parse(data);
			if(com_token[0] == '}')
				break;

			if(!data)
				Host_Error("%s: EOF without closing brace", __FUNCTION__);

			Q_strncpy(keyname, com_token, ARRAYSIZE(keyname) - 1);
			keyname[ARRAYSIZE(keyname) - 1] = 0;

			// Remove tail spaces
			for(n = Q_strlen(keyname) - 1; n >= 0 && keyname[n] == ' '; n--)
				keyname[n] = 0;

			data = COM_Parse(data);
			if(!data)
				Host_Error("%s: EOF without closing brace", __FUNCTION__);

			if(com_token[0] == '}')
				Host_Error("%s: closing brace without data", __FUNCTION__);

			if(className != NULL && !Q_strcmp(className, com_token))
				continue;

			if(!Q_strcmp(keyname, "angle"))
			{
				float value = (float)Q_atof(com_token);
				if(value >= 0.0)
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "%f %f %f", ent->v.angles[0], value, ent->v.angles[2]);
				}
				else if((int)value == -1)
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "-90 0 0");
				}
				else
				{
					Q_snprintf(com_token, COM_TOKEN_LEN, "90 0 0");
				}

				Q_strcpy(keyname, "angles");
			}

			kvd.szClassName = className;
			kvd.szKeyName = keyname;
			kvd.szValue = com_token;
			kvd.fHandled = 0;
			gEntityInterface.pfnKeyValue(ent, &kvd);
		}
	}

	if(!init)
	{
		ent->free = 1;
		ent->serialnumber++;
	}
	return data;
}

void ED_LoadFromFile(char *data)
{
	edict_t *ent;
	int inhibit;

	gGlobalVariables.time = (float)g_psv.time;

	ent = NULL;
	inhibit = 0;
	while(1)
	{
		data = COM_Parse(data);
		if(!data)
		{
			break;
		}
		if(com_token[0] != '{')
		{
			Host_Error("%s: found %s when expecting {", __FUNCTION__, com_token);
		}

		if(ent)
		{
			ent = ED_Alloc();
		}
		else
		{
			ent = g_psv.edicts;
			ReleaseEntityDLLFields(
			g_psv.edicts); // TODO: May be better to call ED_ClearEdict here?
			InitEntityDLLFields(ent);
		}

		data = ED_ParseEdict(data, ent);
		if(ent->free)
		{
			continue;
		}

		if(deathmatch.value != 0.0 && (ent->v.spawnflags & SF_NOTINDEATHMATCH))
		{
			ED_Free(ent);
			++inhibit;
		}
		else
		{
			if(ent->v.classname)
			{
				if(gEntityInterface.pfnSpawn(ent) < 0 || (ent->v.flags & FL_KILLME))
				{
					ED_Free(ent);
				}
			}
			else
			{
				Con_Printf("No classname for:\n");
				ED_Free(ent);
			}
		}
	}
	Con_DPrintf("%i entities inhibited\n", inhibit);
}

NOXREF void PR_Init()
{
	NOXREFCHECK;
}

edict_t *EDICT_NUM(int n)
{
	if(n < 0 || n >= g_psv.max_edicts)
	{
		Sys_Error("%s: bad number %i", __FUNCTION__, n);
	}
	return &g_psv.edicts[n];
}

int NUM_FOR_EDICT(const edict_t *e)
{
	int b;
	b = e - g_psv.edicts;

	if(b < 0 || b >= g_psv.num_edicts)
	{
		Sys_Error("%s: bad pointer", __FUNCTION__);
	}

	return b;
}

bool SuckOutClassname(char *szInputStream, edict_t *pEdict)
{
	char szKeyName[256];
	KeyValueData kvd;

	// key
	szInputStream = COM_Parse(szInputStream);
	while(szInputStream && com_token[0] != '}')
	{
		Q_strncpy(szKeyName, com_token, ARRAYSIZE(szKeyName) - 1);
		szKeyName[ARRAYSIZE(szKeyName) - 1] = 0;

		// value
		szInputStream = COM_Parse(szInputStream);

		if(!Q_strcmp(szKeyName, "classname"))
		{
			kvd.szClassName = NULL;
			kvd.szKeyName = szKeyName;
			kvd.szValue = com_token;
			kvd.fHandled = FALSE;

			gEntityInterface.pfnKeyValue(pEdict, &kvd);

			if(kvd.fHandled == FALSE)
			{
				Host_Error("%s: parse error", __FUNCTION__);
			}

			return true;
		}

		if(!szInputStream)
		{
			break;
		}

		// next key
		szInputStream = COM_Parse(szInputStream);
	}

#ifdef REHLDS_FIXES
	if(pEdict == g_psv.edicts)
	{
		kvd.szClassName = NULL;
		kvd.szKeyName = "classname";
		kvd.szValue = "worldspawn";
		kvd.fHandled = FALSE;

		gEntityInterface.pfnKeyValue(pEdict, &kvd);

		if(kvd.fHandled == FALSE)
		{
			Host_Error("%s: parse error", __FUNCTION__);
		}

		return true;
	}
#endif

	// classname not found
	return false;
}

void ReleaseEntityDLLFields(edict_t *pEdict)
{
	FreeEntPrivateData(pEdict);
}

void InitEntityDLLFields(edict_t *pEdict)
{
	pEdict->v.pContainingEntity = pEdict;
}

void *EXT_FUNC PvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	FreeEntPrivateData(pEdict);

	if(cb <= 0)
	{
		return NULL;
	}

	pEdict->pvPrivateData = Mem_Calloc(1, cb);

#ifdef REHLDS_FLIGHT_REC
	if(rehlds_flrec_pvdata.string[0] != '0')
	{
		FR_AllocEntPrivateData(pEdict->pvPrivateData, cb);
	}
#endif // REHLDS_FLIGHT_REC

	return pEdict->pvPrivateData;
}

void *EXT_FUNC PvEntPrivateData(edict_t *pEdict)
{
	if(!pEdict)
	{
		return NULL;
	}

	return pEdict->pvPrivateData;
}

void FreeEntPrivateData(edict_t *pEdict)
{
	if(pEdict->pvPrivateData)
	{
		if(gNewDLLFunctions.pfnOnFreeEntPrivateData)
		{
			gNewDLLFunctions.pfnOnFreeEntPrivateData(pEdict);
		}

#ifdef REHLDS_FLIGHT_REC
		if(rehlds_flrec_pvdata.string[0] != '0')
		{
			FR_FreeEntPrivateData(pEdict->pvPrivateData);
		}
#endif // REHLDS_FLIGHT_REC

		Mem_Free(pEdict->pvPrivateData);
		pEdict->pvPrivateData = 0;
	}
}

void FreeAllEntPrivateData()
{
	for(int i = 0; i < g_psv.num_edicts; i++)
	{
		FreeEntPrivateData(&g_psv.edicts[i]);
	}
}

edict_t *EXT_FUNC PEntityOfEntOffset(int iEntOffset)
{
	return (edict_t *)((char *)g_psv.edicts + iEntOffset);
}

int EXT_FUNC EntOffsetOfPEntity(const edict_t *pEdict)
{
	return (char *)pEdict - (char *)g_psv.edicts;
}

int EXT_FUNC IndexOfEdict(const edict_t *pEdict)
{
	int index = 0;
	if(pEdict)
	{
		index = pEdict - g_psv.edicts;
#ifdef REHLDS_FIXES
		if(index < 0 ||
		   index >= g_psv.max_edicts) // max_edicts is not valid entity index
#else                                 // REHLDS_FIXES
		if(index < 0 || index > g_psv.max_edicts)
#endif                                // REHLDS_FIXES
		{
			Sys_Error("%s: bad entity", __FUNCTION__);
		}
	}
	return index;
}

edict_t *EXT_FUNC PEntityOfEntIndex(int iEntIndex)
{
	if(iEntIndex < 0 || iEntIndex >= g_psv.max_edicts)
	{
		return NULL;
	}

	edict_t *pEdict = EDICT_NUM(iEntIndex);

#ifdef REHLDS_FIXES
	if(pEdict &&
	   (pEdict->free || (iEntIndex > g_psvs.maxclients &&
	                     !pEdict->pvPrivateData))) // Simplified confition
	{
		return NULL;
	}
#else  // REHLDS_FIXES
	if((!pEdict || pEdict->free || !pEdict->pvPrivateData) &&
	   (iEntIndex >= g_psvs.maxclients || pEdict->free))
	{
		if(iEntIndex >= g_psvs.maxclients || pEdict->free)
		{
			pEdict = NULL;
		}
	}
#endif // REHLDS_FIXES

	return pEdict;
}

const char *EXT_FUNC SzFromIndex(int iString)
{
	return (const char *)(pr_strings + iString);
}

entvars_t *EXT_FUNC GetVarsOfEnt(edict_t *pEdict)
{
	return &pEdict->v;
}

edict_t *EXT_FUNC FindEntityByVars(entvars_t *pvars)
{
	for(int i = 0; i < g_psv.num_edicts; i++)
	{
		edict_t *pEdict = &g_psv.edicts[i];
		if(&pEdict->v == pvars)
		{
			return pEdict;
		}
	}
	return NULL;
}

float EXT_FUNC CVarGetFloat(const char *szVarName)
{
	return Cvar_VariableValue(szVarName);
}

const char *EXT_FUNC CVarGetString(const char *szVarName)
{
	return Cvar_VariableString(szVarName);
}

cvar_t *EXT_FUNC CVarGetPointer(const char *szVarName)
{
	return Cvar_FindVar(szVarName);
}

void EXT_FUNC CVarSetFloat(const char *szVarName, float flValue)
{
	Cvar_SetValue(szVarName, flValue);
}

void EXT_FUNC CVarSetString(const char *szVarName, const char *szValue)
{
	Cvar_Set(szVarName, szValue);
}

void EXT_FUNC CVarRegister(cvar_t *pCvar)
{
	if(pCvar)
	{
		pCvar->flags |= FCVAR_EXTDLL;
		Cvar_RegisterVariable(pCvar);
	}
}

int EXT_FUNC AllocEngineString(const char *szValue)
{
	return ED_NewString(szValue) - pr_strings;
}

void EXT_FUNC SaveSpawnParms(edict_t *pEdict)
{
	int eoffset = NUM_FOR_EDICT(pEdict);
	if(eoffset < 1 || eoffset > g_psvs.maxclients)
	{
		Host_Error("Entity is not a client");
	}
	// Nothing more for this function even on client-side
}

void *EXT_FUNC GetModelPtr(edict_t *pEdict)
{
	if(!pEdict)
		return NULL;
	
	return Mod_Extradata(Mod_Handle(pEdict->v.modelindex));
}
