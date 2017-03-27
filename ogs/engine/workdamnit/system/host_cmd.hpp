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

#include "system/client.hpp"
#include "common/commontypes.h"
#include "engine/eiface.h"
#include "maintypes.h"
#include "public/FileSystem.h"
#include "public/archtypes.h"

#define FILETIME_TO_QWORD(ft) \
	((((uint64)ft.dwHighDateTime) << 32) + ft.dwLowDateTime)

#define FILETIME_TO_PAIR(f, h) (((uint64)f << 32) | h)

typedef void (*SV_SAVEGAMECOMMENT_FUNC)(char *, int);

typedef struct GAME_HEADER_s
{
	char mapName[32];
	char comment[80];
	int mapCount;
} GAME_HEADER;

typedef struct SAVE_HEADER_s
{
	int saveId;
	int version;
	int skillLevel;
	int entityCount;
	int connectionCount;
	int lightStyleCount;
	float time;
	char mapName[32];
	char skyName[32];
	int skyColor_r;
	int skyColor_g;
	int skyColor_b;
	float skyVec_x;
	float skyVec_y;
	float skyVec_z;
} SAVE_HEADER;

typedef struct SAVELIGHTSTYLE_s
{
	int index;
	char style[64];
} SAVELIGHTSTYLE;

typedef struct TITLECOMMENT_s
{
	char *pBSPName;
	char *pTitleName;
} TITLECOMMENT;

#ifdef HOOK_ENGINE

#define r_dointerp (*pr_dointerp)
#define r_origin (*pr_origin)
#define cpuPercent (*pcpuPercent)
#define startTime (*pstartTime)
#define current_skill (*pcurrent_skill)
#define gHostSpawnCount (*pgHostSpawnCount)
#define g_careerState (*pg_careerState)
#define g_pSaveGameCommentFunc (*pg_pSaveGameCommentFunc)
#define g_bMajorMapChange (*pg_bMajorMapChange)
#define voice_recordtofile (*pvoice_recordtofile)
#define voice_inputfromfile (*pvoice_inputfromfile)
#define gTitleComments (*pgTitleComments)
#define gGameHeaderDescription (*pgGameHeaderDescription)
#define gSaveHeaderDescription (*pgSaveHeaderDescription)
#define gAdjacencyDescription (*pgAdjacencyDescription)
#define gEntityTableDescription (*pgEntityTableDescription)
#define gLightstyleDescription (*pgLightstyleDescription)
#define gHostMap (*pgHostMap)
#define g_iQuitCommandIssued (*pg_iQuitCommandIssued)
#define g_pPostRestartCmdLineArgs pg_pPostRestartCmdLineArgs

#endif // HOOK_ENGINE

extern int r_dointerp;
extern vec3_t r_origin;
extern double cpuPercent;

extern int current_skill;
extern int gHostSpawnCount;
extern CareerStateType g_careerState;
extern SV_SAVEGAMECOMMENT_FUNC g_pSaveGameCommentFunc;
extern qboolean g_bMajorMapChange;
extern cvar_t voice_recordtofile;
extern cvar_t voice_inputfromfile;
extern TITLECOMMENT gTitleComments[66];
extern TYPEDESCRIPTION gGameHeaderDescription[3];
extern TYPEDESCRIPTION gSaveHeaderDescription[13];
extern TYPEDESCRIPTION gAdjacencyDescription[4];
extern TYPEDESCRIPTION gEntityTableDescription[5];
extern TYPEDESCRIPTION gLightstyleDescription[2];
extern cvar_t gHostMap;
extern int g_iQuitCommandIssued;
extern char *g_pPostRestartCmdLineArgs;

void SV_GetPlayerHulls();

void SV_CheckBlendingInterface();
void SV_CheckSaveGameCommentInterface();