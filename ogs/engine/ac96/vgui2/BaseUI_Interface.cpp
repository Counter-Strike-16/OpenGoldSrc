#include "precompiled.hpp"
#include "quakedef.hpp"

#include "client.hpp"
#include "cdll_int.h"
#include "APIProxy.hpp"
#include "FilePaths.hpp"

#include "GameUI/ICareerUI.h"
#include "GameUI/IClientVGUI.h"
#include "GameUI/IGameConsole.h"
#include "GameUI/IGameUI.h"

#include <SDK_Color.h>

#include <html/ichromehtmlwrapper.h>
#include <vgui/IInput.h>
#include <vgui/IInputInternal.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Controls.h>

#include "BasePanel.hpp"
#include "BaseUI_Interface.hpp"
#include "BaseUISurface.hpp"
#include "EngineSurface.hpp"
#include "keys.hpp"
#include "SystemWrapper.hpp"

static CBaseUI g_BaseUI;

CBaseUI* g_pBaseUI = nullptr;

CBaseUI& BaseUI()
{
	return g_BaseUI;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CBaseUI, IBaseUI, BASEUI_INTERFACE_VERSION, g_BaseUI );
EXPOSE_SINGLE_INTERFACE( CEngineVGui, IEngineVGui, ENGINEVGUI_INTERFACE_VERSION );

IGameUI* staticGameUIFuncs = nullptr;

IGameConsole* staticGameConsole = nullptr;

ICareerUI* staticCareerUI = nullptr;

IClientVGUI* staticClient = nullptr;

bool m_bConsoleShowing = false;

cl_enginefunc_t gEngfuncs;

static CStaticPanel* staticPanel = nullptr;

CEnginePanel* staticClientDLLPanel = nullptr;

CEnginePanel* staticGameUIPanel = nullptr;

BaseUISurface* staticSurface = nullptr;

IMouseControl* mousecontrol = nullptr;

namespace vgui2
{
//TODO: never actually used? - Solokiller
ISurface* g_pSurface = nullptr;
}

CBaseUI::CBaseUI()
{
	g_pBaseUI = &g_BaseUI;
}

CBaseUI::~CBaseUI()
{
	g_pBaseUI = nullptr;
}

void CBaseUI::Initialize( CreateInterfaceFn* factories, int count )
{
	if( staticGameUIFuncs )
		return;

	m_FactoryList[ 0 ] = factories[ 0 ];
	auto pFSFactory = factories[ 1 ];
	m_FactoryList[ 2 ] = pFSFactory;

	//TODO: Verify that it exists? - Solokiller
	pFSFactory( FILESYSTEM_INTERFACE_VERSION, nullptr );

	m_hVGuiModule = Sys_LoadModule( "vgui2" DEFAULT_SO_EXT );
	m_FactoryList[ 1 ] = Sys_GetFactory( m_hVGuiModule );

	m_hChromeModule = Sys_LoadModule( "chromehtml" DEFAULT_SO_EXT );
	m_FactoryList[ 3 ] = Sys_GetFactory( m_hChromeModule );

	m_iNumFactories = 4;

	char szDllName[ 512 ];

	snprintf( szDllName, ARRAYSIZE( szDllName ), "valve/cl_dlls/gameui" DEFAULT_SO_EXT );
	COM_FixSlashes( szDllName );
	FS_GetLocalCopy( szDllName );
	m_hStaticGameUIModule = Sys_LoadModule( szDllName );

	auto gameUIFactory = Sys_GetFactory( m_hStaticGameUIModule );

	m_FactoryList[ m_iNumFactories ] = gameUIFactory;

	if( gameUIFactory )
	{
		staticGameUIFuncs = static_cast<IGameUI*>( gameUIFactory( GAMEUI_INTERFACE_VERSION, nullptr ) );
		staticGameConsole = static_cast<IGameConsole*>( gameUIFactory( GAMECONSOLE_INTERFACE_VERSION, nullptr ) );
		staticCareerUI = static_cast<ICareerUI*>( gameUIFactory( CAREERUI_INTERFACE_VERSION, nullptr ) );

		++m_iNumFactories;
	}

	g_InputInternal = static_cast<vgui2::IInputInternal*>( m_FactoryList[ 1 ]( VGUI_INPUTINTERNAL_INTERFACE_VERSION, nullptr ) );

	if( cl_funcs.pClientFactory )
	{
		auto factoryFn = Sys_GetFactory( hClientDLL );

		if( factoryFn )
		{
			m_FactoryList[ m_iNumFactories++ ] = factoryFn;

			staticClient = static_cast<IClientVGUI*>( factoryFn( CLIENTVGUI_INTERFACE_VERSION, nullptr ) );
		}
	}

	vgui2::VGui_InitInterfacesList( "BaseUI", m_FactoryList, m_iNumFactories );
}

void CBaseUI::Start( cl_enginefunc_t* engineFuncs, int interfaceVersion )
{
	memcpy( &gEngfuncs, engineFuncs, sizeof( gEngfuncs ) );

	staticPanel = new CStaticPanel( nullptr, "StaticPanel" );

	staticPanel->SetCursor( vgui2::dc_none );

	{
		SDK_Color color = staticPanel->GetBgColor();

		//Set alpha to maximum.
		staticPanel->SetBgColor( SDK_Color( color.r(), color.g(), color.b(), 0xFF ) );
	}

	staticPanel->SetBounds( 0, 0, 40, 30 );

	staticPanel->SetPaintBorderEnabled( false );
	staticPanel->SetPaintBackgroundEnabled( false );
	staticPanel->SetPaintEnabled( false );

	staticPanel->SetCursor( vgui2::dc_none );
	staticPanel->SetVisible( true );

	staticPanel->SetZPos( 0 );

	staticSurface = static_cast<BaseUISurface*>( m_FactoryList[ 0 ]( VGUI_SURFACE_INTERFACE_VERSION_GS, nullptr ) );
	vgui2::g_pSurface = staticSurface;

	//Doesn't inherit from IBaseInterface
	auto pHTMLController = reinterpret_cast<IHTMLChromeController*>( m_FactoryList[ 3 ]( CHROMEHTML_CONTROLLER_INTERFACE_VERSION, nullptr ) );
	
	auto pEngineSurface = static_cast<IEngineSurface*>( m_FactoryList[ 0 ]( ENGINESURFACE_INTERFACE_VERSION, nullptr ) );

	g_BaseUISurface.Init( staticPanel->GetVPanel(), pEngineSurface, pHTMLController );

	staticSurface->m_bIgnoreMouseVisCalc = true;

	mousecontrol = staticSurface;

	//TODO: typo in con printf - Solokiller
	if( !vgui2::scheme()->LoadSchemeFromFile( "resource/trackerscheme.res", "BaseUI" ) )
		gEngfuncs.Con_Printf( "Error loading '%s'\n", "resource/trackerScheme.res" );

	//Add localization files.

	//Add localization for Half-Life.
	vgui2::localize()->AddFile( vgui2::filesystem(), "resource/valve_%language%.txt" );

	//TODO: should be FILENAME_MAX - Solokiller
	char szGameDir[ 32 ];

	strncpy( szGameDir, gEngfuncs.pfnGetGameDirectory(), ARRAYSIZE( szGameDir ) - 1 );
	szGameDir[ ARRAYSIZE( szGameDir ) - 1 ] = '\0';

	//Add all fallback localizations.
	for( int i = 0; i < g_fallbackLocalizationFiles.Count(); ++i )
	{
		vgui2::localize()->AddFile( vgui2::filesystem(), g_fallbackLocalizationFiles[ i ] );

		if( g_fallbackLocalizationFiles[ i ] )
			delete[] g_fallbackLocalizationFiles[ i ];
	}

	g_fallbackLocalizationFiles.RemoveAll();

	//Add fallback for other games.
	if( memcmp( szGameDir, "valve", 6 ) )
	{
		//TODO: should be FILENAME_MAX - Solokiller
		char szFileName[ 128 ];
		snprintf( szFileName, ARRAYSIZE( szFileName ) - 1, "resource/%s_%%language%%.txt", szGameDir );
		szFileName[ ARRAYSIZE( szFileName ) - 1 ] = '\0';

		vgui2::localize()->AddFile( vgui2::filesystem(), szFileName );
	}

	vgui2::ivgui()->Start();
	vgui2::ivgui()->SetSleep( false );

	staticClientDLLPanel = new CEnginePanel( staticPanel, "BaseClientPanel" );

	{
		SDK_Color color = staticClientDLLPanel->GetBgColor();

		//Set alpha to maximum.
		staticClientDLLPanel->SetBgColor( SDK_Color( color.r(), color.g(), color.b(), 0xFF ) );
	}

	staticClientDLLPanel->SetPaintBorderEnabled( false );
	staticClientDLLPanel->SetPaintBackgroundEnabled( false );
	staticClientDLLPanel->SetPaintEnabled( false );

	staticClientDLLPanel->SetVisible( true );
	staticClientDLLPanel->SetCursor( vgui2::dc_none );

	//Draw above static panel.
	staticClientDLLPanel->SetZPos( 25 );

	staticGameUIPanel = new CEnginePanel( staticPanel, "BaseGameUIPanel" );

	{
		SDK_Color color = staticGameUIPanel->GetBgColor();

		//Set alpha to maximum.
		staticGameUIPanel->SetBgColor( SDK_Color( color.r(), color.g(), color.b(), 0xFF ) );
	}

	staticGameUIPanel->SetPaintBorderEnabled( false );
	staticGameUIPanel->SetPaintBackgroundEnabled( false );
	staticGameUIPanel->SetPaintEnabled( false );

	staticGameUIPanel->SetVisible( true );
	staticGameUIPanel->SetCursor( vgui2::dc_none );

	//Draw above static and client panels.
	staticGameUIPanel->SetZPos( 50 );

	if( staticGameUIFuncs )
	{
		staticGameUIFuncs->Initialize( m_FactoryList, m_iNumFactories );
	}

	if( staticClient )
	{
		staticClient->Initialize( m_FactoryList, m_iNumFactories );
		staticSurface->SetVGUI2MouseControl( true );
	}

	if( staticGameUIFuncs )
	{
		staticGameUIFuncs->Start( &gEngfuncs, CLDLL_INTERFACE_VERSION, &gSystemWrapper );
	}

	staticClientDLLPanel->SetScheme( "ClientScheme" );

	if( staticClient )
	{
		staticClient->Start();
		staticClient->SetParent( staticClientDLLPanel->GetVPanel() );
	}

	int swide, stall;
	vgui2::surface()->GetScreenSize( swide, stall );

	staticPanel->SetBounds( 0, 0, swide, stall );
	staticGameUIPanel->SetBounds( 0, 0, swide, stall );
	staticClientDLLPanel->SetBounds( 0, 0, swide, stall );

	staticGameConsole->Initialize();

	if( staticGameConsole )
	{
		staticGameConsole->SetParent( staticGameUIPanel->GetVPanel() );
	}

	staticSurface->m_bIgnoreMouseVisCalc = false;

	staticGameUIFuncs->ActivateGameUI();

	if( COM_CheckParm( "-console" ) )
	{
		ShowConsole();
	}
}

void CBaseUI::Shutdown()
{
	//TODO: both the Windows and Linux versions reverse engineer this as RunFrame.
	//Shouldn't this be Shutdown? - Solokiller
	vgui2::ivgui()->RunFrame();

	staticGameUIFuncs->Shutdown();

	//TODO: should be FILENAME_MAX - Solokiller
	char szGameDir[ 32 ];
	strncpy( szGameDir, gEngfuncs.pfnGetGameDirectory(), ARRAYSIZE( szGameDir ) - 1 );
	szGameDir[ ARRAYSIZE( szGameDir ) - 1 ] = '\0';

	if( staticClient && ( !stricmp( szGameDir, "cstrike" ) && !stricmp( szGameDir, "czero" ) ) )
		staticClient->Shutdown();

	Sys_UnloadModule( m_hStaticGameUIModule );
	m_hStaticGameUIModule = nullptr;

	staticGameUIFuncs = nullptr;
	staticGameConsole = nullptr;
	staticCareerUI = nullptr;

	vgui2::system()->SaveUserConfigFile();

	Sys_UnloadModule( m_hVGuiModule );
	m_hVGuiModule = nullptr;

	staticSurface->Shutdown();

	if( m_hChromeModule )
	{
		Sys_UnloadModule( m_hChromeModule );
		m_hChromeModule = nullptr;
	}
}

int CBaseUI::Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !toggleconsole_key || toggleconsole_key != keynum )
	{
		if( keynum == K_ESCAPE && down )
		{
			const char* pszLevelName = gEngfuncs.pfnGetLevelName();

			if( staticGameUIFuncs->IsGameUIActive() &&
				pszLevelName &&
				*pszLevelName )
			{
				HideGameUI();
				return true;
			}
			else
			{
				ActivateGameUI();
				return true;
			}
		}
		else if( m_bHidingGameUI && keynum == K_MOUSE1 && down )
		{
			m_bHidingGameUI = false;
			return true;
		}
		else
		{
			return vgui2::surface()->NeedKBInput();
		}
	}

	return false;
}

void CBaseUI::CallEngineSurfaceAppHandler( void* pEvent, void* pUserData )
{
	if( staticSurface )
		staticSurface->AppHandler( pEvent, pUserData );
}

void CBaseUI::Paint( int x, int y, int right, int bottom )
{
	if( staticSurface )
	{
		auto embeddedPanel = staticSurface->GetEmbeddedPanel();

		if( embeddedPanel )
		{
			staticGameUIFuncs->RunFrame();
			vgui2::ivgui()->RunFrame();

			g_BaseUISurface.SetScreenBounds( x, y, right - x, bottom - y );
			staticPanel->SetBounds( 0, 0, right, bottom );
			staticGameUIPanel->SetBounds( 0, 0, right, bottom );
			staticClientDLLPanel->SetBounds( 0, 0, right, bottom );

			static_cast<vgui2::IClientPanel*>( staticPanel )->Think();

			vgui2::surface()->PaintTraverse( embeddedPanel );
		}
	}
}

void CBaseUI::HideGameUI()
{
	staticGameUIFuncs->HideGameUI();
	staticGameConsole->Hide();

	const char* pszLevelName = gEngfuncs.pfnGetLevelName();

	if( pszLevelName && *pszLevelName )
	{
		staticGameUIPanel->SetVisible( false );
		staticGameUIPanel->SetPaintBackgroundEnabled( false );

		staticClientDLLPanel->SetVisible( true );
		staticClientDLLPanel->SetMouseInputEnabled( true );

		if( staticClient )
			staticClient->ActivateClientUI();
	}

	if( vgui2::input()->IsMouseDown( vgui2::MOUSE_LEFT ) )
		m_bHidingGameUI = true;
}

void CBaseUI::ActivateGameUI()
{
	staticGameUIFuncs->ActivateGameUI();
	staticGameUIPanel->SetVisible( true );

	staticClientDLLPanel->SetVisible( false );
	staticClientDLLPanel->SetMouseInputEnabled( false );

	if( staticClient )
	{
		staticClient->HideClientUI();
		staticClient->HideScoreBoard();
	}

	if( m_bConsoleShowing )
		staticGameConsole->Activate();
}

void CBaseUI::HideConsole()
{
	m_bConsoleShowing = false;
	staticGameConsole->Hide();
}

void CBaseUI::ShowConsole()
{
	m_bConsoleShowing = true;
	staticGameConsole->Activate();
}

vgui2::VPANEL CEngineVGui::GetPanel( VGUIPANEL type )
{
	switch( type )
	{
	default:
	case PANEL_ROOT:		return staticPanel->GetVPanel();
	case PANEL_CLIENTDLL:	return staticClientDLLPanel->GetVPanel();
	case PANEL_GAMEUIDLL:	return staticGameUIPanel->GetVPanel();
	}
}

bool CEngineVGui::SteamRefreshLogin( const char* password, bool isSecure )
{
	return true;
}

bool CEngineVGui::SteamProcessCall( bool* finished, TSteamProgress* progress, TSteamError* steamError )
{
	return true;
}

void CEngineVGui::SetEngineVisible( bool state )
{
	m_bVisible = state;

	if( !g_bIsDedicatedServer )
	{
		if( state )
			ClientDLL_ActivateMouse();
		else
			ClientDLL_DeactivateMouse();
	}
}