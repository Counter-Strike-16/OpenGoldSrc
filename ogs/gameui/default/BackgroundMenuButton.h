#ifndef BACKGROUNDMENUBUTTON_H
#define BACKGROUNDMENUBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>

class CBackgroundMenuButton : public vgui::Button
{
public:
	CBackgroundMenuButton(vgui::Panel *parent, const char *name);
	~CBackgroundMenuButton(void);

public:
	virtual void SetVisible(bool state);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

public:
	virtual void OnKillFocus(void);
	virtual void OnCommand(const char *command);

protected:
	vgui::Menu *RecursiveLoadGameMenu(KeyValues *datafile);
	vgui::Menu *m_pMenu;

private:
	vgui::IImage *m_pImage, *m_pMouseOverImage;
	typedef vgui::Button BaseClass;
};

#endif