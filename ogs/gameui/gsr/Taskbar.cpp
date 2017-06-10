#include <KeyValues.h>
#include <vgui/IInputInternal.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/QueryBox.h>

#include "BackgroundMenuButton.h"
#include "CareerGame.h"
#include "CareerProfileFrame.h"
#include "CreateMultiplayerGameDialog.h"
#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "LoadGameDialog.h"
#include "ModInfo.h"
#include "MusicManager.h"
#include "NewGameDialog.h"
#include "OptionsDialog.h"
#include "PlayerListDialog.h"
#include "SaveGameDialog.h"
#include "TaskButton.h"
#include "Taskbar.h"
#include "VGuiSystemModuleLoader.h"
#include "BaseUI/IBaseUI.h"

CGameMenuItem::CGameMenuItem( vgui2::Menu* parent, const char* panelName )
	: BaseClass( parent, panelName, "GameMenuItem" )
	, m_bRightAligned( false )
{
}

CGameMenuItem::~CGameMenuItem()
{
}

void CGameMenuItem::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetFgColor( GetSchemeColor( "InGameDesktop/MenuColor", pScheme ) );
	SetBgColor( SDK_Color( 0, 0, 0, 0 ) );
	SetDefaultColor( GetSchemeColor( "InGameDesktop/MenuColor", pScheme ), SDK_Color( 0, 0, 0, 0 ) );
	SetArmedColor( GetSchemeColor( "InGameDesktop/ArmedMenuColor", pScheme ), SDK_Color( 0, 0, 0, 0 ) );
	SetDepressedColor( GetSchemeColor( "InGameDesktop/DepressedMenuColor", pScheme ), SDK_Color( 0, 0, 0, 0 ) );
	SetContentAlignment( vgui2::Label::a_west );

	SetBorder( nullptr );
	SetDefaultBorder( nullptr );
	SetDepressedBorder( nullptr );
	SetKeyFocusBorder( nullptr );

	SetFont( pScheme->GetFont( "Menularge", IsProportional() ) );

	SetTextInset( 0, 0 );

	SetArmedSound( "sound/UI/buttonrollover.wav" );
	SetDepressedSound( "sound/UI/buttonclick.wav" );
	SetReleasedSound( "sound/UI/buttonclickrelease.wav" );

	SetButtonActivationType( vgui2::Button::ACTIVATE_ONPRESSED );

	if( m_bRightAligned )
		SetContentAlignment( vgui2::Label::a_east );
}

CGameMenu::CGameMenu( vgui2::Panel* parent, const char* panelName )
	: BaseClass( parent, panelName )
{
}

CGameMenu::~CGameMenu()
{
}

void CGameMenu::LayoutMenuBorder()
{
	//Nothing
}

void CGameMenu::OnKillFocus()
{
	BaseClass::OnKillFocus();

	vgui2::surface()->MovePopupToBack( GetVPanel() );
}

int CGameMenu::AddMenuItem( const char* itemName, const char* itemText, const char* command, vgui2::Panel* target, KeyValues* userData )
{
	auto item = new CGameMenuItem( this, itemName );

	item->SetPaintEnabled( false );
	item->AddActionSignalTarget( target );
	item->SetCommand( command );
	item->SetText( itemText );
	item->SetUserData( userData );

	return BaseClass::AddMenuItem( item );
}

void CGameMenu::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetMenuItemHeight( strtol( pScheme->GetResourceString( "InGameDesktop/MenuItemHeight" ), nullptr, 10 ) );

	SetBgColor( SDK_Color( 0, 0, 0, 0 ) );

	SetBorder( nullptr );

	m_flMenuItemVisibilityRate = strtod( pScheme->GetResourceString( "InGameDesktop/MenuItemVisibilityRate" ), nullptr );

	if( m_flMenuItemVisibilityRate < 0.0000001 )
		m_flMenuItemVisibilityRate = 0.0000001;
}

void CGameMenu::OnThink()
{
	BaseClass::OnThink();

	if( TheCareerGame )
		TheCareerGame->Update();

	if( m_bRestartListUpdate )
	{
		m_bUpdatingList = true;
		m_flListUpdateStartTime = vgui2::system()->GetFrameTime();
		m_bRestartListUpdate = false;
	}

	if( m_bUpdatingList )
	{
		int iLastVisibleChild = static_cast<int>( floor( ( vgui2::system()->GetFrameTime() - m_flListUpdateStartTime ) / m_flMenuItemVisibilityRate ) );
		
		if( iLastVisibleChild >= GetChildCount() )
		{
			m_bUpdatingList = false;
			iLastVisibleChild = GetChildCount();
		}

		for( int i = 0; i < GetChildCount(); ++i )
		{
			auto pChild = GetChild( i );

			if( pChild )
			{
				pChild->SetPaintEnabled( i > ( GetChildCount() - iLastVisibleChild ) );
			}
		}

		InvalidateLayout();
		Repaint();
	}
}

void CGameMenu::SetVisible( bool state )
{
	BaseClass::SetVisible( true );

	if( !state )
	{
		vgui2::ipanel()->MoveToBack( GetVPanel() );
	}
}

void CGameMenu::OnCommand( const char* command )
{
	if( !stricmp( command, "Open" ) )
	{
		MoveToFront();
		RequestFocus();
		m_bRestartListUpdate = true;
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

int CGameMenu::AddMenuItem( const char* itemName, const char* itemText, KeyValues* command, vgui2::Panel* target, KeyValues* userData )
{
	auto item = new CGameMenuItem( this, itemName );

	item->SetPaintEnabled( false );
	item->AddActionSignalTarget( target );
	item->SetCommand( command );
	item->SetText( itemText );

	item->m_bRightAligned = true;

	item->SetUserData( userData );

	return BaseClass::AddMenuItem( item );
}

CareerQueryBox::CareerQueryBox( const char* title, const char* queryText, vgui2::Panel* parent )
	: BaseClass( title, queryText, parent )
{
}

CareerQueryBox::~CareerQueryBox()
{
}

void CareerQueryBox::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pOkButton->SetArmedSound( "sound/UI/buttonrollover.wav" );
	m_pOkButton->SetDepressedSound( "sound/UI/buttonclick.wav" );
	m_pOkButton->SetReleasedSound( "sound/UI/buttonclickrelease.wav" );

	m_pCancelButton->SetArmedSound( "sound/UI/buttonrollover.wav" );
	m_pCancelButton->SetDepressedSound( "sound/UI/buttonclick.wav" );
	m_pCancelButton->SetReleasedSound( "sound/UI/buttonclickrelease.wav" );
}

CTaskbar::CTaskbar( vgui2::Panel* parent, const char* panelName )
	: BaseClass( parent, panelName )
{
	if( engine->CheckParm( "-steam", nullptr ) ||
		engine->CheckParm( "-showplatform", nullptr ) )
		m_bShowPlatformMenu = true;

	m_pGameMenuButton = new CBackgroundMenuButton( this, "GameMenuButton" );
	m_pGameMenuButton->SetCommand( "OpenGameMenu" );

	m_pPlatformMenuButton = new CBackgroundMenuButton( this, "PlatformMenuButton" );
	m_pPlatformMenuButton->SetCommand( "OpenPlatformMenu" );

	if( !m_bShowPlatformMenu )
	{
		m_pPlatformMenuButton->SetVisible( false );
	}

	m_pPlatformMenu = nullptr;
	m_pGameMenu = nullptr;

	CreateMusicManager();

	CreateGameMenu();
}

CTaskbar::~CTaskbar()
{
	ShutdownMusicManager();
}

void CTaskbar::OnActivateModule(int moduleIndex )
{
	g_VModuleLoader.ActivateModule( moduleIndex );
}

void CTaskbar::ActivateGameUI()
{
	baseuifuncs->ActivateGameUI();
}

void CTaskbar::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetBorder( nullptr );

	m_iGameMenuInset = strtol( pScheme->GetResourceString( "InGameDesktop/GameMenuInset" ), nullptr, 10 );

	m_pGameMenuButton->m_pMouseOverImage = vgui2::scheme()->GetImage( "resource/game_menu_mouseover", false );
	m_pGameMenuButton->m_pImage = vgui2::scheme()->GetImage( "resource/game_menu", false );

	//Refresh button
	m_pGameMenuButton->SetArmed( m_pGameMenuButton->IsArmed() );

	m_pPlatformMenuButton->m_pMouseOverImage = vgui2::scheme()->GetImage( "resource/steam_menu_mouseover", false );
	m_pPlatformMenuButton->m_pImage = vgui2::scheme()->GetImage( "resource/steam_menu", false );

	//Refresh button
	m_pPlatformMenuButton->SetArmed( m_pPlatformMenuButton->IsArmed() );

	SetBgColor( GetSchemeColor( "InGameDesktop/WidescreenBarColor", pScheme ) );

	InvalidateLayout();
}

void CTaskbar::OnOpenNewCareerNameDialog()
{
	if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
	{
		auto pQuery = new CareerQueryBox( "#Career_RestartConfirmationTitle", "#Career_RestartConfirmationText", this );

		pQuery->SetOKButtonText( "#Career_Restart" );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "RestartCareer" ) );
		pQuery->SetCancelCommand( new KeyValues( "Command", "command", "ReleaseModalWindow" ) );

		pQuery->AddActionSignalTarget( this );

		pQuery->DoModal();

		vgui2::surface()->RestrictPaintToSinglePanel( pQuery->GetVPanel() );
	}
	else
	{
		engine->pfnClientCmd( "disconnect\n" );
		OnOpenCareerProfileFrame();
	}
}

void CTaskbar::RunFrame()
{
	InvalidateLayout();

	if( !m_bPlatformMenuInitialized && g_VModuleLoader.IsPlatformReady() )
	{
		if( m_bShowPlatformMenu )
		{
			CreatePlatformMenu();

			PostMessage( m_pPlatformMenu, new KeyValues( "Command", "command", "Open" ) );
		}

		m_bPlatformMenuInitialized = true;
	}
}

void CTaskbar::OnGameUIActivated()
{
	if( m_pGameMenu )
		m_pGameMenu->m_bRestartListUpdate = true;

	if( m_pPlatformMenu )
		m_pPlatformMenu->m_bRestartListUpdate = true;
}

void CTaskbar::OnOpenQuitConfirmationDialog()
{
	auto pQuery = new vgui2::QueryBox( "#GameUI_QuitConfirmationTitle", "#GameUI_QuitConfirmationText", this );
	pQuery->SetOKButtonText( "#GameUI_Quit" );
	pQuery->SetOKCommand( new KeyValues( "Command", "command", "QuitNoConfirm" ) );
	pQuery->SetCancelCommand( new KeyValues( "Command", "command", "ReleaseModalWindow" ) );

	pQuery->AddActionSignalTarget( this );
	pQuery->DoModal();

	vgui2::surface()->RestrictPaintToSinglePanel( pQuery->GetVPanel() );
}

void CTaskbar::OnOpenServerBrowser()
{
	g_VModuleLoader.ActivateModule( "Servers" );
}

void CTaskbar::OnOpenChangeGameDialog()
{
	//Nothing
	//The change game dialog was made obsolete when support was added to Steam to list game mods
	//TODO: reimplement if needed for PowerSource - Solokiller
}

void CTaskbar::OnOpenPlayerListDialog()
{
	if( !m_hPlayerListDialog )
	{
		m_hPlayerListDialog = new CPlayerListDialog( this );
		PositionDialog( m_hPlayerListDialog );
	}

	m_hPlayerListDialog->Activate();
}

void CTaskbar::OnOpenCareerBotDialog()
{
	if( !m_hCareerBotDialog )
	{
		//TODO: implement - Solokiller
		//m_hCareerBotDialog = new CCareerBotFrame( this );
		PositionDialog( m_hCareerBotDialog );
	}
	
	m_hCareerBotDialog->Activate();
}

void CTaskbar::OnOpenCareerMapDialog( bool promptOnOverwrite )
{
	if( !m_hCareerMapDialog )
	{
		//TODO: implement - Solokiller
		//m_hCareerMapDialog = new CCareerMapFrame( this );
		PositionDialog( m_hCareerMapDialog );
	}
	
	m_hCareerMapDialog->Activate();
}

void CTaskbar::OnOpenCreateMultiplayerGameDialog()
{
	if( !m_hCreateMultiplayerGameDialog )
	{
		m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog( this );
		PositionDialog( m_hCreateMultiplayerGameDialog );
	}
	
	m_hCreateMultiplayerGameDialog->Activate();
}

void CTaskbar::OnOpenDemoDialog()
{
	//TODO: implement - Solokiller
}

void CTaskbar::OnOpenOptionsDialog()
{
	if( !m_hOptionsDialog )
	{
		m_hOptionsDialog = new COptionsDialog( this );
		PositionDialog( m_hOptionsDialog );
	}

	m_hOptionsDialog->Activate();
}

void CTaskbar::OnOpenSaveGameDialog()
{
	if( !m_hSaveGameDialog )
	{
		m_hSaveGameDialog = new CSaveGameDialog( this );
		PositionDialog( m_hSaveGameDialog );
	}

	m_hSaveGameDialog->Activate();
}

void CTaskbar::OnOpenLoadGameDialog()
{
	if( !m_hLoadGameDialog )
	{
		m_hLoadGameDialog = new CLoadGameDialog( this );
		PositionDialog( m_hLoadGameDialog );
	}
	
	m_hLoadGameDialog->Activate();
}

void CTaskbar::OnOpenNewGameDialog()
{
	if( !m_hNewGameDialog )
	{
		m_hNewGameDialog = new CNewGameDialog( this );
		PositionDialog( m_hNewGameDialog );
	}

	m_hNewGameDialog->Activate();
}

vgui2::Frame* CTaskbar::OnOpenResumeCareerGameDialog( CareerDifficultyType difficulty )
{
	CCareerGame::Create();
	TheCareerGame->Reset();

	if( TheCareerGame->LoadProfile( difficulty ) )
	{
		if( !m_hCareerMapDialog )
		{
			//TODO: implement - Solokiller
			//m_hCareerMapDialog = new CCareerMapFrame( this );
			PositionDialog( m_hCareerMapDialog );
		}
		
		m_hCareerMapDialog->Activate();
	}

	return m_hCareerMapDialog;
}

void CTaskbar::OnOpenCareerProfileFrame()
{
	if( m_hCareerDifficultyDialog )
	{
		m_hCareerDifficultyDialog->Close();
	}

	if( !m_hCareerProfileDialog )
	{
		m_hCareerProfileDialog = new CCareerProfileFrame( this );
		PositionDialog( m_hCareerProfileDialog );
	}

	m_hCareerProfileDialog->Activate();
}

void CTaskbar::OnOpenCreateCareerGameDialog()
{
	if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
	{
		auto pQuery = new CareerQueryBox( "#Career_RestartConfirmationTitle", "#Career_RestartConfirmationText", this );
		pQuery->SetOKButtonText( "#Career_Restart" );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "RestartCareer" ) );
		pQuery->SetCancelCommand( new KeyValues( "Command", "command", "ReleaseModalWindow" ) );

		pQuery->AddActionSignalTarget( this );
		pQuery->DoModal();
		vgui2::surface()->RestrictPaintToSinglePanel( pQuery->GetVPanel() );
	}
	else
	{
		CCareerGame::Create();
		TheCareerGame->Reset();
		OnOpenCareerProfileFrame();
	}
}

void CTaskbar::OnCommand( const char* command )
{
	if( !stricmp( command, "OpenGameMenu" ) )
	{
		if( m_pGameMenu )
		{
			PostMessage( m_pGameMenu, new KeyValues( "Command", "command", "Open" ) );
		}
	}
	else if( !stricmp( command, "OpenPlatformMenu" ) )
	{
		if( m_pPlatformMenu && m_bShowPlatformMenu )
		{
			PostMessage( m_pPlatformMenu, new KeyValues( "Command", "command", "Open" ) );
		}
	}
	else if( !stricmp( command, "OpenPlayerListDialog" ) )
	{
		OnOpenPlayerListDialog();
	}
	else if( !stricmp( command, "OpenNewGameDialog" ) )
	{
		OnOpenNewGameDialog();
	}
	else if( !stricmp( command, "OpenLoadGameDialog" ) )
	{
		OnOpenLoadGameDialog();
	}
	else if( !stricmp( command, "OpenSaveGameDialog" ) )
	{
		OnOpenSaveGameDialog();
	}
	else if( !stricmp( command, "OpenOptionsDialog" ) )
	{
		OnOpenOptionsDialog();
	}
	else if( !stricmp( command, "OpenServerBrowser" ) )
	{
		g_VModuleLoader.ActivateModule( "Servers" );
	}
	else if( !stricmp( command, "OpenLoadDemoDialog" ) )
	{
		OnOpenDemoDialog();
	}
	else if( !stricmp( command, "OpenCreateMultiplayerGameDialog" ) )
	{
		OnOpenCreateMultiplayerGameDialog();
	}
	else if( !stricmp( command, "OpenNewCareerNameDialog" ) )
	{
		if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
		{
			if( TheCareerGame
				&& TheCareerGame->IsPlayingMatch() )
			{
				OnOpenNewCareerNameDialog();
			}
			else
			{
				engine->pfnClientCmd( "disconnect\n" );
				OnOpenCareerProfileFrame();
			}
		}
	}
	else if( !stricmp( command, "OpenPlayOnLineDialog" ) )
	{
		auto pQuery = new vgui2::QueryBox( "#CZero_PlayOnLine", "#CZero_PlayOnLineText" );
		pQuery->AddActionSignalTarget( this );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "QuitToPlayOnLine" ) );

		pQuery->DoModal();
	}
	else if( !strnicmp( command, "OpenCareerMapDialog", 19 ) )
	{
		if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
			OnOpenCareerMapDialog( false );
	}
	else if( !stricmp( command, "OpenCareerBotDialog" ) )
	{
		if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
			OnOpenCareerBotDialog();
	}
	else if( !stricmp( command, "OpenCreateCareerGameDialog" ) )
	{
		if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
			OnOpenCreateCareerGameDialog();
	}
	else if( !stricmp( command, "OpenChangeGameDialog" ) )
	{
		OnOpenChangeGameDialog();
	}
	else if( !stricmp( command, "Quit" ) )
	{
		OnOpenQuitConfirmationDialog();
	}
	else if( !stricmp( command, "QuitNoConfirm" ) )
	{
		if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
			TheCareerGame->StopMatch( 0, 0 );

		engine->pfnClientCmd( "quit\n" );

		SetVisible( false );
		vgui2::surface()->RestrictPaintToSinglePanel( GetVPanel() );
	}
	else if( !stricmp( command, "QuitToPlayOnLine" ) )
	{
		if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
			TheCareerGame->StopMatch( 0, 0 );

		engine->pfnClientCmd( "quit\n" );

		SetVisible( false );

		vgui2::surface()->RestrictPaintToSinglePanel( GetVPanel() );

		vgui2::system()->ShellExecute( "open", "steaminstall.exe" );
	}
	else if( !stricmp( command, "ResumeGame" ) )
	{
		if( baseuifuncs )
			baseuifuncs->HideGameUI();
	}
	else if( !stricmp( command, "ReallySurrender" ) )
	{
		engine->pfnClientCmd( "disconnect\n" );
		TheCareerGame->OnRoundEndMenuClose( false );
	}
	else if( !stricmp( command, "Disconnect" ) )
	{
		engine->pfnClientCmd( "disconnect\n" );
	}
	else if( !stricmp( command, "RestartCareer" ) )
	{
		TheCareerGame->StopMatch( 0, 0 );
		vgui2::surface()->RestrictPaintToSinglePanel( NULL_HANDLE );
		engine->pfnClientCmd( "disconnect\n" );
		OnOpenNewCareerNameDialog();
	}
	else if( !stricmp( command, "ReleaseModalWindow" ) )
	{
		vgui2::surface()->RestrictPaintToSinglePanel( NULL_HANDLE );
	}
	else if( !stricmp( command, "Surrender" ) )
	{
		auto pQuery = new vgui2::QueryBox( "#Career_Surrender", "#GameUI_QuitConfirmationText", this );
		pQuery->SetProportional( false );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "ReallySurrender" ) );
		pQuery->AddActionSignalTarget( this );
		pQuery->SetScheme( "TrackerScheme" );

		pQuery->DoModal();
	}
	else if( !stricmp( command, "EndRound" ) )
	{
		auto pQuery = new vgui2::QueryBox( "#Career_EndRound", "#Career_EndRoundText", this );
		pQuery->SetProportional( false );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "engine career_endround\n" ) );
		pQuery->AddActionSignalTarget( this );
		pQuery->SetScheme( "TrackerScheme" );

		pQuery->DoModal();
	}
	else
	{
		//Forward engine commands
		auto pszCommand = strstr( command, "engine " );

		if( pszCommand )
		{
			if( pszCommand[ 7 ] )
				gEngfuncs.pfnClientCmd( const_cast<char*>( pszCommand + 7 ) );

			if( baseuifuncs )
				baseuifuncs->HideGameUI();
		}
		else
		{
			BaseClass::OnCommand( command );
		}
	}
}

void CTaskbar::AddTask( vgui2::VPANEL panel )
{
	//Nothing
}

CTaskButton* CTaskbar::FindTask( vgui2::VPANEL panel )
{
	for( int i = 0; i < g_Tasks.Count(); ++i )
	{
		if( g_Tasks[ i ]->GetTaskPanel() == panel )
			return g_Tasks[ i ];
	}

	return nullptr;
}

void CTaskbar::SetTitle( vgui2::VPANEL panel, const wchar_t* title )
{
	for( int i = 0; i < g_Tasks.Count(); ++i )
	{
		if( panel == g_Tasks[ i ]->GetTaskPanel() )
		{
			if( g_Tasks[ i ] )
				g_Tasks[ i ]->SetTitle( title );
		}
	}
}

void CTaskbar::UpdateGameMenus()
{
	int wide, tall;
	vgui2::surface()->GetScreenSize( wide, tall );

	if( m_pPlatformMenu )
	{
		m_pPlatformMenu->SetPos(
			wide - m_iGameMenuInset - m_pPlatformMenu->GetWide(),
			tall - m_pPlatformMenu->GetTall() - 64
		);

		m_pPlatformMenu->SetVisible( true );
	}

	static bool isInCareerGame = false;

	if( isInCareerGame
		&& !TheCareerGame->IsPlayingMatch() )
	{
		TheCareerGame->StopMatch( 0, 0 );
		isInCareerGame = false;
	}

	bool isInGame;
	bool isMulti;
	
	auto pszLevelName = engine->pfnGetLevelName();

	if( pszLevelName && *pszLevelName )
	{
		if( engine->GetMaxClients() <= 1 )
		{
			isMulti = false;
			isInGame = true;
		}
		else if( TheCareerGame
				 && TheCareerGame->IsPlayingMatch() )
		{
			isInCareerGame = true;
			isMulti = true;
			isInGame = true;
		}
		else
		{
			isMulti = true;
			isInGame = true;
		}
	}
	else
	{
		isMulti = false;
		isInGame = false;
	}

	if( TheMusicManager )
	{
		if( ( g_hLoadingDialog ||
			  isInGame ||
			  TheMusicManager->IsPlayingTrack( TRACKTYPE_SPLASH ) )
			&& ( !g_hLoadingDialog ||
				 isInGame ||
				 !isInCareerGame ||
				 TheMusicManager->IsPlayingTrack( TRACKTYPE_SPLASH ) ) )
		{
			if( g_hLoadingDialog &&
				!isInCareerGame &&
				TheMusicManager->IsPlayingTrack( TRACKTYPE_SPLASH ) ||
				isInGame &&
				TheMusicManager->IsPlayingTrack( TRACKTYPE_SPLASH ) )
			{
				TheMusicManager->FadeOutTrack();
			}
		}
		else
		{
			TheMusicManager->StartTrack( TRACKTYPE_SPLASH, true );
		}
	}

	const auto bIsSteam = engine->CheckParm( "-steam", 0 ) != 0;

	//Determine which menu items are visible
	for( int i = 0; i < m_pGameMenu->GetChildCount(); ++i )
	{
		auto pChild = m_pGameMenu->GetChild( i );

		if( !pChild )
			continue;

		auto pMenu = dynamic_cast<vgui2::MenuItem*>( pChild );

		if( !pMenu )
			continue;

		auto pData = pMenu->GetUserData();

		if( !pData )
			continue;

		bool bAvailable;

		if( isInGame )
		{
			bAvailable = pData->GetInt( isInCareerGame ? "NotInCareerGame" : "OnlyInCareerGame", 0 ) != 0;
		}
		else
			bAvailable = pData->GetInt( "OnlyInGame", 0 ) == 0;

		if( isMulti && pData->GetInt( "notmulti", 0 ) )
		{
			bAvailable = false;
		}
		else if( isInGame && !isMulti && pData->GetInt( "notsingle", 0 ) )
		{
			bAvailable = false;
		}
		else if( bIsSteam )
		{
			if( pData->GetInt( "notsteam", 0 ) )
				bAvailable = false;
		}

		pMenu->SetVisible( bAvailable );
	}

	m_pGameMenu->InvalidateLayout();
	m_pGameMenu->SetPos( m_iGameMenuInset, tall - m_pGameMenu->GetTall() - 64 );
	m_pGameMenu->SetVisible( true );
}

void CTaskbar::UpdateTaskButtons()
{
	auto hFocus = vgui2::input()->GetFocus();

	int reorderedButtons = 0;
	int visibleButtonCount = 0;

	for( int i = 0; i < g_Tasks.Count(); )
	{
		auto pTask = g_Tasks[ i ];

		if( hFocus && vgui2::ipanel()->HasParent( hFocus, pTask->GetTaskPanel() ) )
		{
			pTask->SetSelected( true );
		}
		else
		{
			pTask->SetSelected( false );
		}

		if( NULL_HANDLE == pTask->GetTaskPanel() )
		{
			pTask->SetVisible( false );

			pTask->MarkForDeletion();

			g_Tasks.Remove( i );

			visibleButtonCount = 0;

			//Start from beginning
			i = 0;
		}
		else
		{
			if( pTask->ShouldDisplay() )
			{
				++visibleButtonCount;

				//if not visible, move to back
				if( !pTask->IsVisible() )
				{
					//Avoid infinitely swapping buttons
					if( reorderedButtons < 100 )
					{
						//Don't move if already at the back
						if( i != g_Tasks.Count() - 1 )
						{
							auto pOther = g_Tasks[ i ];
							g_Tasks[ i ] = g_Tasks[ i + 1 ];
							g_Tasks[ i + 1 ] = pOther;

							//Start from beginning
							i = 0;
							++reorderedButtons;
							visibleButtonCount = 0;
						}
					}
				}
			}

			++i;
		}
	}

	if( visibleButtonCount == 0 )
		visibleButtonCount = 1;

	int buttonWide = ( GetWide() - 256 ) / visibleButtonCount;

	if( buttonWide > 128 )
		buttonWide = 128;

	int x = 280;

	for( int i = 0; i < g_Tasks.Count(); ++i )
	{
		auto pTask = g_Tasks[ i ];

		if( !pTask->ShouldDisplay() )
		{
			pTask->SetVisible( false );
			continue;
		}

		pTask->SetVisible( true );
		pTask->SetBounds( x, 20, buttonWide - 8, 24 );
		x += buttonWide;
	}
}

void CTaskbar::PerformLayout()
{
	int wide, tall;
	vgui2::surface()->GetScreenSize( wide, tall );

	static int count = 2;

	if( count == 0 )
	{
		GameUI().ValidateCDKey( false, false );
	}

	--count;

	SetPos( 0, tall - 64 );
	SetSize( wide, 64 );

	m_pGameMenuButton->SetPos( m_iGameMenuInset, ( 64 - m_iGameMenuInset ) / 2 );
	m_pGameMenuButton->SetSize( 240, m_iGameMenuInset + 4 );

	m_pPlatformMenuButton->SetPos( wide - ( m_iGameMenuInset + m_pPlatformMenuButton->GetWide() ), 6 );
	m_pPlatformMenuButton->SetSize( 240, m_iGameMenuInset + 4 );

	m_pPlatformMenuButton->SetContentAlignment( vgui2::Label::a_east );

	UpdateTaskButtons();
	UpdateGameMenus();
}

void CTaskbar::CreateGameMenu()
{
	auto pKV = new KeyValues( "GameMenu" );

	if( pKV->LoadFromFile( vgui2::filesystem(), "resource/GameMenu.res" ) )
	{
		m_pGameMenu = RecursiveLoadGameMenu( pKV );
		//TODO: shouldn't be inside the if statement - Solokiller
		pKV->deleteThis();
	}
}

void CTaskbar::CreatePlatformMenu()
{
	m_pPlatformMenu = new CGameMenu( m_pPlatformMenuButton, "PlatformMenu" );

	for( int i = 0; i < g_VModuleLoader.GetModuleCount(); ++i )
	{
		m_pPlatformMenu->AddMenuItem(
			g_VModuleLoader.GetModuleLabel( i ),
			g_VModuleLoader.GetModuleLabel( i ),
			new KeyValues( "ActivateModule", "moduleIndex", i ),
			this,
			nullptr
		);
	}

	m_pPlatformMenu->SetVisible( true );
}

CGameMenu* CTaskbar::RecursiveLoadGameMenu( KeyValues* datafile )
{
	auto pGameMenu = new CGameMenu( this, datafile->GetName() );

	for( auto pKV = datafile->GetFirstSubKey(); pKV; pKV = pKV->GetNextKey() )
	{
		auto pszLabel = pKV->GetString( "label", "<unknown>" );
		auto pszCommand = pKV->GetString( "command", nullptr );
		auto pszName = pKV->GetString( "name", pszLabel );

		pGameMenu->AddMenuItem( pszName, pszLabel, pszCommand, this, pKV );
	}

	return pGameMenu;
}

void CTaskbar::PositionDialog( vgui2::PHandle dlg )
{
	if( dlg )
	{
		int x, y, ww, wt;
		vgui2::surface()->GetWorkspaceBounds( x, y, ww, wt );

		int wide, tall;
		dlg->GetSize( wide, tall );

		dlg->SetPos( x + ( ww - wide ) / 2, y + ( wt - tall ) / 2 );
	}
}
