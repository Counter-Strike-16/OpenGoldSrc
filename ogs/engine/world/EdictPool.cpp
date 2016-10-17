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

#include "world/EdictPool.h"

CEdictPool::CEdictPool(unsigned anMaxEdicts) : mnMaxEdicts(anMaxEdicts)
{
};

CEdictPool::~CEdictPool()
{
};

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
edict_t *CEdictPool::AllocEdict()
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
	
	edict_t *pNewEdict = new edict_t;
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
void CEdictPool::FreeEdict(edict_t *edict)
{
	if(!edict)
		return;
	
	if(!edict->free)
	{
		SV_UnlinkEdict(ed);
		FreeEntPrivateData(ed);
		
		ed->serialnumber++;
		ed->freetime = (float)g_psv.time;
		ed->free = TRUE;
		ed->v.flags = 0;
		ed->v.model = 0;
		
		ed->v.takedamage = 0;
		ed->v.modelindex = 0;
		ed->v.colormap = 0;
		ed->v.skin = 0;
		ed->v.frame = 0;
		ed->v.scale = 0;
		ed->v.gravity = 0;
		ed->v.nextthink = -1.0f;
		ed->v.solid = SOLID_NOT;
		
		ed->v.origin[0] = vec3_origin[0];
		ed->v.origin[1] = vec3_origin[1];
		ed->v.origin[2] = vec3_origin[2];
		ed->v.angles[0] = vec3_origin[0];
		ed->v.angles[1] = vec3_origin[1];
		ed->v.angles[2] = vec3_origin[2];
	};
};