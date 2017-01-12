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
/// @brief these are the only functions outside the refresh allowed to touch the vid buffer

#pragma once

#include "common/commontypes.h"

typedef struct qpic_s qpic_t;

extern qpic_t *draw_disc; // also used on sbar

void Drawer_Init();

void Drawer_DrawCharacter(int x, int y, int num);
void Drawer_DrawDebugChar(char num);

void Drawer_DrawSubPic(int x, int y, qpic_t *pic, int srcx, int srcy, int width, int height);
void Drawer_DrawPic(int x, int y, qpic_t *pic);
void Drawer_DrawTransPic(int x, int y, qpic_t *pic);
void Drawer_DrawTransPicTranslate(int x, int y, qpic_t *pic, byte *translation);

void Drawer_DrawConsoleBackground(int lines);

void Drawer_DrawBeginDisc();
void Drawer_DrawEndDisc();

void Drawer_DrawTileClear(int x, int y, int w, int h);

void Drawer_FillScreen(int x, int y, int w, int h, int c);
void Drawer_FadeScreen();

void Drawer_DrawString(int x, int y, char *str);
void Drawer_DrawAltString(int x, int y, char *str);

qpic_t *Drawer_DrawPicFromWad(char *name);
qpic_t *Drawer_DrawCachePic(char *path);

void Drawer_DrawCrosshair();