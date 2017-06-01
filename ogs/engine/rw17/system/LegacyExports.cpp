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
#include "system/LegacyExports.hpp"
#include "filesystem/FileSystem.hpp"
#include "filesystem/File.hpp"

CFileSystem *gpFileSystem = nullptr;

char *COM_GetToken()
{
	return com_token;
};

unsigned char *EXT_FUNC COM_LoadFile(const char *path, int usehunk, int *pLength)
{
	char base[33];
	unsigned char *buf = NULL;

#ifndef SWDS
	g_engdstAddrs.COM_LoadFile((char**)path, &usehunk, &pLength);
#endif

	if(pLength)
		*pLength = 0;

	CFile *hFile = gpFileSystem->Open(path, "rb");

	if(!hFile)
		return NULL;

	int len = hFile->GetSize();
	
	COM_FileBase(path, base);
	base[32] = 0;

	switch(usehunk)
	{
	case 0:
		buf = (unsigned char *)Z_Malloc(len + 1);
		break;

	case 1:
		buf = (unsigned char *)Hunk_AllocName(len + 1, base);
		break;

	case 2:
		buf = (unsigned char *)Hunk_TempAlloc(len + 1);
		break;

	case 3:
		buf = (unsigned char *)Cache_Alloc(loadcache, len + 1, base);
		break;

	case 4:
		if(len + 1 <= loadsize)
		{
			buf = loadbuf;
		}
		else
		{
			buf = (unsigned char *)Hunk_TempAlloc(len + 1);
		}
		break;

	case 5:
		buf = (unsigned char *)Mem_Malloc(len + 1);
		break;

	default:
#ifdef REHLDS_FIXES
		gpFileSystem->Close(hFile);
#endif
		CSystem::Error("%s: bad usehunk", __FUNCTION__);
	}

	if(!buf)
	{
#ifdef REHLDS_FIXES
		gpFileSystem->Close(hFile);
#endif
		CSystem::Error("%s: not enough space for %s", __FUNCTION__, path);
	}

	hFile->Read(buf, len, 1);
	
	gpFileSystem->Close(hFile);

	buf[len] = 0;

	if(pLength)
		*pLength = len;

	return buf;
};

unsigned char *EXT_FUNC COM_LoadFileForMe(char *filename, int *pLength)
{
	return COM_LoadFile(filename, 5, pLength);
};

void EXT_FUNC COM_FreeFile(void *buffer)
{
#ifndef SWDS
	g_engdstAddrs.COM_FreeFile(&buffer);
#endif

	if(buffer)
		Mem_Free(buffer);
};

const char *COM_ParseFile(const char *data, char *token, int maxtoken)
{
	const char *return_data = COM_Parse(data);
	Q_strncpy(token, com_token, maxtoken);
	return return_data;
};

int EXT_FUNC COM_CompareFileTime(char *filename1, char *filename2, int *iCompare)
{
	*iCompare = 0;

	if(filename1 && filename2)
	{
		int ft1 = gpFileSystem->GetFileTime(filename1);
		int ft2 = gpFileSystem->GetFileTime(filename2);

		if(ft1 >= ft2)
		{
			if(ft1 > ft2)
				*iCompare = 1;

			return 1;
		}
		else
		{
			*iCompare = -1;
			return 1;
		};
	};

	return 0;
};

NOXREF void COM_AddAppDirectory(char *pszBaseDir, const char *appName)
{
	NOXREFCHECK;

	gpFileSystem->AddSearchPath(pszBaseDir, "PLATFORM");
};

typedef struct
{
	unsigned char chunkID[4];
	long chunkSize;
	short wFormatTag;
	unsigned short wChannels;
	unsigned long dwSamplesPerSec;
	unsigned long dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
} FormatChunk;

constexpr auto WAVE_HEADER_LENGTH = 128;

unsigned int EXT_FUNC COM_GetApproxWavePlayLength(const char *filepath)
{
	char buf[WAVE_HEADER_LENGTH + 1];
	int filelength;
	FormatChunk format;

	CFile *hFile = gpFileSystem->Open(filepath, "rb");

	if(hFile)
	{
		filelength = hFile->GetSize();

		if(filelength <= WAVE_HEADER_LENGTH)
			return 0;

		hFile->Read(buf, WAVE_HEADER_LENGTH, 1);
		
		gpFileSystem->Close(hFile);

		buf[WAVE_HEADER_LENGTH] = 0;

		if(!Q_strnicmp(buf, "RIFF", 4) && !Q_strnicmp(&buf[8], "WAVE", 4) &&
		   !Q_strnicmp(&buf[12], "fmt ", 4))
		{
			Q_memcpy(&format, &buf[12], sizeof(FormatChunk));

			filelength -= WAVE_HEADER_LENGTH;

			if(format.dwAvgBytesPerSec > 999)
				return filelength / (format.dwAvgBytesPerSec / 1000);

			return 1000 * filelength / format.dwAvgBytesPerSec;
		}
	}

	return 0;
};

void EXT_FUNC COM_GetGameDir(char *szGameDir)
{
	if(szGameDir)
		Q_snprintf(szGameDir, MAX_PATH - 1, "%s", gpFileSystem->GetGameDirectory());
};

int COM_CheckParm(char *parm)
{
	//gpCmdLine->HasArg(parm);
	return 0;
};

//

double Sys_FloatTime()
{
	return CSystem::GetFloatTime();
};

void EXT_FUNC EngineFprintf(void *pfile, const char *szFmt, ...)
{
	AlertMessage(at_console, "EngineFprintf:  Obsolete API\n");
};

void EXT_FUNC AlertMessage(ALERT_TYPE atype, const char *szFmt, ...)
{
	va_list argptr;
	static char szOut[1024];

	va_start(argptr, szFmt);
	
	if(atype == at_logged && g_psvs.maxclients > 1)
	{
		Q_vsnprintf(szOut, sizeof(szOut), szFmt, argptr);
		Log_Printf("%s", szOut);
	};
	
	else if(developer.value != 0.0f)
	{
		switch(atype)
		{
		case at_notice:
			Q_strcpy(szOut, "NOTE:  ");
			break;
		case at_console:
			szOut[0] = 0;
			break;
		case at_aiconsole:
			if(developer.value < 2.0f)
				return;
			szOut[0] = 0;
			break;
		case at_warning:
			Q_strcpy(szOut, "WARNING:  ");
			break;
		case at_error:
			Q_strcpy(szOut, "ERROR:  ");
			break;
		case at_logged:
			break;
		default:
			break;
		};
		
		int iLen = Q_strlen(szOut);
		Q_vsnprintf(&szOut[iLen], sizeof(szOut) - iLen, szFmt, argptr);
		Con_Printf("%s", szOut);
	};
	va_end(argptr);
};

//