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

#include "precompiled.h"

void COM_ClearCustomizationList(customization_t *pHead, qboolean bCleanDecals)
{
	customization_s *pCurrent, *pNext;
	cachewad_t *     pWad;
	cachepic_t *     pic;

	pCurrent = pHead->pNext;
	if(!pCurrent)
		return;

	while(pCurrent)
	{
		pNext = pCurrent->pNext;

		if(pCurrent->bInUse)
		{
			if(pCurrent->pBuffer)
				Mem_Free(pCurrent->pBuffer);

			if(pCurrent->pInfo)
			{
				if(pCurrent->resource.type == t_decal)
				{
					if(bCleanDecals && cls.state == ca_active)
					{
						R_DecalRemoveAll(-1 - pCurrent->resource.playernum);
					}

					pWad = (cachewad_t *)pCurrent->pInfo;

					Mem_Free(pWad->lumps);

					for(int i = 0; i < pWad->cacheCount; i++)
					{
						pic = &pWad->cache[i];
						if(Cache_Check(&pic->cache))
							Cache_Free(&pic->cache);
					}

					Mem_Free(pWad->name);
					Mem_Free(pWad->cache);
				}

				Mem_Free(pCurrent->pInfo);
			}
		}

		Mem_Free(pCurrent);
		pCurrent = pNext;
	}

	pHead->pNext = NULL;
}

qboolean COM_CreateCustomization(customization_t *pListHead, resource_t *pResource, int playernumber, int flags, customization_t **pCustomization, int *nLumps)
{
	customization_t *pCust;  //    91
	qboolean         bError; //    92

	bError = FALSE;
	if(pCustomization)
		*pCustomization = NULL;
	pCust               = (customization_t *)Mem_ZeroMalloc(sizeof(customization_t));

	Q_memcpy(&pCust->resource, pResource, sizeof(pCust->resource));
	if(pResource->nDownloadSize <= 0)
	{
		bError = TRUE;
		goto CustomizationError;
	}

	pCust->bInUse = TRUE;

	if(flags & FCUST_FROMHPAK)
	{
		if(!HPAK_GetDataPointer("custom.hpk", pResource, (uint8 **)&pCust->pBuffer, NULL))
		{
			bError = TRUE;
			goto CustomizationError;
		}
	}
	else
	{
		pCust->pBuffer = COM_LoadFile(pResource->szFileName, 5, NULL);
	}

	if((pCust->resource.ucFlags & RES_CUSTOM) && pCust->resource.type == t_decal)
	{
		pCust->resource.playernum = playernumber;
		if(!CustomDecal_Validate(pCust->pBuffer, pResource->nDownloadSize))
		{
			bError = TRUE;
			goto CustomizationError;
		}

		if(!(flags & RES_CUSTOM))
		{
			cachewad_t *pWad = (cachewad_t *)Mem_ZeroMalloc(sizeof(cachewad_t));
			pCust->pInfo     = pWad;
			if(pResource->nDownloadSize >= 1024 && pResource->nDownloadSize <= 20480)
			{
				bError = (0 == CustomDecal_Init(pWad, pCust->pBuffer, pResource->nDownloadSize, playernumber));
				if(bError)
					goto CustomizationError;

				if(pWad->lumpCount > 0)
				{
					if(nLumps)
						*nLumps = pWad->lumpCount;

					pCust->bTranslated = TRUE;
					pCust->nUserData1  = 0;
					pCust->nUserData2  = pWad->lumpCount;
					if(flags & FCUST_WIPEDATA)
					{
						Mem_Free(pWad->name);
						Mem_Free(pWad->cache);
						Mem_Free(pWad->lumps);
						Mem_Free(pCust->pInfo);
						pCust->pInfo = NULL;
					}
				}
			}
		}
	}

CustomizationError:
	if(bError)
	{
		if(pCust->pBuffer)
			Mem_Free(pCust->pBuffer);
		if(pCust->pInfo)
			Mem_Free(pCust->pInfo);
		Mem_Free(pCust);
	}
	else
	{
		if(pCustomization)
			*pCustomization = pCust;
		pCust->pNext        = pListHead->pNext;
		pListHead->pNext    = pCust;
	}
	return bError == FALSE;
}

int COM_SizeofResourceList(resource_t *pList, resourceinfo_t *ri)
{
	resource_t *p;
	int         nSize;

	nSize = 0;
	Q_memset(ri, 0, sizeof(*ri));
	for(p = pList->pNext; p != pList; p = p->pNext)
	{
#ifdef REHLDS_FIXES
		//skip resources with invalid type
		if(p->type >= rt_max)
			continue;
#endif
		nSize += p->nDownloadSize;
		if(p->type != t_model || p->nIndex != 1)
		{
			if((unsigned int)p->type < sizeof(ri->info))
				ri->info[p->type].size += p->nDownloadSize;
		}
		else
		{
			ri->info[t_world].size += p->nDownloadSize;
		}
	}
	return nSize;
}
