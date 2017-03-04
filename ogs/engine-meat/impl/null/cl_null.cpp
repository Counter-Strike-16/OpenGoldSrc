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
/// @brief this file can stub out the entire client system for pure dedicated servers

#include "precompiled.hpp"
#include "client/client.hpp"

void CL_RecordHUDCommand(char *cmdname)
{
};

qboolean CL_CheckFile(sizebuf_t *msg, char *filename)
{
	return 1;
};

void EXT_FUNC CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel)
{
};

void CL_PrintLogos()
{
};

qboolean CL_RequestMissingResources()
{
	return 0;
};

void CL_Move()
{
};

void CL_HudMessage(const char *pMessage)
{
};

void Chase_Init()
{
};

int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf)
{
	return 0;
};



void CL_SetLastUpdate()
{
}

void CL_WriteMessageHistory(int starting_count, int cmd)
{
};

void CL_MoveSpectatorCamera()
{
};



void CL_UpdateModuleC()
{
}




void SetCareerAudioState(int state)
{
};