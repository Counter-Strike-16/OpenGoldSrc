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

#include "IRender.hpp"

class CRenderImpl : public IRender
{
public:
	CRenderImpl();
	~CRenderImpl();

	bool Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWnd, void *apWndProc);
	void Shutdown();

	void AppActivate(bool abActivate);

	void BeginRegistration(const char *asMap);
	void EndRegistration();

	struct model_s *RegisterModel(const char *asName);
	struct image_s *RegisterSkin(const char *asName);
	struct image_s *RegisterPic(const char *asName);

	void SetSky(const char *asName, float afRotate, vec3_t avAxis);

	void BeginFrame(float afCameraSeparation);
	void RenderFrame(refdef_t *apRenderSettings);
	void EndFrame();

	void DrawGetPicSize(int *w, int *h, char *name);
	void DrawPic(int x, int y, char *name);
	void DrawStretchPic(int x, int y, int w, int h, char *name);
	void DrawChar(int x, int y, int c);
	void DrawTileClear(int x, int y, int w, int h, char *name);
	void DrawFill(int x, int y, int w, int h, int c);
	void DrawFadeScreen();

	void DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data);

	void CinematicSetPalette(const byte *palette);
};