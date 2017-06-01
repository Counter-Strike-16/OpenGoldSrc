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

#include "IEngineSurface.h"

class CEngineSurface : public IEngineSurface
{
public:
	void pushMakeCurrent(int *p1, int *p2, int *r, bool useInsets);
	void popMakeCurrent();
	
	void drawFilledRect(int x0, int y0, int x1, int y1);
	void drawOutlinedRect(int x0, int y0, int x1, int y1);
	
	void drawLine(int x0, int y0, int x1, int y1);
	void drawPolyLine(int *px, int *py, int numPoints);
	
	void drawTexturedPolygon(int *p, int n);
	
	void drawSetTextureRGBA(int id, const byte *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	void drawSetTexture(int id);
	
	void drawTexturedRect(int x0, int y0, int x1, int y1);
	
	int createNewTextureID();
	
	void drawSetColor(int r, int g, int b, int a);
	void drawSetTextColor(int r, int g, int b, int a);
	
	void drawSetTextPos(int x, int y);
	void drawGetTextPos(int &x, int &y);
	
	void drawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1);
	void drawPrintCharAdd(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1);
	
	void drawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload);
	void drawGetTextureSize(int id, int &wide, int &tall);
	
	bool isTextureIDValid(int id);
	
	bool drawSetSubTextureRGBA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall);
	
	void drawFlushText();
	
	void resetViewPort();
	
	void drawSetTextureBGRA(int id, const byte *rgba, int wide, int tall, int hardwareFilter, bool forceUpload);
	void drawUpdateRegionTextureBGRA(int textureID, int drawX, int drawY, unsigned const char *rgba, int subTextureWide, int subTextureTall);
};