#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "BitmapImagePanel.h"

CBitmapImagePanel::CBitmapImagePanel( vgui2::Panel* parent, const char* panelName, const char* filename )
	: BaseClass( parent, panelName )
{
	SetBounds( 0, 0, 100, 100 );

	if( filename && *filename )
	{
		strcpy( m_szTexture, filename );
	}
}

void CBitmapImagePanel::PaintBackground()
{
	if( m_szTexture[ 0 ] )
	{
		if( !m_bUploaded )
		{
			forceUpload();
		}

		int wide, tall;
		GetSize( wide, tall );

		vgui2::surface()->DrawSetColor( SDK_Color( 255, 255, 255, 255 ) );
		vgui2::surface()->DrawSetTexture( m_nTextureId );
		vgui2::surface()->DrawTexturedRect( 0, 0, wide, tall );
	}
}

void CBitmapImagePanel::setTexture( const char* filename )
{
	strcpy( m_szTexture, filename );

	if( m_bUploaded )
	{
		forceReload();
	}
	else
	{
		forceUpload();
	}
}

void CBitmapImagePanel::forceUpload()
{
	if( m_szTexture[ 0 ] )
	{
		m_bUploaded = true;
		m_nTextureId = vgui2::surface()->CreateNewTextureID();
		vgui2::surface()->DrawSetTextureFile( m_nTextureId, m_szTexture, false, true );
	}
}

void CBitmapImagePanel::forceReload()
{
	if( m_bUploaded )
	{
		vgui2::surface()->DrawSetTextureFile( m_nTextureId, m_szTexture, false, true );
	}
}
