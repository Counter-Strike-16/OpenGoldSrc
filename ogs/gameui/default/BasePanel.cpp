#include "EngineInterface.h"
#include "BasePanel.h"

#include "vgui/IInputInternal.h"
#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "FileSystem.h"
#include "GameConsole.h"
#include "GameUI_Interface.h"

#include "IGameuiFuncs.h"

#include "vgui_controls/AnimationController.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/MenuItem.h"
#include "vgui_controls/PHandle.h"
#include "vgui_controls/MessageBox.h"
#include "vgui_controls/QueryBox.h"
#include "vgui_controls/ControllerMap.h"
#include "vgui_controls/KeyRepeat.h"

#include "tier1/fmtstr.h"

#include "ModInfo.h"
#include "LoadingDialog.h"
#include "BackgroundMenuButton.h"
#include "OptionsDialog.h"
#include "CreateMultiplayerGameDialog.h"
#include "BinkPanel.h"
#include "WebExplorer.h"
#include "ToolBar.h"
#include "CVARs.h"
#include "plugins.h"
#include <keydefs.h>

using namespace vgui;

extern vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;
extern CBasePanel *                  staticPanel;

static CBasePanel *g_pBasePanel         = NULL;
static float       g_flAnimationPadding = 0.01f;

CBasePanel *BasePanel(void)
{
	return g_pBasePanel;
}

VPANEL GetGameUIBasePanel(void)
{
	return BasePanel()->GetVPanel();
}

CGameMenuItem::CGameMenuItem(vgui::Menu *parent, const char *name)
    : BaseClass(parent, name, "GameMenuItem")
{
	m_bRightAligned = false;
}

void CGameMenuItem::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetFgColor(GetSchemeColor("MainMenu.TextColor", pScheme));
	SetBgColor(Color(0, 0, 0, 0));
	SetDefaultColor(GetSchemeColor("MainMenu.TextColor", pScheme), Color(0, 0, 0, 0));
	SetArmedColor(GetSchemeColor("MainMenu.ArmedTextColor", pScheme), Color(0, 0, 0, 0));
	SetDepressedColor(GetSchemeColor("MainMenu.DepressedTextColor", pScheme), Color(0, 0, 0, 0));
	SetContentAlignment(Label::a_west);
	SetBorder(NULL);
	SetDefaultBorder(NULL);
	SetDepressedBorder(NULL);
	SetKeyFocusBorder(NULL);

	vgui::HFont hMainMenuFont = pScheme->GetFont("MainMenuFont", IsProportional());

	if(hMainMenuFont)
		SetFont(hMainMenuFont);
	else
		SetFont(pScheme->GetFont("MenuLarge", IsProportional()));

	SetTextInset(0, 0);
	SetArmedSound("UI/buttonrollover.wav");
	SetDepressedSound("UI/buttonclick.wav");
	SetReleasedSound("UI/buttonclickrelease.wav");
	SetButtonActivationType(Button::ACTIVATE_ONPRESSED);

	if(m_bRightAligned)
		SetContentAlignment(Label::a_east);
}

void CGameMenuItem::PaintBackground(void)
{
	BaseClass::PaintBackground();
}

void CGameMenuItem::SetRightAlignedText(bool state)
{
	m_bRightAligned = state;
}

class CGameMenu : public vgui::Menu
{
	DECLARE_CLASS_SIMPLE(CGameMenu, vgui::Menu);

public:
	CGameMenu(vgui::Panel *parent, const char *name)
	    : BaseClass(parent, name)
	{
		m_pConsoleFooter = NULL;
	}

	virtual void ApplySchemeSettings(IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);

		SetMenuItemHeight(atoi(pScheme->GetResourceString("MainMenu.MenuItemHeight")));
		SetBgColor(Color(0, 0, 0, 0));
		SetBorder(NULL);
	}

	virtual void LayoutMenuBorder(void)
	{
	}

	virtual void SetVisible(bool state)
	{
		BaseClass::SetVisible(true);

		if(!state)
			ipanel()->MoveToBack(GetVPanel());
	}

	virtual int AddMenuItem(const char *itemName, const char *itemText, const char *command, Panel *target, KeyValues *userData = NULL)
	{
		if(!strcmp(command, "OpenServerBrowser") && !GameUI().IsServerBrowserValid())
			return 0;

		MenuItem *item = new CGameMenuItem(this, itemName);
		item->AddActionSignalTarget(target);
		item->SetCommand(command);
		item->SetText(itemText);
		item->SetUserData(userData);

		return BaseClass::AddMenuItem(item);
	}

	virtual void SetMenuItemBlinkingState(const char *itemName, bool state)
	{
		for(int i = 0; i < GetChildCount(); i++)
		{
			Panel *   child    = GetChild(i);
			MenuItem *menuItem = dynamic_cast<MenuItem *>(child);

			if(menuItem)
			{
				if(Q_strcmp(menuItem->GetCommand()->GetString("command", ""), itemName) == 0)
					menuItem->SetBlink(state);
			}
		}

		InvalidateLayout();
	}

	virtual void OnCommand(const char *command)
	{
		m_KeyRepeat.Reset();

		if(!stricmp(command, "Open"))
		{
			MoveToFront();
			RequestFocus();
		}
		else
			BaseClass::OnCommand(command);
	}

	virtual void OnKeyCodePressed(KeyCode code)
	{
		m_KeyRepeat.KeyDown(code);
#ifdef _DEBUG
		if(engine->pfnGetCvarFloat("developer") >= 1)
		{
			switch(code)
			{
			case KEY_F1:
			{
				engine->pfnClientCmd("connect 127.1:27015\n");
				break;
			}

			case KEY_F2:
			{
				engine->pfnClientCmd("connect 127.1:4242\n");
				break;
			}
			}
		}
#else
		if(code >= KEY_F1 && code <= KEY_F12)
		{
			const char *binding = gameuifuncs->Key_BindingForKey(K_F1 + (code - KEY_F1));

			if(binding && binding[0])
			{
				char szCommand[256];
				Q_strncpy(szCommand, binding, sizeof(szCommand));
				engine->pfnClientCmd(szCommand);
			}
		}
#endif
		BaseClass::OnKeyCodePressed(code);
	}

	void OnKeyCodeReleased(vgui::KeyCode code)
	{
		m_KeyRepeat.KeyUp(code);
		BaseClass::OnKeyCodeReleased(code);
	}

	void OnThink(void)
	{
		vgui::KeyCode code = m_KeyRepeat.KeyRepeated();

		if(code)
			OnKeyCodeTyped(code);

		BaseClass::OnThink();
	}

	virtual void OnKillFocus(void)
	{
		BaseClass::OnKillFocus();

		surface()->MovePopupToBack(GetVPanel());

		m_KeyRepeat.Reset();
	}

	void ShowFooter(bool bShow)
	{
		if(m_pConsoleFooter)
			m_pConsoleFooter->SetVisible(bShow);
	}

	void UpdateMenuItemState(bool isInGame, bool isMultiplayer)
	{
		for(int i = 0; i < GetChildCount(); i++)
		{
			Panel *   child    = GetChild(i);
			MenuItem *menuItem = dynamic_cast<MenuItem *>(child);

			if(menuItem)
			{
				bool       shouldBeVisible = true;
				KeyValues *kv              = menuItem->GetUserData();

				if(!kv)
					continue;

				if(!isInGame && kv->GetInt("OnlyInGame"))
					shouldBeVisible = false;
				else if(isMultiplayer && kv->GetInt("notmulti"))
					shouldBeVisible = false;
				else if(isInGame && !isMultiplayer && kv->GetInt("notsingle"))
					shouldBeVisible = false;
				else if(kv->GetInt("ConsoleOnly"))
					shouldBeVisible = false;

				menuItem->SetVisible(shouldBeVisible);
			}
		}

		if(!isInGame)
		{
			for(int j = 0; j < GetChildCount() - 2; j++)
				MoveMenuItem(j, j + 1);
		}
		else
		{
			for(int i = 0; i < GetChildCount(); i++)
			{
				for(int j = i; j < GetChildCount() - 2; j++)
				{
					int iID1 = GetMenuID(j);
					int iID2 = GetMenuID(j + 1);

					MenuItem *menuItem1 = GetMenuItem(iID1);
					MenuItem *menuItem2 = GetMenuItem(iID2);

					KeyValues *kv1 = menuItem1->GetUserData();
					KeyValues *kv2 = menuItem2->GetUserData();

					if(kv1->GetInt("InGameOrder") > kv2->GetInt("InGameOrder"))
						MoveMenuItem(iID2, iID1);
				}
			}
		}

		InvalidateLayout();

		if(m_pConsoleFooter)
		{
			const char *pHelpName;

			if(!isInGame)
				pHelpName = "MainMenu";
			else
				pHelpName = "GameMenu";

			if(!m_pConsoleFooter->GetHelpName() || V_stricmp(pHelpName, m_pConsoleFooter->GetHelpName()))
			{
				m_pConsoleFooter->SetHelpNameAndReset(pHelpName);
				m_pConsoleFooter->AddNewButtonLabel("#GameUI_Action", "#GameUI_Icons_A_BUTTON");

				if(isInGame)
					m_pConsoleFooter->AddNewButtonLabel("#GameUI_Close", "#GameUI_Icons_B_BUTTON");
			}
		}
	}

private:
	CFooterPanel *          m_pConsoleFooter;
	vgui::CKeyRepeatHandler m_KeyRepeat;
};

static CBackgroundMenuButton *CreateMenuButton(CBasePanel *parent, const char *panelName, const wchar_t *panelText)
{
	CBackgroundMenuButton *pButton = new CBackgroundMenuButton(parent, panelName);
	pButton->SetProportional(true);
	pButton->SetCommand("OpenGameMenu");
	pButton->SetText(panelText);

	return pButton;
}

CBasePanel::CBasePanel(void)
    : Panel(NULL, "BaseGameUIPanel")
{
	g_pBasePanel                     = this;
	m_bLevelLoading                  = false;
	m_eBackgroundState               = BACKGROUND_INITIAL;
	m_flTransitionStartTime          = 0.0f;
	m_flTransitionEndTime            = 0.0f;
	m_flFrameFadeInTime              = 0.5f;
	m_bRenderingBackgroundTransition = false;
	m_bFadingInMenus                 = false;
	m_bEverActivated                 = false;
	m_iGameMenuInset                 = 24;
	m_bHaveDarkenedBackground        = false;
	m_bHaveDarkenedTitleText         = true;
	m_bForceTitleTextUpdate          = true;
	m_BackdropColor                  = Color(0, 0, 0, 128);
	m_pConsoleAnimationController    = NULL;
	m_pConsoleControlSettings        = NULL;
	m_iToolBarSize                   = 40;
	m_bInitialLoading                = true;

	m_pGameMenuButtons.AddToTail(CreateMenuButton(this, "GameMenuButton", ModInfo().GetGameTitle()));
	m_pGameMenuButtons.AddToTail(CreateMenuButton(this, "GameMenuButton2", ModInfo().GetGameTitle2()));

	m_pGameMenu = NULL;
	m_pGameLogo = NULL;

	CreateGameMenu();
	CreateGameLogo();
	CreateBinkPanel();
	CreateBackGround();
	CreateToolbar();

	SetMenuAlpha(0);

	m_pFocusParent = NULL;
	m_pFocusPanel  = NULL;
}

KeyValues *CBasePanel::GetConsoleControlSettings(void)
{
	return m_pConsoleControlSettings;
}

CBasePanel::~CBasePanel(void)
{
	g_pBasePanel = NULL;

	if(m_pBinkPanel)
		delete m_pBinkPanel;
}

void CBasePanel::PaintBackground(void)
{
	if(!m_hOptionsDialog.Get())
	{
		m_hOptionsDialog               = new COptionsDialog(this);
		m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog(this);

		PositionDialog(m_hOptionsDialog);
		PositionDialog(m_hCreateMultiplayerGameDialog);
	}

	if(!GameUI().IsInLevel() || g_hLoadingDialog.Get())
	{
		DrawBackgroundImage();
	}
	else
	{
		if(m_pBinkPanel)
			m_pBinkPanel->SetVisible(false);
	}

	if(m_flBackgroundFillAlpha)
	{
		int swide, stall;
		surface()->GetScreenSize(swide, stall);
		surface()->DrawSetColor(0, 0, 0, m_flBackgroundFillAlpha);
		surface()->DrawFilledRect(0, 0, swide, stall);
	}
}

void CBasePanel::SetVisible(bool state)
{
	BaseClass::SetVisible(state);
}

bool CBasePanel::IsMenuFading(void)
{
	return m_bFadingInMenus;
}

bool CBasePanel::IsInitialLoading(void)
{
	return m_bInitialLoading;
}

void CBasePanel::UpdateBackgroundState(void)
{
	GameConsole().SetParent(GetVPanel());

	if(GameUI().IsInLevel())
	{
		SetBackgroundRenderState(BACKGROUND_LEVEL);
	}
	else if(!m_bLevelLoading)
	{
		if(IsPC())
			SetBackgroundRenderState(BACKGROUND_MAINMENU);
	}
	else if(m_bLevelLoading && g_hLoadingDialog.Get())
	{
		SetBackgroundRenderState(BACKGROUND_LOADING);
	}
	else if(m_bEverActivated)
	{
		SetBackgroundRenderState(BACKGROUND_DISCONNECTED);
	}

	bool bHaveActiveDialogs = false;
	bool bIsInLevel         = GameUI().IsInLevel();

	for(int i = 0; i < GetChildCount(); ++i)
	{
		VPANEL      child = ipanel()->GetChild(GetVPanel(), i);
		const char *name  = ipanel()->GetName(child);

		if(child && ipanel()->IsVisible(child) && ipanel()->IsPopup(child) && child != m_pGameMenu->GetVPanel())
		{
			bHaveActiveDialogs = true;
			break;
		}
	}

	if(!bHaveActiveDialogs)
	{
		VPANEL parent = GetVParent();

		for(int i = 0; i < ipanel()->GetChildCount(parent); ++i)
		{
			VPANEL child = ipanel()->GetChild(parent, i);

			if(child && ipanel()->IsVisible(child) && ipanel()->IsPopup(child) && child != GetVPanel())
			{
				bHaveActiveDialogs = true;
				break;
			}
		}
	}

	bool bNeedDarkenedBackground = (bHaveActiveDialogs || bIsInLevel);

	if(m_bHaveDarkenedBackground != bNeedDarkenedBackground)
	{
		float targetAlpha, duration;

		if(bNeedDarkenedBackground || m_eBackgroundState == BACKGROUND_LOADING)
		{
			targetAlpha = m_BackdropColor[3];
			duration    = m_flFrameFadeInTime;
		}
		else
		{
			targetAlpha = 0.0f;
			duration    = 2.0f;
		}

		m_bHaveDarkenedBackground = bNeedDarkenedBackground;

		if(g_bIsDebuggerPresent)
			duration = 0.1f;

		GetAnimationController()->RunAnimationCommand(this, "m_flBackgroundFillAlpha", targetAlpha, 0.0f, duration, AnimationController::INTERPOLATOR_LINEAR);
	}

	if(m_bLevelLoading)
		return;

	bool bNeedDarkenedTitleText = bHaveActiveDialogs;

	if(m_bHaveDarkenedTitleText != bNeedDarkenedTitleText || m_bForceTitleTextUpdate)
	{
		float targetTitleAlpha, duration;

		if(bHaveActiveDialogs || m_eBackgroundState == BACKGROUND_LOADING)
		{
			duration         = m_flFrameFadeInTime;
			targetTitleAlpha = 128.0f;
		}
		else
		{
			duration         = 2.0f;
			targetTitleAlpha = 255.0f;
		}

		if(g_bIsDebuggerPresent)
			duration = 0.1f;

		if(m_pBinkPanel)
			GetAnimationController()->RunAnimationCommand(m_pBinkPanel, "alpha", targetTitleAlpha, 0.0f, duration, AnimationController::INTERPOLATOR_LINEAR);

		if(m_pGameLogo)
			GetAnimationController()->RunAnimationCommand(m_pGameLogo, "alpha", targetTitleAlpha, 0.0f, duration, AnimationController::INTERPOLATOR_LINEAR);

		if(m_pGameMenu)
			GetAnimationController()->RunAnimationCommand(m_pGameMenu, "alpha", targetTitleAlpha, 0.0f, duration, AnimationController::INTERPOLATOR_LINEAR);

		for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
			GetAnimationController()->RunAnimationCommand(m_pGameMenuButtons[i], "alpha", targetTitleAlpha, 0.0f, duration, AnimationController::INTERPOLATOR_LINEAR);

		m_bFadingInMenus         = false;
		m_bHaveDarkenedTitleText = bNeedDarkenedTitleText;
		m_bForceTitleTextUpdate  = false;
	}
}

void CBasePanel::SetBackgroundRenderState(EBackgroundState state)
{
	if(state == m_eBackgroundState)
		return;

	float frametime = engine->GetAbsoluteTime();

	m_bRenderingBackgroundTransition = false;
	m_bFadingInMenus                 = false;

	if(state == BACKGROUND_EXITING)
	{
	}
	else if(state == BACKGROUND_DISCONNECTED || state == BACKGROUND_MAINMENU)
	{
		m_bFadingInMenus      = true;
		m_flFadeMenuStartTime = frametime;
		m_flFadeMenuEndTime   = frametime + 3.0f;

		if(m_pBinkPanel)
			m_pBinkPanel->SetVisible(true);
	}
	else if(state == BACKGROUND_LOADING)
	{
		SetMenuAlpha(0);

		if(m_pBinkPanel)
			m_pBinkPanel->SetVisible(false);
	}
	else if(state == BACKGROUND_LEVEL)
	{
		SetMenuAlpha(255);

		if(m_pBinkPanel)
			m_pBinkPanel->SetVisible(false);
	}

	m_eBackgroundState = state;
}

void CBasePanel::OnSizeChanged(int newWide, int newTall)
{
}

void CBasePanel::OnLevelLoadingStarted(const char *levelName)
{
	m_bLevelLoading = true;
	m_pGameMenu->ShowFooter(false);

	static char imageName[MAX_PATH];
	sprintf(imageName, "resource/maploading/loadingbg_%s", levelName);

	if(!g_hLoadingDialog.Get())
		g_hLoadingDialog = new CLoadingDialog(this);

	g_hLoadingDialog->SetBackgroundImage(imageName);
}

void CBasePanel::OnLevelLoadingFinished(void)
{
	m_bLevelLoading = false;
}

void CBasePanel::DrawBackgroundImage(void)
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	int wide, tall;
	GetSize(wide, tall);

	float frametime = engine->GetAbsoluteTime();
	int   alpha     = 255;

	if(m_bRenderingBackgroundTransition)
	{
		alpha = (m_flTransitionEndTime - frametime) / (m_flTransitionEndTime - m_flTransitionStartTime) * 255;
		alpha = clamp(alpha, 0, 255);
	}

	if(m_pBinkPanel)
	{
		float playtime = m_pBinkPanel->GetPlayTime();

		if(!g_pcvarEnableBink->value || (!playtime || (IsPC() && (m_bRenderingBackgroundTransition || m_eBackgroundState == BACKGROUND_LOADING))))
		{
			float xScale = swide / 800.0f;
			float yScale = stall / 600.0f;

			int ypos = 0;

			for(int y = 0; y < BACKGROUND_ROWS; y++)
			{
				int xpos = 0;

				for(int x = 0; x < BACKGROUND_COLUMNS; x++)
				{
					bimage_t &bimage = m_ImageID[y][x];

					int dx = (int)ceil(xpos * xScale);
					int dy = (int)ceil(ypos * yScale);
					int dw = (int)ceil((xpos + bimage.width) * xScale);
					int dt = (int)ceil((ypos + bimage.height) * yScale);

					if(x == 0)
						dx = 0;

					if(y == 0)
						dy = 0;

					surface()->DrawSetColor(255, 255, 255, alpha);
					surface()->DrawSetTexture(bimage.imageID);
					surface()->DrawTexturedRect(dx, dy, dw, dt);

					xpos += bimage.width;
				}

				ypos += m_ImageID[y][0].height;
			}

			m_pBinkPanel->SetVisible(false);
		}
		else
		{
			surface()->DrawSetColor(0, 0, 0, 255);
			surface()->DrawFilledRect(0, 0, swide, stall);

			if(!m_pBinkPanel->IsPlaying())
				m_pBinkPanel->Play();

			m_pBinkPanel->SetVisible(true);
		}
	}

	if(IsPC() && (m_bRenderingBackgroundTransition || m_eBackgroundState == BACKGROUND_LOADING))
	{
		if(m_pGameMenu->GetAlpha() < 255)
		{
			surface()->DrawSetColor(255, 255, 255, alpha);
			surface()->DrawSetTexture(m_iLoadingImageID);

			int twide, ttall;
			surface()->DrawGetTextureSize(m_iLoadingImageID, twide, ttall);
			surface()->DrawTexturedRect(wide - twide, tall - ttall, wide, tall);
		}
	}

	if(m_bFadingInMenus)
	{
		alpha = (frametime - m_flFadeMenuStartTime) / (m_flFadeMenuEndTime - m_flFadeMenuStartTime) * 255;
		alpha = clamp(alpha, 0, 255);

		for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
			m_pGameMenuButtons[i]->SetAlpha(alpha);

		if(alpha == 255)
			m_bFadingInMenus = false;

		m_pGameMenu->SetAlpha(alpha);
	}
}

void CBasePanel::CreateGameMenu(void)
{
	KeyValues *datafile = new KeyValues("GameMenu");
	datafile->UsesEscapeSequences(true);

	if(datafile->LoadFromFile(g_pFullFileSystem, "Resource/GameMenu.res"))
		m_pGameMenu = RecursiveLoadGameMenu(datafile);

	if(!m_pGameMenu)
	{
		Error("Could not load file Resource/GameMenu.res");
	}
	else
	{
		SETUP_PANEL(m_pGameMenu);
		m_pGameMenu->SetAlpha(0);
	}

	datafile->deleteThis();
}

void CBasePanel::CreateGameLogo(void)
{
	if(ModInfo().UseGameLogo())
	{
		m_pGameLogo = new CMainMenuGameLogo(this, "GameLogo");

		if(m_pGameLogo)
		{
			SETUP_PANEL(m_pGameLogo);
			m_pGameLogo->InvalidateLayout(true, true);
			m_pGameLogo->SetAlpha(0);
		}
	}
	else
		m_pGameLogo = NULL;
}

void CBasePanel::CreateBinkPanel(void)
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	m_pBinkPanel = new CBinkPanel(this, "BinkPlayer");

	if(!m_pBinkPanel->OpenBink("resource/bg_bink.bik", BINKSURFACE24R))
	{
		delete m_pBinkPanel;
		m_pBinkPanel = NULL;
		return;
	}

	m_pBinkPanel->SetBounds(0, 0, swide, stall);
	m_pBinkPanel->SetVisible(true);
	m_pBinkPanel->SetZPos(-1);
	m_pBinkPanel->SetMouseInputEnabled(false);
	m_pBinkPanel->SetKeyBoardInputEnabled(false);
}

void CBasePanel::CreateBackGround(void)
{
}

void CBasePanel::CreateToolbar(void)
{
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	m_pToolBar = new CToolBar(this, "ToolBar");
	m_pToolBar->SetZPos(-20);
	m_pToolBar->SetVisible(false);
	m_pToolBar->SetBounds(0, stall - m_iToolBarSize, swide, m_iToolBarSize);
}

void CBasePanel::UpdateGameMenus(void)
{
	bool isInGame = GameUI().IsInLevel();
	bool isMulti  = isInGame && (engine->GetMaxClients() > 1);

	m_pGameMenu->UpdateMenuItemState(isInGame, isMulti);
	m_pGameMenu->SetVisible(true);

	InvalidateLayout();
}

CGameMenu *CBasePanel::RecursiveLoadGameMenu(KeyValues *datafile)
{
	CGameMenu *menu = new CGameMenu(this, datafile->GetName());

	for(KeyValues *dat = datafile->GetFirstSubKey(); dat != NULL; dat = dat->GetNextKey())
	{
		const char *label = dat->GetString("label", "<unknown>");
		const char *cmd   = dat->GetString("command", NULL);
		const char *name  = dat->GetString("name", label);

		menu->AddMenuItem(name, label, cmd, this, dat);
	}

	return menu;
}

void CBasePanel::RunFrame(void)
{
	if(!IsVisible())
		return;

	if(surface()->GetModalPanel())
		surface()->PaintTraverse(GetVPanel());

	GetAnimationController()->UpdateAnimations(engine->GetAbsoluteTime());
	UpdateBackgroundState();
}

void CBasePanel::PerformLayout(void)
{
	BaseClass::PerformLayout();

	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);

	int menuWide, menuTall;
	m_pGameMenu->GetSize(menuWide, menuTall);

	int idealMenuY = tall - menuTall - m_iGameMenuInset;
	int yDiff      = idealMenuY - m_iGameMenuPos.y;

	for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
	{
		m_pGameMenuButtons[i]->SizeToContents();
		m_pGameMenuButtons[i]->SetPos(m_iGameTitlePos[i].x, m_iGameTitlePos[i].y + yDiff);
	}

	if(m_pGameLogo)
		m_pGameLogo->SetPos(m_iGameMenuPos.x + m_pGameLogo->GetOffsetX(), idealMenuY - m_pGameLogo->GetTall() + m_pGameLogo->GetOffsetY());

	m_pGameMenu->SetPos(m_iGameMenuPos.x, idealMenuY);

	if(m_bInitialLoading)
	{
		m_bInitialLoading = false;
		GameConsole().CheckPending();
	}

	UpdateGameMenus();
}

void CBasePanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_iGameMenuInset = atoi(pScheme->GetResourceString("MainMenu.Inset"));
	m_iGameMenuInset *= 2;

	CUtlVector<Color> buttonColor;

	if(pScheme)
	{
		m_iGameTitlePos.RemoveAll();

		for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
		{
			m_pGameMenuButtons[i]->SetFont(pScheme->GetFont("TitleFont"));

			m_iGameTitlePos.AddToTail(coord());
			m_iGameTitlePos[i].x = atoi(pScheme->GetResourceString(CFmtStr("Main.Title%d.X", i + 1)));
			m_iGameTitlePos[i].x = scheme()->GetProportionalScaledValue(m_iGameTitlePos[i].x);
			m_iGameTitlePos[i].y = atoi(pScheme->GetResourceString(CFmtStr("Main.Title%d.Y", i + 1)));
			m_iGameTitlePos[i].y = scheme()->GetProportionalScaledValue(m_iGameTitlePos[i].y);

			buttonColor.AddToTail(pScheme->GetColor(CFmtStr("Main.Title%d.Color", i + 1), Color(255, 255, 255, 255)));
		}

		m_iGameMenuPos.x = atoi(pScheme->GetResourceString("Main.Menu.X"));
		m_iGameMenuPos.x = scheme()->GetProportionalScaledValue(m_iGameMenuPos.x);
		m_iGameMenuPos.y = atoi(pScheme->GetResourceString("Main.Menu.Y"));
		m_iGameMenuPos.y = scheme()->GetProportionalScaledValue(m_iGameMenuPos.y);

		m_iGameMenuInset = atoi(pScheme->GetResourceString("Main.BottomBorder"));
		m_iGameMenuInset = scheme()->GetProportionalScaledValue(m_iGameMenuInset);
	}
	else
	{
		for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
		{
			m_pGameMenuButtons[i]->SetFont(pScheme->GetFont("TitleFont"));

			buttonColor.AddToTail(Color(255, 255, 255, 255));
		}
	}

	for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
	{
		m_pGameMenuButtons[i]->SetDefaultColor(buttonColor[i], Color(0, 0, 0, 0));
		m_pGameMenuButtons[i]->SetArmedColor(buttonColor[i], Color(0, 0, 0, 0));
		m_pGameMenuButtons[i]->SetDepressedColor(buttonColor[i], Color(0, 0, 0, 0));
	}

	SetBgColor(Color(0, 0, 0, 0));

	m_flFrameFadeInTime = atof(pScheme->GetResourceString("Frame.TransitionEffectTime"));
	m_BackdropColor     = pScheme->GetColor("mainmenu.backdrop", Color(0, 0, 0, 128));

	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);

	float aspectRatio   = (float)screenWide / (float)screenTall;
	bool  bIsWidescreen = aspectRatio >= 1.5999f;

	for(int y = 0; y < BACKGROUND_ROWS; y++)
	{
		for(int x = 0; x < BACKGROUND_COLUMNS; x++)
		{
			bimage_t &bimage = m_ImageID[y][x];
			bimage.imageID   = surface()->CreateNewTextureID();

			char filename[MAX_PATH];
			sprintf(filename, "resource/background/800_%d_%c_loading", y + 1, 'a' + x);
			surface()->DrawSetTextureFile(bimage.imageID, filename, false, false);
			surface()->DrawGetTextureSize(bimage.imageID, bimage.width, bimage.height);
		}
	}

	if(IsPC())
	{
		m_iLoadingImageID = surface()->CreateNewTextureID();
		surface()->DrawSetTextureFile(m_iLoadingImageID, "gfx/vgui/console/startup_loading", false, false);
	}
}

void CBasePanel::OnActivateModule(int moduleIndex)
{
}

void CBasePanel::OnGameUIActivated(void)
{
	if(!m_bEverActivated)
	{
		UpdateGameMenus();
		m_bEverActivated = true;
	}

	if(GameUI().IsInLevel())
		OnCommand("OpenPauseMenu");
}

void CBasePanel::RunMenuCommand(const char *command)
{
	if(!Q_stricmp(command, "OpenServerBrowser"))
	{
		OnOpenServerBrowser();
	}
	else if(!Q_stricmp(command, "OpenCreateMultiplayerGameDialog"))
	{
		OnOpenCreateMultiplayerGameDialog();
	}
	else if(!Q_stricmp(command, "OpenOptionsDialog"))
	{
		OnOpenOptionsDialog();
	}
	else if(!Q_stricmp(command, "ResumeGame"))
	{
		engine->pfnClientCmd("cancelselect");
	}
	else if(!Q_stricmp(command, "Disconnect"))
	{
		engine->pfnClientCmd("disconnect\n");
	}
	else if(!Q_stricmp(command, "Quit"))
	{
		OnOpenQuitConfirmationDialog();
	}
	else if(!Q_stricmp(command, "QuitNoConfirm"))
	{
		SetVisible(false);
		vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
		engine->pfnClientCmd("quit\n");
	}
	else if(!Q_stricmp(command, "ReleaseModalWindow"))
	{
		vgui::surface()->RestrictPaintToSinglePanel(NULL);
	}
	else
		BaseClass::OnCommand(command);
}

void CBasePanel::OnCommand(const char *command)
{
	RunMenuCommand(command);
}

void CBasePanel::RunAnimationWithCallback(vgui::Panel *parent, const char *animName, KeyValues *msgFunc)
{
	if(!m_pConsoleAnimationController)
		return;

	m_pConsoleAnimationController->StartAnimationSequence(animName);

	float sequenceLength = m_pConsoleAnimationController->GetAnimationSequenceLength(animName);

	if(sequenceLength)
		sequenceLength += g_flAnimationPadding;

	if(parent && msgFunc)
		PostMessage(parent, msgFunc, sequenceLength);
}

class CQuitQueryBox : public vgui::QueryBox
{
	DECLARE_CLASS_SIMPLE(CQuitQueryBox, vgui::QueryBox);

public:
	CQuitQueryBox(const char *title, const char *info, Panel *parent)
	    : BaseClass(title, info, parent)
	{
	}

	void DoModal(Frame *pFrameOver)
	{
		BaseClass::DoModal(pFrameOver);
		vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
	}

	void OnKeyCodePressed(KeyCode code)
	{
		if(code == KEY_ESCAPE)
		{
			SetAlpha(0);
			Close();
		}
		else
			BaseClass::OnKeyCodePressed(code);
	}

	virtual void OnClose(void)
	{
		BaseClass::OnClose();
		vgui::surface()->RestrictPaintToSinglePanel(NULL);
	}
};

void CBasePanel::OnOpenQuitConfirmationDialog(void)
{
	if(GameUI().IsInLevel() && engine->GetMaxClients() == 1)
	{
	}
	else
	{
		if(!m_hQuitQueryBox.Get())
		{
			m_hQuitQueryBox = new CQuitQueryBox("#GameUI_QuitConfirmationTitle", "#GameUI_QuitConfirmationText", this);
			m_hQuitQueryBox->SetOKButtonText("#GameUI_Quit");
			m_hQuitQueryBox->SetOKCommand(new KeyValues("Command", "command", "QuitNoConfirm"));
			m_hQuitQueryBox->SetCancelCommand(new KeyValues("Command", "command", "ReleaseModalWindow"));
			m_hQuitQueryBox->AddActionSignalTarget(this);
			m_hQuitQueryBox->DoModal();
		}
	}
}

void CBasePanel::OnOpenServerBrowser(void)
{
	GameUI().ActivateServerBrowser();
}

void CBasePanel::OnOpenOptionsDialog(void)
{
	if(!m_hOptionsDialog.Get())
	{
		m_hOptionsDialog = new COptionsDialog(this);
		PositionDialog(m_hOptionsDialog);
	}

	m_hOptionsDialog->Activate();
}

void CBasePanel::OnOpenCreateMultiplayerGameDialog(void)
{
	if(!m_hCreateMultiplayerGameDialog.Get())
	{
		m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog(this);
		PositionDialog(m_hCreateMultiplayerGameDialog);
	}

	m_hCreateMultiplayerGameDialog->Activate();
}

void CBasePanel::PositionDialog(vgui::PHandle dlg)
{
	if(!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds(x, y, ww, wt);
	dlg->GetSize(wide, tall);
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}

void CBasePanel::OnGameUIHidden(void)
{
	if(m_hOptionsDialog.Get())
		PostMessage(m_hOptionsDialog.Get(), new KeyValues("GameUIHidden"));
}

void CBasePanel::SetMenuAlpha(int alpha)
{
	m_pGameMenu->SetAlpha(alpha);

	if(m_pGameLogo)
		m_pGameLogo->SetAlpha(alpha);

	for(int i = 0; i < m_pGameMenuButtons.Count(); ++i)
		m_pGameMenuButtons[i]->SetAlpha(alpha);

	m_bForceTitleTextUpdate = true;
}

void CBasePanel::SetMenuItemBlinkingState(const char *itemName, bool state)
{
	for(int i = 0; i < GetChildCount(); i++)
	{
		Panel *    child     = GetChild(i);
		CGameMenu *pGameMenu = dynamic_cast<CGameMenu *>(child);

		if(pGameMenu)
			pGameMenu->SetMenuItemBlinkingState(itemName, state);
	}
}

void CBasePanel::RunEngineCommand(const char *command)
{
	engine->pfnClientCmd((char *)command);
}

void CBasePanel::RunCloseAnimation(const char *animName)
{
	RunAnimationWithCallback(this, animName, new KeyValues("FinishDialogClose"));
}

void CBasePanel::FinishDialogClose(void)
{
}

CFooterPanel::CFooterPanel(Panel *parent, const char *panelName)
    : BaseClass(parent, panelName)
{
	SetVisible(true);
	SetAlpha(0);

	m_pHelpName    = NULL;
	m_pSizingLabel = new vgui::Label(this, "SizingLabel", "");
	m_pSizingLabel->SetVisible(false);

	m_nButtonGap        = 32;
	m_nButtonGapDefault = 32;
	m_ButtonPinRight    = 100;
	m_FooterTall        = 80;

	int wide, tall;
	surface()->GetScreenSize(wide, tall);

	if(tall <= 480)
		m_FooterTall = 60;

	m_ButtonOffsetFromTop = 0;
	m_ButtonSeparator     = 4;
	m_TextAdjust          = 0;

	m_bPaintBackground  = false;
	m_bCenterHorizontal = false;

	m_szButtonFont[0] = '\0';
	m_szTextFont[0]   = '\0';
	m_szFGColor[0]    = '\0';
	m_szBGColor[0]    = '\0';
}

CFooterPanel::~CFooterPanel(void)
{
	SetHelpNameAndReset(NULL);

	delete m_pSizingLabel;
}

void CFooterPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_hButtonFont = pScheme->GetFont((m_szButtonFont[0] != '\0') ? m_szButtonFont : "GameUIButtons");
	m_hTextFont   = pScheme->GetFont((m_szTextFont[0] != '\0') ? m_szTextFont : "MenuLarge");

	SetFgColor(pScheme->GetColor(m_szFGColor, Color(255, 255, 255, 255)));
	SetBgColor(pScheme->GetColor(m_szBGColor, Color(0, 0, 0, 255)));

	int x, y, w, h;
	GetParent()->GetBounds(x, y, w, h);
	SetBounds(x, h - m_FooterTall, w, m_FooterTall);
}

void CFooterPanel::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_nButtonGap          = inResourceData->GetInt("buttongap", 32);
	m_nButtonGapDefault   = m_nButtonGap;
	m_ButtonPinRight      = inResourceData->GetInt("button_pin_right", 100);
	m_FooterTall          = inResourceData->GetInt("tall", 80);
	m_ButtonOffsetFromTop = inResourceData->GetInt("buttonoffsety", 0);
	m_ButtonSeparator     = inResourceData->GetInt("button_separator", 4);
	m_TextAdjust          = inResourceData->GetInt("textadjust", 0);

	m_bCenterHorizontal = (inResourceData->GetInt("center", 0) == 1);
	m_bPaintBackground  = (inResourceData->GetInt("paintbackground", 0) == 1);

	Q_strncpy(m_szTextFont, inResourceData->GetString("fonttext", "MenuLarge"), sizeof(m_szTextFont));
	Q_strncpy(m_szButtonFont, inResourceData->GetString("fontbutton", "GameUIButtons"), sizeof(m_szButtonFont));

	Q_strncpy(m_szFGColor, inResourceData->GetString("fgcolor", "White"), sizeof(m_szFGColor));
	Q_strncpy(m_szBGColor, inResourceData->GetString("bgcolor", "Black"), sizeof(m_szBGColor));

	for(KeyValues *pButton = inResourceData->GetFirstSubKey(); pButton != NULL; pButton = pButton->GetNextKey())
	{
		const char *pName = pButton->GetName();

		if(!Q_stricmp(pName, "button"))
		{
			const char *pText = pButton->GetString("text", "NULL");
			const char *pIcon = pButton->GetString("icon", "NULL");
			AddNewButtonLabel(pText, pIcon);
		}
	}

	InvalidateLayout(false, true);
}

void CFooterPanel::AddButtonsFromMap(vgui::Frame *pMenu)
{
	CControllerMap *pMap = dynamic_cast<CControllerMap *>(pMenu->FindChildByName("ControllerMap"));

	if(pMap)
	{
		int buttonCt = pMap->NumButtons();

		for(int i = 0; i < buttonCt; ++i)
		{
			const char *pText = pMap->GetBindingText(i);

			if(pText)
				AddNewButtonLabel(pText, pMap->GetBindingIcon(i));
		}
	}

	SetHelpNameAndReset(pMenu->GetName());
}

void CFooterPanel::SetStandardDialogButtons(void)
{
	SetHelpNameAndReset("Dialog");
	AddNewButtonLabel("#GameUI_Action", "#GameUI_Icons_A_BUTTON");
	AddNewButtonLabel("#GameUI_Close", "#GameUI_Icons_B_BUTTON");
}

void CFooterPanel::SetHelpNameAndReset(const char *pName)
{
	if(m_pHelpName)
	{
		free(m_pHelpName);
		m_pHelpName = NULL;
	}

	if(pName)
		m_pHelpName = strdup(pName);

	ClearButtons();
}

const char *CFooterPanel::GetHelpName(void)
{
	return m_pHelpName;
}

void CFooterPanel::ClearButtons(void)
{
	m_ButtonLabels.PurgeAndDeleteElements();
}

void CFooterPanel::AddNewButtonLabel(const char *text, const char *icon)
{
	ButtonLabel_t *button = new ButtonLabel_t;

	Q_strncpy(button->name, text, MAX_PATH);
	button->bVisible = true;

	wchar_t *pIcon = g_pVGuiLocalize->Find(icon);

	if(pIcon)
	{
		button->icon[0] = pIcon[0];
		button->icon[1] = '\0';
	}
	else
		button->icon[0] = '\0';

	wchar_t *pText = g_pVGuiLocalize->Find(text);

	if(pText)
		wcsncpy(button->text, pText, wcslen(pText) + 1);
	else
		button->text[0] = '\0';

	m_ButtonLabels.AddToTail(button);
}

void CFooterPanel::ShowButtonLabel(const char *name, bool show)
{
	for(int i = 0; i < m_ButtonLabels.Count(); ++i)
	{
		if(!Q_stricmp(m_ButtonLabels[i]->name, name))
		{
			m_ButtonLabels[i]->bVisible = show;
			break;
		}
	}
}

void CFooterPanel::SetButtonText(const char *buttonName, const char *text)
{
	for(int i = 0; i < m_ButtonLabels.Count(); ++i)
	{
		if(!Q_stricmp(m_ButtonLabels[i]->name, buttonName))
		{
			wchar_t *wtext = g_pVGuiLocalize->Find(text);

			if(text)
				wcsncpy(m_ButtonLabels[i]->text, wtext, wcslen(wtext) + 1);
			else
				m_ButtonLabels[i]->text[0] = '\0';

			break;
		}
	}
}

void CFooterPanel::PaintBackground(void)
{
	if(!m_bPaintBackground)
		return;

	BaseClass::PaintBackground();
}

void CFooterPanel::Paint(void)
{
	int wide  = GetWide();
	int right = wide - m_ButtonPinRight;

	int buttonHeight = vgui::surface()->GetFontTall(m_hButtonFont);
	int fontHeight   = vgui::surface()->GetFontTall(m_hTextFont);
	int textY        = (buttonHeight - fontHeight) / 2 + m_TextAdjust;

	if(textY < 0)
		textY = 0;

	int y = m_ButtonOffsetFromTop;

	if(!m_bCenterHorizontal)
	{
		int x = right;

		for(int i = 0; i < m_ButtonLabels.Count(); ++i)
		{
			ButtonLabel_t *pButton = m_ButtonLabels[i];

			if(!pButton->bVisible)
				continue;

			m_pSizingLabel->SetFont(m_hTextFont);
			m_pSizingLabel->SetText(pButton->text);
			m_pSizingLabel->SizeToContents();

			int iTextWidth = m_pSizingLabel->GetWide();

			if(iTextWidth == 0)
				x += m_nButtonGap;
			else
				x -= iTextWidth;

			vgui::surface()->DrawSetTextFont(m_hTextFont);
			vgui::surface()->DrawSetTextColor(GetFgColor());
			vgui::surface()->DrawSetTextPos(x, y + textY);
			vgui::surface()->DrawPrintText(pButton->text, wcslen(pButton->text));

			x -= (vgui::surface()->GetCharacterWidth(m_hButtonFont, pButton->icon[0]) + m_ButtonSeparator);

			vgui::surface()->DrawSetTextFont(m_hButtonFont);
			vgui::surface()->DrawSetTextColor(255, 255, 255, 255);
			vgui::surface()->DrawSetTextPos(x, y);
			vgui::surface()->DrawPrintText(pButton->icon, 1);

			x -= m_nButtonGap;
		}
	}
	else
	{
		int x            = wide / 2;
		int totalWidth   = 0;
		int i            = 0;
		int nButtonCount = 0;

		for(i = 0; i < m_ButtonLabels.Count(); ++i)
		{
			ButtonLabel_t *pButton = m_ButtonLabels[i];

			if(!pButton->bVisible)
				continue;

			m_pSizingLabel->SetFont(m_hTextFont);
			m_pSizingLabel->SetText(pButton->text);
			m_pSizingLabel->SizeToContents();

			totalWidth += vgui::surface()->GetCharacterWidth(m_hButtonFont, pButton->icon[0]);
			totalWidth += m_ButtonSeparator;
			totalWidth += m_pSizingLabel->GetWide();

			nButtonCount++;
		}

		totalWidth += (nButtonCount - 1) * m_nButtonGap;
		x -= (totalWidth / 2);

		for(i = 0; i < m_ButtonLabels.Count(); ++i)
		{
			ButtonLabel_t *pButton = m_ButtonLabels[i];

			if(!pButton->bVisible)
				continue;

			m_pSizingLabel->SetFont(m_hTextFont);
			m_pSizingLabel->SetText(pButton->text);
			m_pSizingLabel->SizeToContents();

			int iTextWidth = m_pSizingLabel->GetWide();

			vgui::surface()->DrawSetTextFont(m_hButtonFont);
			vgui::surface()->DrawSetTextColor(255, 255, 255, 255);
			vgui::surface()->DrawSetTextPos(x, y);
			vgui::surface()->DrawPrintText(pButton->icon, 1);

			x += vgui::surface()->GetCharacterWidth(m_hButtonFont, pButton->icon[0]) + m_ButtonSeparator;

			vgui::surface()->DrawSetTextFont(m_hTextFont);
			vgui::surface()->DrawSetTextColor(GetFgColor());
			vgui::surface()->DrawSetTextPos(x, y + textY);
			vgui::surface()->DrawPrintText(pButton->text, wcslen(pButton->text));

			x += iTextWidth + m_nButtonGap;
		}
	}
}

DECLARE_BUILD_FACTORY(CFooterPanel);

CMainMenuGameLogo::CMainMenuGameLogo(vgui::Panel *parent, const char *name)
    : vgui::EditablePanel(parent, name)
{
	m_nOffsetX = 0;
	m_nOffsetY = 0;
}

void CMainMenuGameLogo::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_nOffsetX = inResourceData->GetInt("offsetX", 0);
	m_nOffsetY = inResourceData->GetInt("offsetY", 0);
}

void CMainMenuGameLogo::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("Resource/GameLogo.res");
}

void CBasePanel::CloseBaseDialogs(void)
{
	if(m_hCreateMultiplayerGameDialog.Get())
		m_hCreateMultiplayerGameDialog->Close();
}