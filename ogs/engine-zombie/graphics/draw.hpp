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
/// @brief these are the only functions outside the render allowed to touch the vid buffer

#pragma once

#include "common/commontypes.h"

typedef struct qpic_s qpic_t;

extern qpic_t *draw_disc; // also used on sbar

void Draw_Init();

void Draw_Character(int x, int y, int num);
void Draw_DebugChar(char num);

void Draw_SubPic(int x, int y, qpic_t *pic, int srcx, int srcy, int width, int height);
void Draw_Pic(int x, int y, qpic_t *pic);
void Draw_TransPic(int x, int y, qpic_t *pic);
void Draw_TransPicTranslate(int x, int y, qpic_t *pic, byte *translation);

void Draw_ConsoleBackground(int lines);

void Draw_BeginDisc();
void Draw_EndDisc();

void Draw_TileClear(int x, int y, int w, int h);

void Draw_FillScreen(int x, int y, int w, int h, int c);
void Draw_FadeScreen();

void Draw_String(int x, int y, char *str);
void Draw_AltString(int x, int y, char *str);

qpic_t *Draw_PicFromWad(char *name);
qpic_t *Draw_CachePic(char *path);

void Draw_Crosshair();

class CGraphicsDrawer
{
public:
	void Init();

	void DrawCharacter(int x, int y, int num);
	void DrawDebugChar(char num);

	void DrawSubPic(int x, int y, qpic_t *pic, int srcx, int srcy, int width, int height);
	void DrawPic(int x, int y, qpic_t *pic);
	void DrawTransPic(int x, int y, qpic_t *pic);
	void DrawTransPicTranslate(int x, int y, qpic_t *pic, byte *translation);

	void DrawConsoleBackground(int lines);

	void DrawBeginDisc();
	void DrawEndDisc();

	void DrawTileClear(int x, int y, int w, int h);

	void FillScreen(int x, int y, int w, int h, int c);
	void FadeScreen();

	void DrawString(int x, int y, char *str);
	void DrawAltString(int x, int y, char *str);

	qpic_t *DrawPicFromWad(char *name);
	qpic_t *DrawCachePic(char *path);

	void DrawCrosshair();
};