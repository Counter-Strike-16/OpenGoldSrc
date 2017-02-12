/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#pragma once

#include "common/commontypes.h"

// draw.h - these are the only functions outside the refresh allowed
// to touch the vid buffer

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