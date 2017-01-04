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
#include "public/FileSystem.h"
#include "rehlds/model.h"

const int MAX_DECALS = 512;

#define DECAL_EXTRASIZE sizeof(texture_t) - sizeof(miptex_t)

typedef struct lumpinfo_s lumpinfo_t;
typedef struct texture_s  texture_t;

typedef struct decalname_s
{
	char name[16];
	byte ucFlags;
} decalname_t;

typedef struct lumplist_s
{
	lumpinfo_t *lump;
	qboolean    breplaced;
	lumplist_s *next;
} lumplist_t;

#ifdef HOOK_ENGINE
#define decal_wad (*pdecal_wad)
#define menu_wad (*pmenu_wad)
#define szCustName (*pszCustName)
#define decal_names (*pdecal_names)
#define m_bDrawInitialized (*pm_bDrawInitialized)
#define gfCustomBuild (*pgfCustomBuild)
#endif // HOOK_ENGINE

extern cachewad_t *decal_wad;
extern cachewad_t *menu_wad;

extern char        szCustName[10];
extern char        decal_names[MAX_DECALS][16];

extern qboolean    m_bDrawInitialized;
extern qboolean    gfCustomBuild;

void Draw_Shutdown();
void Draw_DecalShutdown();
void Draw_AllocateCacheSpace(cachewad_t *wad, int cacheMax, int tempWad);
void Draw_CacheWadInitFromFile(FileHandle_t hFile, int len, char *name, int cacheMax, cachewad_t *wad);
void Draw_CacheWadInit(char *name, int cacheMax, cachewad_t *wad);
qboolean Draw_CustomCacheWadInit(int cacheMax, cachewad_t *wad, void *raw, int nFileSize);
void Draw_MiptexTexture(cachewad_t *wad, unsigned char *data);
void Draw_CacheWadHandler(cachewad_t *wad, PFNCACHE fn, int extraDataSize);
void Draw_FreeWad(cachewad_t *pWad);
NOXREF void Draw_DecalSetName(int decal, char *name);
NOXREF int Draw_DecalIndex(int id);
int Draw_DecalCount();
int Draw_DecalSize(int number);
const char *Draw_DecalName(int number);
NOXREF texture_t *Draw_DecalTexture(int index);
int Draw_CacheByIndex(cachewad_t *wad, int nIndex, int playernum);
NOXREF int Draw_DecalIndexFromName(char *name);

void Decal_ReplaceOrAppendLump(lumplist_t **ppList, lumpinfo_t *lump, qboolean bsecondlump);
int Decal_CountLumps(lumplist_t *plist);
int Decal_SizeLumps(lumplist_t *plist);
void Decal_MergeInDecals(cachewad_t *pwad, const char *pathID);
void Decal_Init();

qboolean CustomDecal_Validate(void *raw, int nFileSize);
qboolean CustomDecal_Init(struct cachewad_s *wad, void *raw, int nFileSize, int playernum);

NOXREF void *Draw_CacheGet(cachewad_t *wad, int index);
void *Draw_CustomCacheGet(cachewad_t *wad, void *raw, int rawsize, int index);
NOXREF qboolean Draw_CacheReload(cachewad_t *wad, int i, lumpinfo_t *pLump, cachepic_t *pic, char *clean, char *path);
qboolean Draw_ValidateCustomLogo(cachewad_t *wad, unsigned char *data, lumpinfo_t *lump);
qboolean Draw_CacheLoadFromCustom(char *clean, cachewad_t *wad, void *raw, int rawsize, cachepic_t *pic);
NOXREF int Draw_CacheIndex(cachewad_t *wad, char *path);
NOXREF int Draw_CacheFindIndex(cachewad_t *wad, char *path);