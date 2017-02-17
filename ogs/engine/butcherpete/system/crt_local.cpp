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
#include "system/common.hpp"

#ifdef Q_functions

NOBODY void Q_memset(void *dest, int fill, int count);
NOBODY void Q_memcpy(void *dest, const void *src, int count);
NOBODY int Q_memcmp(void *m1, void *m2, int count);

void Q_strcpy(char *dest, const char *src)
{
	char *c;
	const char *s;

	s = src;
	for(c = dest; s; *c++ = *s++)
	{
		if(!c)
			break;
		if(!*s)
			break;
	}
	*c = 0;
}

NOBODY void Q_strncpy(char *dest, const char *src, int count);

int Q_strlen(const char *str)
{
	int result = 0;
	if(str)
	{
		if(*str)
		{
			while(str[result++ + 1])
				;
		}
	}
	return result;
}

NOBODY char *Q_strrchr(char *s, char c);
NOBODY void Q_strcat(char *dest, char *src);
NOBODY int Q_strcmp(const char *s1, const char *s2);
NOBODY int Q_strncmp(const char *s1, const char *s2, int count);
NOBODY int Q_strncasecmp(const char *s1, const char *s2, int n);
NOBODY int Q_strcasecmp(const char *s1, const char *s2);
NOBODY int Q_stricmp(const char *s1, const char *s2);
NOBODY int Q_strnicmp(const char *s1, const char *s2, int n);
NOBODY int Q_atoi(const char *str);
NOBODY float Q_atof(const char *str);
NOBODY char *Q_strlwr(char *src);
NOBODY int Q_FileNameCmp(char *file1, char *file2);
NOBODY char *Q_strstr(const char *s1, const char *search);
NOBODY uint64 Q_strtoull(char *str);

#endif // Q_functions