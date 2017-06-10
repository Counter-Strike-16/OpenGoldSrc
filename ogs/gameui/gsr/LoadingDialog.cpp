#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/ProgressBar.h>

#include "BitmapImagePanel.h"
#include "CareerGame.h"
#include "CareerMapFrame.h"
#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "igameuifuncs.h"
#include "LoadingDialog.h"
#include "ModInfo.h"

CLoadingDialog* LoadingDialog()
{
	return static_cast<CLoadingDialog*>(g_hLoadingDialog.Get() );
}

CareerRichText::CareerRichText( vgui2::Panel* parent )
	: BaseClass( parent, "CareerInfo" )
{
}

CareerRichText::~CareerRichText()
{
}

void CareerRichText::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_addedScrollbar = false;
	m_fontHeight = vgui2::surface()->GetFontTall( pScheme->GetFont( "Default", IsProportional() ) );
}

void CareerRichText::PerformLayout()
{
	BaseClass::PerformLayout();

	int wide, tall;
	GetSize( wide, tall );

	if( ( m_fontHeight + 1 ) * GetNumLines() + 3 > tall && !m_addedScrollbar )
	{
		SetVerticalScrollbar( true );
		m_addedScrollbar = true;
		InsertString( L"" );
	}
}

CLoadingDialog::CLoadingDialog( vgui2::Panel* parent )
	: BaseClass( parent, "LoadingDialog", true )
{
	SetSize( 416, 100 );
	SetTitle( "#GameUI_Loading", true );

	m_pProgress = new vgui2::ProgressBar( this, "Progress" );
	m_pProgress2 = new vgui2::ProgressBar( this, "Progress2" );
	m_pInfoLabel = new vgui2::Label( this, "InfoLabel", "" );
	m_pCancelButton = new vgui2::Button( this, "CancelButton", "#GameUI_Cancel" );
	m_pTimeRemainingLabel = new vgui2::Label( this, "TimeRemainingLabel", "" );

	m_pCancelButton->SetCommand( "Cancel" );

	m_pCareerMapDescLabel = nullptr;

	m_pInfoLabel->SetBounds( 20, 32, 392, 24 );
	m_pProgress->SetBounds( 20, 64, 300, 24 );
	m_pCancelButton->SetBounds( 330, 64, 72, 24 );

	m_pInfoLabel->SetTextColorState( vgui2::Label::CS_DULL );

	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );

	m_pProgress2->SetVisible( false );

	if( TheCareerGame && TheCareerGame->IsPlayingMatch() )
	{
		m_pCareerMapDescLabel = new CareerRichText( this );
		m_isCareerLoadingDialog = true;

		//TODO: should be MAX_PATH - Solokiller
		char buf[ 128 ];

		snprintf( buf, ARRAYSIZE( buf ), "gfx/thumbnails/maps/%s", TheCareerGame->GetLastMap() );

		m_pCareerMapImagePanel = new CBitmapImagePanel( this, "CareerMapImage", nullptr );
		m_pCareerMapImagePanel->AddActionSignalTarget( this );
		m_pCareerMapImagePanel->setTexture( buf );

		m_pCareerMapLabel = new vgui2::Label( this, "CareerMapTitle", "" );

		snprintf( buf, ARRAYSIZE( buf ), "#Career_Map_%s", TheCareerGame->GetLastMap() );

		if( !vgui2::localize()->Find( buf ) )
		{
			snprintf( buf, ARRAYSIZE( buf ), "%s", TheCareerGame->GetLastMap() );
		}

		m_pCareerMapLabel->SetText( buf );

		m_pCareerScenarioInfo = new vgui2::Label( this, "CareerScenarioInfo", "" );

		if( TheCareerGame->PlayAsCT() )
		{
			if( !strncmp( "de_", TheCareerGame->GetLastMap(), 3 ) )
			{
				m_pCareerScenarioInfo->SetText( "#Career_BombDefusalCT" );
			}
			else
			{
				m_pCareerScenarioInfo->SetText( "#Career_HostageRescueCT" );
			}
		}
		else
		{
			if( !strncmp( "de_", TheCareerGame->GetLastMap(), 3 ) )
			{
				m_pCareerScenarioInfo->SetText( "#Career_BombDefusalT" );
			}
			else
			{
				m_pCareerScenarioInfo->SetText( "#Career_HostageRescueT" );
			}
		}

		auto pScheme = vgui2::scheme()->GetIScheme( GetScheme() );

		//TODO: verify that these colors are correct - Solokiller
		auto normalColor = pScheme->GetColor( "ControlText", SDK_Color( 255, 255, 255, 255 ) );
		auto friendlyFireColor = pScheme->GetColor( "BrightControlText", SDK_Color( 255, 255, 255, 255 ) );

		ShowCareerMapDescription(
			m_pCareerMapDescLabel,
			TheCareerGame->GetLastMapPtr(),
			normalColor,
			friendlyFireColor
		);

		LoadControlSettings( "Resource/CareerLoadingDialog.res" );
		m_pCareerMapDescLabel->SetVerticalScrollbar( false );
	}
	else if( engine->CheckParm( "-steam", nullptr ) || engine->CheckParm( "-showplatform", nullptr ) )
	{
		if( gameuifuncs->IsConnectedToVACSecureServer() )
		{
			LoadControlSettings( "Resource/LoadingDialogVAC.res" );
			m_bShowingVACInfo = true;
		}
		else
		{
			LoadControlSettings( "Resource/LoadingDialog.res" );
		}
	}
	else
	{
		if( ModInfo().BShowSimpleLoadingDialog() )
			LoadControlSettings( "Resource/LoadingDialogNoBannerSingle.res" );
		else
			LoadControlSettings( "Resource/LoadingDialogNoBanner.res" );
	}
}

CLoadingDialog::~CLoadingDialog()
{
}

void CLoadingDialog::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	if( m_pCareerMapImagePanel )
	{
		m_pCareerMapImagePanel->SetBorder( pScheme->GetBorder( "InsetBorder" ) );
	}
}

void CLoadingDialog::OnClose()
{
	vgui2::surface()->RestrictPaintToSinglePanel( NULL_HANDLE );
	BaseClass::OnClose();
	MarkForDeletion();
}

void CLoadingDialog::PerformLayout()
{
	MoveToCenterOfScreen();
	BaseClass::PerformLayout();
}

void CLoadingDialog::OnKeyCodePressed( vgui2::KeyCode code )
{
	if( code == vgui2::KEY_ESCAPE )
		OnCommand( "Cancel" );
	else
		BaseClass::OnKeyCodePressed( code );
}

void CLoadingDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Cancel" ) )
	{
		if( m_isHttpDownloadDialog )
		{
			engine->pfnClientCmd( "httpstop\n" );
		}
		else
		{
			if( TheCareerGame &&
				TheCareerGame->IsPlayingMatch() &&
				!m_pProgress->IsVisible() )
			{
				GameUI().FinishCareerLoad();
			}

			engine->pfnClientCmd( "disconnect\n" );
		}
		
		Close();
	}
	else if( !stricmp( command, "StartCareer" ) )
	{
		GameUI().FinishCareerLoad();
		Close();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CLoadingDialog::DisplayProgressBar()
{
	SetTitle( "#GameUI_Loading", true );

	vgui2::surface()->RestrictPaintToSinglePanel( GetVPanel() );

	Activate();

	m_pProgress->SetVisible( true );

	if( !ModInfo().BShowSimpleLoadingDialog() )
	{
		m_pInfoLabel->SetVisible( true );
	}

	m_pInfoLabel->SetText( L"" );

	m_pCancelButton->SetText( "#GameUI_Cancel" );

	m_pCancelButton->SetCommand( "Cancel" );
}

void CLoadingDialog::SetupControlSettingsForErrorDisplay( const char* settingsFile )
{
	SetTitle( "#GameUI_Disconnected", true );
	LoadControlSettings( settingsFile );

	vgui2::surface()->RestrictPaintToSinglePanel( GetVPanel() );

	Activate();
	m_pProgress->SetVisible( false );
	m_pInfoLabel->SetVisible( true );

	m_pCancelButton->SetText( "#GameUI_Close" );
	m_pCancelButton->SetCommand( "Close" );

	m_pInfoLabel->SetText( "" );
	m_pCancelButton->SetEnabled( false );

	if( m_pCareerMapDescLabel )
	{
		m_pCareerMapDescLabel->SetVisible( false );
		m_pCareerMapLabel->SetVisible( false );
		m_pCareerScenarioInfo->SetVisible( false );
		m_pCareerMapImagePanel->SetVisible( false );
	}
}

void CLoadingDialog::DisplayGenericError( const char* failureReason, const char* extendedReason )
{
	if( m_bShowingVACError )
	{
		m_bShowingVACError = false;
		return;
	}

	SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogError.res" );

	if( !extendedReason || !( *extendedReason ) )
	{
		m_pInfoLabel->SetText( failureReason );
		return;
	}

	wchar_t finalMsg[ 512 ];
	wchar_t compositeReason[ 256 ];
	wchar_t formatStr[ 256 ];

	const wchar_t* pszLocalized;
	
	if( *extendedReason == '#'
		&& ( ( pszLocalized = vgui2::localize()->Find( extendedReason ) ) != nullptr ) )
	{
		wcsncpy( compositeReason, pszLocalized, ARRAYSIZE( compositeReason ) );
	}
	else
	{
		vgui2::localize()->ConvertANSIToUnicode( extendedReason, compositeReason, sizeof( compositeReason ) );
	}

	if( *failureReason == '#'
		&& ( ( pszLocalized = vgui2::localize()->Find( failureReason ) ) != nullptr ) )
	{
		wcsncpy( formatStr, pszLocalized, ARRAYSIZE( compositeReason ) );
	}
	else
	{
		vgui2::localize()->ConvertANSIToUnicode( failureReason, formatStr, sizeof( formatStr ) );
	}

	vgui2::localize()->ConstructString(
		finalMsg, sizeof( finalMsg ),
		formatStr, 1,
		compositeReason
	);

	m_pInfoLabel->SetText( finalMsg );
}

void CLoadingDialog::DisplayVACBannedError()
{
	SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogErrorVACBanned.res" );
	SetTitle( "#VAC_ConnectionRefusedTitle", true );

	m_bShowingVACError = true;
}

void CLoadingDialog::DisplayNoSteamConnectionError()
{
	SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogErrorNoSteamConnection.res" );
	m_bShowingVACError = true;
}

void CLoadingDialog::DisplaySteamConnectionError( const char* username, ESteamLoginFailure steamLoginFailure )
{
	switch( steamLoginFailure )
	{
	case STEAMLOGINFAILURE_CONNECTIONLOST:
		SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogConnectionLost.res" );
		break;
	case STEAMLOGINFAILURE_NOCONNECTION:
		SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogErrorNoSteamConnection.res" );
		break;
	case STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE:
		SetupControlSettingsForErrorDisplay( "Resource/LoadingDialogErrorLoggedInElsewhere.res" );
		break;

	default: break;
	}

	m_bShowingVACError = true;

	m_pCancelButton->SetText( "#GameUI_Close" );
	m_pCancelButton->SetCommand( "close" );

	strncpy( m_szUserName, username, ARRAYSIZE( m_szUserName ) );
	m_szUserName[ ARRAYSIZE( m_szUserName ) - 1 ] = '\0';
}

void CLoadingDialog::SetStatusText( const char* statusText )
{
	m_pInfoLabel->SetText( statusText );
}

void CLoadingDialog::SetProgressPoint( int progressPoint )
{
	if( !m_bShowingVACInfo && gameuifuncs->IsConnectedToVACSecureServer() )
	{
		LoadControlSettings( "Resource/LoadingDialogVAC.res" );
		m_bShowingVACInfo = true;
	}

	m_flProgress = static_cast<double>( progressPoint ) / ( m_iRangeMax - m_iRangeMin );

	m_pProgress->SetProgress( m_flProgress );
}

void CLoadingDialog::SetProgressRange( int min, int max )
{
	m_iRangeMin = min;
	m_iRangeMax = max;
}

void CLoadingDialog::SetSecondaryProgress( float progress )
{
	if( !m_bShowingSecondaryProgress )
	{
		if( progress > 0.99 )
			return;

		if( m_bShowingVACInfo || gameuifuncs->IsConnectedToVACSecureServer() )
		{
			LoadControlSettings( "Resource/LoadingDialogDualProgressVAC.res" );
			m_bShowingVACInfo = true;
		}
		else
		{
			LoadControlSettings( "Resource/LoadingDialogDualProgress.res" );
		}

		m_bShowingSecondaryProgress = true;
		m_pProgress->SetProgress( m_flProgress );

		m_pProgress2->SetVisible( true );

		m_flSecondaryProgressStartTime = vgui2::system()->GetFrameTime();
	}

	if( progress > m_flSecondaryProgress )
	{
		m_pProgress2->SetProgress( progress );
		m_flSecondaryProgress = progress;
		m_flLastSecondaryProgressUpdateTime = vgui2::system()->GetFrameTime();
	}

	if( m_flSecondaryProgress > progress )
	{
		m_pProgress2->SetProgress( progress );
		m_flSecondaryProgress = progress;
		m_flLastSecondaryProgressUpdateTime = vgui2::system()->GetFrameTime();
		m_flSecondaryProgressStartTime = vgui2::system()->GetFrameTime();
	}
}

void CLoadingDialog::SetSecondaryProgressText( const char* statusText )
{
	SetControlString( "SecondaryProgressLabel", statusText );
}

void CLoadingDialog::SwitchToHttpDownloadDialog()
{
	m_isHttpDownloadDialog = true;
	LoadControlSettings( "Resource/LoadingDialog.res" );
}

void CLoadingDialog::SwitchToPausedCareerDialog()
{
	if( m_isCareerLoadingDialog )
	{
		m_switchTime = vgui2::system()->GetFrameTime() + 1.5;
	}
	else
	{
		GameUI().FinishCareerLoad();
		Close();
	}
}

void CLoadingDialog::CompleteSwitchToPausedCareerDialog()
{
	if( m_isCareerLoadingDialog )
	{
		m_switchTime = 0;

		m_pCancelButton->SetText( "#Career_Start" );
		m_pCancelButton->SetCommand( "StartCareer" );

		int bX, bY, bW, bH;
		m_pCancelButton->GetBounds( bX, bY, bW, bH );

		int pX, pY, pW, pH;
		m_pProgress->GetBounds( pX, pY, pW, pH );

		int iX, iY, iW, iH;
		m_pInfoLabel->GetBounds( iX, iY, iW, iH );

		int wide = GetWide();
		bX = wide / 2 - bW;
		bY = iY + ( pH + pY - iY ) / 2 - bH / 2;

		m_pProgress->SetVisible( false );
		m_pInfoLabel->SetVisible( false );

		m_pCancelButton->SetBounds( bX, bY, 2 * bW, bH );
		m_pCancelButton->SetContentAlignment( vgui2::Label::a_center );
		m_pCancelButton->SetEnabled( true );
	}
}

void CLoadingDialog::OnThink()
{
	BaseClass::OnThink();

	if( m_bShowingSecondaryProgress )
	{
		if( m_flSecondaryProgress >= 1.0 )
		{
			m_pTimeRemainingLabel->SetText( "complete" );
		}
		else
		{
			wchar_t unicode[ 512 ];

			if( vgui2::ProgressBar::ConstructTimeRemainingString(
				unicode,
				sizeof( unicode ),
				m_flSecondaryProgressStartTime,
				vgui2::system()->GetFrameTime(),
				m_flSecondaryProgress,
				m_flLastSecondaryProgressUpdateTime,
				true ) )
			{
				m_pTimeRemainingLabel->SetText( unicode );
			}
			else
			{
				m_pTimeRemainingLabel->SetText( "" );
			}
		}
	}

	if( m_switchTime && m_switchTime < vgui2::system()->GetFrameTime() )
	{
		CompleteSwitchToPausedCareerDialog();
	}
}
