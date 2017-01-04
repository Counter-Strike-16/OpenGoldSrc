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

#pragma once

//#include "precompiled.hpp"
#include "resources/modinfo.hpp"
#include "system/common.hpp"

void DLL_SetModKey(modinfo_t *pinfo, char *pkey, char *pvalue)
{
	if(!Q_stricmp(pkey, "url_info"))
	{
		pinfo->bIsMod = 1;
		Q_strncpy(pinfo->szInfo, pvalue, sizeof(pinfo->szInfo) - 1);
		pinfo->szInfo[sizeof(pinfo->szInfo) - 1] = 0;
	}
	else if(!Q_stricmp(pkey, "url_dl"))
	{
		pinfo->bIsMod = 1;
		Q_strncpy(pinfo->szDL, pvalue, sizeof(pinfo->szDL) - 1);
		pinfo->szDL[sizeof(pinfo->szDL) - 1] = 0;
	}
	else if(!Q_stricmp(pkey, "version"))
	{
		pinfo->bIsMod  = 1;
		pinfo->version = Q_atoi(pvalue);
	}
	else if(!Q_stricmp(pkey, "size"))
	{
		pinfo->bIsMod = 1;
		pinfo->size   = Q_atoi(pvalue);
	}
	else if(!Q_stricmp(pkey, "svonly"))
	{
		pinfo->bIsMod = 1;
		pinfo->svonly = Q_atoi(pvalue) != 0;
	}
	else if(!Q_stricmp(pkey, "cldll"))
	{
		pinfo->bIsMod = 1;
		pinfo->cldll  = Q_atoi(pvalue) != 0;
	}
	else if(!Q_stricmp(pkey, "secure"))
	{
		pinfo->bIsMod = 1;
		pinfo->secure = Q_atoi(pvalue) != 0;
	}
	else if(!Q_stricmp(pkey, "hlversion"))
	{
		Q_strncpy(pinfo->szHLVersion, pvalue, sizeof(pinfo->szHLVersion) - 1);
		pinfo->szHLVersion[sizeof(pinfo->szHLVersion) - 1] = 0;
	}
	else if(!Q_stricmp(pkey, "edicts"))
	{
		pinfo->num_edicts = Q_atoi(pvalue);
		if(pinfo->num_edicts < NUM_EDICTS)
			pinfo->num_edicts = NUM_EDICTS;
	}
	else if(!Q_stricmp(pkey, "crcclientdll"))
	{
		pinfo->bIsMod       = 1;
		pinfo->clientDllCRC = Q_atoi(pvalue) != 0;
	}
	else if(!Q_stricmp(pkey, "type"))
	{
		if(!Q_stricmp(pvalue, "singleplayer_only"))
			pinfo->type = SINGLEPLAYER_ONLY;
		else if(!Q_stricmp(pvalue, "multiplayer_only"))
			pinfo->type = MULTIPLAYER_ONLY;
		else
			pinfo->type = BOTH;
	}
	else if(!Q_stricmp(pkey, "fallback_dir"))
		COM_AddDefaultDir(pvalue);
};