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
#include "graphics/sprite.hpp"

/*
====================
SPR_AdjustSize

draw hudsprite routine
====================
*/
static void SPR_AdjustSize(float *x, float *y, float *w, float *h)
{
	if(!x && !y && !w && !h)
		return;

	// scale for screen sizes
	float xscale = scr_width->integer / (float)clgame.scrInfo.iWidth;
	float yscale = scr_height->integer / (float)clgame.scrInfo.iHeight;

	if(x)
		*x *= xscale;
	if(y)
		*y *= yscale;
	if(w)
		*w *= xscale;
	if(h)
		*h *= yscale;
}

static qboolean SPR_Scissor(float *x, float *y, float *width, float *height, float *u0, float *v0, float *u1, float *v1)
{
	float dudx, dvdy;

	// clip sub rect to sprite
	if((width == 0) || (height == 0))
		return false;

	if(*x + *width <= clgame.ds.scissor_x)
		return false;
	if(*x >= clgame.ds.scissor_x + clgame.ds.scissor_width)
		return false;
	if(*y + *height <= clgame.ds.scissor_y)
		return false;
	if(*y >= clgame.ds.scissor_y + clgame.ds.scissor_height)
		return false;

	dudx = (*u1 - *u0) / *width;
	dvdy = (*v1 - *v0) / *height;

	if(*x < clgame.ds.scissor_x)
	{
		*u0 += (clgame.ds.scissor_x - *x) * dudx;
		*width -= clgame.ds.scissor_x - *x;
		*x = clgame.ds.scissor_x;
	}

	if(*x + *width > clgame.ds.scissor_x + clgame.ds.scissor_width)
	{
		*u1 -=
		(*x + *width - (clgame.ds.scissor_x + clgame.ds.scissor_width)) * dudx;
		*width = clgame.ds.scissor_x + clgame.ds.scissor_width - *x;
	}

	if(*y < clgame.ds.scissor_y)
	{
		*v0 += (clgame.ds.scissor_y - *y) * dvdy;
		*height -= clgame.ds.scissor_y - *y;
		*y = clgame.ds.scissor_y;
	}

	if(*y + *height > clgame.ds.scissor_y + clgame.ds.scissor_height)
	{
		*v1 -= (*y + *height - (clgame.ds.scissor_y + clgame.ds.scissor_height)) *
		dvdy;
		*height = clgame.ds.scissor_y + clgame.ds.scissor_height - *y;
	}

	return true;
}

HSPRITE SPR_LoadExt(const char *szPicName, uint texFlags)
{
	char name[64];
	int i;

	if(!szPicName || !*szPicName)
	{
		MsgDev(D_ERROR, "CL_LoadSprite: bad name!\n");
		return 0;
	}

	Q_strncpy(name, szPicName, sizeof(name));
	COM_FixSlashes(name);

	// slot 0 isn't used
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!Q_stricmp(clgame.sprites[i].name, name))
		{
			// prolonge registration
			clgame.sprites[i].needload = clgame.load_sequence;
			return i;
		}
	}

	// find a free model slot spot
	for(i = 1; i < MAX_IMAGES; i++)
	{
		if(!clgame.sprites[i].name[0])
			break; // this is a valid spot
	}

	if(i >= MAX_IMAGES)
	{
		MsgDev(D_ERROR, "SPR_Load: can't load %s, MAX_HSPRITES limit exceeded\n", szPicName);
		return 0;
	}

	// load new model
	if(CL_LoadHudSprite(name, &clgame.sprites[i], false, texFlags))
	{
		if(i < MAX_IMAGES - 1)
			clgame.sprites[i].needload = clgame.load_sequence;

		return i;
	}
	return 0;
}

HSPRITE_ SPR_Load(const char *szPicName)
{
	int texFlags = TF_NOPICMIP;

	if(cl_sprite_nearest->value)
		texFlags |= TF_NEAREST;

	return SPR_LoadExt(szPicName, texFlags);
};

int SPR_Frames(HSPRITE_ hPic)
{
	int numFrames;

	R_GetSpriteParms(NULL, NULL, &numFrames, 0, CL_GetSpritePointer(hPic));

	return numFrames;
};

int SPR_Height(HSPRITE_ hPic, int frame)
{
	int sprHeight;

	R_GetSpriteParms(NULL, &sprHeight, NULL, frame, CL_GetSpritePointer(hPic));

	return sprHeight;
};

int SPR_Width(HSPRITE_ hPic, int frame)
{
	int sprWidth;

	R_GetSpriteParms(&sprWidth, NULL, NULL, frame, CL_GetSpritePointer(hPic));

	return sprWidth;
};

void SPR_Set(HSPRITE_ hPic, int r, int g, int b)
{
	clgame.ds.pSprite = CL_GetSpritePointer(hPic);
	clgame.ds.spriteColor[0] = bound(0, r, 255);
	clgame.ds.spriteColor[1] = bound(0, g, 255);
	clgame.ds.spriteColor[2] = bound(0, b, 255);
	clgame.ds.spriteColor[3] = 255;

	// set default state
	pglDisable(GL_BLEND);
	pglDisable(GL_ALPHA_TEST);
	pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
};

void SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height)
{
	pglEnable(GL_BLEND);
	pglBlendFunc(blendsrc, blenddst); // g-cont. are params is valid?
	SPR_DrawGeneric(frame, x, y, width, height, prc);
};

void SPR_Draw(int frame, int x, int y, const wrect_t *prc)
{
	pglEnable(GL_ALPHA_TEST);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

void SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc)
{
	GL_SetRenderMode(kRenderTransAlpha);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc)
{
	GL_SetRenderMode(kRenderTransAdd);
	SPR_DrawGeneric(frame, x, y, -1, -1, prc);
};

void SPR_EnableScissor(int x, int y, int width, int height)
{
	// check bounds
	x = bound(0, x, clgame.scrInfo.iWidth);
	y = bound(0, y, clgame.scrInfo.iHeight);

	width = bound(0, width, clgame.scrInfo.iWidth - x);
	height = bound(0, height, clgame.scrInfo.iHeight - y);

	clgame.ds.scissor_x = x;
	clgame.ds.scissor_width = width;
	clgame.ds.scissor_y = y;
	clgame.ds.scissor_height = height;
	clgame.ds.scissor_test = true;
};

void SPR_DisableScissor()
{
	clgame.ds.scissor_x = 0;
	clgame.ds.scissor_width = 0;
	clgame.ds.scissor_y = 0;
	clgame.ds.scissor_height = 0;
	clgame.ds.scissor_test = false;
};

/*
=========
pfnSPR_GetList

for parsing half-life scripts - hud.txt etc
=========
*/
client_sprite_t *SPR_GetList(char *psz, int *piCount)
{
	client_sprite_t *pList;
	int index, numSprites = 0;
	char *afile, *pfile;
	string token;
	byte *pool;

	if(piCount)
		*piCount = 0;

	if(!clgame.itemspath[0]) // typically it's sprites\*.txt
		FS_ExtractFilePath(psz, clgame.itemspath);

	afile = (char *)FS_LoadFile(psz, NULL, false);
	if(!afile)
		return NULL;

	pfile = afile;
	pfile = COM_ParseFile(pfile, token);
	numSprites = Q_atoi(token);

	if(!cl.video_prepped)
		pool = cls.mempool; // static memory
	else
		pool = com_studiocache; // temporary

	// name, res, pic, x, y, w, h
	// NOTE: we must use com_studiocache because it will be purge on next restart
	// or change map
	pList = Mem_Alloc(pool, sizeof(client_sprite_t) * numSprites);

	for(index = 0; index < numSprites; index++)
	{
		if((pfile = COM_ParseFile(pfile, token)) == NULL)
			break;

		Q_strncpy(pList[index].szName, token, sizeof(pList[index].szName));

		// read resolution
		pfile = COM_ParseFile(pfile, token);
		pList[index].iRes = Q_atoi(token);

		// read spritename
		pfile = COM_ParseFile(pfile, token);
		Q_strncpy(pList[index].szSprite, token, sizeof(pList[index].szSprite));

		// parse rectangle
		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.left = Q_atoi(token);

		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.top = Q_atoi(token);

		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.right = pList[index].rc.left + Q_atoi(token);

		pfile = COM_ParseFile(pfile, token);
		pList[index].rc.bottom = pList[index].rc.top + Q_atoi(token);

		if(piCount)
			(*piCount)++;
	}

	if(index < numSprites)
		MsgDev(D_WARN, "SPR_GetList: unexpected end of %s (%i should be %i)\n", psz, numSprites, index);

	Mem_Free(afile);
	return pList;
};

const struct model_s *EngFunc_GetSpritePointer(HSPRITE hSprite) // CL_
{
	if(hSprite <= 0 || hSprite > (MAX_IMAGES - 1))
		return NULL; // bad image

	return &clgame.sprites[hSprite];
};

/*
HSPRITE_ SPR_Load(const char *szPicName){};

int SPR_Frames(HSPRITE_ hPic){};

int SPR_Height(HSPRITE_ hPic, int frame){};

int SPR_Width(HSPRITE_ hPic, int frame){};

void SPR_Set(HSPRITE_ hPic, int r, int g, int b){};

void SPR_Draw(int frame, int x, int y, const wrect_t *prc){};

void SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc){};

void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc){};

void SPR_EnableScissor(int x, int y, int width, int height){};

void SPR_DisableScissor(){};

client_sprite_t *SPR_GetList(char *psz, int *piCount){};

const struct model_s *EngFunc_GetSpritePointer(HSPRITE hSprite){};
*/