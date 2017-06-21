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
/// @brief legacy engine exports from system module

#pragma once

//============================================================================
// Common functionality
//============================================================================

char *COM_GetToken();

unsigned char *COM_LoadFile(char *path, int usehunk, int *pLength); // was const char *path
unsigned char *COM_LoadFileForMe(char *filename, int *pLength);
void COM_FreeFile(void *buffer);

const char *COM_ParseFile(const char *data, char *token, int maxtoken);

int COM_CompareFileTime(char *filename1, char *filename2, int *iCompare);

NOXREF void COM_AddAppDirectory(char *pszBaseDir, const char *appName);

unsigned int COM_GetApproxWavePlayLength(const char *filepath);

void COM_GetGameDir(char *szGameDir);

int COM_CheckParm(char *parm);

//COM_ExpandFileName();
//int COM_FileSize(const char *name);

//============================================================================
// Info string
//============================================================================

void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize);
const char *Info_ValueForKey(const char *s, const char *key);

void Info_RemoveKey(char *s, const char *key);