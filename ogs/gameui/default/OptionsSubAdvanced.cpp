#include <metahook.h>

#include "EngineInterface.h"
#include "OptionsSubAdvanced.h"
#include "CvarSlider.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Slider.h>
#include "ivoicetweak.h"
#include "CvarToggleCheckButton.h"
#include "tier1/KeyValues.h"
#include "tier1/UtlVector.h"
#include "GameUI_Interface.h"

#include "configs.h"
#include "plugins.h"
#include "cmd.h"

using namespace vgui;

CUtlVector<char *> g_vLanguageList;

char *g_LanguageList[] =
{
  "english",
  "schinese",
  "tchinese",
  "macedonian"
};

COptionsSubAdvanced::COptionsSubAdvanced(vgui::Panel *parent)
    : PropertyPage(parent, NULL)
{
	GetLanguageList();

	g_pVGuiLocalize->AddFile("Resource/language_%language%.txt");

	m_pVersionInfo = new vgui::Label(this, "VersionInfo", "");
	m_pLanguage = new vgui::ComboBox(this, "Language", g_vLanguageList.Size(), true);

	LoadControlSettingsFromScheme("OptionsSubAdvanced.res");
	InitLanguageList(m_pLanguage);
	GetVersionInfo(m_pVersionInfo);

	if(g_bIsUseSteam)
		m_pLanguage->SetEnabled(false);
}

COptionsSubAdvanced::~COptionsSubAdvanced(void)
{
	for(int i = 0; i < g_vLanguageList.Size(); i++)
		free(g_vLanguageList[i]);

	g_vLanguageList.RemoveAll();
}

void COptionsSubAdvanced::OnPageShow(void)
{
}

void COptionsSubAdvanced::OnResetData(void)
{
}

void COptionsSubAdvanced::OnSliderMoved(int position)
{
}

void COptionsSubAdvanced::OnCheckButtonChecked(int state)
{
}

void COptionsSubAdvanced::OnThink(void)
{
	BaseClass::OnThink();
}

void COptionsSubAdvanced::OnApplyChanges(void)
{
	int i = m_pLanguage->GetActiveItem();
	KeyValues *data = m_pLanguage->GetActiveItemUserData();
	const char *language = data->GetString("language");

	if(strcmp(language, gConfigs.szLanguage))
	{
		char cmd[64];
		sprintf(cmd, "_setlanguage %s\n", language);
		engine->pfnClientCmd(cmd);
		engine->pfnClientCmd("_restart");
	}
}

void COptionsSubAdvanced::OnCommand(const char *command)
{
	BaseClass::OnCommand(command);
}

void COptionsSubAdvanced::GetLanguageList(void)
{
	int j = g_vLanguageList.AddToTail();
	g_vLanguageList[j] = strdup("english");

	int gamedir_len = strlen(gConfigs.szGameDir);

	char szWildCard[MAX_PATH];
	sprintf(szWildCard, "%s_*", gConfigs.szGameDir);

	WIN32_FIND_DATA findData;
	HANDLE findHandle = FindFirstFile(szWildCard, &findData);

	while(findHandle != INVALID_HANDLE_VALUE)
	{
		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			j = g_vLanguageList.AddToTail();
			g_vLanguageList[j] = strdup(&findData.cFileName[gamedir_len + 1]);
		}

		if(!FindNextFile(findHandle, &findData))
			break;
	}

	FindClose(findHandle);
}

char *UTIL_va(char *format, ...);

void COptionsSubAdvanced::InitLanguageList(vgui::ComboBox *cb)
{
	int initialLang = -1;
	int unknownLang = 0;

	KeyValues *data = new KeyValues("data");
	data->Clear();

	for(int i = 0; i < g_vLanguageList.Size(); i++)
	{
		if(!strcmp(gConfigs.szLanguage, g_vLanguageList[i]))
			initialLang = i;
	}

	if(initialLang == -1)
	{
		m_pLanguage->AddItem("* unknown", NULL);
		unknownLang = 1;
	}

	for(int i = 0; i < g_vLanguageList.Size(); i++)
	{
		data->SetString("language", g_vLanguageList[i]);

		char *tokenName = UTIL_va("#Language_%s", g_vLanguageList[i]);
		wchar_t *wtext = g_pVGuiLocalize->Find(tokenName);

		if(wtext)
			m_pLanguage->AddItem(wtext, data);
		else
			m_pLanguage->AddItem(UTIL_va("* %s", g_vLanguageList[i]), data);
	}

	m_pLanguage->ActivateItem(initialLang + unknownLang);
	data->deleteThis();
}

void COptionsSubAdvanced::GetVersionInfo(vgui::Label *l)
{
	char szBuffer[256];
	sprintf(szBuffer, "Game Version: %s\nEngine Buildnum: %d\nBuild Date: %s\nVideo Renderer: %s\n", gConfigs.szGameVersion, g_dwEngineBuildnum, __DATE__, g_iVideoMode == VIDEOMODE_OPENGL ? "OpenGL" : "Direct3D");
	l->SetText(szBuffer);
}

void COptionsSubAdvanced::OnPageHide(void)
{
	BaseClass::OnPageHide();
}

void COptionsSubAdvanced::OnControlModified(void)
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

void COptionsSubAdvanced::OnTextChanged(char const *text)
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}