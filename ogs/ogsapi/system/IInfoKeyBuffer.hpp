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

#pragma once

#include "common/commontypes.h"

struct IInfoKeyBuffer
{
	virtual const char *GetKeyValue(const char *key) = 0;
	
	virtual void RemoveKey(const char *key) = 0;
	virtual void RemovePrefixedKeys(const char prefix) = 0;
	
	virtual qboolean IsKeyImportant(const char *key) = 0;
	
	virtual char *FindLargestKey(int maxsize) = 0;
	
	virtual void SetStarKeyValue(const char *key, const char *value, int maxsize) = 0;
	virtual void SetKeyValue(const char *key, const char *value, int maxsize) = 0;
	
	virtual void Print() = 0;
	
	virtual qboolean IsValid() = 0;
	
	virtual void CollectFields(char *destInfo, const char *collectedKeysOfFields) = 0;
	
	//virtual void WriteVars(FileHandle_t fp) = 0;
};