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
#include "rehlds/model.h"

// Looks like no more than 8096 visibility leafs per world model
#define MODEL_MAX_PVS 1024

#ifdef HOOK_ENGINE
#define gPAS (*pgPAS)
#define gPVS (*pgPVS)
#define gPVSRowBytes (*pgPVSRowBytes)
#define mod_novis (*pmod_novis)
#endif // HOOK_ENGINE

extern unsigned char *gPAS;
extern unsigned char *gPVS;
extern int gPVSRowBytes;
extern unsigned char mod_novis[MODEL_MAX_PVS];

void Mod_Init();
unsigned char *Mod_DecompressVis(unsigned char *in, model_t *model);
unsigned char *Mod_LeafPVS(mleaf_t *leaf, model_t *model);
void CM_DecompressPVS(unsigned char *in, unsigned char *decompressed, int byteCount);
unsigned char *CM_LeafPVS(int leafnum);
unsigned char *CM_LeafPAS(int leafnum);
void CM_FreePAS();
void CM_CalcPAS(model_t *pModel);
qboolean CM_HeadnodeVisible(mnode_t *node, unsigned char *visbits, int *first_visible_leafnum);