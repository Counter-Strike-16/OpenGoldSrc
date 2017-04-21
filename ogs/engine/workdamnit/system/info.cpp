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
#include "system/info.hpp"

const char *EXT_FUNC Info_ValueForKey(const char *s, const char *key)
{
	
}

void Info_RemoveKey(char *s, const char *key)
{
	
}

void Info_RemovePrefixedKeys(char *s, const char prefix)
{
	
}

qboolean Info_IsKeyImportant(const char *key)
{
	if(key[0] == '*')
		return true;
	if(!Q_strcmp(key, "name"))
		return true;
	if(!Q_strcmp(key, "model"))
		return true;
	if(!Q_strcmp(key, "rate"))
		return true;
	if(!Q_strcmp(key, "topcolor"))
		return true;
	if(!Q_strcmp(key, "bottomcolor"))
		return true;
	if(!Q_strcmp(key, "cl_updaterate"))
		return true;
	if(!Q_strcmp(key, "cl_lw"))
		return true;
	if(!Q_strcmp(key, "cl_lc"))
		return true;
#ifndef REHLDS_FIXES
	// keys starts from '*' already checked
	if(!Q_strcmp(key, "*hltv"))
		return true;
	if(!Q_strcmp(key, "*sid"))
		return true;
#endif

	return false;
}

char *Info_FindLargestKey(char *s, int maxsize)
{
	
}

void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize)
{
	
}

void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize)
{
	
}

void Info_Print(const char *s)
{
	
}

qboolean Info_IsValid(const char *s)
{
	char key[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *c;
	int nCount;

	while(*s)
	{
		if(*s == '\\')
		{
			s++; // skip the slash
		}

		// Copy a key
		nCount = 0;
		c = key;
		while(*s != '\\')
		{
			if(!*s)
			{
				return FALSE; // key should end with a \, not a NULL
			}
			if(nCount >= MAX_KV_LEN)
			{
				return FALSE; // key length should be less then MAX_KV_LEN
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		s++; // skip the slash

		// Copy a value
		nCount = 0;
		c = value;
		while(*s != '\\')
		{
			if(!*s)
			{
				break; // allow value to be ended with NULL
			}
			if(nCount >= MAX_KV_LEN)
			{
				return FALSE; // value length should be less then MAX_KV_LEN
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		if(value[0] == 0)
		{
			return FALSE; // empty values are not valid
		}

		if(!*s)
		{
			return TRUE; // EOL, info is valid
		}
	}

	return FALSE;
}

#ifdef REHLDS_FIXES
void Info_CollectFields(char *destInfo, const char *srcInfo, const char *collectedKeysOfFields)
{
	char keys[MAX_INFO_STRING];
	Q_strcpy(keys, collectedKeysOfFields);

	size_t userInfoLength = 0;
	for(const char *key = strtok(keys, "\\"); key; key = strtok(nullptr, "\\"))
	{
		const char *value = Info_ValueForKey(srcInfo, key);

		if(value[0] == '\0')
			continue;

		// Integer fields
		if(!Q_strcmp(key, "*hltv") || !Q_strcmp(key, "bottomcolor") ||
		   !Q_strcmp(key, "topcolor"))
		{
			int intValue = Q_atoi(value);

			if(!intValue)
				continue;

			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], key);
			userInfoLength += Q_strlen(key);

			destInfo[userInfoLength++] = '\\';
			userInfoLength += Q_sprintf(&destInfo[userInfoLength], "%d", intValue);
		}
		// String fields
		else
		{
			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], key);
			userInfoLength += Q_strlen(key);

			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], value);
			userInfoLength += Q_strlen(value);
		}
	}
	destInfo[userInfoLength] = '\0';
}
#endif // REHLDS_FIXES
