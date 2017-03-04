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

#include "common/commontypes.h"
#include "common/com_model.h"
#include "public/FileSystem.h"
#include "system/IInfoKeyBuffer.hpp"

/// Max key/value length (with a NULL char)
const int MAX_KV_LEN = 127; // 512

// Key + value + 2 x slash + NULL
// const int MAX_INFO_STRING = 256;

const int INFO_MAX_BUFFER_VALUES = 4;

#ifdef REHLDS_FIXES
const int MAX_LOCALINFO = 4096;
#else
const int MAX_LOCALINFO = MAX_INFO_STRING * 128;
#endif // REHLDS_FIXES

const char *Info_ValueForKey(const char *s, const char *key);
void Info_RemoveKey(char *s, const char *key);
void Info_RemovePrefixedKeys(char *s, const char prefix);
qboolean Info_IsKeyImportant(const char *key);
char *Info_FindLargestKey(char *s, int maxsize);
void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize);
void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize);
void Info_Print(const char *s);
qboolean Info_IsValid(const char *s);

#ifdef REHLDS_FIXES
void Info_CollectFields(char *destInfo, const char *srcInfo, const char *collectedKeysOfFields);
#endif

NOBODY void Info_WriteVars(FileHandle_t fp);

class CInfoBuffer : public IInfoKeyBuffer
{
public:
	const char *GetKeyValue(const char *key);
	
	void RemoveKey(const char *key);
	void RemovePrefixedKeys(const char prefix);
	
	qboolean IsKeyImportant(const char *key);
	
	char *FindLargestKey(int maxsize);
	
	void SetStarKeyValue(const char *key, const char *value, int maxsize);
	void SetKeyValue(const char *key, const char *value, int maxsize);
	
	void Print();
	
	qboolean IsValid();
	
	void CollectFields(char *destInfo, const char *collectedKeysOfFields);
	
	//void WriteVars(FileHandle_t fp);
private:
	char *msBuffer;
	//key-value strings map here?
};