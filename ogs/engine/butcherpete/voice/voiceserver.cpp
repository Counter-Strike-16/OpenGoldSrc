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

//#include "precompiled.hpp"
#include "maintypes.h"
#include "server/server.hpp"

qboolean EXT_FUNC Voice_GetClientListening(int iReceiver, int iSender)
{
	--iReceiver;
	--iSender;

	if(iReceiver < 0 || iSender < 0 || iReceiver >= g_psvs.maxclients ||
	   iSender >= g_psvs.maxclients)
		return 0;

#ifdef REHLDS_FIXES
	return (g_psvs.clients[iSender].m_VoiceStreams[iReceiver >> 5] &
	        (1 << iReceiver)) != 0;
#else  // REHLDS_FIXES
	return (1 << iReceiver) &
	(g_psvs.clients[iSender].m_VoiceStreams[iReceiver >> 5] != 0);
#endif // REHLDS_FIXES
}

qboolean EXT_FUNC Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
	--iReceiver;
	--iSender;

	if(iReceiver < 0 || iSender < 0 || iReceiver >= g_psvs.maxclients ||
	   iSender >= g_psvs.maxclients)
		return 0;

	uint32 *pDest = g_psvs.clients[iSender].m_VoiceStreams;

	if(bListen)
		pDest[iReceiver >> 5] |= 1 << iReceiver;
	else
		pDest[iReceiver >> 5] &= ~(1 << iReceiver);

	return 1;
};