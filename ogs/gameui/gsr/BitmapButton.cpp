#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Controls.h>

#include "BitmapButton.h"

CBitmapButton::CBitmapButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* normalImage, const char* armedImage, const char* clickedImage, const char* disabledImage )
	: BaseClass( parent, panelName, text )
{
	const char* const pszImages[] = 
	{
		normalImage,
		armedImage,
		clickedImage,
		disabledImage
	};

	for( int i = 0; i < IMAGE_MAX; ++i )
	{
		auto& imageID = m_ImageID[ i ];

		imageID.imageID = vgui2::surface()->CreateNewTextureID();

		vgui2::surface()->DrawSetTextureFile( imageID.imageID, pszImages[ i ] ? pszImages[ i ] : "", true, false );
		vgui2::surface()->DrawGetTextureSize( imageID.imageID, imageID.width, imageID.height );
	}

	m_shouldFlash = false;
	m_isFlashColor = false;
	m_transitionsLeft = 0;
	m_totalTransitions = 0;
}

void CBitmapButton::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetBorder( nullptr );
	SetDefaultBorder( nullptr );
	SetDepressedBorder( nullptr );
	SetKeyFocusBorder( nullptr );

	m_interval = strtod( pScheme->GetResourceString( "JumpButtonFlashPeriod" ), nullptr );
	m_totalTransitions = strtol( pScheme->GetResourceString( "JumpButtonNumFlashes" ), nullptr, 10 );
}

void CBitmapButton::PaintBackground()
{
	int image;

	if( IsEnabled() )
	{
		if( IsDepressed() )
		{
			image = IMAGE_CLICKED;
		}
		else
		{
			if( IsArmed() )
			{
				image = IMAGE_ARMED;
			}
			else
			{
				image = IMAGE_NORMAL;
			}
		}
	}
	else
	{
		image = IMAGE_DISABLED;
	}

	if( m_shouldFlash && m_isFlashColor && image == IMAGE_NORMAL )
		image = IMAGE_ARMED;

	int w, h;
	GetSize( w, h );

	vgui2::surface()->DrawSetColor( 255, 255, 255, 255 );
	vgui2::surface()->DrawSetTexture( m_ImageID[ image ].imageID );
	vgui2::surface()->DrawTexturedRect( 0, 0, w, h );
}

void CBitmapButton::OnThink()
{
	if( m_shouldFlash )
	{
		if( m_transitionsLeft < 0 )
		{
			if( m_totalTransitions )
				m_transitionsLeft = m_totalTransitions;
		}

		const float now = vgui2::system()->GetCurrentTime();

		if( now >= m_lastTransition + m_interval )
		{
			if( m_transitionsLeft > 0 )
			{
				m_isFlashColor = !m_isFlashColor;
				m_lastTransition = now;
				--m_transitionsLeft;
				return;
			}
		}

		if( m_transitionsLeft <= 0 )
			m_isFlashColor = false;
	}
}

void CBitmapButton::Flash()
{
	m_shouldFlash = true;
	m_transitionsLeft = -1;
}
