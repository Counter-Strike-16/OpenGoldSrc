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

#include "RenderImpl.hpp"

CRenderImpl::CRenderImpl()
{
};

CRenderImpl::~CRenderImpl()
{
};

bool CRenderImpl::Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWndProc)
{
};

void CRenderImpl::Shutdown()
{
};

void CRenderImpl::AppActivate(bool abActivate)
{
};

void CRenderImpl::BeginRegistration(const char *asMap)
{
};

void CRenderImpl::EndRegistration()
{
};

struct model_s *CRenderImpl::RegisterModel(const char *asName)
{
};

struct image_s *CRenderImpl::RegisterSkin(const char *asName)
{
};

struct image_s *CRenderImpl::RegisterPic(const char *asName)
{
};

void CRenderImpl::SetSky(const char *asName, float afRotate, vec3_t avAxis)
{
};

void CRenderImpl::BeginFrame(float afCameraSeparation)
{
};

void CRenderImpl::RenderFrame(refdef_t *apRenderSettings)
{
};

void CRenderImpl::EndFrame()
{
};

void CRenderImpl::DrawGetPicSize(int *w, int *h, char *name)
{
};

void CRenderImpl::DrawPic(int x, int y, char *name)
{
};

void CRenderImpl::DrawStretchPic(int x, int y, int w, int h, char *name)
{
};

void CRenderImpl::DrawChar(int x, int y, int c)
{
};

void CRenderImpl::DrawTileClear(int x, int y, int w, int h, char *name)
{
};

void CRenderImpl::DrawFill(int x, int y, int w, int h, int c)
{
};

void CRenderImpl::DrawFadeScreen()
{
};

void CRenderImpl::DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data)
{
};

void CRenderImpl::CinematicSetPalette(const byte *palette)
{
};