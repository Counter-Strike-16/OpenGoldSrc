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

// MetamodEntityListener.hpp - header of entity listener component for metamod

#ifndef METAMOD_ENTITYLISTENER_HPP
#define METAMOD_ENTITYLISTENER_HPP

#include "world/IEntityListener.hpp"

class CMetamodEntityListener : public IEntityListener
{
public:
	int OnEntitySpawn(edict_t *apEntity);
	void OnEntityThink(edict_t *apEntity);
	void OnEntityUse(edict_t *apEntity, edict_t *apUsedBy);
	
	void EntityKeyValue(edict_t *apEntity, KeyValueData *apKeyValueData);
	
	void OnEntitySave(edict_t *apEntity, SAVERESTOREDATA *apSaveData);
	int OnEntityRestore(edict_t *apEntity, SAVERESTOREDATA *apSaveData, int anGlobalEntity);
	
	void OnEntityFreePrivateData(edict_t *apEntity);
};

#endif // METAMOD_ENTITYLISTENER_HPP