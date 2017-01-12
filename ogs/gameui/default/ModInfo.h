#ifndef MODINFO_H
#define MODINFO_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>

class KeyValues;

class CModInfo
{
public:
	CModInfo(void);
	~CModInfo(void);

public:
	void FreeModInfo(void);
	void LoadCurrentGameInfo(void);
	void LoadGameInfoFromBuffer(const char *buffer);
	const wchar_t *GetGameTitle(void);
	const wchar_t *GetGameTitle2(void);
	const char *   GetGameName(void);
	bool           IsMultiplayerOnly(void);
	bool           IsSinglePlayerOnly(void);
	bool           HasPortals(void);
	bool           NoDifficulty(void);
	bool           NoModels(void);
	bool           NoHiModel(void);
	bool           NoCrosshair(void);
	bool           AdvCrosshair(void);
	const char *   GetFallbackDir(void);
	bool           UseGameLogo(void);
	KeyValues *    GetHiddenMaps(void);

private:
	wchar_t    m_wcsGameTitle[128];
	wchar_t    m_wcsGameTitle2[128];
	KeyValues *m_pModData;
};

extern CModInfo &ModInfo(void);
#endif