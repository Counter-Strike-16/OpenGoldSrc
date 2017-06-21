#include <vgui_controls/TextImage.h>

#include "CareerBaseFrame.h"

CCareerButton::CCareerButton( vgui2::Panel* parent, const char* buttonName, const char* buttonText, const char* image, bool textFirst )
	: BaseClass( parent, buttonName, "" )
	, m_textFirst( textFirst )
{
	m_textImage = new vgui2::TextImage( buttonText );

	m_image = vgui2::scheme()->GetImage( image, true );
}

void CCareerButton::SetArmedBorder( vgui2::IBorder* border )
{
	m_armedBorder = border;
	InvalidateLayout();
}

void CCareerButton::Paint()
{
	int buttonWide, buttonTall;
	GetSize( buttonWide, buttonTall );

	int imageWide, imageTall;
	m_image->GetSize( imageWide, imageTall );

	int iTextStart = m_textPad;

	if( m_textFirst )
	{
		m_image->SetPos(
			buttonWide - imageWide - m_imagePad,
			( buttonTall - imageTall ) / 2
		);

		m_image->Paint();
	}
	else
	{
		m_image->SetPos(
			m_imagePad,
			( buttonTall - imageTall ) / 2
		);

		m_image->Paint();

		iTextStart += imageWide + m_imagePad;
	}

	int textWide, textTall;
	m_textImage->GetSize( textWide, textTall );

	const int iTextWidth = buttonWide - imageWide - m_imagePad + -2 * m_textPad;

	if( IsEnabled() )
	{
		m_textImage->SetColor( m_textNormalColor );
	}
	else
	{
		m_textImage->SetColor( m_textDisabledColor );
	}

	m_textImage->SetPos(
		( iTextWidth - textWide ) / 2 + iTextStart,
		( buttonTall - textTall ) / 2
	);

	m_textImage->Paint();

	if( HasFocus() && IsEnabled() )
	{
		DrawFocusBorder( 3, 3, buttonWide - 4, buttonTall - 2 );
	}
}

void CCareerButton::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetDefaultBorder( pScheme->GetBorder( "CareerButtonBorder" ) );
	SetDepressedBorder( pScheme->GetBorder( "CareerButtonDepressedBorder" ) );

	m_armedBorder = pScheme->GetBorder( "CareerButtonArmedBorder" );

	m_textNormalColor = pScheme->GetColor( "BrightBaseText", SDK_Color( 0, 0, 0, 0 ) );
	m_textDisabledColor = pScheme->GetColor( "DisabledText1", SDK_Color( 0, 0, 0, 0 ) );

	m_textImage->SetColor( GetFgColor() );

	m_textImage->SetFont( pScheme->GetFont( "Default", IsProportional() ) );

	m_textPad = strtol( pScheme->GetResourceString( "CareerButtonTextPad" ), nullptr, 10 );
	m_imagePad = strtol( pScheme->GetResourceString( "CareerButtonImagePad" ), nullptr, 10 );

	if( IsProportional() )
	{
		m_textPad = vgui2::scheme()->GetProportionalScaledValue( m_textPad );
		m_imagePad = vgui2::scheme()->GetProportionalScaledValue( m_imagePad );
	}

	char buf[ 128 ];

	GetText( buf, ARRAYSIZE( buf ) );

	m_textImage->SetText( buf );

	m_textImage->ResizeImageToContent();

	int buttonWide, buttonTall;
	GetSize( buttonWide, buttonTall );

	int imageWide, imageTall;
	m_image->GetSize( imageWide, imageTall );

	int v15 = buttonWide - imageWide - this->m_imagePad + -2 * this->m_textPad;

	int textWide, textTall;
	m_textImage->GetContentSize( textWide, textTall );

	if( v15 < textWide )
		m_textImage->SetSize(v15, textTall );

	auto color = pScheme->GetColor( "CareerButtonBG", SDK_Color( 0, 0, 0, 0 ) );

	SetDefaultColor( color, color );
	SetArmedColor( color, color );
	SetDepressedColor( color, color );
}

vgui2::IBorder* CCareerButton::GetBorder( bool depressed, bool armed, bool selected, bool keyfocus )
{
	if( armed && !depressed && IsEnabled() )
		return m_armedBorder;

	return BaseClass::GetBorder( depressed, armed, selected, keyfocus );
}

CCareerBaseFrame::CCareerBaseFrame( vgui2::Panel* parent, const char* panelName, bool showTaskbarIcon )
	: BaseClass( parent, panelName, showTaskbarIcon )
{
	//TODO: implement - Solokiller
}
