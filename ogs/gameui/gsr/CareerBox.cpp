#include <KeyValues.h>
#include <vgui/IInputInternal.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>

#include "CareerBaseFrame.h"
#include "CareerBox.h"
#include "CareerGame.h"
#include "CareerProfile.h"
#include "CvarTextEntry.h"
#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"
#include "shared_util.h"

CCareerBaseBox::CCareerBaseBox( vgui2::Panel* parent, const char* panelName, bool loadResources )
	: BaseClass( parent, panelName, false )
{
	m_bgColor = SDK_Color( 0, 0, 0, 0 );

	SetScheme( "CareerBoxScheme" );
	SetProportional( true );
	SetMoveable( false );
	SetSizeable( false );

	m_bgColor = SDK_Color( 0, 0, 0, 0 );

	m_pTopLeftPanel = new vgui2::CBitmapImagePanel( this, "TopLeftPanel", "gfx/career/round_corner_nw" );
	m_pTopRightPanel = new vgui2::CBitmapImagePanel( this, "TopRightPanel", "gfx/career/round_corner_ne" );
	m_pBottomLeftPanel = new vgui2::CBitmapImagePanel( this, "BottomLeftPanel", "gfx/career/round_corner_sw" );
	m_pBottomRightPanel = new vgui2::CBitmapImagePanel( this, "BottomLeftPanel", "gfx/career/round_corner_se" );

	m_pTextLabel = new vgui2::Label( this, "TextLabel", "" );
	m_pOkButton = new CCareerButton( this, "OkButton", "", "", false );
	m_pCancelButton = new CCareerButton( this, "CancelButton", "", "", false );

	m_pOkButton->SetVisible( false );

	AddButton( m_pOkButton );
	AddButton( m_pCancelButton );

	if( loadResources )
	{
		auto pszResourceName = new char[ strlen( panelName ) + 14 ];

		strcpy( pszResourceName, "resource/" );
		strcat( pszResourceName, panelName );
		strcat( pszResourceName, ".res" );

		LoadControlSettings( pszResourceName );

		delete[] pszResourceName;
	}
}

void CCareerBaseBox::ShowWindow()
{
	SetVisible( true );
	SetEnabled( true );
	MoveToFront();

	if( m_pCancelButton->IsVisible() && m_cancelFocus )
	{
		m_pCancelButton->RequestFocus();
	}
	else if( m_pOkButton->IsVisible() )
	{
		m_pOkButton->RequestFocus();
	}
	else
	{
		RequestFocus();
	}

	InvalidateLayout();
}

void CCareerBaseBox::OnCommand( const char* command )
{
	if( stricmp( command, "close" ) )
	{
		PostMessage( this, new KeyValues( "Command", "command", command ) );
	}

	BaseClass::OnCommand( command );

	Close();
}

void CCareerBaseBox::DoModal()
{
	ShowWindow();

	vgui2::input()->SetAppModalSurface( GetVPanel() );
}

void CCareerBaseBox::PaintBackground()
{
	vgui2::surface()->DrawSetColor( m_bgColor );

	int x1, y1;
	int x2, y2;

	m_pTopLeftPanel->GetPos( x1, y1 );
	m_pBottomLeftPanel->GetPos( x2, y2 );

	y1 += m_pTopLeftPanel->GetTall();
	x2 += m_pBottomLeftPanel->GetWide();

	vgui2::surface()->DrawFilledRect( x1, y1, x2, y2 );

	m_pTopLeftPanel->GetPos( x1, y1 );
	m_pBottomRightPanel->GetPos( x2, y2 );

	x1 += m_pTopLeftPanel->GetWide();
	y2 += m_pBottomRightPanel->GetTall();

	vgui2::surface()->DrawFilledRect( x1, y1, x2, y2 );

	m_pTopRightPanel->GetPos( x1, y1 );
	m_pBottomRightPanel->GetPos( x2, y2 );

	y1 += m_pTopRightPanel->GetTall();
	x2 += m_pBottomRightPanel->GetWide();

	vgui2::surface()->DrawFilledRect( x1, y1, x2, y2 );
}

void CCareerBaseBox::PerformLayout()
{
	BaseClass::PerformLayout();

	int x, y, w, h;
	GetBounds( x, y, w, h );

	int screenWide, screenTall;
	vgui2::surface()->GetScreenSize( screenWide, screenTall );

	if( ( w / 2 + x ) != ( screenWide / 2 ) )
	{
		SetPos( screenWide / 2 - w / 2, screenTall / 2 - h / 2 );
		GetBounds( x, y, w, h );
	}

	m_pTopLeftPanel->SetBounds( 0, 0, 10, 10 );
	m_pTopRightPanel->SetBounds( w - 10, 0, 10, 10 );
	m_pBottomLeftPanel->SetBounds( 0, h - 10, 10, 10 );
	m_pBottomRightPanel->SetBounds( w - 10, h - 10, 10, 10 );
}

void CCareerBaseBox::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	for( int i = 0; i < m_buttons.Count(); ++i )
	{
		auto pButton = m_buttons[ i ];

		pButton->SetArmedSound( "sound/UI/buttonrollover.wav" );
		pButton->SetDepressedSound( "sound/UI/buttonclick.wav" );
		pButton->SetReleasedSound( "sound/UI/buttonclickrelease.wav" );
	}

	m_bgColor = GetSchemeColor( "CareerBoxBG", GetSchemeColor( "ControlBG", pScheme ), pScheme );

	m_pTopLeftPanel->SetBgColor( SDK_Color( 255, 255, 255, m_bgColor.a() ) );
	m_pTopRightPanel->SetBgColor( SDK_Color( 255, 255, 255, m_bgColor.a() ) );
	m_pBottomLeftPanel->SetBgColor( SDK_Color( 255, 255, 255, m_bgColor.a() ) );
	m_pBottomRightPanel->SetBgColor( SDK_Color( 255, 255, 255, m_bgColor.a() ) );

	SetBorder( nullptr );
}

void CCareerBaseBox::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( code == vgui2::KEY_ESCAPE )
	{
		OnCommand( "Cancel" );
	}
	else if( code == vgui2::KEY_ENTER )
	{
		BaseClass::OnKeyCodeTyped( vgui2::KEY_SPACE );
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}

void CCareerBaseBox::SetCancelButtonAsDefault()
{
	m_cancelFocus = true;
}

void CCareerBaseBox::SetLabelText( const wchar_t* text )
{
	m_pTextLabel->SetText( text );
}

void CCareerBaseBox::AddButton( vgui2::Button* pButton )
{
	if( m_buttons.Find( pButton ) != m_buttons.InvalidIndex() )
		return;

	m_buttons.AddElement( pButton );
}

CCareerQueryBox::CCareerQueryBox( const char* title, const char* labelText, const char* panelName, vgui2::Panel* parent )
	: BaseClass( parent, panelName, true )
{
}

CCareerQueryBox::CCareerQueryBox( const wchar_t* title, const wchar_t* labelText, const char* panelName, vgui2::Panel* parent )
	: BaseClass( parent, panelName, true )
{
}

CCareerQueryBox::~CCareerQueryBox()
{
}

CCareerDifficultyBox::CCareerDifficultyBox( const char* panelName, vgui2::Panel* parent )
	: BaseClass( parent, panelName, false )
{
	CCareerButton* pDifficultyButton[ MAX_CAREER_DIFFICULTY ] = 
	{
		new CCareerButton( this, "Difficulty0Button", "", "", false ),
		new CCareerButton( this, "Difficulty1Button", "", "", false ),
		new CCareerButton( this, "Difficulty2Button", "", "", false ),
		new CCareerButton( this, "Difficulty3Button", "", "", false )
	};

	m_pEnableTutorCheck = new CCvarToggleCheckButton( this, "Tutor", "#CStrike_Tutor_Enabled", "tutor_enable" );
	
	for( auto pButton : pDifficultyButton )
	{
		AddButton( pButton );
	}

	AddButton( m_pEnableTutorCheck );
	
	LoadControlSettings( "resource/CareerBoxDifficulty.res" );

	auto pszPlayerName = engine->pfnGetCvarString( "name" );

	auto pszSaveName = CCareerGame::GetSaveGamePlayerName( pszPlayerName );

	CCareerProfileData* pProfile = TheCareerGame->GetProfileForName( pszSaveName );

	if( pszPlayerName )
	{
		m_pTextLabel->SetText( SharedWVarArgs( L"%hs", pszPlayerName ) );
	}

	m_pFocusButton = pDifficultyButton[ pProfile->lastDifficulty ];

	char buf[ 64 ];
	wchar_t constructedBuf[ 64 ];

	for( int i = 0; i < MAX_CAREER_DIFFICULTY; ++i )
	{
		snprintf( buf, ARRAYSIZE( buf ), "#Career_Difficulty%d", i );

		if( pProfile->difficulty[ i ].played )
		{
			auto pszContinue = vgui2::localize()->Find( "#Career_ContinueDifficulty" );
			auto pszDifficulty = vgui2::localize()->Find( buf );

			vgui2::localize()->ConstructString(
				constructedBuf, sizeof( constructedBuf ),
				L"%s1 (%s2)", 2,
				pszDifficulty, pszContinue
			);

			pDifficultyButton[ i ]->SetText( constructedBuf );
		}
		else
		{
			pDifficultyButton[ i ]->SetText( buf );
		}
	}

	delete pProfile;
}

void CCareerDifficultyBox::ShowWindow()
{
	SetVisible( true );
	SetEnabled( true );
	MoveToFront();

	m_pFocusButton->SetEnabled( false );
	
	InvalidateLayout();
}

void CCareerDifficultyBox::OnCommand( const char* command )
{
	if( strstr( command, "ChooseDifficulty" ) )
		m_pEnableTutorCheck->ApplyChanges();

	if( stricmp( command, "close" ) )
	{
		PostActionSignal( new KeyValues( "Command", "command", command ) );
	}

	BaseClass::OnCommand( command );

	Close();
}

CCareerNewProfileBox::CCareerNewProfileBox( const char* panelName, vgui2::Panel* parent )
	: BaseClass( parent, panelName, false )
{
	m_pEntry = new CCvarTextEntry( this, "ProfileTextEntry", "name" );

	LoadControlSettings( "resource/CareerBoxNewProfile.res" );

	m_pEntry->SetText( engine->pfnGetCvarString( "name" ) );
}

void CCareerNewProfileBox::ShowWindow()
{
	BaseClass::ShowWindow();

	m_pEntry->RequestFocus();

	InvalidateLayout();
}

void CCareerNewProfileBox::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( code == vgui2::KEY_ESCAPE )
	{
		OnCommand( "Cancel" );
	}
	else if( code == vgui2::KEY_ENTER )
	{
		OnCommand( "Ok" );
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}

void CCareerNewProfileBox::OnCommand( const char* command )
{
	if( !stricmp( command, "Ok" ) )
	{
		m_pEntry->ApplyChanges( true );

		//TODO: figure out what's going on here - Solokiller
		if( stricmp( "CreateProfile", "close" ) )
		{
			PostActionSignal( new KeyValues( "Command", "command", "CreateProfile" ) );
		}

		BaseClass::OnCommand( "CreateProfile" );
	}
	else
	{
		if( stricmp( command, "close" ) )
		{
			PostActionSignal( new KeyValues( "Command", "command", command ) );
		}

		BaseClass::OnCommand( command );
	}

	Close();
}
