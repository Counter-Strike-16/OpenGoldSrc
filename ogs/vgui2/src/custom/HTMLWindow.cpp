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

#include "HTMLWindow.hpp"

namespace vgui
{

HtmlWindow::HtmlWindow(vgui::IHTMLEvents *events, vgui::VPANEL c, HWND parent, bool AllowJavaScript, bool DirectToHWND)
{
	m_oleObject = NULL;
	m_oleInPlaceObject = NULL;
	m_webBrowser = NULL;

	m_events = events;
	m_ieHWND = 0;

	w = 0;
	h = 0;
	window_x = 0;
	window_y = 0;
	textureID = 0;
	m_HtmlEventsAdviseCookie = 0;
	m_Bitmap = NULL;
	m_bVisible = false;

	hdcMem = NULL;
	lasthDC = NULL;
	hBitmap = NULL;
	m_hHook = NULL;

	m_cleared = false;
	m_newpage = false;
	m_bHooked = false;
	m_bDirectToHWND = DirectToHWND;
	m_bParentCreated = false;
	m_hIEWndProc = NULL;

	strcpy(m_currentUrl, "");
	m_specificallyOpened = false;
	m_parent = parent;
	m_vcontext = c;

	if (!DirectToHWND)
	{
		char tmp[50];

		if (::GetClassName(parent,tmp,50) == 0)
		{
			WNDCLASS wc;
			memset(&wc, 0, sizeof(wc));

			wc.style = CS_OWNDC;
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpszClassName = "VGUI_HTML";

			UnregisterClass("VGUI_HTML", GetModuleHandle(NULL));

			RegisterClass(&wc);
			strcpy(tmp, "VGUI_HTML");
		}

		m_parent = CreateWindowEx(0, tmp, "", WS_CHILD | WS_DISABLED, 0, 0, 1, 1, parent, NULL,GetModuleHandle(NULL), NULL);
		m_bParentCreated = true;
	}

	CreateBrowser(AllowJavaScript);
}

HtmlWindow::~HtmlWindow(void)
{
	html_windows.FindAndRemove(this);

	if (m_hHook)
	{
		::UnhookWindowsHookEx(m_hHook);
	}

	if (m_webBrowser)
	{
		m_webBrowser->Release();
	}

	if (m_oleInPlaceObject)
	{
		if (!m_bVisibleAtRuntime)
			m_oleInPlaceObject->InPlaceDeactivate();

		m_oleInPlaceObject->UIDeactivate();
		m_oleInPlaceObject->Release();
	}

	if (m_connectionPoint)
	{
		m_connectionPoint->Unadvise(m_adviseCookie);
		m_connectionPoint->Unadvise(m_HtmlEventsAdviseCookie);
		m_connectionPoint->Release();
	}

	if (m_oleObject)
	{
		if (!m_bVisibleAtRuntime)
			m_oleObject->Close(OLECLOSE_NOSAVE);

		m_oleObject->SetClientSite(NULL);
		m_oleObject->Release();
	}

	if (m_viewObject)
	{
		m_viewObject->Release();
	}

	if (m_fs)
	{
		delete (FrameSite *)m_fs;
	}

	if (lasthDC)
	{
		::DeleteObject(hBitmap);
		::DeleteDC(hdcMem);
	}

	if (m_bParentCreated)
	{
		DestroyWindow(m_parent);
	}

	delete m_Bitmap;
};

void CHTMLWindow::OpenURL(const char *url)
{
};

bool CHTMLWindow::StopLoading()
{
	HRESULT hret = m_webBrowser->Stop();
	
	if(SUCCEEDED(hret))
		return true;
	
	return false;
};

bool CHTMLWindow::Refresh()
{
	DEBUG("onrefresh");
	
	OpenURL(m_currentUrl);
	return true;
};

bool CHTMLWindow::Show(bool shown)
{
	if(m_webBrowser)
		m_webBrowser->put_Visible(shown);
	
	return true;
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
	if(m_webBrowser)
		m_webBrowser->put_Visible(state);
	
	m_bVisible = state;
	
	if(m_newpage || !state)
	{
		if(m_bDirectToHWND)
			::ShowWindow(m_oleObjectHWND, state ? SW_SHOWNA : SW_HIDE);
		else
			::ShowWindow(m_oleObjectHWND, SW_HIDE);
	};
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