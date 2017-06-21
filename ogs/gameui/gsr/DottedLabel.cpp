#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "DottedLabel.h"

CDottedBgLabel::CDottedBgLabel( vgui2::Panel* parent, const char* name, const char* text, int dash, int gap, int corner )
	: BaseClass( parent, name, text )
{
	m_color = SDK_Color( 0, 0, 0, 0 );

	m_dash = dash;
	m_gap = gap;
	m_corner = corner;

	if( !dash )
		m_dash = 2;
	if( !gap )
		m_gap = 2;
	if( !corner )
		m_corner = 6;

	m_color = vgui2::scheme()->GetIScheme( GetScheme() )->GetColor( "BorderBright", m_color );
}

void CDottedBgLabel::PaintBackground()
{
	BaseClass::PaintBackground();

	int x, y, w, h;
	GetBounds( x, y, w, h );

	vgui2::surface()->DrawSetColor( m_color );

	DrawDashedLine( 0, 0, w, 1, m_dash, m_gap );
	DrawDashedLine( 0, 0, 1, h, m_dash, m_gap );
	DrawDashedLine( w - 1, 0, w, h, m_dash, m_gap );
	DrawDashedLine( 0, h - 1, w, h, m_dash, m_gap );

	vgui2::surface()->DrawFilledRect( 0, 0, m_corner, 1 );
	vgui2::surface()->DrawFilledRect( 0, 0, 1, m_corner );

	vgui2::surface()->DrawFilledRect( w - m_corner, 0, w, 1 );
	vgui2::surface()->DrawFilledRect( w - 1, 0, w, m_corner );

	vgui2::surface()->DrawFilledRect( 0, h - 1, m_corner, h );
	vgui2::surface()->DrawFilledRect( 0, h - m_corner, 1, h );

	vgui2::surface()->DrawFilledRect( w - m_corner, h - 1, w, h );
	vgui2::surface()->DrawFilledRect( w - 1, h - m_corner, w, h );
}
