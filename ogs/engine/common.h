#pragma once
/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/
#include "common_rehlds.h"
#include "bspfile.h"
#include "FileSystem.h"
#include "quakedef.h"
#include "usercmd.h"
#include "info.h"
#include "com_model.h"

#ifndef REHLDS_FIXES
#ifdef _WIN32
	#define __BUILD_TIME__ "11:17:23"
	#define __BUILD_DATE__ "Aug  8 2013"
#else
	#define __BUILD_TIME__ "13:14:09"
	#define __BUILD_DATE__ "Aug 29 2013"
#endif
#else // REHLDS_FIXES
	#define __BUILD_TIME__ APP_COMMIT_TIME
	#define __BUILD_DATE__ APP_COMMIT_DATE
#endif // REHLDS_FIXES

#ifdef HOOK_ENGINE
#define serverinfo (*pserverinfo)

#define gpszVersionString (*pgpszVersionString)
#define gpszProductString (*pgpszProductString)

#define bfread (*pbfread)
#define bfwrite (*pbfwrite)

#define msg_badread (*pmsg_badread)
#define msg_readcount (*pmsg_readcount)

#define bigendien (*pbigendien)

#define BigShort (*pBigShort)
#define LittleShort (*pLittleShort)
#define BigLong (*pBigLong)
#define LittleLong (*pLittleLong)
#define BigFloat (*pBigFloat)
#define LittleFloat (*pLittleFloat)

#define com_argc (*pcom_argc)
#define com_argv (*pcom_argv)
#define com_token (*pcom_token)
#define com_ignorecolons (*pcom_ignorecolons)
#define s_com_token_unget (*ps_com_token_unget)
#define com_clientfallback (*pcom_clientfallback)
#define com_gamedir (*pcom_gamedir)
#define com_cmdline (*pcom_cmdline)

#define loadcache (*ploadcache)
#define loadbuf (*ploadbuf)
#define loadsize (*ploadsize)
#endif // HOOK_ENGINE


extern char serverinfo[MAX_INFO_STRING];

extern char gpszVersionString[32];
extern char gpszProductString[32];

typedef struct bf_read_s bf_read_t;
typedef struct bf_write_s bf_write_t;

extern bf_read_t bfread;
extern bf_write_t bfwrite;

extern int msg_badread;
extern int msg_readcount;

extern qboolean bigendien;

extern short (*BigShort)(short l);
extern short (*LittleShort)(short l);
extern int (*BigLong)(int l);
extern int (*LittleLong)(int l);
extern float (*BigFloat)(float l);
extern float (*LittleFloat)(float l);

extern int com_argc;
extern char **com_argv;

extern char com_token[COM_TOKEN_LEN];

extern qboolean com_ignorecolons;
extern qboolean s_com_token_unget;
extern char com_clientfallback[MAX_PATH];
extern char com_gamedir[MAX_PATH];
extern char com_cmdline[COM_MAX_CMD_LINE];

extern cache_user_t *loadcache;
extern unsigned char *loadbuf;
extern int loadsize;


//#define Q_functions
#ifndef Q_functions

#ifndef _WIN32
#define _strlwr(p) for (int i = 0; p[i] != 0; i++) p[i] = tolower(p[i]);
#endif

#if defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)
#define Q_memset A_memset
#define Q_memcpy A_memcpy
#define Q_memmove A_memmove
#define Q_strlen A_strlen
#define Q_memcmp A_memcmp
#define Q_strcpy A_strcpy
#define Q_strncpy strncpy
#define Q_strrchr strrchr
#define Q_strcat A_strcat
#define Q_strncat strncat
#define Q_strcmp A_strcmp
#define Q_strncmp strncmp
//#define Q_strcasecmp _stricmp		// Use Q_stricmp
//#define Q_strncasecmp _strnicmp	// Use Q_strnicmp
#define Q_stricmp A_stricmp
#define Q_strnicmp _strnicmp
#define Q_strstr A_strstr
#define Q_strchr strchr
#define Q_strlwr A_strtolower
#define Q_sprintf sprintf
#define Q_snprintf _snprintf
#define Q_atoi atoi
#define Q_atof atof
#define Q_sqrt M_sqrt
//#define Q_strtoull strtoull
//#define Q_FileNameCmp FileNameCmp
#define Q_vsnprintf _vsnprintf
#else
#define Q_memset memset
#define Q_memcpy memcpy
#define Q_memmove memmove
#define Q_strlen strlen
#define Q_memcmp memcmp
#define Q_strcpy strcpy
#define Q_strncpy strncpy
#define Q_strrchr strrchr
#define Q_strcat strcat
#define Q_strncat strncat
#define Q_strcmp strcmp
#define Q_strncmp strncmp
//#define Q_strcasecmp _stricmp		// Use Q_stricmp
//#define Q_strncasecmp _strnicmp	// Use Q_strnicmp
#define Q_stricmp _stricmp
#define Q_strnicmp _strnicmp
#define Q_strstr strstr
#define Q_strchr strchr
#define Q_strlwr _strlwr
#define Q_sprintf sprintf
#define Q_snprintf _snprintf
#define Q_atoi atoi
#define Q_atof atof
#define Q_sqrt sqrt
//#define Q_strtoull strtoull
//#define Q_FileNameCmp FileNameCmp
#define Q_vsnprintf _vsnprintf
#endif // defined(REHLDS_OPT_PEDANTIC) || defined(REHLDS_FIXES)

#else // Q_functions

void Q_strcpy(char *dest, const char *src);
int Q_strlen(const char *str);
NOBODY void Q_memset(void *dest, int fill, int count);
NOBODY void Q_memcpy(void *dest, const void *src, int count);
NOBODY int Q_memcmp(void *m1, void *m2, int count);
NOBODY void Q_strncpy(char *dest, const char *src, int count);
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

//strcpy that works correctly with overlapping src and dst buffers
char* strcpy_safe(char* dst, char* src);

int build_number(void);
char *Info_Serverinfo(void);

unsigned char COM_Nibble(char c);
void COM_HexConvert(const char *pszInput, int nInputLength, unsigned char *pOutput);
NOXREF char *COM_BinPrintf(unsigned char *buf, int nLen);
void COM_ExplainDisconnection(qboolean bPrint, char *fmt, ...);
NOXREF void COM_ExtendedExplainDisconnection(qboolean bPrint, char *fmt, ...);

int LongSwap(int l);
short ShortSwap(short l);
short ShortNoSwap(short l);
int LongNoSwap(int l);
float FloatSwap(float f);
float FloatNoSwap(float f);

void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteByte(sizebuf_t *sb, int c);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteWord(sizebuf_t *sb, int c);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void MSG_WriteBuf(sizebuf_t *sb, int iSize, void *buf);
void MSG_WriteAngle(sizebuf_t *sb, float f);
void MSG_WriteHiresAngle(sizebuf_t *sb, float f);
void MSG_WriteUsercmd(sizebuf_t *buf, usercmd_t *to, usercmd_t *from);
void COM_BitOpsInit(void);
void MSG_WriteOneBit(int nValue);
void MSG_StartBitWriting(sizebuf_t *buf);
NOXREF qboolean MSG_IsBitWriting(void);
void MSG_EndBitWriting(sizebuf_t *buf);
void MSG_WriteBits(uint32 data, int numbits);
void MSG_WriteSBits(int data, int numbits);
void MSG_WriteBitString(const char *p);
void MSG_WriteBitData(void *src, int length);
void MSG_WriteBitAngle(float fAngle, int numbits);
float MSG_ReadBitAngle(int numbits);
int MSG_CurrentBit(void);
NOXREF qboolean MSG_IsBitReading(void);
void MSG_StartBitReading(sizebuf_t *buf);
void MSG_EndBitReading(sizebuf_t *buf);
int MSG_ReadOneBit(void);
uint32 MSG_ReadBits(int numbits);
NOXREF uint32 MSG_PeekBits(int numbits);
int MSG_ReadSBits(int numbits);
NOXREF char *MSG_ReadBitString(void);
int MSG_ReadBitData(void *dest, int length);
NOXREF float MSG_ReadBitCoord(void);
void MSG_WriteBitCoord(const float f);
NOXREF void MSG_ReadBitVec3Coord(vec3_t fa);
void MSG_WriteBitVec3Coord(const vec3_t fa);
NOXREF float MSG_ReadCoord(void);
void MSG_WriteCoord(sizebuf_t *sb, const float f);
NOXREF void MSG_ReadVec3Coord(sizebuf_t *sb, vec3_t fa);
NOXREF void MSG_WriteVec3Coord(sizebuf_t *sb, const vec3_t fa);
void MSG_BeginReading(void);
int MSG_ReadChar(void);
int MSG_ReadByte(void);
int MSG_ReadShort(void);
NOXREF int MSG_ReadWord(void);
int MSG_ReadLong(void);
NOXREF float MSG_ReadFloat(void);
int MSG_ReadBuf(int iSize, void *pbuf);
char *MSG_ReadString(void);
char *MSG_ReadStringLine(void);
NOXREF float MSG_ReadAngle(void);
NOXREF float MSG_ReadHiresAngle(void);
void MSG_ReadUsercmd(usercmd_t *to, usercmd_t *from);

void SZ_Alloc(const char *name, sizebuf_t *buf, int startsize);
void SZ_Clear(sizebuf_t *buf);
void *SZ_GetSpace(sizebuf_t *buf, int length);
void SZ_Write(sizebuf_t *buf, const void *data, int length);
void SZ_Print(sizebuf_t *buf, const char *data);

NOXREF char *COM_SkipPath(char *pathname);
void COM_StripExtension(char *in, char *out);
char *COM_FileExtension(char *in);
void COM_FileBase(const char *in, char *out);
void COM_DefaultExtension(char *path, char *extension);
void COM_UngetToken(void);
char *COM_Parse(char *data);
char *COM_ParseLine(char *data);
int COM_TokenWaiting(char *buffer);
int COM_CheckParm(char *parm);
void COM_InitArgv(int argc, char *argv[]);
void COM_Init(char *basedir);
char *va(char *format, ...);
char *vstr(vec_t *v);
NOXREF int memsearch(unsigned char *start, int count, int search);
NOXREF void COM_WriteFile(char *filename, void *data, int len);
void COM_FixSlashes(char *pname);
void COM_CreatePath(char *path);
NOXREF void COM_CopyFile(char *netpath, char *cachepath);
NOXREF int COM_ExpandFilename(char *filename);
int COM_FileSize(char *filename);
unsigned char *COM_LoadFile(const char *path, int usehunk, int *pLength);
void COM_FreeFile(void *buffer);
void COM_CopyFileChunk(FileHandle_t dst, FileHandle_t src, int nSize);
NOXREF unsigned char *COM_LoadFileLimit(char *path, int pos, int cbmax, int *pcbread, FileHandle_t *phFile);
unsigned char *COM_LoadHunkFile(char *path);
unsigned char *COM_LoadTempFile(char *path, int *pLength);
void COM_LoadCacheFile(char *path, struct cache_user_s *cu);
NOXREF unsigned char *COM_LoadStackFile(char *path, void *buffer, int bufsize, int *length);
void COM_Shutdown(void);
NOXREF void COM_AddAppDirectory(char *pszBaseDir, const char *appName);
void COM_AddDefaultDir(char *pszDir);
void COM_StripTrailingSlash(char *ppath);
void COM_ParseDirectoryFromCmd(const char *pCmdName, char *pDirName, const char *pDefault);
qboolean COM_SetupDirectories(void);
void COM_CheckPrintMap(dheader_t *header, const char *mapname, qboolean bShowOutdated);
void COM_ListMaps(char *pszSubString);
void COM_Log(char *pszFile, char *fmt, ...);
unsigned char *COM_LoadFileForMe(char *filename, int *pLength);
int COM_CompareFileTime(char *filename1, char *filename2, int *iCompare);
void COM_GetGameDir(char *szGameDir);
int COM_EntsForPlayerSlots(int nPlayers);
void COM_NormalizeAngles(vec_t *angles);
void COM_Munge(unsigned char *data, int len, int seq);
void COM_UnMunge(unsigned char *data, int len, int seq);
void COM_Munge2(unsigned char *data, int len, int seq);
void COM_UnMunge2(unsigned char *data, int len, int seq);
void COM_Munge3(unsigned char *data, int len, int seq);
NOXREF void COM_UnMunge3(unsigned char *data, int len, int seq);
unsigned int COM_GetApproxWavePlayLength(const char *filepath);
