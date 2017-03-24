/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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
#include "resources/ResourceList.hpp"

void CResourceList::Clear()
{
	resource_t *n = nullptr;

	for(resource_t *p = mpList->pNext; p && p != mpList; p = n)
	{
		n = p->pNext;

		Remove(p);
		Mem_Free(p);
	};

	mpList->pPrev = mpList;
	mpList->pNext = mpList;
};

void CResourceList::Add(resource_t *pResource)
{
	if(pResource->pPrev || pResource->pNext)
	{
		mpConsole->Printf("Resource is already linked!\n"); // TODO: which resource
		return;
	};

	pResource->pPrev = mpList->pPrev;
	pResource->pNext = mpList;
	mpList->pPrev->pNext = pResource;
	mpList->pPrev = pResource;
};

void CResourceList::Remove(resource_t *pResource)
{
	pResource->pPrev->pNext = pResource->pNext;
	pResource->pNext->pPrev = pResource->pPrev;
	pResource->pPrev = nullptr;
	pResource->pNext = nullptr;
};