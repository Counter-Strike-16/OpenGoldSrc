#ifndef WEBEXPLORER_H
#define WEBEXPLORER_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/HTML.h>

class CWebExplorer : public vgui::Frame
{
public:
	CWebExplorer(vgui::Panel *parent, const char *panelName);
	~CWebExplorer(void);

public:
	void PerformLayout(void);

public:
	void Activate(const char *msg);
	void Activate(const wchar_t *msg);

public:
	bool IsURL(const char *str);
	void OnFinishURL(void);

public:
	vgui::HTML *m_pMessage;
	int         m_iXOffset, m_iYOffset;
};

#endif