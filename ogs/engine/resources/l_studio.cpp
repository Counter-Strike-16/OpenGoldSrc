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

//#include "precompiled.hpp"
#include "maintypes.h"
#include "resources/l_studio.hpp"
#include "system/common.hpp"

//int giTextureSize;

void EXT_FUNC Mod_LoadStudioModel_internal(model_t *mod, void *buffer)
{
	uint8 *           poutdata;
	uint8 *           pindata;
	mstudiotexture_t *ptexture;
	int               size;
	int               i;
	uint8 *           pout;

	studiohdr_t *phdr = (studiohdr_t *)buffer;
	i                 = LittleLong(phdr->version);
	if(i != STUDIO_VERSION)
	{
		Q_memset(phdr, 0, 244u);
		Q_strcpy(phdr->name, "bogus");
		phdr->length           = 244;
		phdr->texturedataindex = 244;
	}

	mod->type  = mod_studio;
	mod->flags = phdr->flags;
	Cache_Alloc(&mod->cache, phdr->length + 1280 * phdr->numtextures, mod->name);
	pout = (uint8 *)mod->cache.data;
	if(pout)
	{
		if(phdr->textureindex)
		{
			Q_memcpy(pout, buffer, phdr->texturedataindex);
			poutdata = pout + phdr->texturedataindex;
			pindata  = (uint8 *)buffer + phdr->texturedataindex;
			ptexture = (mstudiotexture_t *)(pout + phdr->textureindex);
			for(i = 0; i < phdr->numtextures; i++, ptexture++)
			{
				size            = ptexture->height * ptexture->width;
				ptexture->index = poutdata - pout;
				Q_memcpy(poutdata, pindata, size);
				poutdata += size;
				pindata += size;

				for(int j = 0; j < 256; j++, pindata += 3, poutdata += 8)
				{
					((uint16 *)poutdata)[0] = texgammatable[pindata[0]];
					((uint16 *)poutdata)[1] = texgammatable[pindata[1]];
					((uint16 *)poutdata)[2] = texgammatable[pindata[2]];
					((uint16 *)poutdata)[3] = 0;
				}
			}
		}
		else
		{
			Q_memcpy(pout, buffer, phdr->length);
		}
	}
}

void Mod_LoadStudioModel(model_t *mod, void *buffer)
{
	g_RehldsHookchains.m_Mod_LoadStudioModel.callChain(&Mod_LoadStudioModel_internal, mod, buffer);
}
