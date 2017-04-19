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
/// @brief misc functions used in client and server

#include "precompiled.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "system/unicode_strtools.h"
#include "console/Console.hpp"
#include "filesystem/FileSystem.hpp"
#include "resources/modinfo.hpp"
#include "network/bf_read.hpp"
#include "network/bf_write.hpp"

char serverinfo[MAX_INFO_STRING];

char *Mem_Strdup(const char *strSource)
{
	return _strdup(strSource);
};

char *strcpy_safe(char *dst, char *src)
{
	int len = Q_strlen(src);
	Q_memmove(dst, src, len + 1);
	return dst;
}

char *Info_Serverinfo()
{
	return serverinfo;
}

// Bit field reading/writing storage.
bf_read_t bfread;
ALIGN16 bf_write_t bfwrite;

void COM_BitOpsInit()
{
	Q_memset(&bfwrite, 0, sizeof(bf_write_t));
	Q_memset(&bfread, 0, sizeof(bf_read_t));
}

#ifndef COM_Functions_region

unsigned char COM_Nibble(char c)
{
	if(c >= '0' && c <= '9')
	{
		return (unsigned char)(c - '0');
	}

	if(c >= 'A' && c <= 'F')
	{
		return (unsigned char)(c - 'A' + 0x0A);
	}

	if(c >= 'a' && c <= 'f')
	{
		return (unsigned char)(c - 'a' + 0x0A);
	}

	return '0';
}

void COM_HexConvert(const char *pszInput, int nInputLength, unsigned char *pOutput)
{
	unsigned char *p;
	int i;
	const char *pIn;

	p = pOutput;
	for(i = 0; i < nInputLength - 1; i += 2)
	{
		pIn = &pszInput[i];
		if(pIn[0] == 0 || pIn[1] == 0)
			break;

		*p = COM_Nibble(pIn[0]) << 4 | COM_Nibble(pIn[1]);

		p++;
	}
}

NOXREF char *COM_BinPrintf(unsigned char *buf, int nLen)
{
	NOXREFCHECK;
	
	static char szReturn[4096];
	unsigned char c;
	char szChunk[10];
	int i;

	Q_memset(szReturn, 0, sizeof(szReturn));

	for(i = 0; i < nLen; i++)
	{
		c = (unsigned char)buf[i];

		Q_snprintf(szChunk, sizeof(szChunk), "%02x", c);
		Q_strncat(szReturn, szChunk, sizeof(szReturn) - Q_strlen(szReturn) - 1);
	}
	return szReturn;
}

void COM_ExplainDisconnection(qboolean bPrint, char *fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	Q_strncpy(gszDisconnectReason, string, sizeof(gszDisconnectReason) - 1);
	gszDisconnectReason[sizeof(gszDisconnectReason) - 1] = 0;
	gfExtendedError = 1;
	if(bPrint)
	{
		if(gszDisconnectReason[0] != '#')
			Con_Printf("%s\n", gszDisconnectReason);
	}
}

NOXREF void COM_ExtendedExplainDisconnection(qboolean bPrint, char *fmt, ...)
{
	NOXREFCHECK;

	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	Q_strncpy(gszExtendedDisconnectReason, string, sizeof(gszExtendedDisconnectReason) - 1);
	gszExtendedDisconnectReason[sizeof(gszExtendedDisconnectReason) - 1] = 0;
	if(bPrint)
	{
		if(gszExtendedDisconnectReason[0] != '#')
			Con_Printf("%s\n", gszExtendedDisconnectReason);
	}
}

#endif // COM_Functions_region

#ifndef COM_Functions_region

//int com_argc;
//char **com_argv;

char com_token[COM_TOKEN_LEN]; // 1024

qboolean com_ignorecolons //= false;
qboolean s_com_token_unget;
char *com_last_in_quotes_data = NULL;
char com_clientfallback[MAX_PATH];

cache_user_t *loadcache;
unsigned char *loadbuf;
int loadsize;

const unsigned char mungify_table[] = { 0x7A, 0x64, 0x05, 0xF1, 0x1B, 0x9B, 0xA0, 0xB5, 0xCA, 0xED, 0x61, 0x0D, 0x4A, 0xDF, 0x8E, 0xC7 };

const unsigned char mungify_table2[] = { 0x05, 0x61, 0x7A, 0xED, 0x1B, 0xCA, 0x0D, 0x9B, 0x4A, 0xF1, 0x64, 0xC7, 0xB5, 0x8E, 0xDF, 0xA0 };

unsigned char mungify_table3[] = { 0x20, 0x07, 0x13, 0x61, 0x03, 0x45, 0x17, 0x72, 0x0A, 0x2D, 0x48, 0x0C, 0x4A, 0x12, 0xA9, 0xB5 };

//============================================================================

/*
============
COM_SkipPath
============
*/
NOXREF char *COM_SkipPath(char *pathname)
{
	NOXREFCHECK;

	char *last = pathname;

	while(*pathname)
	{
		if(*pathname == '/' || *pathname == '\\')
			last = pathname + 1;
		pathname++;
	};
	
	return last;
};

void COM_StripExtension(char *in, char *out)
{
	char *c, *d = NULL;
	int i;

	// Search for the first dot after the last path separator
	c = in;
	while(*c)
	{
		if(*c == '/' || *c == '\\')
		{
			d = NULL; // reset dot location on path separator
		}
		else if(d == NULL && *c == '.')
		{
			d = c; // store first dot location in the file name
		}
		c++;
	}

	if(out == in)
	{
		if(d != NULL)
		{
			*d = 0;
		}
	}
	else
	{
		if(d != NULL)
		{
			i = d - in;
			Q_memcpy(out, in, i);
			out[i] = 0;
		}
		else
		{
			Q_strcpy(out, in);
		}
	}
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension(char *in)
{
	static char exten[MAX_PATH];
	char *c, *d = NULL;
	int i;

	// Search for the first dot after the last path separator
	c = in;
	while(*c)
	{
		if(*c == '/' || *c == '\\')
			d = NULL; // reset dot location on path separator
		else if(d == NULL && *c == '.')
			d = c; // store first dot location in the file name
		
		c++;
	};

	if(d == NULL)
		return "";

	d++; // skip dot
	
	// Copy extension
	for(i = 0; i < (ARRAYSIZE(exten) - 1) && *d; i++, d++)
		exten[i] = *d;
	exten[i] = 0;

	return exten;
};

// Fills "out" with the file name without path and extension.
void COM_FileBase(const char *in, char *out)
{
	const char *start, *end;
	int len;

	*out = 0;

	len = Q_strlen(in);
	if(len <= 0)
		return;

	start = in + len - 1;
	end = in + len;
	while(start >= in && *start != '/' && *start != '\\')
	{
		if(*start == '.')
			end = start;
		start--;
	}
	start++;

	len = end - start;
	Q_strncpy(out, start, len);
	out[len] = 0;
}

void COM_DefaultExtension(char *path, char *extension)
{
	char *src = path + Q_strlen(path) - 1;

	while(*src != '/' && *src != '\\' && src != path)
	{
		if(*src == '.')
			return;

		src--;
	};

	Q_strcat(path, extension);
}



void COM_UngetToken()
{
	s_com_token_unget = 1;
};



char *COM_Parse(char *data)
{
	int c;
	uchar32 wchar;
	int len;

	if(s_com_token_unget)
	{
		s_com_token_unget = 0;
		return data;
	}

	len = 0;
	com_token[0] = 0;

	if(!data)
		return NULL;

	if(com_last_in_quotes_data == data)
	{
		// continue to parse quoted string
		com_last_in_quotes_data = NULL;
		goto inquotes; // crap
	}

skipwhite:
	// skip whitespace
	while(!V_UTF8ToUChar32(data, &wchar) && wchar <= 32)
	{
		if(!wchar)
			return NULL;
		data = Q_UnicodeAdvance(data, 1);
	}

	c = *data;

	// skip // comments till the next line
	if(c == '/' && data[1] == '/')
	{
		while(*data && *data != '\n')
			data++;
		goto skipwhite; // start over new line
	}

	// handle quoted strings specially: copy till the end or another quote
	if(c == '\"')
	{
		data++; // skip starting quote
		while(true)
		{
		inquotes:
			c = *data++; // get char and advance
			if(!c)       // EOL
			{
				com_token[len] = 0;
				return data - 1; // we are done with that, but return data to show that
				                 // token is present
			}
			if(c == '\"') // closing quote
			{
				com_token[len] = 0;
				return data;
			}

			com_token[len] = c;
			len++;

			if(len == COM_TOKEN_LEN - 1) // check if buffer is full
			{
				// remember in-quotes state
				com_last_in_quotes_data = data;

				com_token[len] = 0;
				return data;
			}
		}
	}

	// parse single characters
	if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' ||
	   (!com_ignorecolons && c == ':'))
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}

	// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',' ||
		   (!com_ignorecolons && c == ':'))
			break;
	} while(len < COM_TOKEN_LEN - 1 && (c < 0 || c > 32));

	com_token[len] = 0;
	return data;
}

char *COM_ParseLine(char *data)
{
#ifndef REHLDS_FIXES
	unsigned int c;
#else
	int c;
#endif
	int len;

	if(s_com_token_unget)
	{
		s_com_token_unget = 0;
		return data;
	}

	len = 0;
	com_token[0] = 0;

	if(!data)
	{
		return NULL;
	}

	c = *data;

// parse a line out of the data
#ifndef REHLDS_FIXES
	while((c >= ' ' || c == '\t') && (len < COM_TOKEN_LEN - 1))
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	}
#else
	do
	{
		com_token[len] = c; // TODO: Here c may be any ASCII, \n for example, but we
		                    // are copy it in the token
		data++;
		len++;
		c = *data;
	} while(c >= ' ' &&
	        (len <
	         COM_TOKEN_LEN - 1)); // TODO: Will break on \t, may be it shouldn't?
#endif

	com_token[len] = 0;

	if(c == 0) // end of file
	{
		return NULL;
	}

	// eat whitespace (LF,CR,etc.) at the end of this line
	while((c = *data) < ' ' && c != '\t')
	{
		if(c == 0)
		{
			return NULL; // end of file;
		}
		data++;
	}

	return data;
}

int COM_TokenWaiting(char *buffer)
{
	char *p;

	p = buffer;
	while(*p && *p != '\n')
	{
		if(!isspace(*p) || isalnum(*p))
			return 1;

		p++;
	}

	return 0;
};

void COM_InitArgv(int argc, char *argv[])
{
	gpCmdLine->Init(argc, argv);
};

/*
================
COM_Init
================
*/
void COM_Init(char *basedir)
{
	unsigned short swaptest = 1;
	
	// set the byte swapping variables in a portable manner
	if(*(byte *)&swaptest == 1)
	{
		bigendien = 0; // false
		BigShort = ShortSwap;
		LittleShort = ShortNoSwap;
		BigLong = LongSwap;
		LittleLong = LongNoSwap;
		BigFloat = FloatSwap;
		LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien = 1; // true
		BigShort = ShortNoSwap;
		LittleShort = ShortSwap;
		BigLong = LongNoSwap;
		LittleLong = LongSwap;
		BigFloat = FloatNoSwap;
		LittleFloat = FloatSwap;
	};

	COM_BitOpsInit();
};

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char *va(char *format, ...)
{
	va_list argptr;
	static int current = 0;
	static char string[16][1024];

	current = (current + 1) % 16;

	va_start(argptr, format);
	Q_vsnprintf(string[current], ARRAYSIZE(string[current]), format, argptr);
	va_end(argptr);

	return string[current];
}

NOXREF char *vstr(vec_t *v) // vec_to_string
{
	NOXREFCHECK;

	static int idx = 0;
	static char string[16][1024];

	idx++;
	idx &= 15;

	Q_snprintf(string[idx], ARRAYSIZE(string[idx]), "%.4f %.4f %.4f", v[0], v[1], v[2]);
	return string[idx];
}

/// just for debugging
NOXREF int memsearch(byte *start, int count, int search)
{
	NOXREFCHECK;

	for(int i = 0; i < count; i++)
		if(start[i] == search)
			return i;

	return -1;
};

void COM_CopyFileChunk(FileHandle_t dst, FileHandle_t src, int nSize)
{
	int copysize;
	char copybuf[COM_COPY_CHUNK_SIZE];

	copysize = nSize;

	while(copysize > COM_COPY_CHUNK_SIZE)
	{
		FS_Read(copybuf, COM_COPY_CHUNK_SIZE, 1, src);
		FS_Write(copybuf, COM_COPY_CHUNK_SIZE, 1, dst);
		copysize -= COM_COPY_CHUNK_SIZE;
	}

	FS_Read(copybuf, copysize, 1, src);
	FS_Write(copybuf, copysize, 1, dst);
	FS_Flush(src);
	FS_Flush(dst);
}

NOXREF byte *COM_LoadFileLimit(char *path, int pos, int cbmax, int *pcbread, FileHandle_t *phFile)
{
	NOXREFCHECK;
	
	FileHandle_t hFile;
	byte *buf;
	char base[32];
	int len;
	int cbload;

	hFile = *phFile;
	if(!hFile)
	{
		hFile = FS_Open(path, "rb");
		if(!hFile)
			return NULL;
	}

	len = FS_Size(hFile);
	if(len < pos)
	{
#ifdef REHLDS_FIXES
		FS_Close(hFile);
#endif
		Sys_Error("%s: invalid seek position for %s", __FUNCTION__, path);
	}

	FS_Seek(hFile, pos, FILESYSTEM_SEEK_HEAD);

	if(len > cbmax)
		cbload = cbmax;
	else
		cbload = len;

	*pcbread = cbload;

	if(path)
		COM_FileBase(path, base);

	buf = (byte *)Hunk_TempAlloc(cbload + 1);
	if(!buf)
	{
		if(path)
		{
#ifdef REHLDS_FIXES
			FS_Close(hFile);
#endif
			Sys_Error("%s: not enough space for %s", __FUNCTION__, path);
		}

		FS_Close(hFile);
		return NULL;
	}

	buf[cbload] = 0;
	FS_Read(buf, cbload, 1, hFile);
	*phFile = hFile;

	return buf;
}

byte *COM_LoadHunkFile(char *path)
{
	return COM_LoadFile(path, 1, NULL);
};

byte *COM_LoadTempFile(char *path, int *pLength)
{
	return COM_LoadFile(path, 2, pLength);
};

void EXT_FUNC COM_LoadCacheFile(char *path, struct cache_user_s *cu)
{
	loadcache = cu;
	COM_LoadFile(path, 3, 0);
};

// uses temp hunk if larger than bufsize
NOXREF byte *COM_LoadStackFile(char *path, void *buffer, int bufsize, int *length)
{
	NOXREFCHECK;

	loadbuf = (byte *)buffer;
	loadsize = bufsize;

	return COM_LoadFile(path, 4, length);
}

void COM_Shutdown()
{
	// Do nothing.
}

void COM_AddDefaultDir(char *pszDir)
{
	if(pszDir && *pszDir)
	{
		FileSystem_AddFallbackGameDir(pszDir);
	}
}

void COM_ParseDirectoryFromCmd(const char *pCmdName, char *pDirName, const char *pDefault)
{
	const char *pParameter = NULL;
	int cmdParameterIndex = COM_CheckParm((char *)pCmdName);

	if(cmdParameterIndex && cmdParameterIndex < com_argc - 1)
	{
		pParameter = com_argv[cmdParameterIndex + 1];

		if(*pParameter == '-' || *pParameter == '+')
		{
			pParameter = NULL;
		}
	}

	// Found a valid parameter on the cmd line?
	if(pParameter)
	{
		// Grab it
		Q_strcpy(pDirName, pParameter);
	}
	else if(pDefault)
	{
		// Ok, then use the default
		Q_strcpy(pDirName, pDefault);
	}
	else
	{
		// If no default either, then just terminate the string
		pDirName[0] = 0;
	}

	CStringHandler::StripTrailingSlash(pDirName);
}

void COM_CheckPrintMap(dheader_t *header, const char *mapname, qboolean bShowOutdated)
{
	if(header->version == HLBSP_VERSION)
	{
		if(!bShowOutdated)
		{
			Con_Printf("%s\n", mapname);
		}
	}
	else
	{
		if(bShowOutdated)
		{
			Con_Printf("OUTDATED:  %s\n", mapname);
		}
	}
}

void COM_ListMaps(char *pszSubString)
{
	dheader_t header;
	FileHandle_t fp;

	char mapwild[64];
	char curDir[4096];
	char pFileName[64];
	const char *findfn;

	int nSubStringLen = 0;

	if(pszSubString && *pszSubString)
	{
		nSubStringLen = Q_strlen(pszSubString);
	}

	Con_Printf("-------------\n");

	for(int bShowOutdated = 1; bShowOutdated >= 0; bShowOutdated--)
	{
		Q_strcpy(mapwild, "maps/*.bsp");
		findfn = Sys_FindFirst(mapwild, NULL);

		while(findfn != NULL)
		{
			Q_snprintf(curDir, ARRAYSIZE(curDir), "maps/%s", findfn);
			FS_GetLocalPath(curDir, curDir, ARRAYSIZE(curDir));

			if(strstr(curDir, com_gamedir) &&
			   (!nSubStringLen ||
			    !Q_strnicmp(findfn, pszSubString, nSubStringLen)))
			{
				Q_memset(&header, 0, sizeof(dheader_t));
				Q_sprintf(pFileName, "maps/%s", findfn);

				fp = FS_Open(pFileName, "rb");

				if(fp)
				{
					FS_Read(&header, sizeof(dheader_t), 1, fp);
					FS_Close(fp);
				}

				COM_CheckPrintMap(&header, findfn, bShowOutdated != 0);
			}

			findfn = Sys_FindNext(NULL);
		}

		Sys_FindClose();
	}
}

void COM_Log(char *pszFile, char *fmt, ...)
{
	char *pfilename;
	char string[1024];

	if(!pszFile)
	{
#ifdef REHLDS_FIXES
		pfilename = "hllog.txt";
#else
		// Why so serious?
		pfilename = "c:\\hllog.txt";
#endif
	}
	else
	{
		pfilename = pszFile;
	}

	va_list argptr;
	va_start(argptr, fmt);
	Q_vsnprintf(string, ARRAYSIZE(string) - 1, fmt, argptr);
	va_end(argptr);

	string[ARRAYSIZE(string) - 1] = 0;

	FileHandle_t fp = FS_Open(pfilename, "a+t");

	if(fp)
	{
		FS_FPrintf(fp, "%s", string);
		FS_Close(fp);
	}
}







int COM_EntsForPlayerSlots(int nPlayers)
{
	int numedicts = gmodinfo.num_edicts;
	int p = COM_CheckParm("-num_edicts");

	if(p && p < com_argc - 1)
	{
		p = Q_atoi(com_argv[p + 1]);

		if(numedicts < p)
		{
			numedicts = p;
		}
	}

	return (numedicts + 15 * (nPlayers - 1));
}

// Anti-proxy/aimbot obfuscation code
// COM_UnMunge should reversably fixup the data
void COM_Munge(unsigned char *data, int len, int seq)
{
	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= ~seq;
		c = LongSwap(c);

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table[(i + j) & 0x0f]);
		}

		c ^= seq;
		*pc = c;
	}
}

void COM_UnMunge(unsigned char *data, int len, int seq)
{
	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= seq;

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table[(i + j) & 0x0f]);
		}

		c = LongSwap(c);
		c ^= ~seq;
		*pc = c;
	}
}

#ifdef REHLDS_FIXES
// unrolled version
void COM_Munge2(unsigned char *data, int len, int seq)
{
	unsigned int *pc;
	unsigned int *end;
	unsigned int mSeq;

	mSeq = bswap(~seq) ^ seq;
	len /= 4;
	end = (unsigned int *)data + (len & ~15);

	for(pc = (unsigned int *)data; pc < end; pc += 16)
	{
		pc[0] = bswap(pc[0]) ^ mSeq ^ 0xFFFFE7A5;
		pc[1] = bswap(pc[1]) ^ mSeq ^ 0xBFEFFFE5;
		pc[2] = bswap(pc[2]) ^ mSeq ^ 0xFFBFEFFF;
		pc[3] = bswap(pc[3]) ^ mSeq ^ 0xBFEFBFED;
		pc[4] = bswap(pc[4]) ^ mSeq ^ 0xBFAFEFBF;
		pc[5] = bswap(pc[5]) ^ mSeq ^ 0xFFBFAFEF;
		pc[6] = bswap(pc[6]) ^ mSeq ^ 0xFFEFBFAD;
		pc[7] = bswap(pc[7]) ^ mSeq ^ 0xFFFFEFBF;
		pc[8] = bswap(pc[8]) ^ mSeq ^ 0xFFEFF7EF;
		pc[9] = bswap(pc[9]) ^ mSeq ^ 0xBFEFE7F5;
		pc[10] = bswap(pc[10]) ^ mSeq ^ 0xBFBFE7E5;
		pc[11] = bswap(pc[11]) ^ mSeq ^ 0xFFAFB7E7;
		pc[12] = bswap(pc[12]) ^ mSeq ^ 0xBFFFAFB5;
		pc[13] = bswap(pc[13]) ^ mSeq ^ 0xBFAFFFAF;
		pc[14] = bswap(pc[14]) ^ mSeq ^ 0xFFAFA7FF;
		pc[15] = bswap(pc[15]) ^ mSeq ^ 0xFFEFA7A5;
	}

	switch(len & 15)
	{
	case 15:
		pc[14] = bswap(pc[14]) ^ mSeq ^ 0xFFAFA7FF;
	case 14:
		pc[13] = bswap(pc[13]) ^ mSeq ^ 0xBFAFFFAF;
	case 13:
		pc[12] = bswap(pc[12]) ^ mSeq ^ 0xBFFFAFB5;
	case 12:
		pc[11] = bswap(pc[11]) ^ mSeq ^ 0xFFAFB7E7;
	case 11:
		pc[10] = bswap(pc[10]) ^ mSeq ^ 0xBFBFE7E5;
	case 10:
		pc[9] = bswap(pc[9]) ^ mSeq ^ 0xBFEFE7F5;
	case 9:
		pc[8] = bswap(pc[8]) ^ mSeq ^ 0xFFEFF7EF;
	case 8:
		pc[7] = bswap(pc[7]) ^ mSeq ^ 0xFFFFEFBF;
	case 7:
		pc[6] = bswap(pc[6]) ^ mSeq ^ 0xFFEFBFAD;
	case 6:
		pc[5] = bswap(pc[5]) ^ mSeq ^ 0xFFBFAFEF;
	case 5:
		pc[4] = bswap(pc[4]) ^ mSeq ^ 0xBFAFEFBF;
	case 4:
		pc[3] = bswap(pc[3]) ^ mSeq ^ 0xBFEFBFED;
	case 3:
		pc[2] = bswap(pc[2]) ^ mSeq ^ 0xFFBFEFFF;
	case 2:
		pc[1] = bswap(pc[1]) ^ mSeq ^ 0xBFEFFFE5;
	case 1:
		pc[0] = bswap(pc[0]) ^ mSeq ^ 0xFFFFE7A5;
	}
}
#else  // REHLDS_FIXES

void COM_Munge2(unsigned char *data, int len, int seq)
{
	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= ~seq;
		c = LongSwap(c);

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table2[(i + j) & 0x0f]);
		}

		c ^= seq;
		*pc = c;
	}
}
#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
// unrolled version
void COM_UnMunge2(unsigned char *data, int len, int seq)
{
	unsigned int *pc;
	unsigned int *end;
	unsigned int mSeq;

	mSeq = bswap(~seq) ^ seq;
	len /= 4;
	end = (unsigned int *)data + (len & ~15);

	for(pc = (unsigned int *)data; pc < end; pc += 16)
	{
		pc[0] = bswap(pc[0] ^ mSeq ^ 0xFFFFE7A5);
		pc[1] = bswap(pc[1] ^ mSeq ^ 0xBFEFFFE5);
		pc[2] = bswap(pc[2] ^ mSeq ^ 0xFFBFEFFF);
		pc[3] = bswap(pc[3] ^ mSeq ^ 0xBFEFBFED);
		pc[4] = bswap(pc[4] ^ mSeq ^ 0xBFAFEFBF);
		pc[5] = bswap(pc[5] ^ mSeq ^ 0xFFBFAFEF);
		pc[6] = bswap(pc[6] ^ mSeq ^ 0xFFEFBFAD);
		pc[7] = bswap(pc[7] ^ mSeq ^ 0xFFFFEFBF);
		pc[8] = bswap(pc[8] ^ mSeq ^ 0xFFEFF7EF);
		pc[9] = bswap(pc[9] ^ mSeq ^ 0xBFEFE7F5);
		pc[10] = bswap(pc[10] ^ mSeq ^ 0xBFBFE7E5);
		pc[11] = bswap(pc[11] ^ mSeq ^ 0xFFAFB7E7);
		pc[12] = bswap(pc[12] ^ mSeq ^ 0xBFFFAFB5);
		pc[13] = bswap(pc[13] ^ mSeq ^ 0xBFAFFFAF);
		pc[14] = bswap(pc[14] ^ mSeq ^ 0xFFAFA7FF);
		pc[15] = bswap(pc[15] ^ mSeq ^ 0xFFEFA7A5);
	}

	switch(len & 15)
	{
	case 15:
		pc[14] = bswap(pc[14] ^ mSeq ^ 0xFFAFA7FF);
	case 14:
		pc[13] = bswap(pc[13] ^ mSeq ^ 0xBFAFFFAF);
	case 13:
		pc[12] = bswap(pc[12] ^ mSeq ^ 0xBFFFAFB5);
	case 12:
		pc[11] = bswap(pc[11] ^ mSeq ^ 0xFFAFB7E7);
	case 11:
		pc[10] = bswap(pc[10] ^ mSeq ^ 0xBFBFE7E5);
	case 10:
		pc[9] = bswap(pc[9] ^ mSeq ^ 0xBFEFE7F5);
	case 9:
		pc[8] = bswap(pc[8] ^ mSeq ^ 0xFFEFF7EF);
	case 8:
		pc[7] = bswap(pc[7] ^ mSeq ^ 0xFFFFEFBF);
	case 7:
		pc[6] = bswap(pc[6] ^ mSeq ^ 0xFFEFBFAD);
	case 6:
		pc[5] = bswap(pc[5] ^ mSeq ^ 0xFFBFAFEF);
	case 5:
		pc[4] = bswap(pc[4] ^ mSeq ^ 0xBFAFEFBF);
	case 4:
		pc[3] = bswap(pc[3] ^ mSeq ^ 0xBFEFBFED);
	case 3:
		pc[2] = bswap(pc[2] ^ mSeq ^ 0xFFBFEFFF);
	case 2:
		pc[1] = bswap(pc[1] ^ mSeq ^ 0xBFEFFFE5);
	case 1:
		pc[0] = bswap(pc[0] ^ mSeq ^ 0xFFFFE7A5);
	}
}
#else  // REHLDS_FIXES

void COM_UnMunge2(unsigned char *data, int len, int seq)
{
	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= seq;

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table2[(i + j) & 0x0f]);
		}

		c = LongSwap(c);
		c ^= ~seq;
		*pc = c;
	}
}
#endif // REHLDS_FIXES

void COM_Munge3(unsigned char *data, int len, int seq)
{
	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= ~seq;
		c = LongSwap(c);

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table3[(i + j) & 0x0f]);
		}

		c ^= seq;
		*pc = c;
	}
}

NOXREF void COM_UnMunge3(unsigned char *data, int len, int seq)
{
	NOXREFCHECK;

	int i;
	int mungelen;
	int c;
	int *pc;
	unsigned char *p;
	int j;

	mungelen = len & ~3;
	mungelen /= 4;

	for(i = 0; i < mungelen; i++)
	{
		pc = (int *)&data[i * 4];
		c = *pc;
		c ^= seq;

		p = (unsigned char *)&c;
		for(j = 0; j < 4; j++)
		{
			*p++ ^= (0xa5 | (j << j) | j | mungify_table3[(i + j) & 0x0f]);
		}

		c = LongSwap(c);
		c ^= ~seq;
		*pc = c;
	}
}



#endif // COM_Functions_region
