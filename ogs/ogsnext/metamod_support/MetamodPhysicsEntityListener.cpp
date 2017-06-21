/*
*	This file is part of the Magenta Engine
*
*	Copyright (C) 2016 Sh1ft0x0EF
*
*	Magenta Engine is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	Magenta Engine is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with Magenta Engine. If not, see <http://www.gnu.org/licenses/>.
*
*	In addition, as a special exception, the author gives permission to
*	link the code of this program with the Half-Life Game Engine("HL
*	Engine") and Modified Game Libraries("MODs") developed by Valve,
*	L.L.C("Valve").  You must obey the GNU General Public License in all
*	respects for all of the code used other than the HL Engine and MODs
*	from Valve.  If you modify this file, you may extend this exception
*	to your version of the file, but you are not obligated to do so.  If
*	you do not wish to do so, delete this exception statement from your
*	version.
*/

// MetamodPhysicsEntityListener.cpp - source of physics entity listener component for metamod

#include "MetamodPhysicsEntityListener.hpp"
#include "dllapi.h"

void CMetamodPhysicsEntityListener::OnEntityTouch(edict_t *apEdict, edict_t *apTouchedBy)
{
	META_DLLAPI_HANDLE_void(FN_DISPATCHTOUCH, pfnTouch, 2p, (pentTouched, pentOther));
	RETURN_API_void();
};

void CMetamodPhysicsEntityListener::OnEntityBlocked(edict_t *apEdict, edict_t *apBlockedBy)
{
	META_DLLAPI_HANDLE_void(FN_DISPATCHBLOCKED, pfnBlocked, 2p, (pentBlocked, pentOther));
	RETURN_API_void();
};

int CMetamodPhysicsEntityListener::ShouldEntityCollide(edict_t *apEdict, edict_t *apCollideWith)
{
	META_NEWAPI_HANDLE(int, 1, FN_SHOULDCOLLIDE, pfnShouldCollide, 2p, (pentTouched, pentOther));
	RETURN_API(int);
};

void CMetamodPhysicsEntityListener::SetEntityAbsBox(edict_t *apEdict)
{
	META_DLLAPI_HANDLE_void(FN_DISPATCHOBJECTCOLLISIONBOX, pfnSetAbsBox, p, (pent));
	RETURN_API_void();
};