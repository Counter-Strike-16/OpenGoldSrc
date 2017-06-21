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

#include "common/commontypes.h"
//#include "engine/custom.h"
//#include "server/server.hpp"

typedef struct client_s client_t;
typedef struct sizebuf_s sizebuf_t;
typedef struct resource_s resource_t;

qboolean SV_CheckFile(sizebuf_t *msg, char *filename);
void SV_ClearResourceLists(client_t *cl);
void SV_CreateCustomizationList(client_t *pHost);
void SV_Customization(client_t *pPlayer, resource_t *pResource, qboolean bSkipPlayer);
void SV_RegisterResources();
void SV_MoveToOnHandList(resource_t *pResource);
void SV_AddToResourceList(resource_t *pResource, resource_t *pList);
void SV_ClearResourceList(resource_t *pList);
void SV_RemoveFromResourceList(resource_t *pResource);
int SV_EstimateNeededResources();
qboolean SV_UploadComplete(client_t *cl);
void SV_BatchUploadRequest(client_t *cl);

qboolean SV_RequestMissingResources();
void SV_RequestMissingResourcesFromClients();

void SV_ParseResourceList(client_t *pSenderClient);