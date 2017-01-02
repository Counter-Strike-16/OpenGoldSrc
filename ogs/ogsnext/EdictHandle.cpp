/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#include "world/EdictHandle.h"

/*
=================
ED_ClearEdict

Sets everything to NULL
=================
*/
void CEdictHandle::Clear()
{
	Q_memset(&mpHandlingEdict->v, 0, sizeof(mpHandlingEdict->v));
	mpHandlingEdict->free = FALSE;
	ReleaseEntityDLLFields(mpHandlingEdict);
	InitEntityDLLFields(mpHandlingEdict);
};

entvars_t *CEdictHandle::GetEntVars()
{
	if(mpHandlingEdict)
		return &mpHandlingEdict->v;

	return NULL;
};

void *CEdictHandle::GetModelPtr()
{
	if(!mpHandlingEdict)
		return NULL;

	return Mod_Extradata(g_psv.models[mpHandlingEdict->v.modelindex]);
}

void *CEdictHandle::GetPrivateData()
{
	if(!mpHandlingEdict)
		return NULL;

	return mpHandlingEdict->pvPrivateData;
};

void CEdictHandle::FreePrivateData()
{
	if(mpHandlingEdict->pvPrivateData)
	{
		if(gNewDLLFunctions.pfnOnFreeEntPrivateData)
			gNewDLLFunctions.pfnOnFreeEntPrivateData(mpHandlingEdict);

#ifdef REHLDS_FLIGHT_REC
		if(rehlds_flrec_pvdata.string[0] != '0')
			FR_FreeEntPrivateData(mpHandlingEdict->pvPrivateData);
#endif //REHLDS_FLIGHT_REC

		Mem_Free(mpHandlingEdict->pvPrivateData);
		mpHandlingEdict->pvPrivateData = 0;
	};
};