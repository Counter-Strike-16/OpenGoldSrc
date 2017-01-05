#include "EngineInterface.h"
#include "ModInfo.h"
#include "KeyValues.h"
#include "vgui_controls/Controls.h"
#include "FileSystem.h"
#include "configs.h"
#include "encode.h"

CModInfo &ModInfo(void)
{
	static CModInfo s_ModInfo;
	return s_ModInfo;
}

CModInfo::CModInfo(void)
{
	m_pModData = new KeyValues("ModData");
	m_wcsGameTitle[0] = 0;

	char *p = strchr(gConfigs.szGameName, ':');
	int len1 = p - gConfigs.szGameName;

	wcsncpy(m_wcsGameTitle, ANSIToUnicode(gConfigs.szGameName), len1);
	m_wcsGameTitle[len1] = 0;

	while (*(++p) == ' ') {}

	wcscpy(m_wcsGameTitle2, ANSIToUnicode(p));
}

CModInfo::~CModInfo(void)
{
}

void CModInfo::FreeModInfo(void)
{
	if (m_pModData)
	{
		m_pModData->deleteThis();
		m_pModData = NULL;
	}
}

bool CModInfo::IsMultiplayerOnly(void)
{
	return true;
}

bool CModInfo::IsSinglePlayerOnly(void)
{
	return false;
}

const char *CModInfo::GetFallbackDir(void)
{
	return "cstrike";
}

const wchar_t *CModInfo::GetGameTitle(void)
{
	return m_wcsGameTitle;
}

const wchar_t *CModInfo::GetGameTitle2(void)
{
	return m_wcsGameTitle2;
}

const char *CModInfo::GetGameName(void)
{
	return gConfigs.szGameName;
}

KeyValues *CModInfo::GetHiddenMaps(void)
{
	return NULL;
}

bool CModInfo::HasPortals(void)
{
	return false;
}

bool CModInfo::NoDifficulty(void)
{
	return false;
}

bool CModInfo::NoModels(void)
{
	return true;
}

bool CModInfo::NoHiModel(void)
{
	return true;
}

bool CModInfo::NoCrosshair(void)
{
	return false;
}

bool CModInfo::AdvCrosshair(void)
{
	return false;
}

void CModInfo::LoadCurrentGameInfo(void)
{
}

void CModInfo::LoadGameInfoFromBuffer(const char *buffer)
{
	m_pModData->LoadFromBuffer("", buffer);
}

bool CModInfo::UseGameLogo(void)
{
	return true;
}