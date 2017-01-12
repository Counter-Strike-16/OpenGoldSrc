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

#include "RendererImpl.hpp"
#include "gl_local.hpp"

static CRendererImpl gRendererImpl;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRendererImpl, IRenderer, OGS_RENDERER_INTERFACE_VERSION, gRendererImpl);

IFileSystem *gpFileSystem = nullptr;

CRendererImpl::CRendererImpl(){};

CRendererImpl::~CRendererImpl(){};

bool CRendererImpl::Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWndProc)
{
	gpFileSystem = (IFileSystem *)afnEngineFactory(FILESYSTEM_INTERFACE_VERSION, nullptr);

	if(!mpFileSystem)
		return false;

	R_Init();
	return true;
};

void CRendererImpl::Shutdown()
{
	R_Shutdown();
};

void CRendererImpl::AppActivate(bool abActivate)
{
	GLimp_AppActivate(abActivate);
};

void CRendererImpl::BeginRegistration(const char *asMap)
{
	R_BeginRegistration(asMap);
};

void CRendererImpl::EndRegistration()
{
	R_EndRegistration();
};

void CRendererImpl::SetRenderMode(int nMode)
{
	switch(mode)
	{
	case kRenderNormal:
	default:
		pglDisable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	case kRenderTransColor:
	case kRenderTransAlpha:
	case kRenderTransTexture:
		// NOTE: TriAPI doesn't have 'solid' mode
		pglEnable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	case kRenderGlow:
	case kRenderTransAdd:
		pglEnable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglBlendFunc(GL_SRC_ALPHA, GL_ONE);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	};
};

void CRendererImpl::BeginRendering(int nMode)
{
	switch(nMode)
	{
	case TRI_POINTS:
		mode = GL_POINTS;
		break;
	case TRI_TRIANGLES:
		mode = GL_TRIANGLES;
		break;
	case TRI_TRIANGLE_FAN:
		mode = GL_TRIANGLE_FAN;
		break;
	case TRI_QUADS:
		mode = GL_QUADS;
		break;
	case TRI_LINES:
		mode = GL_LINES;
		break;
	case TRI_TRIANGLE_STRIP:
		mode = GL_TRIANGLE_STRIP;
		break;
	case TRI_QUAD_STRIP:
		mode = GL_QUAD_STRIP;
		break;
	case TRI_POLYGON:
	default:
		mode = GL_POLYGON;
		break;
	};

	pglBegin(mode);
};

void CRendererImpl::EndRendering()
{
	pglEnd();
	pglDisable(GL_ALPHA_TEST);
};

struct model_s *CRendererImpl::RegisterModel(const char *asName)
{
	return R_RegisterModel(asName);
};

struct image_s *CRendererImpl::RegisterSkin(const char *asName)
{
	return R_RegisterSkin(asName);
};

struct image_s *CRendererImpl::RegisterPic(const char *asName)
{
	return Draw_FindPic(asName);
};

void CRendererImpl::SetSky(const char *asName, float afRotate, vec3_t avAxis)
{
	R_SetSky(asName, afRotate, avAxis);
};

void CRendererImpl::BeginFrame(float afCameraSeparation)
{
	R_BeginFrame(afCameraSeparation);
};

void CRendererImpl::RenderFrame(refdef_t *apRenderSettings)
{
	R_RenderFrame(apRenderSettings);
};

void CRendererImpl::EndFrame()
{
	GLimp_EndFrame();
};

void CRendererImpl::DrawGetPicSize(int *w, int *h, char *name)
{
	Draw_GetPicSize(w, h, name);
};

void CRendererImpl::DrawPic(int x, int y, char *name)
{
	Draw_Pic(x, y, name);
};

void CRendererImpl::DrawStretchPic(int x, int y, int w, int h, char *name)
{
	Draw_StretchPic(x, y, w, h, name);
};

void CRendererImpl::DrawChar(int x, int y, int c)
{
	Draw_Char(x, y, c);
};

void CRendererImpl::DrawTileClear(int x, int y, int w, int h, char *name)
{
	Draw_TileClear(x, y, w, h, name);
};

void CRendererImpl::DrawFill(int x, int y, int w, int h, int c)
{
	Draw_Fill(x, y, w, h, c);
};

void CRendererImpl::DrawFadeScreen()
{
	Draw_FadeScreen();
};

void CRendererImpl::DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data)
{
	Draw_StretchRaw(x, y, w, h, cols, rows, data);
};

void CRendererImpl::CinematicSetPalette(const byte *palette)
{
	R_SetPalette(palette);
};