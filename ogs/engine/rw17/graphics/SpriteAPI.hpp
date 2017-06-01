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

#include "common/wrect.h"
#include "engine/cdll_int.h"

HSPRITE_ SPR_Load(const char *szPicName);

int SPR_Frames(HSPRITE_ hPic);

int SPR_Height(HSPRITE_ hPic, int frame);
int SPR_Width(HSPRITE_ hPic, int frame);

void SPR_Set(HSPRITE_ hPic, int r, int g, int b);

void SPR_DrawGeneric(int frame, int x, int y, const wrect_t *prc, int blendsrc, int blenddst, int width, int height);
void SPR_Draw(int frame, int x, int y, const wrect_t *prc);
void SPR_DrawHoles(int frame, int x, int y, const wrect_t *prc);
void SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc);

void SPR_EnableScissor(int x, int y, int width, int height);
void SPR_DisableScissor();

client_sprite_t *SPR_GetList(char *psz, int *piCount);

const struct model_s *EngFunc_GetSpritePointer(HSPRITE hSprite);