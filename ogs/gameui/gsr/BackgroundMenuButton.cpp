#include "BackgroundMenuButton.h"

CBackgroundMenuButton::CBackgroundMenuButton( vgui2::Panel* parent, const char* name )
	: BaseClass( parent, name, "" )
{
}

CBackgroundMenuButton::~CBackgroundMenuButton()
{
}

void CBackgroundMenuButton::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetFgColor( SDK_Color( 255, 255, 255, 255 ) );
	SetBgColor( SDK_Color( 0, 0, 0, 0 ) );

	SetDefaultColor( SDK_Color( 255, 255, 255, 255 ), SDK_Color( 0, 0, 0, 0 ) );
	SetArmedColor( SDK_Color( 255, 0, 255, 255 ), SDK_Color( 0, 0, 0, 0 ) );
	SetDepressedColor( SDK_Color( 255, 0, 255, 255 ), SDK_Color( 0, 0, 0, 0 ) );

	SetContentAlignment( vgui2::Label::a_west );

	SetBorder( nullptr );
	SetDefaultBorder( nullptr );
	SetDepressedBorder( nullptr );
	SetKeyFocusBorder( nullptr );
	SetTextInset( 0, 0 );

	SetArmedSound( "sound/UI/buttonrollover.wav" );
	SetDepressedSound( "sound/UI/buttonclick.wav" );
	SetReleasedSound( "sound/UI/buttonclickrelease.wav" );
}

void CBackgroundMenuButton::OnCommand( const char* command )
{
	BaseClass::OnCommand( command );
}
