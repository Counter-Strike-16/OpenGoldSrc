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

#include "world/EdictPool.hpp"

CEdictPool::CEdictPool(unsigned anMaxEdicts)
    : mnMaxEdicts(anMaxEdicts){};

CEdictPool::~CEdictPool(){};

/*
=================
ED_Alloc

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
CEdictHandle *CEdictPool::AllocEdict()
{
	// Search for free edict and return it if found
	for(tEdictVecIt It = mvEdicts.begin(); It < mvEdicts.end(); ++It)
		if(*It->free)
			return *It;

	// Didn't found - allocate new if we can

	// Check if we are out of free edicts
	if(mvEdicts.size() >= mnMaxEdicts)
	{
		if(!g_psv.max_edicts)
			Sys_Error(__FUNCTION__ ": no edicts yet");

		Sys_Error(__FUNCTION__ ": no free edicts");
		return NULL;
	};

	// new pointer to edict (new edict) : edict = handling edict; destructor->delete handled edict?; release method
	CEdictHandle *pNewEdict = new CEdictHandle(new edict_t); // ouch; is edict_t's deletion handled?
	mvEdicts.push_back(pNewEdict);

	pNewEdict->Clear();
	return pNewEdict;
};

/*
=================
ED_Free

Marks the edict as free
FIXME: walk all entities and NULL out references to this entity
=================
*/
void CEdictPool::FreeEdict(edict_t *edict) // can be turned into method of edicthandle
{
	if(!edict)
		return;

	if(!edict->free)
	{
		// unlink from world bsp
		SV_UnlinkEdict(ed);

		FreeEntPrivateData(ed);

		ed->serialnumber++;
		ed->freetime = (float)g_psv.time;
		ed->free     = TRUE;
		ed->v.flags  = 0;
		ed->v.model  = 0;

		ed->v.takedamage = 0;
		ed->v.modelindex = 0;
		ed->v.colormap   = 0;
		ed->v.skin       = 0;
		ed->v.frame      = 0;
		ed->v.scale      = 0;
		ed->v.gravity    = 0;
		ed->v.nextthink  = -1.0f;
		ed->v.solid      = SOLID_NOT;

		//

		//VectorCopy(vec3_origin, ed->v.origin);
		//VectorCopy(vec3_origin, ed->v.angles);

		//#define vec3_t Vector // defined in sdk so this below should work
		//ed->v.origin = vec3_origin

		//

		ed->v.origin[0] = vec3_origin[0];
		ed->v.origin[1] = vec3_origin[1];
		ed->v.origin[2] = vec3_origin[2];
		ed->v.angles[0] = vec3_origin[0];
		ed->v.angles[1] = vec3_origin[1];
		ed->v.angles[2] = vec3_origin[2];

		//
	};
};