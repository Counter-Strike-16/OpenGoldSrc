#include <tier0/platform.h>

#ifdef WIN32
#include "winheaders.h"
#include <direct.h>
#endif

#include <FileSystem.h>
#include <KeyValues.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Controls.h>

#include "interface.h"

#include "BasePanel.h"
#include "CareerGame.h"
#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "ienginevgui.h"
#include "igameuifuncs.h"
#include "LoadingDialog.h"
#include "ModInfo.h"
#include "Sys_Utils.h"
#include "Taskbar.h"
#include "VGuiSystemModuleLoader.h"
#include "BaseUI/IBaseUI.h"
#include "cl_dll/IGameClientExports.h"
#include "steam/friends/IFriendsUser.h"

static CBasePanel* staticPanel = nullptr;

static IEngineVGui* g_pEngineVGUI = nullptr;

IEngineVGui* EngineVGUI()
{
	return g_pEngineVGUI;
}

IGameUIFuncs* gameuifuncs = nullptr;

static vgui2::ISurface* enginesurfacefuncs = nullptr;

IBaseUI* baseuifuncs = nullptr;

static IGameClientExports* g_pGameClientExports = nullptr;

IGameClientExports* GameClientExports()
{
	return g_pGameClientExports;
}

cl_enginefunc_t gEngfuncs;

cl_enginefunc_t* engine = nullptr;

IBaseSystem* g_pSystemWrapper = nullptr;

CTaskbar* g_pTaskbar = nullptr;

WHANDLE g_hMutex = NULL;
WHANDLE g_hWaitMutex = NULL;

vgui2::PHandle g_hLoadingDialog;

IFriendsUser* g_pFriendsUser = nullptr;

static CGameUI g_GameUI;

CGameUI* g_pGameUI = nullptr;

CGameUI& GameUI()
{
	return g_GameUI;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, g_GameUI );

CGameUI::CGameUI()
{
	g_pGameUI = this;
}

CGameUI::~CGameUI()
{
	g_pGameUI = nullptr;
}

void CGameUI::Initialize( CreateInterfaceFn* factories, int count )
{
	auto pEngFactory = factories[ 0 ];
	CreateInterfaceFn pClientFactory = nullptr;

	if( count > 5 )
		pClientFactory = factories[ 5 ];

	m_FactoryList[ 0 ] = Sys_GetFactoryThis();
	m_FactoryList[ 1 ] = pEngFactory;
	m_FactoryList[ 2 ] = factories[ 1 ];
	m_FactoryList[ 3 ] = factories[ 2 ];
	m_FactoryList[ 4 ] = pClientFactory;

	m_iNumFactories = count;

	vgui2::VGui_InitInterfacesList( "GameUI", m_FactoryList, count );

	vgui2::localize()->AddFile( vgui2::filesystem(), "resource/gameui_%language%.txt" );

	staticPanel = new CBasePanel();
	staticPanel->SetBounds( 0, 0, 400, 300 );

	staticPanel->SetPaintBorderEnabled( false );
	staticPanel->SetPaintBackgroundEnabled( true );
	staticPanel->SetPaintEnabled( false );

	staticPanel->SetVisible( true );
	staticPanel->SetMouseInputEnabled( false );
	staticPanel->SetKeyBoardInputEnabled( false );

	g_pEngineVGUI = static_cast<IEngineVGui*>( pEngFactory( ENGINEVGUI_INTERFACE_VERSION, nullptr ) );

	if( g_pEngineVGUI )
	{
		staticPanel->SetParent( g_pEngineVGUI->GetPanel( PANEL_GAMEUIDLL ) );
	}

	gameuifuncs = static_cast<IGameUIFuncs*>( pEngFactory( GAMEUIFUNCS_INTERFACE_VERSION, nullptr ) );
	enginesurfacefuncs = static_cast<vgui2::ISurface*>( pEngFactory( VGUI_SURFACE_INTERFACE_VERSION_GS, nullptr ) );
	baseuifuncs = static_cast<IBaseUI*>( pEngFactory( BASEUI_INTERFACE_VERSION, nullptr ) );

	if( pClientFactory )
	{
		g_pGameClientExports = static_cast<IGameClientExports*>( pClientFactory( GAMECLIENTEXPORTS_INTERFACE_VERSION, nullptr ) );
	}

	m_flRetryPlatformReleaseMessageTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Callback function; sends platform Shutdown message to specified window
//-----------------------------------------------------------------------------
int __stdcall SendShutdownMsgFunc( WHANDLE hwnd, int lparam )
{
	Sys_PostMessage( hwnd, Sys_RegisterWindowMessage( "ShutdownValvePlatform" ), 0, 1 );
	return 1;
}

void CGameUI::Start( cl_enginefunc_t* engineFuncs, int interfaceVersion, IBaseSystem* system )
{
	m_pMaster = nullptr;

	memcpy( &gEngfuncs, engineFuncs, sizeof( gEngfuncs ) );

	engine = &gEngfuncs;

	g_pSystemWrapper = system;

	ModInfo().LoadCurrentGameInfo();

	if( FindPlatformDirectory( m_szPlatformDir, ARRAYSIZE( m_szPlatformDir ) ) )
	{
		char szConfigDir[ 512 ];

		strcpy( szConfigDir, m_szPlatformDir );

		auto uiLength = strlen( szConfigDir );

		szConfigDir[ uiLength++ ] = CORRECT_PATH_SEPARATOR;

		strcpy( &szConfigDir[ uiLength ], "config" );
		szConfigDir[ uiLength + strlen( "config" ) ] = '\0';

		vgui2::filesystem()->AddSearchPath( szConfigDir, "CONFIG" );

		_mkdir( szConfigDir );

		vgui2::ivgui()->DPrintf( "Platform config directory: %s\n", szConfigDir );

		vgui2::system()->SetUserConfigFile( "InGameDialogConfig.vdf", "CONFIG" );

		vgui2::localize()->AddFile( vgui2::filesystem(), "resource/platform_%language%.txt" );
		vgui2::localize()->AddFile( vgui2::filesystem(), "resource/vgui_%language%.txt" );
	}

	g_pTaskbar = new CTaskbar( staticPanel, "TaskBar" );
	g_pTaskbar->SetVisible( false );

	if( m_szPlatformDir[ 0 ] )
	{
		vgui2::ivgui()->DPrintf2( "Initializing platform...\n" );

		//From the Alien Swarm SDK - Solokiller
		Sys_SetLastError( SYS_NO_ERROR );

		if( IsPC() )
		{
			g_hMutex = Sys_CreateMutex( "ValvePlatformUIMutex" );
			g_hWaitMutex = Sys_CreateMutex( "ValvePlatformWaitMutex" );
			if( g_hMutex == NULL || g_hWaitMutex == NULL || Sys_GetLastError() == SYS_ERROR_INVALID_HANDLE )
			{
				// error, can't get handle to mutex
				if( g_hMutex )
				{
					Sys_ReleaseMutex( g_hMutex );
				}
				if( g_hWaitMutex )
				{
					Sys_ReleaseMutex( g_hWaitMutex );
				}
				g_hMutex = NULL;
				g_hWaitMutex = NULL;
				vgui2::ivgui()->DPrintf2( "Tracker Error: Could not access Tracker, bad mutex\n" );
				return;
			}
			unsigned int waitResult = Sys_WaitForSingleObject( g_hMutex, 0 );
			if( !( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED ) )
			{
				// mutex locked, need to deactivate Steam (so we have the Friends/ServerBrowser data files)
				// get the wait mutex, so that Steam.exe knows that we're trying to acquire ValveTrackerMutex
				waitResult = Sys_WaitForSingleObject( g_hWaitMutex, 0 );
				if( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED )
				{
					Sys_EnumWindows( SendShutdownMsgFunc, 1 );
				}

				m_flRetryPlatformReleaseMessageTime = 2;
			}

			// now we are set up to check every frame to see if we can friends/server browser
			m_bTryingToLoadTracker = true;
		}

		//From the Alien Swarm SDK end - Solokiller
	}

	staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_DESKTOPIMAGE );
	engine->pfnClientCmd( "mp3 loop media/gamestartup.mp3" );
}

void CGameUI::Shutdown()
{
	// notify all the modules of Shutdown
	g_VModuleLoader.ShutdownPlatformModules();

	// unload the modules them from memory
	g_VModuleLoader.UnloadPlatformModules();

	ModInfo().FreeModInfo();

	// release platform mutex
	// close the mutex
	if( g_hMutex )
	{
		Sys_ReleaseMutex( g_hMutex );
	}
	if( g_hWaitMutex )
	{
		Sys_ReleaseMutex( g_hWaitMutex );
	}
}

int CGameUI::ActivateGameUI()
{
	if( IsGameUIActive() )
		return true;

	m_bActivatedUI = true;
	staticPanel->SetVisible( true );
	g_pTaskbar->SetVisible( true );
	engine->pfnClientCmd( "setpause" );

	if( g_pTaskbar )
		g_pTaskbar->OnGameUIActivated();

	return true;
}

int CGameUI::ActivateDemoUI()
{
	if( g_pTaskbar )
	{
		g_pTaskbar->OnOpenDemoDialog();
		return true;
	}

	return false;
}

int CGameUI::HasExclusiveInput()
{
	return IsGameUIActive() != 0;
}

void CGameUI::RunFrame()
{
	int wide, tall;
	vgui2::surface()->GetScreenSize( wide, tall );
	staticPanel->SetSize( wide, tall );

	g_VModuleLoader.RunFrame();

	if( g_pTaskbar )
		g_pTaskbar->RunFrame();

	if( IsPC() && m_bTryingToLoadTracker && g_hMutex && g_hWaitMutex )
	{
		// try and load Steam platform files
		unsigned int waitResult = Sys_WaitForSingleObject( g_hMutex, 0 );
		if( waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED )
		{
			// we got the mutex, so load Friends/Serverbrowser
			// clear the loading flag
			m_bTryingToLoadTracker = false;
			m_flRetryPlatformReleaseMessageTime = 0;

			g_VModuleLoader.LoadPlatformModules( m_FactoryList, m_iNumFactories, false );

			for( int i = 0; i < g_VModuleLoader.GetModuleCount(); ++i )
			{
				if( !g_pFriendsUser )
				{
					auto factory = g_VModuleLoader.GetModuleFactory( i );

					if( factory )
					{
						g_pFriendsUser = reinterpret_cast<IFriendsUser*>( factory( FRIENDSUSER_INTERFACE_VERSION, nullptr ) );
					}
				}
			}

			// release the wait mutex
			Sys_ReleaseMutex( g_hWaitMutex );

			// notify the game of our game name
			KeyValues *pKV = new KeyValues( "ActiveGameName" );
			pKV->SetString( "name", engine->pfnGetGameDirectory() );
			pKV->SetInt( "type", 1 );
			pKV->SetInt( "appid", engine->pfnGetAppID() );
			pKV->SetString( "game", ModInfo().GetGameDescription() );

			g_VModuleLoader.PostMessageToAllModules( pKV );

			// notify the ui of a game connect if we're already in a game
			if( m_iGameIP )
			{
				KeyValues* kv = new KeyValues( "ConnectedToGame" );
				kv->SetInt( "ip", m_iGameIP );
				kv->SetInt( "connectionport", m_iGamePort );
				kv->SetInt( "queryport", m_iGamePort );

				g_VModuleLoader.PostMessageToAllModules( kv );
			}
		}
		else
		{
			if( m_flRetryPlatformReleaseMessageTime > vgui2::system()->GetFrameTime() )
			{
				Sys_EnumWindows( SendShutdownMsgFunc, 1 );
				m_flRetryPlatformReleaseMessageTime = vgui2::system()->GetFrameTime() + 5;
			}
		}
	}

	if( vgui2::surface()->GetModalPanel() )
	{
		vgui2::surface()->PaintTraverse( staticPanel->GetVPanel() );
	}
}

void CGameUI::ConnectToServer( const char* game, int IP, int port )
{
	if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
	{
		baseuifuncs->ActivateGameUI();

		StartProgressBar( "CareerPause", 1 );
		staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_CAREERLOAD );
		ContinueProgressBar( 1, 100 );

		SetProgressBarStatusText( "#Career_Start" );

		LoadingDialog()->SwitchToPausedCareerDialog();
		TheCareerGame->FinishMatchStart();
	}
	else
	{
		if( stricmp( game, "valve" ) )
			engine->pfnClientCmd( "mp3 stop\n" );

		baseuifuncs->HideGameUI();
	}

	m_iGameIP = IP;
	m_iGamePort = port;

	KeyValues* kv = new KeyValues( "ConnectedToGame" );
	kv->SetInt( "ip", m_iGameIP );
	kv->SetInt( "connectionport", m_iGamePort );
	kv->SetInt( "queryport", m_iGamePort );

	g_VModuleLoader.PostMessageToAllModules( kv );
}

void CGameUI::DisconnectFromServer()
{
	ActivateGameUI();
	m_iGameIP = 0;
	m_iGamePort = 0;

	g_VModuleLoader.PostMessageToAllModules( new KeyValues( "DisconnectedFromGame" ) );
}

void CGameUI::HideGameUI()
{
	auto pszLevelName = engine->pfnGetLevelName();

	if( pszLevelName && *pszLevelName )
	{
		staticPanel->SetVisible( false );
		g_pTaskbar->SetVisible( false );

		if( !TheCareerGame ||
			!TheCareerGame->IsPlayingMatch() ||
			TheCareerGame->m_canUnpause )
		{
			engine->pfnClientCmd( "unpause" );
		}

		engine->pfnClientCmd( "hideconsole" );
	}
}

int CGameUI::IsGameUIActive()
{
	if( !m_bActivatedUI )
		return false;

	return staticPanel->IsVisible();
}

void CGameUI::LoadingStarted( const char* resourceType, const char* resourceName )
{
	g_VModuleLoader.PostMessageToAllModules(
		new KeyValues(
			"LoadingStarted",
			"type", resourceType,
			"name", resourceName
		)
	);

	if( !stricmp( resourceType, "transition" ) )
		staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_LOADINGTRANSITION );
	else
		staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_LOADING );
}

void CGameUI::LoadingFinished( const char* resourceType, const char* resourceName )
{
	g_VModuleLoader.PostMessageToAllModules(
		new KeyValues(
			"LoadingFinished",
			"type", resourceType,
			"name", resourceName
		)
	);

	staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_DESKTOPIMAGE );

	baseuifuncs->HideGameUI();
}

void CGameUI::StartProgressBar( const char* progressType, int progressSteps )
{
	if( !LoadingDialog() )
	{
		g_hLoadingDialog.Set( new CLoadingDialog( staticPanel ) );
	}

	staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_LOADING );

	m_pszCurrentProgressType = progressType;
	if( m_flProgressStartTime < 0.001 )
	{
		m_flProgressStartTime = vgui2::system()->GetCurrentTime();
	}

	LoadingDialog()->SetProgressRange( 0, progressSteps );
	LoadingDialog()->SetProgressPoint( 0 );
	LoadingDialog()->DisplayProgressBar();

	if( progressType &&
		( !strnicmp( progressType, "http://", 7 ) ||
		  !strnicmp( progressType, "https://", 8 ) )
	)
	{
		LoadingDialog()->SwitchToHttpDownloadDialog();
	}
}

int CGameUI::ContinueProgressBar( int progressPoint, float progressFraction )
{
	if( LoadingDialog() )
	{
		LoadingDialog()->SetProgressPoint( progressPoint );

		return true;
	}
	
	return false;
}

void CGameUI::StopProgressBar( bool bError, const char* failureReason, const char* extendedReason )
{
	if( !LoadingDialog() )
	{
		g_hLoadingDialog.Set( new CLoadingDialog( staticPanel ) );
	}

	if( LoadingDialog() )
	{
		if( bError )
		{
			LoadingDialog()->DisplayGenericError( failureReason, extendedReason );
		}
		else
		{
			LoadingDialog()->Close();

			g_hLoadingDialog.Set( nullptr );
		}

		staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_DESKTOPIMAGE );
	}
}

int CGameUI::SetProgressBarStatusText( const char* statusText )
{
	if( LoadingDialog() )
	{
		LoadingDialog()->SetStatusText( statusText );
		return true;
	}

	return false;
}

void CGameUI::SetSecondaryProgressBar( float progress )
{
	if( LoadingDialog() )
	{
		LoadingDialog()->SetSecondaryProgress( progress );
	}
}

void CGameUI::SetSecondaryProgressBarText( const char* statusText )
{
	if( LoadingDialog() )
	{
		LoadingDialog()->SetSecondaryProgressText( statusText );
	}
}

void CGameUI::ValidateCDKey( bool force, bool inConnect )
{
	//Nothing
}

void CGameUI::OnDisconnectFromServer( int eSteamLoginFailure, const char* username )
{
	DisconnectFromServer();

	switch( eSteamLoginFailure )
	{
	case STEAMLOGINFAILURE_NONE: break;

	case STEAMLOGINFAILURE_NOSTEAMLOGIN:
		{
			if( LoadingDialog() )
			{
				LoadingDialog()->DisplayNoSteamConnectionError();
			}
			break;
		}

	case STEAMLOGINFAILURE_VACBANNED:
		{
			if( LoadingDialog() )
			{
				LoadingDialog()->DisplayVACBannedError();
			}
			break;
		}

	case STEAMLOGINFAILURE_BADTICKET:
	case STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE:
	case STEAMLOGINFAILURE_CONNECTIONLOST:
	case STEAMLOGINFAILURE_NOCONNECTION:
		{
			if( LoadingDialog() )
			{
				LoadingDialog()->DisplaySteamConnectionError(
					username, static_cast<ESteamLoginFailure>( eSteamLoginFailure )
				);
			}
			break;
		}
	}
}

bool CGameUI::FindPlatformDirectory( char* platformDir, int bufferSize )
{
	strncpy( platformDir, "platform", bufferSize );
	return true;
}

void CGameUI::FinishCareerLoad()
{
	engine->pfnClientCmd( "mp3 stop\n" );
	engine->pfnClientCmd( "_careeraudio\n" );
	baseuifuncs->HideGameUI();
	staticPanel->SetBackgroundRenderState( CBasePanel::BACKGROUND_DESKTOPIMAGE );
}

void CGameUI::RunVguiTestScript( const char* script )
{
	//TODO: implement - Solokiller
	/*
	if( !m_hTestScriptController )
	{
		m_hTestScriptController = new vgui2::TestScriptController();
	}

	m_hTestScriptController->ExecuteScript( script );
	*/
}

void RunVguiTestScript()
{
	//Nothing
}