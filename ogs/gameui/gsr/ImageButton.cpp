#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/TextImage.h>

#include "ImageButton.h"

CImageButton::CImageButton( vgui2::Panel* parent, const char* panelName, const char* text )
	: BaseClass( parent, panelName, text )
{
}

CImageButton::~CImageButton()
{
}

void CImageButton::SetTexture( const char* filename )
{
	strcpy( m_szTexture, filename );

	if( m_bUploaded )
		ForceReload();
	else
		ForceUpload();
}

void CImageButton::ForceReload()
{
	if( m_bUploaded )
	{
		vgui2::surface()->DrawSetTextureFile( m_nTextureId, m_szTexture, false, true );
	}
}

SDK_Color CImageButton::GetImageColor()
{
	return GetBgColor();
}

void CImageButton::ForceUpload()
{
	if( m_szTexture[ 0 ] )
	{
		m_bUploaded = true;
		m_nTextureId = vgui2::surface()->CreateNewTextureID();
		vgui2::surface()->DrawSetTextureFile( m_nTextureId, m_szTexture, false, true );
	}
}

void CImageButton::PaintBackground()
{
	int w, h;
	GetSize( w, h );

	vgui2::surface()->DrawSetColor( GetBgColor() );

	vgui2::surface()->DrawFilledRect(
		m_padFill,
		m_padFill,
		w - m_padFill,
		h - m_padFill
	);

	if( m_szTexture[ 0 ] )
	{
		if( !m_bUploaded )
			ForceUpload();

		vgui2::surface()->DrawSetColor( GetImageColor() );
		vgui2::surface()->DrawSetTexture( m_nTextureId );
		vgui2::surface()->DrawTexturedRect(
			m_padImage,
			m_padImage,
			w - m_padImage,
			h - m_padImage
		);

		vgui2::TextImage* pImg = GetTextImage();

		int x, y;
		pImg->GetPos( x, y );
		pImg->SetPos( x + 1, y + 1 );
		pImg->SetColor( SDK_Color( 0, 0, 0, 192 ) );
		pImg->Paint();
		pImg->SetPos( x, y );
	}
}

void CImageButton::SetImagePadding( int pad )
{
	m_padImage = pad;
	InvalidateLayout();
}

void CImageButton::SetFillPadding( int pad )
{
	m_padFill = pad;
	InvalidateLayout();
}
