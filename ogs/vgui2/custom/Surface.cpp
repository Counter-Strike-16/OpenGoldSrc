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

#include "Surface.hpp"

namespace vgui
{

CSurface::CSurface()
{
	CoInitialize(NULL);

	m_DrawColor[0] = m_DrawColor[1] = m_DrawColor[2] = m_DrawColor[3] = 255;
	m_nTranslateX = m_nTranslateY = 0;

	EnableScissor(false);
	SetScissorRect(0, 0, 100000, 100000);

	m_flAlphaMultiplier = 1.0;

	m_bAllowJavaScript = false;
	m_iBoundTexture = 0;

	m_pDrawTextPos[0] = m_pDrawTextPos[1] = 0;
	m_DrawTextColor[0] = m_DrawTextColor[1] = m_DrawTextColor[2] = m_DrawTextColor[3] = 255;

	m_hCurrentCursor = dc_none;
	m_hCurrentFont = NULL;
	m_pRestrictedPanel = NULL;
	m_pMouseTexture = NULL;
	m_pBoundTexture = NULL;
};

CSurface::~CSurface()
{
	for(int i = 0; i < m_HtmlWindows.Size(); i++)
		delete m_HtmlWindows[i];

	for(int i = m_FileTypeImages.First(); i != m_FileTypeImages.InvalidIndex(); i = m_FileTypeImages.Next(i))
		delete m_FileTypeImages[i];

	m_HtmlWindows.RemoveAll();
	m_FileTypeImages.RemoveAll();
	m_iBoundTexture = 0;

	for(int i = 0; i < m_VGuiSurfaceTextures.MaxElement(); ++i)
	{
		if(!m_VGuiSurfaceTextures.IsValidIndex(i))
			continue;
	};

	CoUninitialize();
};

void CSurface::Shutdown()
{
	g_pfnSurface_Shutdown(this, 0);

	FontManager().ClearAllFonts();
};

void CSurface::RunFrame()
{
	g_pfnSurface_RunFrame(this, 0);
};

VPANEL CSurface::GetEmbeddedPanel()
{
	return g_pSurface->GetEmbeddedPanel();
};

void CSurface::SetEmbeddedPanel(VPANEL pPanel)
{
	g_pfnSetEmbeddedPanel(this, 0, pPanel);
};

void CSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
	int insets[4] = { 0, 0, 0, 0 };
	int absExtents[4];
	int clipRect[4];

	if (insets)
		g_pVGuiPanel->GetInset(panel, insets[0], insets[1], insets[2], insets[3]);

	g_pVGuiPanel->GetAbsPos(panel, absExtents[0], absExtents[1]);

	int wide, tall;
	g_pVGuiPanel->GetSize(panel, wide, tall);
	absExtents[2] = absExtents[0] + wide;
	absExtents[3] = absExtents[1] + tall;

	g_pVGuiPanel->GetClipRect(panel, clipRect[0], clipRect[1], clipRect[2], clipRect[3]);

	int i = m_PaintStateStack.AddToTail();
	PaintState_t &paintState = m_PaintStateStack[i];
	paintState.m_pPanel = panel;

	paintState.m_iTranslateX = 0;
	paintState.m_iTranslateY = 0;

	paintState.m_iScissorLeft = clipRect[0] - m_pSurfaceExtents[0] - (insets[0] + (absExtents[0] - m_pSurfaceExtents[0]));
	paintState.m_iScissorTop = clipRect[1] - m_pSurfaceExtents[1] - (insets[1] + (absExtents[1] - m_pSurfaceExtents[1]));
	paintState.m_iScissorRight = clipRect[2] - m_pSurfaceExtents[0] - (insets[0] + (absExtents[0] - m_pSurfaceExtents[0]));
	paintState.m_iScissorBottom = clipRect[3] - m_pSurfaceExtents[1] - (insets[1] + (absExtents[1] - m_pSurfaceExtents[1]));

	SetupPaintState(paintState);
	g_pfnPushMakeCurrent(this, 0, panel, useInsets);
};

void CSurface::PopMakeCurrent(VPANEL panel)
{
	if (m_BatchedCharInfos.Count())
		DrawFlushText();

	int top = m_PaintStateStack.Count() - 1;

	Assert(top >= 0);
	Assert(m_PaintStateStack[top].m_pPanel == panel);

	m_PaintStateStack.Remove(top);

	if (top > 0)
		SetupPaintState(m_PaintStateStack[top - 1]);

	g_pfnPopMakeCurrent(this, 0, panel);
};

void CSurface::DrawSetColor(int r, int g, int b, int a)
{
	m_DrawColor[0] = (unsigned char)r;
	m_DrawColor[1] = (unsigned char)g;
	m_DrawColor[2] = (unsigned char)b;
	m_DrawColor[3] = (unsigned char)(a * m_flAlphaMultiplier);

	g_pfnDrawSetColor(this, 0, r, g, b, a);
};

void CSurface::DrawSetColor(Color col)
{
	DrawSetColor(col[0], col[1], col[2], col[3]);
};

void CSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	Vertex_t rect[2];
	Vertex_t clippedRect[2];

	InitVertex(rect[0], x0, y0, 0, 0);
	InitVertex(rect[1], x1, y1, 0, 0);

	if (!ClipRect(rect[0], rect[1], &clippedRect[0], &clippedRect[1]))
		return;

	DrawSetTexture(0);
	DrawQuad(clippedRect[0], clippedRect[1], m_DrawColor);
};

void CSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	DrawFilledRect(x0, y0, x1, y0 + 1);
	DrawFilledRect(x0, y1 - 1, x1, y1);
	DrawFilledRect(x0, y0 + 1, x0 + 1, y1 - 1);
	DrawFilledRect(x1 - 1, y0 + 1, x1, y1 - 1);
};

void CSurface::DrawLine(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	Vertex_t verts[2];
	verts[0].Init(Vector2D(x0, y0), Vector2D(0, 0));
	verts[1].Init(Vector2D(x1, y1), Vector2D(1, 1));

	DrawSetTexture(0);
	DrawTexturedLineInternal(verts[0], verts[1]);
};

void CSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
	if (m_DrawColor[3] == 0)
		return;

	qglDisable(GL_TEXTURE_2D);
	qglColor4ub(m_DrawColor[0], m_DrawColor[1], m_DrawColor[2], m_DrawColor[3]);
	qglBegin(GL_LINES);

	for (int i = 0; i < numPoints ; i++)
	{
		int inext = (i + 1) % numPoints;

		Vertex_t verts[2];
		Vertex_t clippedVerts[2];

		int x0, y0, x1, y1;

		x0 = px[i];
		x1 = px[inext];
		y0 = py[i];
		y1 = py[inext];

		InitVertex(verts[0], x0, y0, 0, 0);
		InitVertex(verts[1], x1, y1, 1, 1);

		if (!ClipLine(verts, clippedVerts))
			continue;

		qglTexCoord2fv(clippedVerts[0].m_TexCoord.Base());
		qglVertex3f(clippedVerts[0].m_Position.x, clippedVerts[0].m_Position.y, m_flZPos);
		qglTexCoord2fv(clippedVerts[1].m_TexCoord.Base());
		qglVertex3f(clippedVerts[1].m_Position.x, clippedVerts[1].m_Position.y, m_flZPos);
	}

	qglEnd();
	qglEnable(GL_TEXTURE_2D);
};

void CSurface::DrawSetTextFont(HFont font)
{
	m_hCurrentFont = font;
	g_pfnDrawSetTextFont(this, 0, font);
};

void CSurface::DrawSetTextColor(int r, int g, int b, int a)
{
	int adjustedAlpha = (a * m_flAlphaMultiplier);

	if (r != m_DrawTextColor[0] || g != m_DrawTextColor[1] || b != m_DrawTextColor[2] || adjustedAlpha != m_DrawTextColor[3])
	{
		DrawFlushText();

		m_DrawTextColor[0] = (unsigned char)r;
		m_DrawTextColor[1] = (unsigned char)g;
		m_DrawTextColor[2] = (unsigned char)b;
		m_DrawTextColor[3] = (unsigned char)adjustedAlpha;
	}

	g_pfnDrawSetTextColor(this, 0, r, g, b, a);
};

void CSurface::DrawSetTextColor(Color col)
{
	DrawSetTextColor(col[0], col[1], col[2], col[3]);
};

void CSurface::DrawSetTextPos(int x, int y)
{
	m_pDrawTextPos[0] = x;
	m_pDrawTextPos[1] = y;

	g_pSurface->DrawSetTextPos(x, y);
};

void CSurface::DrawGetTextPos(int &x, int &y)
{
	g_pSurface->DrawGetTextPos(x, y);
};

void CSurface::DrawPrintText(const wchar_t *text, int textLen)
{
	if (!text)
		return;

	if (!m_hCurrentFont)
		return;

	int x, y;
	DrawGetTextPos(x, y);

	x += m_nTranslateX;
	y += m_nTranslateY;

	int iTall = GetFontTall(m_hCurrentFont);
	int iLastTexId = -1;

	int iCount = 0;
	Vertex_t *pQuads = (Vertex_t *)stackalloc((2 * iTextLen) * sizeof(Vertex_t));

	int iTotalWidth = 0;

	for (int i = 0; i < iTextLen; ++i)
	{
		wchar_t ch = text[i];

		bool bUnderlined = FontManager().GetFontUnderlined(m_hCurrentFont);
		int abcA, abcB, abcC;
		GetCharABCwide(m_hCurrentFont, ch, abcA, abcB, abcC);
		iTotalWidth += abcA;

		int iWide = abcB;

		if (bUnderlined)
		{
			iWide += (abcA + abcC);
			x-= abcA;
		}

		if (!iswspace(ch) || bUnderlined)
		{
			int iTexId = 0;
			float *texCoords = NULL;

			if (!g_FontTextureCache.GetTextureForChar(m_hCurrentFont, ch, &iTexId, &texCoords))
				continue;

			Assert(texCoords);

			if (iTexId != iLastTexId)
			{
				if (iCount)
				{
					DrawSetTexture(iLastTexId);
					DrawQuadArray(iCount, pQuads, m_DrawTextColor);
					iCount = 0;
				}

				iLastTexId = iTexId;
			}

 			Vertex_t &ul = pQuads[2 * iCount];
 			Vertex_t &lr = pQuads[2 * iCount + 1];
			iCount++;

			ul.m_Position.x = x + iTotalWidth;
			ul.m_Position.y = y;
			lr.m_Position.x = ul.m_Position.x + iWide;
			lr.m_Position.y = ul.m_Position.y + iTall;

			ul.m_TexCoord[0] = texCoords[0];
			ul.m_TexCoord[1] = texCoords[1];
			lr.m_TexCoord[0] = texCoords[2];
			lr.m_TexCoord[1] = texCoords[3];
		}

		iTotalWidth += iWide + abcC;

		if (bUnderlined)
		{
			iTotalWidth -= abcC;
		}
	}

	if (iCount)
	{
		DrawSetTexture(iLastTexId);
		DrawQuadArray(iCount, pQuads, m_DrawTextColor);
	}

	DrawSetTextPos(x + iTotalWidth, y);
	stackfree(pQuads);
};

void CSurface::DrawUnicodeChar(wchar_t wch)
{
	CharRenderInfo info;
	info.additive = false;

	if (DrawGetUnicodeCharRenderInfo(wch, info))
		DrawRenderCharFromInfo(info);
};

void CSurface::DrawUnicodeCharAdd(wchar_t wch)
{
	CharRenderInfo info;
	info.additive = true;

	if (DrawGetUnicodeCharRenderInfo(wch, info))
		DrawRenderCharFromInfo(info);
};

void CSurface::DrawFlushText()
{
	if (!m_BatchedCharInfos.Count())
		return;

	for (int i = 0; i < m_BatchedCharInfos.Count(); i++)
	{
		DrawSetTexture(m_BatchedCharInfos[i].textureId);
		DrawQuad(m_BatchedCharInfos[i].verts[0], m_BatchedCharInfos[i].verts[1], m_DrawTextColor);
	}

	m_BatchedCharInfos.RemoveAll();
};

IHTML *CSurface::CreateHTMLWindow(IHTMLEvents *events, VPANEL context)
{
#if defined (ENABLE_HTMLWINDOW)
	VPANEL parent = g_pIPanel->GetParent(context);

	if (!parent)
		return NULL;

	HtmlWindow *IE = new HtmlWindow(events, context, g_hMainWnd, m_bAllowJavaScript, SupportsFeature(DIRECT_HWND_RENDER));
	IE->SetVisible(g_pIPanel->IsVisible(parent));

	m_HtmlWindows.AddToTail(IE);
	return dynamic_cast<IHTML *>(IE);
#else
	Assert(0);
	return NULL;
#endif
};

void CSurface::PaintHTMLWindow(IHTML *htmlwin)
{
#if defined (ENABLE_HTMLWINDOW)
	HtmlWindow *IE = static_cast<HtmlWindow *>(htmlwin);

	if (IE)
	{
		HDC hdc = ::GetDC(reinterpret_cast<HWND>(g_hMainWnd));
		IE->OnPaint(hdc);
		::ReleaseDC(reinterpret_cast<HWND>(g_hMainWnd), hdc);
	}
#endif
};

void CSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
#if defined (ENABLE_HTMLWINDOW)
	HtmlWindow *IE = dynamic_cast<HtmlWindow *>(htmlwin);

	if (IE)
	{
		m_HtmlWindows.FindAndRemove(IE);
		delete IE;
	}
#else
	#error "GameUI now NEEDS the HTML component!!"
#endif
};

void CSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);
	
	if (!texture || !texture->_filename || (texture->_filename && stricmp(filename, texture->_filename)) || forceReload)
	{
		char name[MAX_PATH];
		Q_snprintf(name, sizeof(name), "%s.tga", filename);

		int width, height;
		bool success = LoadTGA2(name, m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);

		if (!success)
		{
			const char *psz = Q_stristr(name, "vgui/");

			if (psz)
				success = LoadTGA2(name + strlen("vgui/"), m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);
		}

		if (!success)
		{
			Q_snprintf(name, sizeof(name), "%s.bmp", filename);

			if (!LoadBMP(name, m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height))
			{
				const char *psz = Q_stristr(name, "vgui/");

				if (psz)
					success = LoadBMP(name + strlen("vgui/"), m_TextureBuffer, sizeof(m_TextureBuffer), &width, &height);
			}
			else
			{
				success = true;
			}
		}

		if (success)
		{
			if (IsPowerOfTwo(width) == false && IsPowerOfTwo(height) == false && engine->pfnGetCvarFloat("fs_perf_warnings"))
				engine->Con_DPrintf("fs_perf_warnings: Resampling non-power-of-2 image '%s' (%dx%d)\n", filename, width, height);

			DrawSetTextureRGBA(id, m_TextureBuffer, width, height, hardwareFilter, forceReload);
		}
		else
		{
			Msg("Error: texture file '%s' does not exist or is invalid\n", filename);
			return;
		}

		if (!texture)
		{
			texture = AllocTextureForId(id);
			memset(texture, 0, sizeof(Texture));
		}

		texture->_id = id;
		texture->_filename = filename;
		texture->_wide = width;
		texture->_tall = height;
		texture->_s0 = 0;
		texture->_t0 = 0;
		texture->_s1 = 1;
		texture->_t1 = 1;
	}
};


void CSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
};

void CSurface::DrawSetTexture(int id)
{
	if (id != m_iBoundTexture)
	{
		m_iBoundTexture = id;
		m_pBoundTexture = GetTextureById(id);
	}

	g_pfnDrawSetTexture(this, 0, id);
};

void CSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
	Texture *texture = GetTextureById(id);

	if (!texture)
		return;

	wide = texture->_wide;
	tall = texture->_tall;
};

void CSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	if (m_DrawColor[3] == 0)
		return;

	if (!m_pBoundTexture)
		return;

	Vertex_t rect[2];
	Vertex_t clippedRect[2];

	InitVertex(rect[0], x0, y0, m_pBoundTexture->_s0, m_pBoundTexture->_t0);
	InitVertex(rect[1], x1, y1, m_pBoundTexture->_s1, m_pBoundTexture->_t1);

	if (!ClipRect(rect[0], rect[1], &clippedRect[0], &clippedRect[1]))
		return;

	if (g_iVideoMode == VIDEOMODE_D3D)
	{
		clippedRect[0].m_TexCoord.x += 0.5 / m_pBoundTexture->_wide;
		clippedRect[0].m_TexCoord.y += 0.5 / m_pBoundTexture->_tall;
		clippedRect[1].m_TexCoord.x -= 0.5 / m_pBoundTexture->_wide;
		clippedRect[1].m_TexCoord.y -= 0.5 / m_pBoundTexture->_tall;
	}

	DrawQuad(clippedRect[0], clippedRect[1], m_DrawColor);
};

bool CSurface::IsTextureIDValid(int id)
{
	return (GetTextureById(id) != NULL);
};

int CSurface::CreateNewTextureID(bool procedural)
{
	return g_pfnCreateNewTextureID(this, 0, procedural);
};

void CSurface::GetScreenSize(int &wide, int &tall)
{
	wide = g_iVideoWidth;
	tall = g_iVideoHeight;
};

void CSurface::SetAsTopMost(VPANEL panel, bool state)
{
	g_pSurface->SetAsTopMost(panel, state);
};

void CSurface::BringToFront(VPANEL panel)
{
	g_pSurface->BringToFront(panel);
};

void CSurface::SetForegroundWindow(VPANEL panel)
{
	g_pSurface->SetForegroundWindow(panel);
};

void CSurface::SetPanelVisible(VPANEL panel, bool state)
{
	g_pSurface->SetPanelVisible(panel, state);
};

void CSurface::SetMinimized(VPANEL panel, bool state)
{
	g_pSurface->SetMinimized(panel, state);
};

bool CSurface::IsMinimized(VPANEL panel)
{
	return g_pSurface->IsMinimized(panel);
};

void CSurface::FlashWindow(VPANEL panel, bool state)
{
	g_pSurface->FlashWindow(panel, state);
};

void CSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
	g_pSurface->SetTitle(panel, title);
};

void CSurface::SetAsToolBar(VPANEL panel, bool state)
{
	g_pSurface->SetAsToolBar(panel, state);
};

void CSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
	g_pSurface->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
};

void CSurface::SwapBuffers(VPANEL panel)
{
	g_pSurface->SwapBuffers(panel);
};

void CSurface::Invalidate(VPANEL panel)
{
	g_pSurface->Invalidate(panel);
};

void CSurface::SetCursor(HCursor cursor)
{
	if (m_hCurrentCursor != cursor)
	{
		m_hCurrentCursor = cursor;
		g_pfnSetCursor(this, 0, cursor);
	}
};

bool CSurface::IsCursorVisible()
{
	return m_hCurrentCursor != dc_none;
};

void CSurface::ApplyChanges()
{
	g_pSurface->ApplyChanges();
};

bool CSurface::IsWithin(int x, int y)
{
	return g_pSurface->IsWithin(x, y);
};

bool CSurface::HasFocus()
{
	bool result = g_pfnHasFocus(this, 0);

	if (!result)
	{
		HWND focus = ::GetFocus();

		for (int i = 0; i < m_HtmlWindows.Count(); i++)
		{
			if (m_HtmlWindows[i]->GetIEHWND() == focus)
				return true;
		}
	}

	return result;
};

bool CSurface::SupportsFeature(SurfaceFeature_e feature)
{
	switch (feature)
	{
		case OUTLINE_FONTS: return true;
	}

	return g_pfnSupportsFeature(this, 0, feature);
};

void CSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
	m_pRestrictedPanel = panel;
	g_pfnRestrictPaintToSinglePanel(this, 0, panel);
};

void CSurface::SetModalPanel(VPANEL panel)
{
	g_pSurface->SetModalPanel(panel);
};

VPANEL CSurface::GetModalPanel()
{
	return 0;
};

void CSurface::UnlockCursor()
{
	g_pfnUnlockCursor(this, 0);
};

void CSurface::LockCursor()
{
	g_pfnLockCursor(this, 0);
};

void CSurface::SetTranslateExtendedKeys(bool state)
{
	g_pSurface->SetTranslateExtendedKeys(state);
};

VPANEL CSurface::GetTopmostPopup()
{
	return g_pSurface->GetTopmostPopup();
};

void CSurface::SetTopLevelFocus(VPANEL panel)
{
	g_pSurface->SetTopLevelFocus(panel);
};

HFont CSurface::CreateFont()
{
	return FontManager().CreateFont();
};

bool CSurface::AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return false;
};

bool CSurface::AddCustomFontFile(const char *fontFileName)
{
	return 0;
};

int CSurface::GetFontTall(HFont font)
{
	return 0;
};

void CSurface::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
};

int CSurface::GetCharacterWidth(HFont font, int ch)
{
	return 0;
};

void CSurface::GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
{
};

VPANEL CSurface::GetNotifyPanel()
{
	return 0;
};

void CSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
};

void CSurface::PlaySound(const char *fileName)
{
};

int CSurface::GetPopupCount()
{
	return 0;
};

VPANEL CSurface::GetPopup(int index)
{
	return 0;
};

bool CSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return false;
};

bool CSurface::RecreateContext(VPANEL panel)
{
	return false;
};

void CSurface::AddPanel(VPANEL panel)
{
};

void CSurface::ReleasePanel(VPANEL panel)
{
};

void CSurface::MovePopupToFront(VPANEL panel)
{
};

void CSurface::MovePopupToBack(VPANEL panel)
{
};

void CSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings)
{
};

void CSurface::PaintTraverse(VPANEL panel)
{
};

void CSurface::EnableMouseCapture(VPANEL panel, bool state)
{
};

void CSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
};

void CSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
};

void CSurface::GetProportionalBase(int &width, int &height)
{
};

void CSurface::CalculateMouseVisible()
{
};

bool CSurface::NeedKBInput()
{
	return false;
};

bool CSurface::HasCursorPosFunctions()
{
	return false;
};

void CSurface::SurfaceGetCursorPos(int &x, int &y)
{
};

void CSurface::SurfaceSetCursorPos(int x, int y)
{
};

void CSurface::DrawTexturedPolygon(int *p, int n)
{
};

int CSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return 0;
};

void CSurface::SetAllowHTMLJavaScript(bool state)
{
};

}; // namespace vgui