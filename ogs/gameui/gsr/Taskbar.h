#ifndef GAMEUI_TASKBAR_H
#define GAMEUI_TASKBAR_H

#include <UtlVector.h>

#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/QueryBox.h>

#include "GameUI/CareerDefs.h"

class CBackgroundMenuButton;
class CTaskButton;

class CGameMenuItem : public vgui2::MenuItem
{
public:
	using BaseClass = vgui2::MenuItem;

	CGameMenuItem( vgui2::Menu* parent, const char* panelName );
	~CGameMenuItem();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

public:
	bool m_bRightAligned;

private:
	CGameMenuItem( const CGameMenuItem& ) = delete;
	CGameMenuItem& operator=( const CGameMenuItem& ) = delete;
};

class CGameMenu : public vgui2::Menu
{
protected:
	friend class CTaskbar;

public:
	using BaseClass = vgui2::Menu;

	CGameMenu( vgui2::Panel* parent, const char* panelName );
	~CGameMenu();

	void LayoutMenuBorder() override;

	void OnKillFocus() override;

	virtual int AddMenuItem( const char* itemName, const char* itemText, const char* command, vgui2::Panel* target, KeyValues* userData );

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnThink() override;

	void SetVisible( bool state ) override;

	void OnCommand( const char* command ) override;

	virtual int AddMenuItem( const char* itemName, const char* itemText, KeyValues* command, vgui2::Panel* target, KeyValues* userData );

private:
	bool m_bRestartListUpdate = false;
	bool m_bUpdatingList = false;
	float m_flListUpdateStartTime = 0;
	float m_flMenuItemVisibilityRate;

private:
	CGameMenu( const CGameMenu& ) = delete;
	CGameMenu& operator=( const CGameMenu& ) = delete;
};

class CareerQueryBox : public vgui2::QueryBox
{
public:
	using BaseClass = vgui2::QueryBox;

	CareerQueryBox( const char* title, const char* queryText, vgui2::Panel* parent );
	~CareerQueryBox();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

private:
	CareerQueryBox( const CareerQueryBox& ) = delete;
	CareerQueryBox& operator=( const CareerQueryBox& ) = delete;
};

class CTaskbar : public vgui2::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTaskbar, vgui2::EditablePanel );

public:
	CTaskbar( vgui2::Panel* parent, const char* panelName );
	~CTaskbar();

	MESSAGE_FUNC_INT( OnActivateModule, "ActivateModule", moduleIndex );

	MESSAGE_FUNC( ActivateGameUI, "ActivateGameUI" );

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnOpenNewCareerNameDialog();

	void RunFrame();

	void OnGameUIActivated();

	void OnOpenQuitConfirmationDialog();

	void OnOpenServerBrowser();

	void OnOpenChangeGameDialog();

	void OnOpenPlayerListDialog();

	void OnOpenCareerBotDialog();

	void OnOpenCareerMapDialog( bool promptOnOverwrite );

	void OnOpenCreateMultiplayerGameDialog();

	void OnOpenDemoDialog();

	void OnOpenOptionsDialog();

	void OnOpenSaveGameDialog();

	void OnOpenLoadGameDialog();

	void OnOpenNewGameDialog();

	vgui2::Frame* OnOpenResumeCareerGameDialog( CareerDifficultyType difficulty );

	void OnOpenCareerProfileFrame();

	void OnOpenCreateCareerGameDialog();

	void OnCommand( const char* command ) override;

	void AddTask( vgui2::VPANEL panel );

	CTaskButton* FindTask( vgui2::VPANEL panel );

	void SetTitle( vgui2::VPANEL panel, const wchar_t* title );

	void PerformLayout() override;

private:
	void CreateGameMenu();
	void CreatePlatformMenu();

	CGameMenu* RecursiveLoadGameMenu( KeyValues* datafile );

	void PositionDialog( vgui2::PHandle dlg );

	void UpdateGameMenus();

	void UpdateTaskButtons();

private:
	CBackgroundMenuButton* m_pGameMenuButton;
	CBackgroundMenuButton* m_pPlatformMenuButton;

	CGameMenu* m_pPlatformMenu = nullptr;
	CGameMenu* m_pGameMenu = nullptr;

	bool m_bShowPlatformMenu = false;
	bool m_bPlatformMenuInitialized = false;
	int m_iGameMenuInset = 24;

	vgui2::DHANDLE<vgui2::Frame> m_hNewGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hLoadGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hSaveGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hOptionsDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCreateMultiplayerGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hDemoPlayerDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hChangeGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hPlayerListDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCareerProfileDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCareerDifficultyDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCareerMapDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCareerBotDialog;

	CUtlVector<CTaskButton*> g_Tasks;

private:
	CTaskbar( const CTaskbar& ) = delete;
	CTaskbar& operator=( const CTaskbar& ) = delete;
};

#endif //GAMEUI_TASKBAR_H
