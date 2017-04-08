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
#include "system/InfoKeyStore.hpp"
#include "console/Console.hpp"
#include "system/common.hpp"
#include "system/SystemTypes.hpp"
#include "system/unicode_strtools.h"

// NOTE: This file contains a lot of fixes that are not covered by REHLDS_FIXES
// define.

// TODO: Most of the Info_ functions can be speedup via removing unneded copy of
// key and values.

// Searches the string for the given
// key and returns the associated value, or an empty string.
const char *CInfoKeyBuffer::GetValueForKey(const char *key)
{
	// use few (two?) buffers so compares work without stomping on each other
	static char value[INFO_MAX_BUFFER_VALUES][MAX_KV_LEN];
	static int valueindex;
	char pkey[MAX_KV_LEN];
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
		c = pkey;
		while(*s != '\\')
		{
			if(!*s)
			{
				return ""; // key should end with a \, not a NULL, but suppose its value
				           // as absent
			}
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		s++; // skip the slash

		// Copy a value
		nCount = 0;
		c = value[valueindex];
		while(*s != '\\')
		{
			if(!*s)
				break; // allow value to be ended with NULL
			
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		if(!Q_strcmp(key, pkey))
		{
			c = value[valueindex];
			valueindex = (valueindex + 1) % INFO_MAX_BUFFER_VALUES;
			return c;
		};
	};

	return "";
};

void CInfoKeyBuffer::RemoveKey(const char *key)
{
	char pkey[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *start;
	char *c;
	int cmpsize;
	int nCount;

	if(Q_strstr(key, "\\"))
	{
		Con_Printf("Can't use a key with a \\\n");
		return;
	}

	cmpsize = Q_strlen(key);
	if(cmpsize > MAX_KV_LEN - 1)
		cmpsize = MAX_KV_LEN - 1;

	while(*s)
	{
		start = s;

		if(*s == '\\')
		{
			s++; // skip the slash
		}

		// Copy a key
		nCount = 0;
		c = pkey;
		while(*s != '\\')
		{
			if(!*s)
			{
				break; // key should end with a \, not a NULL, but allow to remove it
			}
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if(*s)
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
				s++;
				continue; // skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		// Compare keys
		if(!Q_strncmp(key, pkey, cmpsize))
		{
			strcpy_safe(start, s); // remove this part
			s = start;             // continue searching
		}
	};
};

void CInfoKeyBuffer::RemovePrefixedKeys(const char prefix)
{
	char pkey[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *start;
	char *c;
	int nCount;

	while(*s)
	{
		start = s;

		if(*s == '\\')
		{
			s++; // skip the slash
		}

		// Copy a key
		nCount = 0;
		c = pkey;
		while(*s != '\\')
		{
			if(!*s)
			{
				break; // key should end with a \, not a NULL, but allow to remove it
			}
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if(*s)
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
				s++;
				continue; // skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		if(pkey[0] == prefix)
		{
			strcpy_safe(start, s); // remove this part
			s = start;             // continue searching
		};
	};
};

bool CInfoKeyBuffer::IsKeyImportant(const char *key)
{
	return Info_
};

char *CInfoKeyBuffer::FindLargestKey(int maxsize)
{
	return Info_
};

void CInfoKeyBuffer::SetKeyValue(const char *key, const char *value, int maxsize)
{
	if(key[0] == '*')
	{
		mpConsole->Printf("Can't set * keys\n");
		return;
	};

	SetValueForStarKey(key, value, maxsize);
};

void CInfoKeyBuffer::SetStarKeyValue(const char *key, const char *value, int maxsize)
{
	char newArray[MAX_INFO_STRING];
	char *v;
	int c;

	if(!key || !value)
	{
		mpConsole->Printf("Keys and values can't be null\n");
		return;
	};

	if(key[0] == 0)
	{
		mpConsole->Printf("Keys can't be an empty string\n");
		return;
	};

	if(Q_strstr(key, "\\") || Q_strstr(value, "\\"))
	{
		mpConsole->Printf("Can't use keys or values with a \\\n");
		return;
	};

	if(Q_strstr(key, "..") || Q_strstr(value, ".."))
	{
		// TODO: Why silently return?
		// mpConsole->Printf("Can't use keys or values with a ..\n");
		return;
	};

	if(Q_strstr(key, "\"") || Q_strstr(value, "\""))
	{
		mpConsole->Printf("Can't use keys or values with a \"\n");
		return;
	};

	int keyLen = Q_strlen(key);
	int valueLan = Q_strlen(value);

	if(keyLen >= MAX_KV_LEN || valueLan >= MAX_KV_LEN)
	{
		Con_Printf("Keys and values must be < %i characters\n", MAX_KV_LEN);
		return;
	}

	if(!Q_UnicodeValidate(key) || !Q_UnicodeValidate(value))
	{
		Con_Printf("Keys and values must be valid utf8 text\n");
		return;
	}

	// Remove current key/value and return if we doesn't specified to set a value
	RemoveKey(key);
	
	if(value[0] == 0)
		return;

	// Create key/value pair
	Q_snprintf(newArray, MAX_INFO_STRING - 1, "\\%s\\%s", key, value);
	newArray[MAX_INFO_STRING - 1] = 0;

	int neededLength = Q_strlen(newArray);
	if((int)Q_strlen(s) + neededLength >= maxsize)
	{
		// no more room in the buffer to add key/value
		if(!Info_IsKeyImportant(key))
		{
			// no room to add setting
			mpConsole->Printf("Info string length exceeded\n");
			return;
		}

		// keep removing the largest key/values until we have a room
		char *largekey;
		do
		{
			largekey = Info_FindLargestKey(s, maxsize);
			if(largekey[0] == 0)
			{
				// no room to add setting
				mpConsole->Printf("Info string length exceeded\n");
				return;
			};
			
			RemoveKey(largekey);
		}
		while((int)Q_strlen(s) + neededLength >= maxsize);
	}

	// auto lowercase team
	bool lowerCaseValue = Q_stricmp(key, "team") == 0;
	s += Q_strlen(s);
	v = newArray;
	
	while(*v)
	{
		c = (unsigned char)*v++;
		
		if(lowerCaseValue)
			c = tolower(c);
		
		*s++ = c;
	};
	
	*s = 0;
};

void CInfoKeyBuffer::Print()
{
	char key[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *c;
	int l;
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
				break; // key should end with a \, not a NULL, but allow to print it
			}
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if(*s)
			s++; // skip the slash

		// Pad and print a key
		l = c - key;
		if(l < 20)
		{
			Q_memset(c, ' ', 20 - l);
			key[20] = 0;
		};
		
		mpConsole->Printf("%s", key);

		if(!*s)
		{
			mpConsole->Printf("MISSING VALUE\n");
			return;
		}

		// Copy a value
		nCount = 0;
		c = value;
		while(*s != '\\')
		{
			if(!*s)
				break; // allow value to be ended with NULL
			
			if(nCount >= MAX_KV_LEN)
			{
				s++;
				continue; // skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		mpConsole->Printf("%s\n", value);
	};
};

bool CInfoKeyBuffer::IsValid()
{
	return Info_IsValid();
};

void CInfoKeyBuffer::CollectFields(char *destInfo, const char *collectedKeysOfFields)
{
	Info_CollectFields();
};

NOXREF void CInfoKeyBuffer::WriteToFile(FileHandle_t fp)
{
	NOXREFCHECK;

	static char value[4][512];
	static int valueindex;

	valueindex = (valueindex + 1) % 4;
	char *s = &msInternalString[0];

	if(*s == '\\')
		s++;
	
	char *o;
	char pkey[512];
	
	while(1)
	{
		o = pkey;
		while(*s != '\\')
		{
			if(!*s)
				return;
			*o++ = *s++;
		}

		*o = 0;
		s++;

		o = value[valueindex];

		while(*s != '\\' && *s)
		{
			if(!*s)
				return;
			*o++ = *s++;
		};
		
		*o = 0;

		cvar_t *pcvar = Cvar_FindVar(pkey);
		
		if(!pcvar && pkey[0] != '*')
			FS_FPrintf(fp, "setinfo \"%s\" \"%s\"\n", pkey, value[valueindex]);

		if(!*s)
			return;
		s++;
	};
};