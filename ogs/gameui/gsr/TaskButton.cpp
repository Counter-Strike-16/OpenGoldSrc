#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Controls.h>

#include "TaskButton.h"

CWindowStateImage::CWindowStateImage()
{
	SetSize( 12, 24 );
	m_iState = WINDOW_STATE_OUTOFFOCUS;
}

void CWindowStateImage::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	const int xPos = ( wide - 8 ) / 2;

	switch( m_iState )
	{
	case WINDOW_STATE_FOCUS:
		{
			DrawSetColor( 255, 255, 255, 255 );
			DrawFilledRect( xPos, tall - 11, wide - xPos, tall - 3 );
			break;
		}
	case WINDOW_STATE_OUTOFFOCUS:
		{
			DrawSetColor( 100, 120, 100, 255 );
			DrawFilledRect( xPos, tall - 11, wide - xPos, tall - 3 );
			break;
		}
	case WINDOW_STATE_MINIMIZED:
		{
			DrawSetColor( 100, 120, 100, 255 );
			DrawFilledRect( xPos, tall - 7, wide - xPos, tall - 3 );
			break;
		}
	}
}

void CWindowStateImage::SetTall( int tall )
{
	SetSize( 12, tall );
}

void CWindowStateImage::SetFocused()
{
	m_iState = WINDOW_STATE_FOCUS;
}

void CWindowStateImage::SetOutOfFocus()
{
	m_iState = WINDOW_STATE_OUTOFFOCUS;
}

void CWindowStateImage::SetMinimized()
{
	m_iState = WINDOW_STATE_MINIMIZED;
}

CTaskButton::CTaskButton( vgui2::Panel* parent, vgui2::VPANEL task )
	: BaseClass( parent, "TaskButton", "<unknown>" )
{
	m_hTask = vgui2::ivgui()->PanelToHandle( task );

	SetTextImageIndex( 1 );
	SetImageAtIndex( 0, &m_WindowStateImage, 4 );
	SetImagePreOffset( 1, 4 );
}

void CTaskButton::OnMouseDoublePressed( vgui2::MouseCode mouseCode )
{
	OnMousePressed( mouseCode );
}

void CTaskButton::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_LabelColor = GetSchemeColor( "DisabledText1", pScheme );
	m_SelectedColor = GetSchemeColor( "BrightBaseText", pScheme );

	SetContentAlignment( vgui2::Label::a_west );
	SetBgColor( SDK_Color( 0, 0, 0, 0 ) );

	SetTextImageIndex( 1 );
	SetImageAtIndex( 0, &m_WindowStateImage, 4 );
	SetImagePreOffset( 1, 4 );

	static_cast<vgui2::IImage*>( &m_WindowStateImage )->SetSize(
		12, vgui2::surface()->GetFontTall( GetFont() )
	);
}

void CTaskButton::OnMousePressed( vgui2::MouseCode mouseCode )
{
	if( m_bSelected )
	{
		auto task = GetTaskPanel();

		if( task )
		{
			vgui2::surface()->SetMinimized( task, true );
		}
	}
	else
	{
		SetSelected( true );
	}
}

void CTaskButton::SetTitle( const wchar_t* title )
{
	SetText( title );

	if( title && *title )
		m_bHasTitle = true;
}

void CTaskButton::SetSelected( bool state )
{
	if( m_bSelected != state )
	{
		m_bSelected = state;

		auto task = GetTaskPanel();

		if( task )
		{
			if( m_bSelected )
			{
				vgui2::surface()->SetMinimized( task, false );
				vgui2::ipanel()->SetVisible( task, true );

				if( !vgui2::ipanel()->HasParent( task, vgui2::surface()->GetTopmostPopup() ) )
				{
					vgui2::ipanel()->MoveToFront( task );
				}

				SetFgColor( m_SelectedColor );

				m_WindowStateImage.SetFocused();
			}
			else
			{
				SetFgColor( m_LabelColor );

				if( vgui2::surface()->IsMinimized( task ) )
					m_WindowStateImage.SetMinimized();
				else
					m_WindowStateImage.SetOutOfFocus();
			}
		}
	}
}

bool CTaskButton::ShouldDisplay()
{
	if( !m_bHasTitle )
		return false;

	auto task = GetTaskPanel();

	if( NULL_HANDLE == task )
		return false;

	if( vgui2::ipanel()->IsVisible( task ) )
		return true;

	return vgui2::surface()->IsMinimized( task );
}

vgui2::VPANEL CTaskButton::GetTaskPanel()
{
	return vgui2::ivgui()->HandleToPanel( m_hTask );
}
