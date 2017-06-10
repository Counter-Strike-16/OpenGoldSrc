#include <vgui/IInputInternal.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "vcontrolslistpanel.h"

CInlineEditPanel::~CInlineEditPanel()
{
}

void CInlineEditPanel::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( GetParent() )
	{
		GetParent()->OnKeyCodeTyped( code );
	}
}

void CInlineEditPanel::OnMousePressed( vgui2::MouseCode code )
{
	if( GetParent() )
	{
		GetParent()->OnMousePressed( code );
	}
}

void CInlineEditPanel::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetBorder( pScheme->GetBorder( "DepressedButtonBorder" ) );
}

void CInlineEditPanel::Paint()
{
	int wide, tall;
	GetSize( wide, tall );

	vgui2::surface()->DrawSetColor( 149, 136, 49, 255 );
	vgui2::surface()->DrawFilledRect( 0, 0, wide, tall );

	vgui2::surface()->DrawSetColor( 45, 49, 40, 255 );
	vgui2::surface()->DrawFilledRect( -3, -3, wide + 2, -2 );
	vgui2::surface()->DrawFilledRect( -2, -2, -3, tall + 3 );

	vgui2::surface()->DrawSetColor( 150, 159, 142, 255 );
	vgui2::surface()->DrawFilledRect( -2, tall + 3, wide + 3, tall + 2 );
	vgui2::surface()->DrawFilledRect( wide + 3, -3, wide + 2, tall + 2 );
}

VControlsListPanel::VControlsListPanel( vgui2::Panel* parent, const char* listName )
	: BaseClass( parent, listName )
{
	m_pInlineEditPanel = new CInlineEditPanel( nullptr, "InlineEditPanel" );
}

VControlsListPanel::~VControlsListPanel()
{
	m_pInlineEditPanel->MarkForDeletion();
}

int VControlsListPanel::GetItemOfInterest()
{
	return m_nClickRow;
}

void VControlsListPanel::SetItemOfInterest( int itemID )
{
	m_nClickRow = itemID;
}

bool VControlsListPanel::IsCapturing()
{
	return m_bCaptureMode;
}

void VControlsListPanel::StartCaptureMode( vgui2::HCursor hCursor )
{
	m_bCaptureMode = true;

	EnterEditMode( m_nClickRow, 1, m_pInlineEditPanel );

	vgui2::input()->SetMouseFocus( m_pInlineEditPanel->GetVPanel() );
	vgui2::input()->SetMouseCapture( m_pInlineEditPanel->GetVPanel() );

	if( hCursor )
	{
		m_pInlineEditPanel->SetCursor( hCursor );

		if( hCursor == vgui2::dc_none )
		{
			vgui2::input()->GetCursorPos( m_iMouseX, m_iMouseY );

			if( gEngfuncs.GetWindowCenterX && gEngfuncs.GetWindowCenterY )
			{
				gEngfuncs.pfnSetMousePos(
					gEngfuncs.GetWindowCenterX(),
					gEngfuncs.GetWindowCenterY()
				);
			}
		}
	}

	vgui2::surface()->SetTranslateExtendedKeys( false );
}

void VControlsListPanel::EndCaptureMode( vgui2::HCursor hCursor )
{
	m_bCaptureMode = false;

	vgui2::input()->SetMouseCapture( NULL_HANDLE );

	vgui2::surface()->SetTranslateExtendedKeys( true );

	LeaveEditMode();
	RequestFocus();
	vgui2::input()->SetMouseFocus( GetVPanel() );

	if( hCursor )
	{
		m_pInlineEditPanel->SetCursor( hCursor );

		if( GameUI().IsGameUIActive() )
		{
			vgui2::surface()->SetCursor( hCursor );

			if( hCursor != vgui2::dc_none )
			{
				vgui2::input()->SetCursorPos( m_iMouseX, m_iMouseY );
			}
		}
	}
}

void VControlsListPanel::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_hFont = pScheme->GetFont( "Default", IsProportional() );
}

void VControlsListPanel::OnMouseDoublePressed( vgui2::MouseCode code )
{
	if( IsItemIDValid( GetSelectedItem() ) )
		OnKeyCodePressed( vgui2::KEY_ENTER );
	else
		BaseClass::OnMouseDoublePressed( code );
}

void VControlsListPanel::OnMousePressed( vgui2::MouseCode code )
{
	if( IsCapturing() )
	{
		if( GetParent() )
		{
			GetParent()->OnMousePressed( code );
		}
	}
	else
	{
		BaseClass::OnMousePressed( code );
	}
}
