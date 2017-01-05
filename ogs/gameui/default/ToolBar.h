#ifndef TOOLBAR_H
#define TOOLBAR_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/HTML.h>

class CToolBar : public vgui::Panel
{
	typedef vgui::Panel BaseClass;

public:
	CToolBar(vgui::Panel *parent, const char *panelName);
	~CToolBar(void);

public:
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout(void);
	void PaintBackground(void);
};

#endif