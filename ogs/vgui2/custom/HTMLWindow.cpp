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

#include "HTMLWindow.hpp"

namespace vgui
{

void CHTMLWindow::OpenURL(const char *url)
{
};

bool CHTMLWindow::StopLoading()
{
	return false;
};

bool CHTMLWindow::Refresh()
{
	return false;
};

bool CHTMLWindow::Show(bool shown)
{
	return false;
};

char *CHTMLWindow::GetOpenedPage()
{
	return nullptr;
};

void CHTMLWindow::OnSize(int x, int y, int w, int h)
{
};

void CHTMLWindow::GetHTMLSize(int &wide, int &tall)
{
};

void CHTMLWindow::Clear()
{
};

void CHTMLWindow::AddText(const char *text)
{
};

void CHTMLWindow::OnMouse(MouseCode code, MOUSE_STATE s, int x, int y)
{
};

void CHTMLWindow::OnChar(wchar_t unichar)
{
};

void CHTMLWindow::OnKeyDown(KeyCode code)
{
};

IImage *CHTMLWindow::GetBitmap()
{
	return nullptr;
};

void CHTMLWindow::SetVisible(bool state)
{
};

bool CHTMLWindow::OnStartURL(const char *url, const char *target, bool first)
{
	return false;
};

void CHTMLWindow::OnFinishURL(const char *url)
{
};

void CHTMLWindow::OnProgressURL(long current, long maximum)
{
};

void CHTMLWindow::OnSetStatusText(const char *text)
{
};

void CHTMLWindow::OnUpdate()
{
};

void CHTMLWindow::OnLink()
{
};

void CHTMLWindow::OffLink()
{
};

}; // namespace vgui