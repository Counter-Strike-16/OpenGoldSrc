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

#include "maintypes.h"
#include "common/commontypes.h"
#include "engine/custom.h"
#include "public/FileSystem.h"

const char HASHPAK_EXTENSION[] = ".hpk";

const int HASHPAK_VERSION = 0x0001;

const int MAX_FILE_SIZE = 0x20000;
const int MAX_FILE_ENTRIES = 0x8000;

typedef struct hash_pack_queue_s
{
	char *                    pakname;
	resource_t                resource;
	int                       datasize;
	void *                    data;
	struct hash_pack_queue_s *next;
} hash_pack_queue_t;

typedef struct hash_pack_entry_s
{
	resource_t resource;
	int        nOffset;
	int        nFileLength;
} hash_pack_entry_t;

typedef struct hash_pack_directory_s
{
	int                nEntries;
	hash_pack_entry_t *p_rgEntries;
} hash_pack_directory_t;

typedef struct hash_pack_header_s
{
	char szFileStamp[4];
	int  version;
	int  nDirectoryOffset;
} hash_pack_header_t;

#ifdef HOOK_ENGINE
	#define gp_hpak_queue (*pgp_hpak_queue)
	#define hash_pack_dir (*phash_pack_dir)
	#define hash_pack_header (*phash_pack_header)
#endif // HOOK_ENGINE

extern hash_pack_queue_t *gp_hpak_queue;

// TODO: used only in hashpak
extern hash_pack_directory_t hash_pack_dir;
extern hash_pack_header_t    hash_pack_header;

qboolean HPAK_GetDataPointer(char *pakname, struct resource_s *pResource, unsigned char **pbuffer, int *bufsize);
qboolean HPAK_FindResource(hash_pack_directory_t *pDir, unsigned char *hash, struct resource_s *pResourceEntry);
void HPAK_AddToQueue(char *pakname, struct resource_s *pResource, void *pData, FileHandle_t fpSource);
void HPAK_FlushHostQueue();
void HPAK_AddLump(qboolean bUseQueue, char *pakname, struct resource_s *pResource, void *pData, FileHandle_t fpSource);
void HPAK_RemoveLump(char *pakname, struct resource_s *pResource);
qboolean HPAK_ResourceForIndex(char *pakname, int nIndex, struct resource_s *pResource);
qboolean HPAK_ResourceForHash(char *pakname, unsigned char *hash, struct resource_s *pResourceEntry);
void HPAK_List_f();
void HPAK_CreatePak(char *pakname, struct resource_s *pResource, void *pData, FileHandle_t fpSource);
void         HPAK_Remove_f();
void         HPAK_Validate_f();
void         HPAK_Extract_f();
void         HPAK_Init();
NOXREF char *HPAK_GetItem(int item);
void HPAK_CheckSize(char *pakname);
void HPAK_ValidatePak(char *fullpakname);
void HPAK_CheckIntegrity(char *pakname);